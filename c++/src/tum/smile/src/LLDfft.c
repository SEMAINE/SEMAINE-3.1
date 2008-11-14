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
 
 /*  IMPLEMENTATION of the feature extractor module LLDfft
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDfft"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
//#include "LLDfft.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function

// NOTE: frameSize changes through the processing are NOT supported
//      this would require checks in each call to the extractor, if frameSize is the same as for the last frame
//      if a change is detected, all the fft variables would have to be reinitialized!

// declaration of fft function defined in fft4g.c, fft8g.c or fftsg.c
//void rdft(int n, int isgn, double *a, int *ip, double *w);


// default constructor 
pLLDfft LLDfft_create( pLLDfft obj )
#define FUNCTION "LLDfft_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDfft)calloc(1,sizeof(sLLDfft));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }          
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 




// flushToMem: copy final data from ftex to feature memory element array
int LLDfft_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDfft_flushToFtMem"
{_FUNCTION_ENTER_

  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->fft == NULL) _FUNCTION_RETURN_(0);
  if (lldex->current[level]->fft->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_fft, 
                                           lldex->current[level]->fft->nBins, 0, 
                                           lldex->current[level]->timestamp );
   //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)
   // -> ok (at least here...)


  if (values != NULL) {
   
    LONG_IDX n;  
    //FEATUM_DEBUG(10,"values okay... filled = %i  (el=%i) (curIdx = %i)",mem->level[level].ft[ft_lld_fft][mem->level[level].curIdx].filled,ft_lld_fft,mem->level[level].curIdx);
   
    // flush linear magnitudes only
    int i;
    FLOAT_TYPE_FFT *arr= lldex->current[level]->fft->magnitudes;
    for (i=0; i< lldex->current[level]->fft->nBins; i++) {
      values[i] = (FLOAT_TYPE_FTMEM)(arr[i]);
    }
   
    lldex->current[level]->fft->flush_status = LLDEX_FLUSHED;

    _FUNCTION_RETURN_(1);
  } else {
    FEATUM_WARNING(1,"values array is NULL");       
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDfft_extractor( pLLDfft obj, pLLDex lldex, int level )
#define FUNCTION "LLDfft_extractor"
{_FUNCTION_ENTER_
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    
    pPcmBuffer pcm = lldex->current[level]->pcm;
    if (pcm == NULL) {
      FEATUM_WARNING(1,"nothing to extract, pcm data is NULL");
      _FUNCTION_RETURN_(0);        
    }

    

    
    
    #ifndef USE_SLOW_DFT
    // WARNING: fft computation will crash if pcm->nBlocks is not a power of two!!
    // therefore we check for this constraint here:
    if (isPowerOf2(pcm->nBlocks) && (pcm->nBlocks >= 4) ) {
    #endif
       
      if (lldex->current[level]->fft == NULL) {
        LLDfft_createLLDex( lldex->current[level] );                               
      }
      
      #ifdef SUPPORT_FRAMESIZE_CHANGE
      // TODO: test if this code really does work!!
      if (obj->frameSize_last != pcm->nBlocks) { // reinit if framesize has changed!
        FEATUM_DEBUG(3,"framesize change detected! new size = %i",pcm->nBlocks);
        if (obj->ip != NULL) { free(obj->ip); obj->ip = NULL; }
        if (obj->w != NULL) { free(obj->w); obj->w = NULL; }
        obj->frameSize_last = pcm->nBlocks;
        FEATUM_DEBUG(3,"gggs %i %i",lldex->current[level]->fft->nBins, pcm->nBlocks / 2);
        if (lldex->current[level]->fft->nBins != pcm->nBlocks / 2) {
          // also reallocate magnitudes and phases arrays if new framesize is larger!
          if (lldex->current[level]->fft->nBins < pcm->nBlocks / 2) {
            lldex->current[level]->fft->nBins = pcm->nBlocks / 2;
            lldex->current[level]->fft->nBlocks = pcm->nBlocks;
            if (lldex->current[level]->fft->complex != NULL) free(lldex->current[level]->fft->complex);
            if (lldex->current[level]->fft->magnitudes != NULL) free(lldex->current[level]->fft->magnitudes);
            #ifdef COMPUTE_PHASE
            if (lldex->current[level]->fft->phases != NULL) free(lldex->current[level]->fft->phases);
            #endif
            lldex->current[level]->fft->complex= (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*lldex->current[level]->fft->nBlocks);
            lldex->current[level]->fft->magnitudes = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*lldex->current[level]->fft->nBins);
            #ifdef COMPUTE_PHASE
            lldex->current[level]->fft->phases = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*lldex->current[level]->fft->nBins);
            #endif
          
          } else {
            lldex->current[level]->fft->nBins = pcm->nBlocks / 2;
            lldex->current[level]->fft->nBlocks = pcm->nBlocks;
          }
        }
      }
      #endif
       
      #ifdef USE_SLOW_DFT
      if (obj->ip == NULL) {
        obj->ip = (FLOAT_TYPE_FFT *)malloc((pcm->nBlocks+2)*sizeof(FLOAT_TYPE_FFT)); 
        *(obj->ip) = -99.0;
        if (obj->w == NULL) {
          obj->w = (FLOAT_TYPE_FFT*)malloc(((pcm->nBlocks+2)*sizeof(FLOAT_TYPE_FFT));
          if (obj->w == NULL) FEATUM_ERROR(0,"Error allocating memory"); 
        }
        if (obj->ip == NULL) FEATUM_ERROR(0,"Error allocating memory");
      }
      #else
      if (obj->ip == NULL) {
        obj->ip = (int *)calloc(1,(pcm->nBlocks+2)*sizeof(int)); 
        if (obj->w == NULL) {
          obj->w = (double*)calloc(1,((pcm->nBlocks * 5)/4+1)*sizeof(double));
          if (obj->w == NULL) FEATUM_ERROR(0,"Error allocating memory"); 
        }
        if (obj->ip == NULL) FEATUM_ERROR(0,"Error allocating memory");
      }
      #endif
                            
      #ifdef COMPUTE_PHASE
      pcmProcess_fft_All( pcm, lldex->current[level]->fft->complex, lldex->current[level]->fft->magnitudes, lldex->current[level]->fft->phases, obj->ip, obj->w );
      #else
      pcmProcess_fft_All( pcm, lldex->current[level]->fft->complex, lldex->current[level]->fft->magnitudes, NULL, obj->ip, obj->w );
      #endif

      #ifdef DATASAVE_FFT
      if (!datasave_appendvec_d("fft.dat",lldex->current[level]->fft->magnitudes,pcm->nBlocks/2)) { FEATUM_ERROR(0,"error writing to fft.dat"); }
      #endif

      FEATUM_DEBUG(10,"fft0: %f",lldex->current[level]->fft->magnitudes[0]);
      _FUNCTION_RETURN_(1);
    #ifndef USE_SLOW_DFT
    } else {
      FEATUM_WARNING(2,"pcm->nBlocks (%i) is not a power of 2 (or smaller than 4)! This is required for fast FFT with fft4g/fft8g/fftsg.c!",pcm->nBlocks);
      _FUNCTION_RETURN_(0);
    }
    #endif
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDfft_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDfft_createLLDex"
{_FUNCTION_ENTER_
  pLLDexFFT p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->fft != NULL) LLDfft_freeLLDex(obj);
    p = (pLLDexFFT) calloc(1,sizeof(sLLDexFFT));
    // TODO: create fft arrays...  use obj->pcm->...frameSize... as basis
    
    if (p != NULL) {
      if (obj->pcm != NULL) {
        p->nBins = obj->pcm->nBlocks / 2;  // is this already available at this point?? seems to be ok... check where createLLDex function gets called
        p->nBlocks = obj->pcm->nBlocks;
        if (obj->pcm->sampleRate > 0) {
          p->fStep = (FLOAT_TYPE)obj->pcm->sampleRate / (FLOAT_TYPE)obj->pcm->nBlocks;
          FEATUM_DEBUG(9,"fStep: %f",p->fStep);
        }
        p->complex = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*obj->pcm->nBlocks);
        p->magnitudes = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*p->nBins);
        #ifdef COMPUTE_PHASE
        p->phases = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*p->nBins);
        if ((p->magnitudes == NULL)||(p->complex == NULL)||(p->phases == NULL)) {
        #else
        if ((p->magnitudes == NULL)||(p->complex == NULL)) {
        #endif
          FEATUM_ERROR(0,"Error allocating memory");
          p->nBins = 0;
        }
      } else {
        p->nBins = 0;
        p->nBlocks = 0;
        FEATUM_ERROR(0,"cannot initialize fft region in lldex struct because pcm region is not initialised!");
      }
      p->freeLLDex = &LLDfft_freeLLDex;
      ret = 1;
    }
    obj->fft = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDfft_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDfft_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->fft != NULL) {
      // TODO: free fft arrays
      if (obj->fft->complex != NULL) free(obj->fft->complex);
      if (obj->fft->magnitudes != NULL) free(obj->fft->magnitudes);
      #ifdef COMPUTE_PHASE
      if (obj->fft->phases != NULL) free(obj->fft->phases);
      #endif
      free(obj->fft); obj->fft = 0;  
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDfft feature extractor object
pLLDfft LLDfft_destroy( pLLDfft obj )
#define FUNCTION "LLDfft_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->ip != NULL) { free(obj->ip); }
    if (obj->w != NULL) { free(obj->w);  }
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



