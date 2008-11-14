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
 
 /*  IMPLEMENTATION of the feature extractor module LLDcepstrum
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDcepstrum"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "featum_util.h"       //( optional )
#include "feature-memory.h"
//#include "LLDcepstrum.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function


// default constructor 
pLLDcepstrum LLDcepstrum_create( pLLDcepstrum obj )
#define FUNCTION "LLDcepstrum_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDcepstrum)calloc(1,sizeof(sLLDcepstrum));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }  
    obj->complexCepstrum = 0;        
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


// custom configuration of extractor parameters

int LLDcepstrum_configure(pLLDcepstrum obj, int numCeps, int complex )
#define FUNCTION "LLDcepstrum_configure"
{_FUNCTION_ENTER_
  if (obj != NULL) {
     if (numCeps > 0) 
       obj->numCeps = numCeps;
       obj->complexCepstrum = complex;
     _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 



// setup names in feature memory object
/*
int LLDcepstrum_setupFtmem( int level, pFeatureMemory mem )
#define FUNCTION "LLDcepstrum_setupNames"
{_FUNCTION_ENTER_
  int ret;
  ret = featureMemory_setupElement( mem, level, FT_LLD_CEPSTRUM, FT_LLD_CEPSTRUM_nVal, (char **)&lld_cepstrum_ftMem_names );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 
*/  

//int featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal )

// flushToMem: copy final data from ftex to feature memory element array
int LLDcepstrum_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDcepstrum_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->cepstrum == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->cepstrum->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_cepstrum, 
                                           lldex->current[level]->cepstrum->numCeps, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {

   int i;
   for (i=0; i<lldex->current[level]->cepstrum->numCeps; i++) {
     values[i] = (FLOAT_TYPE_FTMEM)lldex->current[level]->cepstrum->cepstrum[i];
   }

   lldex->current[level]->cepstrum->flush_status = LLDEX_FLUSHED;
   //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)

    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDcepstrum_extractor( pLLDcepstrum obj, pLLDex lldex, int level )
#define FUNCTION "LLDcepstrum_extractor"
{_FUNCTION_ENTER_
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (lldex->current[level]->fft == NULL) {
      FEATUM_ERROR(9,"fft data in lldex struct is NULL, but required for cepstrum!");
      _FUNCTION_RETURN_(0);
    }
/*    #ifdef DATASAVE_CEPSTRUM
    FLOAT_TYPE_FFT * data2=1;
    // save cepstrum data for visualisation and debugging purposes:
    if (!datasave_appendvec_d("xyz.dat",data2,63)) { FEATUM_ERROR_FATAL(0,"error writing to cepstrum.dat"); }
    #endif
*/
    // algorithm:
    /* compute log magnitude spectrum und transform back into time domain */

    LONG_IDX N = lldex->current[level]->fft->nBlocks;
    if (isPowerOf2(N) && (N>=4) ) {  // only continue if size is power of 2 and greater or equal 4
    
    FLOAT_TYPE_FFT * data = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*N);
    if (data == NULL) {
      FEATUM_ERROR(0,"Error allocating memory");         
      _FUNCTION_RETURN_(0);
    }
    memcpy(data,lldex->current[level]->fft->complex,N*sizeof(FLOAT_TYPE_FFT));
    
    // square and log the spectrum:
    LONG_IDX i; FLOAT_TYPE_FFT *tmp = data;

    *tmp = log(fabs(*tmp));   // 0th coefficient is real only
    *(tmp+1) = log(fabs(*tmp)); tmp += 2;   // nyquist frequency is also real only
    FLOAT_TYPE_FFT magSq,phase,re,im;
    if (obj->complexCepstrum) {
      for (i=2; i<N-1; i += 2) {   // compute the complex logarithm
        magSq = (*tmp)*(*tmp) + (*(tmp+1))*(*(tmp+1));
        phase = atan((*(tmp+1))/(*tmp));
        *tmp = log(magSq);
        *(tmp+1) = phase;
        tmp += 2;
      }
    } else {
      for (i=2; i<N-1; i += 2) {   // compute the logarithm of the magnitude only
        magSq = (*tmp)*(*tmp) + (*(tmp+1))*(*(tmp+1));
        *tmp = log(magSq);
        *(tmp+1) = 0.0;
        tmp += 2;
      }
    }
    
    // check for valid ip and w arrays:
          //    #ifdef SUPPORT_FRAMESIZE_CHANGE
         // TODO: implement support for framesize change (copy from LLDfft.c)

    if (obj->ip == NULL) {
      obj->ip = (int *)calloc(1,(N+2)*sizeof(int)); 
      if (obj->w == NULL) {
        obj->w = (double*)calloc(1,((N * 5)/4+1)*sizeof(double));
      }
      if ((obj->ip == NULL)||(obj->w == NULL)) {
        FEATUM_ERROR(0,"Error allocating memory");
        _FUNCTION_RETURN_(0);
      }
    }
    
    // save powerspec data for visualisation and debugging purposes:
    //if (!datasave_appendvec_d("pspec.dat",data,N)) { FEATUM_ERROR(0,"error writing to pspec.dat"); }
    
    // inverse transform:
      // do the idft (inplace)
    FEATUM_DEBUG(10,"before inverse rdft");
    rdft(N, -1, data, obj->ip, obj->w);
    LONG_IDX j; FLOAT_TYPE_FFT *d = data;
    for (; d < data+N ; d++) { // scaling
      *d *= 2.0 / N;
    }
    FEATUM_DEBUG(10,"done with inverse rdft");

    if (lldex->current[level]->cepstrum == NULL) {
       LLDcepstrum_createLLDex( lldex->current[level] );                               
    }
    
    #ifdef DATASAVE_CEPSTRUM
    // save cepstrum data for visualisation and debugging purposes:
    if (!datasave_appendvec_d("xyz.dat",data,N/2)) { FEATUM_ERROR_FATAL(0,"error writing to cepstrum.dat"); }
    #endif
    
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
    FLOAT_TYPE_FFT meanVal = mean_abs_dvfft(data+preskip-1,exlen);
    //printf("maxVal: %f  -meanVal: %f  -maxIdx: %i\n",maxVal,meanVal,maxIdx);
//    lldex->current[level]->cepstrum->f0Index[0] = maxIdx;
    if ((maxVal >= CEPS_F0_THRESH*meanVal)&&(maxIdx > 0)) { // this threshold determines how well seperation between voiced and unvoiced/noise/silence works!
//      lldex->current[level]->cepstrum->F0freq[0] = ( 1.0 / ( (FLOAT_TYPE)(maxIdx+preskip-1) * (FLOAT_TYPE)Tsamp ) ) / (FLOAT_TYPE)CEPS_MAX_PITCH;
       //printf("F0freq ceps: %f (s: %f)\n", ( 1.0 / ( (FLOAT_TYPE)(maxIdx+preskip-1) * (FLOAT_TYPE)Tsamp ) ) / (FLOAT_TYPE)CEPS_MAX_PITCH , (FLOAT_TYPE)(maxVal/meanVal));
//      lldex->current[level]->cepstrum->F0strength[0] = (FLOAT_TYPE)(maxVal-meanVal);
    } else {
//      lldex->current[level]->cepstrum->F0freq[0] = 0.0;           
//      lldex->current[level]->cepstrum->F0strength[0] = (FLOAT_TYPE)(maxVal-meanVal);
    }
    // TODO: find second and third peak(!)  -> smoothing and peak picking, then sorting... complex algorithm!


    // -----
    // now save cepstrum up to numCeps
    if ((obj->numCeps > 0) && (obj->numCeps < N/2))
      lldex->current[level]->cepstrum->numCeps = obj->numCeps;
    else
      lldex->current[level]->cepstrum->numCeps = N/2;
    
    for (i=0; i<lldex->current[level]->cepstrum->numCeps; i++)
      lldex->current[level]->cepstrum->cepstrum[i] = data[i];
      
    free(data);
    
    } // end if(isPowerOf2.......)
    
  }

  _FUNCTION_RETURN_(1);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDcepstrum_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDcepstrum_createLLDex"
{_FUNCTION_ENTER_
  pLLDexCepstrum p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->cepstrum != NULL) LLDcepstrum_freeLLDex(obj);          
    p = (pLLDexCepstrum) calloc(1,sizeof(sLLDexCepstrum));
    if (p != NULL) {
      p->freeLLDex = &LLDcepstrum_freeLLDex;
      if (obj->pcm != NULL) {
        p->cepstrum = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*obj->pcm->nBlocks);
      }
      ret = 1;
    }
    obj->cepstrum = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDcepstrum_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDcepstrum_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->cepstrum != NULL) {
      if (obj->cepstrum->cepstrum != NULL) free(obj->cepstrum->cepstrum); 
      free(obj->cepstrum);  obj->cepstrum = NULL;
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDcepstrum feature extractor object
pLLDcepstrum LLDcepstrum_destroy( pLLDcepstrum obj )
#define FUNCTION "LLDcepstrum_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->ip != NULL) { free(obj->ip); }
    if (obj->w != NULL) { free(obj->w);  }
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



