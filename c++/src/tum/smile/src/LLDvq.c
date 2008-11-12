/*******************************************************************************
 * feaTUM, fast, efficient audio feature extractor by TUM
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer
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
 
 /*  IMPLEMENTATION of the feature extractor module LLDvq
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDvq"
 
 // this file contains the function definitions and the private class data

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function

#ifdef FT_LLD_VQ

// default constructor 
pLLDvq LLDvq_create( pLLDvq obj )
#define FUNCTION "LLDvq_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDvq)calloc(1,sizeof(sLLDvq));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }        
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 

/*
int LLDvq_configure( pLLDvq obj, int nMel, int nMFCC, int cepLifter, int firstMFCC )
#define FUNCTION "LLDvq_configure"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (nMel >= 1) {
      obj->nMel = nMel;
    }
    if (nMFCC >= 1) {
      obj->nMFCC = nMFCC;  
      if (obj->sintable != NULL) { free(obj->sintable); obj->sintable = NULL; }
      if (obj->costable != NULL) { free(obj->costable); obj->costable = NULL; }
    }
    if (cepLifter >= 1)
      obj->cepLifter = cepLifter;  
    if (firstMFCC >= 0)
      obj->firstMFCC = firstMFCC;
    _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 
*/

// custom configuration of extractor parameters
/*  this is only a template function:
int LLDvq_configure( pLLDvq obj )
#define FUNCTION "LLDvq_create"
{_FUNCTION_ENTER_
  if (obj != NULL) {
     _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 
*/



// flushToMem: copy final data from ftex to feature memory element array
int LLDvq_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDvq_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->vq == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->vq->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_vq, 
                                           lldex->current[level]->vq->nMFCC, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {
    values[0] = lldex->current[level]->vq->hnr;
    values[1] = lldex->current[level]->vq->jitter;
    values[2] = lldex->current[level]->vq->shimmer;
    
    lldex->current[level]->vq->flush_status = LLDEX_FLUSHED;
    //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)

    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDvq_extractor( pLLDvq obj, pLLDex lldex, int level )
#define FUNCTION "LLDvq_extractor"
{_FUNCTION_ENTER_
  int i,m;
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (lldex->current[level]->fft == NULL) {
      FEATUM_ERROR(9,"fft data in lldex struct is NULL, but required for vq!");
      _FUNCTION_RETURN_(0);
    }
    
    LONG_IDX N = lldex->current[level]->fft->nBins;
    

    // copy & square the fft magnitude
    FLOAT_TYPE_FFT *magSrc = lldex->current[level]->fft->magnitudes;
    if (magSrc == NULL) _FUNCTION_RETURN_(0);
    FLOAT_TYPE_FFT *mag = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT)*N);
    if (mag == NULL) {
      FEATUM_ERROR(0,"Error allocating memory");
      _FUNCTION_RETURN_(0);        
    }
    int n;
    for (n=0; n<N; n++) {
      mag[n] = magSrc[n]*magSrc[n];
    } 
        
    // done... free used memory
    free(mag);

/*    
    // compute dct of mel data:
    outp =  currentVq->vqUL;
    FLOAT_TYPE * outpL =  currentVq->vq;
    FLOAT_TYPE * mel = currentVq->melBand;
    FLOAT_TYPE factor = sqrt(2.0/(FLOAT_TYPE)(obj->nMel));
    int nM = obj->nMel; 
    for (i=obj->firstMFCC; i < obj->firstMFCC+obj->nMFCC; i++) {
       FLOAT_TYPE * outc = outp + (i-obj->firstMFCC); *outc = 0.0;
       for (m=0; m<obj->nMel; m++) {
         *outc +=  mel[m] * obj->costable[m+(i-obj->firstMFCC)*nM];
       }
       *outc *= factor; 
       // do cepstral liftering:
       outpL[i-obj->firstMFCC]  =  *outc * obj->sintable[i-obj->firstMFCC];
    }
*/
        
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDvq_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDvq_createLLDex"
{_FUNCTION_ENTER_
  pLLDexVq p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->vq != NULL) LLDvq_freeLLDex(obj);          
    obj->vq = NULL;
    p = (pLLDexVq) calloc(1,sizeof(sLLDexVq));
    if (p != NULL) {
      p->freeLLDex = &LLDvq_freeLLDex;
      ret = 1;
    }
    obj->vq = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDvq_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDvq_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->vq != NULL) {
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDvq feature extractor object
pLLDvq LLDvq_destroy( pLLDvq obj )
#define FUNCTION "LLDvq_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION


#endif //FT_LLD_VQ
