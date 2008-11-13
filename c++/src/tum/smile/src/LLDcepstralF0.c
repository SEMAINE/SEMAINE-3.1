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
 
 /*  IMPLEMENTATION of the feature extractor module LLDcepstralF0
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDcepstralF0"
 
 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
//#include "LLDcepstralF0.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function


// default constructor 
pLLDcepstralF0 LLDcepstralF0_create( pLLDcepstralF0 obj )
#define FUNCTION "LLDcepstralF0_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDcepstralF0)calloc(1,sizeof(sLLDcepstralF0));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
      _FUNCTION_RETURN_( NULL );
    } 
    obj->maxPitch = CEPS_MAX_PITCH;
    obj->threshold = CEPS_F0_THRESH;
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


// custom configuration of extractor parameters

int LLDcepstralF0_configure(pLLDcepstralF0 obj, FLOAT_TYPE_FFT maxPitch, FLOAT_TYPE_FFT threshold )
#define FUNCTION "LLDcepstralF0_configure"
{_FUNCTION_ENTER_
  if (obj != NULL) {
      obj->maxPitch = maxPitch;
      obj->threshold = threshold;
     _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 



// flushToMem: copy final data from ftex to feature memory element array
int LLDcepstralF0_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDcepstralF0_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->cepstralF0 == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->cepstralF0->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_cepstralF0, 
                                           FT_LLD_CEPSTRALF0_nVal, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {

   values[0] // 0
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->cepstralF0->F0freq[0]; 
   values[1] // 1
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->cepstralF0->F0strength[0];

   lldex->current[level]->cepstralF0->flush_status = LLDEX_FLUSHED;

    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDcepstralF0_extractor( pLLDcepstralF0 obj, pLLDex lldex, int level )
#define FUNCTION "LLDcepstralF0_extractor"
{_FUNCTION_ENTER_
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (lldex->current[level]->cepstrum == NULL) {
      FEATUM_ERROR(9,"cepstrum object in LLDex is NULL! required for cepstralF0!\n");
      _FUNCTION_RETURN_(0);                                    
    }

    // algorithm:
    /* compute log magnitude spectrum und transform back into time domain */

    LONG_IDX N = lldex->current[level]->cepstrum->numCeps;
    FLOAT_TYPE_FFT * data = lldex->current[level]->cepstrum->cepstrum;
 

    if (lldex->current[level]->cepstralF0 == NULL) {
       LLDcepstralF0_createLLDex( lldex->current[level] );                               
    }
    // save cepstralF0 data for visualisation and debugging purposes:
    //if (!datasave_appendvec_d("cepstralF0.dat",data,N)) { FEATUM_ERROR_FATAL(0,"error writing to cepstralF0.dat"); }

    // peak picking:
    double Tsamp = 1.0/(double)(lldex->current[level]->pcm->sampleRate);  // sampling period
    LONG_IDX maxIdx=0;
    
    double maxPitch = CEPS_MAX_PITCH;
    int preskip = (int)(1.0 / (maxPitch*Tsamp));  // this variable determines the maximal pitch detectable
    if (preskip < 0) preskip = 0;
    FEATUM_DEBUG(10,"preskip = %i",preskip);
    int exlen = N/2-preskip;
    
    // TODO: maybe find first significant max. after first significant min.!
    FLOAT_TYPE_FFT maxVal = max_dvfft(data+preskip-1,exlen,&maxIdx);
    FLOAT_TYPE_FFT meanVal = mean_dvfft(data+preskip-1,exlen);
    lldex->current[level]->cepstralF0->f0Index[0] = maxIdx;
    if ((maxVal >= CEPS_F0_THRESH*meanVal)&&(maxIdx > 0)) { // this threshold determines how well seperation between voiced and unvoiced/noise/silence works!
      lldex->current[level]->cepstralF0->F0freq[0] = ( 1.0 / ( (FLOAT_TYPE)(maxIdx+preskip-1) * (FLOAT_TYPE)Tsamp ) ) / (FLOAT_TYPE)CEPS_MAX_PITCH;
      lldex->current[level]->cepstralF0->F0strength[0] = (FLOAT_TYPE)(maxVal-meanVal);
    } else {
      lldex->current[level]->cepstralF0->F0freq[0] = 0.0;           
      lldex->current[level]->cepstralF0->F0strength[0] = (FLOAT_TYPE)(maxVal-meanVal);
    }
    // TODO: find second and third peak(!)  -> smoothing and peak picking, then sorting... complex algorithm!


    // -----
    
    FEATUM_DEBUG(7,"cepstral F0freq = %f  (strength = %f)",lldex->current[level]->cepstralF0->F0freq, lldex->current[level]->cepstralF0->F0strength);
    
    free(data);
    
  }

  _FUNCTION_RETURN_(1);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDcepstralF0_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDcepstralF0_createLLDex"
{_FUNCTION_ENTER_
  pLLDexCepstralF0 p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->cepstralF0 != NULL) LLDcepstralF0_freeLLDex(obj);          
    p = (pLLDexCepstralF0) calloc(1,sizeof(sLLDexCepstralF0));
    if (p != NULL) {
      p->freeLLDex = &LLDcepstralF0_freeLLDex;
      ret = 1;
    }
    obj->cepstralF0 = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDcepstralF0_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDcepstralF0_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->cepstralF0 != NULL) {
      free(obj->cepstralF0);  obj->cepstralF0 = NULL;
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDcepstralF0 feature extractor object
pLLDcepstralF0 LLDcepstralF0_destroy( pLLDcepstralF0 obj )
#define FUNCTION "LLDcepstralF0_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



