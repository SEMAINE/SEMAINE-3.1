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
 
 /*  IMPLEMENTATION of the feature extractor module LLDfft
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDfft"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include "featum_util.h"       
#include "feature-memory.h"
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

  if (values != NULL) {
    LONG_IDX n;  
   
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

    if (pcm->nBlocks < 4) {
      FEATUM_WARNING(5,"nBlocks < 4, cannot run FFT, skipping this block!");
      _FUNCTION_RETURN_(0);        
    }

    // do zero padding if blocksize is not power of 2
    int pad = 0; 
    if (!isPowerOf2_long(pcm->nBlocks)) {
      pPcmBuffer pcm0 = pcm; pad = 1;
      long nBlocksPow2 = ceilPosPow2_long((long)(pcm->nBlocks));
      pcm = pcmBuffer_duplicateResize( pcm0, nBlocksPow2 );
      pcm->nBlocks = nBlocksPow2;
      pcmBufferFree(lldex->current[level]->pcm,0);
      lldex->current[level]->pcm = pcm;
    }
	
    {       
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
      }
      #else 
      if (obj->frameSize_last != pcm->nBlocks) { // reinit if framesize has changed!
        FEATUM_DEBUG(3,"framesize change detected! new size = %i",pcm->nBlocks);
        FEATUM_WARNING(6,"framesize change not supported, not computing this frame!");
      }
      #endif
       
      if (obj->ip == NULL) {
        obj->ip = (int *)calloc(1,(pcm->nBlocks+2)*sizeof(int)); 
        if (obj->w == NULL) {
          obj->w = (FLOAT_TYPE_FFT*)calloc(1,((pcm->nBlocks * 5)/4+1)*sizeof(FLOAT_TYPE_FFT));
          if (obj->w == NULL) FEATUM_ERROR(0,"Error allocating memory"); 
        }
        if (obj->ip == NULL) FEATUM_ERROR(0,"Error allocating memory");
      }
                            
      #ifdef COMPUTE_PHASE
      pcmProcess_fft_All( pcm, lldex->current[level]->fft->complex, lldex->current[level]->fft->magnitudes, lldex->current[level]->fft->phases, obj->ip, obj->w );
      #else
      pcmProcess_fft_All( pcm, lldex->current[level]->fft->complex, lldex->current[level]->fft->magnitudes, NULL, obj->ip, obj->w );
      #endif

      #ifdef DATASAVE_FFT
      if (!datasave_appendvec_d("fft.dat",lldex->current[level]->fft->magnitudes,pcm->nBlocks/2)) { FEATUM_ERROR(0,"error writing to fft.dat"); }
      #endif

      FEATUM_DEBUG(10,"fft0: %f",lldex->current[level]->fft->magnitudes[0]);
	  //NOT needed:      if (pad) pcmBufferFree(pcm,0);
      _FUNCTION_RETURN_(1);

    }
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



