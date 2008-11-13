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
 
 /*  IMPLEMENTATION of the feature extractor module LLDpitch
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDpitch"
 
 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
//#include "LLDpitch.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function


// default constructor 
pLLDpitch LLDpitch_create( pLLDpitch obj )
#define FUNCTION "LLDpitch_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDpitch)calloc(1,sizeof(sLLDpitch));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }          
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


// custom configuration of extractor parameters
/*  this is only a template function:
int LLDpitch_configure( pLLDpitch obj )
#define FUNCTION "LLDpitch_create"
{_FUNCTION_ENTER_
  if (obj != NULL) {
     _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 
*/


// setup names in feature memory object
/*
int LLDpitch_setupFtmem( int level, pFeatureMemory mem )
#define FUNCTION "LLDpitch_setupNames"
{_FUNCTION_ENTER_
  int ret;
  ret = featureMemory_setupElement( mem, level, FT_LLD_PITCH, FT_LLD_PITCH_nVal, (char **)&lld_pitch_ftMem_names );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 
*/  

//int featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal )


// flushToMem: copy final data from ftex to feature memory element array
int LLDpitch_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDpitch_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->pitch == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->pitch->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_pitch, 
                                           FT_LLD_PITCH_nVal, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {

   values[0] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->pitch->F0freq; 
   values[1] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->pitch->F0strength;
   values[2] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->pitch->F0quality;

   lldex->current[level]->pitch->flush_status = LLDEX_FLUSHED;
   //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)

    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

// find the first significant peak when searching from right to left
inline FLOAT_TYPE_FFT pitchPeak(FLOAT_TYPE_FFT *a, LONG_IDX n, LONG_IDX *idx)
{
  FLOAT_TYPE_FFT max;
  FLOAT_TYPE_FFT sum = 0.0;
  LONG_IDX i = 0;

  // first pass: compute mean and find absolute max
  max = a[n-1];
  for (i=n-1; i>=0; i--) {
    sum += abs(a[i]);
    if (a[i] > max) max = a[i];
  }
  sum /= (FLOAT_TYPE_FFT) n;

  // second pass: peak detection
  for (i=0; i<n-1; i++) {
    if (a[i] > (max+sum)/2.0) {
      if ( (a[i-1] < a[i]) && (a[i] > a[i+1]) ) { // we found a peak!
        if (idx != NULL) *idx = i;
        return a[i];
      }
    }
  }
  
  if (idx != NULL) *idx = 0;
  return 0.0;  
}

// find the first significant peak when searching from right to left
inline FLOAT_TYPE_FFT voicingProb(FLOAT_TYPE_FFT *a, LONG_IDX n, LONG_IDX skip, FLOAT_TYPE_FFT *Zcr)
{
  LONG_IDX i, zcr=0, mcr=0;
  FLOAT_TYPE_FFT mean=0.0;
  FLOAT_TYPE_FFT max;
  
  n /= 2;
  // zero crossings and max
  max = a[n-1];
  mean = a[skip];
  for (i=1; i<n; i++) {
    if ( a[i-1]*a[i] < 0 ) zcr++;
    if (i>=skip) {
      if ((a[i] > max) && (a[i-1] < a[i])) max = a[i];
      mean += a[i];
    }
  }
  mean /= (FLOAT_TYPE_FFT)(n-skip+1);

  for (i=1; i<n; i++) {
    if ( (a[i-1]-mean)*(a[i]-mean) < 0 ) mcr++;
  }

  if (Zcr != NULL) {
    if (mcr > zcr) {
      *Zcr = (FLOAT_TYPE_FFT)mcr / (FLOAT_TYPE_FFT)n;
    } else {
      *Zcr = (FLOAT_TYPE_FFT)zcr / (FLOAT_TYPE_FFT)n;
    }
  }

  if (a[0] > 0) 
    return max/a[0];  
  else 
    return 0.0;
}

#if 0
// advanced resonator method .... takes full acf output buffer as argument
inline FLOAT_TYPE_FFT pitchPeak2(FLOAT_TYPE_FFT *a, LONG_IDX n, LONG_IDX *idx, LONG_IDX minPeriod)
#define FUNCTION "pitchPeak2"
{
       // maxPeriod is n/2;
  
  FLOAT_TYPE_FFT max = 0.0;
  LONG_IDX maxPos = 0;
//  FLOAT_TYPE_FFT sum = 0.0;

  FLOAT_TYPE_FFT *resonances = NULL;
  LONG_IDX i, j, N;
  
  resonances = (FLOAT_TYPE_FFT*) calloc(1,sizeof(FLOAT_TYPE_FFT) * (n/2 - minPeriod));
  
  for (j=minPeriod; j<n/2; j++) {
    N=0; resonances[j-minPeriod] = 0.0;
    for (i=j; i<n/2; i += j) {
      resonances[j-minPeriod] += a[i];
      N++;
    }
    if (N>0)
      resonances[j-minPeriod] /= (FLOAT_TYPE_FFT)N;
    else
      resonances[j-minPeriod] = 0.0;
    
    if (resonances[j-minPeriod] > max) {
      max = resonances[j-minPeriod];
      maxPos = j;
    }
  }
  
  if (!datasave_appendvec_d("acfres.dat",resonances,n/2-minPeriod)) { FEATUM_ERROR(0,"error writing to acfres.dat"); }
  free(resonances);
  
  if (idx != NULL) *idx = maxPos;
  return max;
 /* 
  // first pass: compute mean and find absolute max
  max = a[n-1];
  for (i=n-1; i>=0; i--) {
    sum += a[i];
    if (a[i] > max) max = a[i];
  }
  sum /= (FLOAT_TYPE_FFT) n;

  // second pass: peak detection
  for (i=n-2; i>0; i--) {
    if (a[i] > (max+sum)/2.0) {
      if ( (a[i-1] < a[i]) && (a[i] > a[i+1]) ) { // we found a peak!
        if (idx != NULL) *idx = i;
        return a[i];
      }
    }
  }
  
  if (idx != NULL) *idx = 0;
  */
  return 0.0;  
}
#undef FUNCTION
#endif

// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDpitch_extractor( pLLDpitch obj, pLLDex lldex, int level )
#define FUNCTION "LLDpitch_extractor"
{_FUNCTION_ENTER_
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (lldex->current[level]->fft == NULL) {
      FEATUM_ERROR(9,"fft data in lldex struct is NULL, but required for pitch!");
      _FUNCTION_RETURN_(0);
    }



    // algorithm:
    /* use fft data to perform acf. in frequency domain

    */

    FLOAT_TYPE energy = lldex->current[level]->energy->rmsEnergy;
    
    LONG_IDX N = lldex->current[level]->fft->nBlocks;
    if (isPowerOf2(N) && (N>=4) ) {  // only continue if size is power of 2 and greater or equal 4
    
    FLOAT_TYPE_FFT * data = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*N);
    if (data == NULL) {
      FEATUM_ERROR(0,"Error allocating memory");         
      _FUNCTION_RETURN_(0);
    }
    memcpy(data,lldex->current[level]->fft->complex,N*sizeof(FLOAT_TYPE_FFT));
    
    
    FLOAT_TYPE_FFT *data2 = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*N);
    if (data2 == NULL) {
      free(data);
      FEATUM_ERROR(0,"Error allocating memory");
      _FUNCTION_RETURN_(0);
    }

    // square the spectrum:
    LONG_IDX i; 
    FLOAT_TYPE_FFT *tmp = data;
    FLOAT_TYPE_FFT *tmp2 = data2;
    /*  C = AxB : rectangular multiplication in the complex domain
      Re(C) = Re(A)*Re(B) - Im(A)*Im(B)
      Im(C) = Im(A)*Re(B) + Re(A)*Im(B)
    */
    if (*tmp != 0.0)
      *tmp2 = log(fabs(*tmp));
    else 
      *tmp2 = 0.0;
    *tmp = (*tmp) * (*tmp);   // 0th coefficient is real only

    if (*(tmp+1) != 0.0)
      *(tmp2+1) = log(fabs(*(tmp+1)));
    else
      *(tmp2+1) = 0.0;
    *(tmp+1) = *(tmp+1) * *(tmp+1);   // nyquist frequency is also real only

    tmp += 2; tmp2 += 2;
    FLOAT_TYPE_FFT magSq;
    for (i=2; i<N; i += 2) {
      magSq = (*tmp)*(*tmp) + (*(tmp+1))*(*(tmp+1));
      if (magSq != 0.0) 
        *tmp2 = log(magSq);
      else
        *tmp2 = 0.0;
      *(tmp2+1) = 0.0; //phase

      *tmp = magSq;
      *(tmp+1) = 0.0;

      tmp += 2; tmp2 += 2;
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
        free(data); free(data2);
        FEATUM_ERROR(0,"Error allocating memory");
        _FUNCTION_RETURN_(0);
      }
    }
    
    // save powerspec data for visualisation and debugging purposes:
    //if (!datasave_appendvec_d("pspec.dat",data,N)) { FEATUM_ERROR(0,"error writing to pspec.dat"); }
    
    // inverse transform:
      // do the idfts (inplace)
    FEATUM_DEBUG(10,"before inverse rdft(s)");
    rdft(N, -1, data, obj->ip, obj->w);
    rdft(N, -1, data2, obj->ip, obj->w);
    LONG_IDX j; 
    FLOAT_TYPE_FFT *d = data;
    FLOAT_TYPE_FFT *d2 = data2;
    for (; d < data+N ; d++) { // scaling
      *d2 *= 2.0 / N;
      *d *= 2.0 / N;
      d2++;
    }
    FEATUM_DEBUG(10,"done with inverse rdft(s)");

    if (lldex->current[level]->pitch == NULL) {
       LLDpitch_createLLDex( lldex->current[level] );                               
    }
    
    // peak picking:
    LONG_IDX maxIdx;
    FLOAT_TYPE_FFT maxVal;
    FLOAT_TYPE_FFT voicing;
    FLOAT_TYPE_FFT acfZcr = 0.0;
    
    double Tsamp = 1.0/(double)(lldex->current[level]->pcm->sampleRate);  // sampling period
    double maxPitch = MAX_PITCH;  
    // minPitch and pitchResolution are limited by the framesize
    int preskip = (int)(1.0 / (maxPitch*Tsamp));  // this variable determines the maximal pitch detectable
    FEATUM_DEBUG(10,"preskip = %i",preskip);
    int halflen = N/2-preskip+1;
    

    #ifdef DATASAVE_F0ACF
    // save acf data for visualisation and debugging purposes:
    if (!datasave_appendvec_d("acf.dat",data,N/2)) { FEATUM_ERROR(0,"error writing to acf.dat"); }
    if (!datasave_appendvec_d("acflog.dat",data2+preskip-1,halflen)) { FEATUM_ERROR(0,"error writing to acflog.dat"); }
    #endif

    // detect voicing prob. and zero crossing rate (clarity) from normal acf:
    voicing = voicingProb(data,N,preskip,&acfZcr);
    // find pitch peak in log acf (cepstrum):
    maxVal = pitchPeak(data2+preskip-1,halflen,&maxIdx);

    FLOAT_TYPE f0Hypo = ( 1.0 / ( (FLOAT_TYPE)(maxIdx+preskip-1) * (FLOAT_TYPE)Tsamp ) ) / (FLOAT_TYPE)MAX_PITCH;
//    if (voicing*(1.0-acfZcr) > 0.5) { // this threshold determines how well seperation between voiced and unvoiced/noise/silence works!
    if (voicing > 0.5) { // this threshold determines how well seperation between voiced and unvoiced/noise/silence works!
      if (maxIdx > 0) {
        lldex->current[level]->pitch->F0freq = ( 1.0 / ( (FLOAT_TYPE)(maxIdx+preskip-1) * (FLOAT_TYPE)Tsamp ) ) / (FLOAT_TYPE)MAX_PITCH;
      }
      lldex->current[level]->pitch->F0strength = (FLOAT_TYPE)(voicing);
      lldex->current[level]->pitch->F0quality = (FLOAT_TYPE)(1.0-acfZcr);
    } else {
      lldex->current[level]->pitch->F0freq = 0.0;           
      lldex->current[level]->pitch->F0strength = (FLOAT_TYPE)(voicing);
      lldex->current[level]->pitch->F0quality = (FLOAT_TYPE)(1.0-acfZcr);
    }

    // compute smoothed F0 over last frames....
    //    LLDex_getHistory( pLLDex obj, int id, int nBack );

    FEATUM_DEBUG(7,"pitch F0freq = %f  (strength = %f)",lldex->current[level]->pitch->F0freq, lldex->current[level]->pitch->F0strength);

    free(data);
    free(data2);
    } // end if(isPowerOf2.......)
    
  }

  _FUNCTION_RETURN_(1);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDpitch_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDpitch_createLLDex"
{_FUNCTION_ENTER_
  pLLDexPitch p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->pitch != NULL) LLDpitch_freeLLDex(obj);          
    p = (pLLDexPitch) calloc(1,sizeof(sLLDexPitch));
    if (p != NULL) {
      p->freeLLDex = &LLDpitch_freeLLDex;
      ret = 1;
    }
    obj->pitch = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDpitch_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDpitch_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->pitch != NULL) {
      free(obj->pitch);  obj->pitch = NULL;
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDpitch feature extractor object
pLLDpitch LLDpitch_destroy( pLLDpitch obj )
#define FUNCTION "LLDpitch_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->ip != NULL) { free(obj->ip); }
    if (obj->w != NULL) { free(obj->w);  }
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



