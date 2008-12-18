/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/
 
 /*  IMPLEMENTATION of the feature extractor module LLDlpc
     See the corresponding header file (.h) for documentation!  */

// NOTE::: This code is incomplete and untested... (TODO.....)

#define MODULE "LLDlpc"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function


// default constructor 
pLLDlpc LLDlpc_create( pLLDlpc obj )
#define FUNCTION "LLDlpc_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDlpc)calloc(1,sizeof(sLLDlpc));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }     
    obj->lpcOrder = DEFAULT_lpcOrder;
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 

// private method:
int alloc_internal( pLLDlpc obj )
{
  if (obj->lpcOrder > 0) {
    if (obj->E != NULL) free(obj->E);
    obj->E = (FLOAT_TYPE *)calloc(1,sizeof(FLOAT_TYPE)*obj->lpcOrder);
    if (obj->a != NULL) free(obj->a);
    obj->a = (FLOAT_TYPE *)calloc(1,sizeof(FLOAT_TYPE)*(obj->lpcOrder*obj->lpcOrder));
  }    
}

int LLDlpc_configure( pLLDlpc obj, int lpcOrder )
#define FUNCTION "LLDlpc_configure"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (lpcOrder > 0) {
      obj->lpcOrder = lpcOrder;
    }
    alloc_internal(obj);
    _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 


// flushToMem: copy final data from ftex to feature memory element array
int LLDlpc_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDlpc_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->lpc == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->lpc->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_lpc, 
                                           lldex->current[level]->lpc->lpcOrder, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {
    // flush the reflexion coefficients
    int i;
    FLOAT_TYPE *lpcarr = lldex->current[level]->lpc->k;
    for (i=0; i< lldex->current[level]->lpc->lpcOrder; i++) {
      values[i] = (FLOAT_TYPE_FTMEM)(lpcarr[i]);
    }   
    // TODO: flush other coefficients : for simplicity: flush lpcOrder (instead of lpcOrder-1) lpc coefficients, resulting in 3x lpcOrder sized elements
    
    lldex->current[level]->lpc->flush_status = LLDEX_FLUSHED;
    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION



// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDlpc_extractor( pLLDlpc obj, pLLDex lldex, int level )
#define FUNCTION "LLDlpc_extractor"
{_FUNCTION_ENTER_
  int i,m;
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
 
    pPcmBuffer b = lldex->current[level]->pcm;
    if (b == NULL) {
      FEATUM_WARNING(1,"nothing to extract, pcm data is NULL");
      _FUNCTION_RETURN_(0);        
    }

    // todo: split this extractor in multiple ones, first acf, then lpcRef, lpc and lpCepstra  ??

    LONG_IDX N = b->nBlocks;
    if ((b == NULL)||((N == 0))) {
        _FUNCTION_RETURN_( 0 );
    }

    if ((obj->E == NULL)||(obj->a == NULL)) alloc_internal(obj);
    
    int M = obj->lpcOrder;
    
    /*# default code for creation of LLDex object #*/
    pLLDexLpc currentLpc = lldex->current[level]->lpc;
    if (currentLpc == NULL) {
       LLDlpc_createLLDex( lldex->current[level] );
       currentLpc = lldex->current[level]->lpc;
    }
    /** end default code; begin custom alloc code... */
    if (currentLpc->lpcOrder == 0) {
       if (obj->lpcOrder == 0) { _FUNCTION_RETURN_(0); }
       currentLpc->lpcOrder = obj->lpcOrder;
       currentLpc->k = (FLOAT_TYPE *) malloc(sizeof(FLOAT_TYPE)*(M));
       currentLpc->a = (FLOAT_TYPE *) malloc(sizeof(FLOAT_TYPE)*(M));
       currentLpc->r = (FLOAT_TYPE *) calloc(1,sizeof(FLOAT_TYPE)*(M+1));
       // TODO: check for allocation errors:
       
    }
    /*- end custom alloc code -*/
    
    if (M>0) {
    // acf (assume zero outside of window, reasonable due to windowing function..)
    LONG_IDX i,j;
    WORK_SAMPLE * s = (WORK_SAMPLE *)(b->data);
    FLOAT_TYPE *r = currentLpc->r;
    FLOAT_TYPE *k = currentLpc->k;
    for (i=0; i<=M; i++) {
      for(j=1; j<N-i; j++) { r[i] += s[j]*s[j+i]; }
    }

    // reflexion coefficients:
    FLOAT_TYPE *E = obj->E;
    FLOAT_TYPE *a = obj->a;
    E[0] = r[0];
    a[0] = k[0] = r[1]/r[0];
    E[1] = E[0]*(1.0-k[0]*k[0]);
    
    int m;
    for (m=1; m < M; m++) {
      FLOAT_TYPE sm = 0.0;
      for (i=0; i<m-1; i++) { sm += a[i+m*M] * r[m-i]; }
      a[m+m*M] = k[m] = (r[m+1] - sm)/E[m];  
      for (i=0; i<m-1; i++) { a[i+m*M] = a[i+(m-1)*M] - k[m]*a[m-i+(m-1)*M]; }
      E[m+1] = E[m] * (1.0 - k[m]*k[m]);
    }
    
    a += (m-1)*M;  // last iteration...
    for (i=0; i<M; i++) {
      lldex->current[level]->lpc->a[i] = a[i];
    }
    }

    #ifdef DATASAVE_LPC
    if (!datasave_appendvec_d("lpc.dat",lldex->current[level]->lpc->a,lldex->current[level]->lpc->lpcOrder)) { FEATUM_ERROR(0,"error writing to lpc.dat"); }
    //if (!datasave_appendvec_d("melspec.dat",lldex->current[level]->lpc->melBand,lldex->current[level]->lpc->nMel)) { FEATUM_ERROR(0,"error writing to melspec.dat"); }
    #endif
        
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDlpc_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDlpc_createLLDex"
{_FUNCTION_ENTER_
  pLLDexLpc p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->lpc != NULL) LLDlpc_freeLLDex(obj);
    p = (pLLDexLpc) calloc(1,sizeof(sLLDexLpc));
    if (p != NULL) {
      p->freeLLDex = &LLDlpc_freeLLDex;
      ret = 1;
    }
    obj->lpc = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDlpc_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDlpc_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->lpc != NULL) {
      if (obj->lpc->k != NULL) { free(obj->lpc->k); obj->lpc->k=NULL; }
      if (obj->lpc->a != NULL) { free(obj->lpc->a); obj->lpc->a=NULL; }
      if (obj->lpc->r != NULL) { free(obj->lpc->r); obj->lpc->r=NULL; }
      free(obj->lpc); obj->lpc = NULL;
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDlpc feature extractor object
pLLDlpc LLDlpc_destroy( pLLDlpc obj )
#define FUNCTION "LLDlpc_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->E != NULL) { free(obj->E); }
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



