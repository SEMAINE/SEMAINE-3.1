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
 
 /*  IMPLEMENTATION of the feature extractor module LLDspectral
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDspectral"
 
 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function


// default constructor 
pLLDspectral LLDspectral_create( pLLDspectral obj )
#define FUNCTION "LLDspectral_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDspectral)calloc(1,sizeof(sLLDspectral));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }        
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 

/*
int LLDspectral_configure( pLLDspectral obj, int nMel, int nMFCC, int cepLifter, int firstMFCC )
#define FUNCTION "LLDspectral_configure"
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
int LLDspectral_configure( pLLDspectral obj )
#define FUNCTION "LLDspectral_create"
{_FUNCTION_ENTER_
  if (obj != NULL) {
     _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 
*/

// flushToMem: copy final data from ftex to feature memory element array
int LLDspectral_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDspectral_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->spectral == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->spectral->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_spectral, 
                                           FT_LLD_SPECTRAL_nVal, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {
    values[0] = lldex->current[level]->spectral->en0_250;
    values[1] = lldex->current[level]->spectral->en0_650;
    values[2] = lldex->current[level]->spectral->en250_650;
    values[3] = lldex->current[level]->spectral->en1000_4000;
    values[4] = lldex->current[level]->spectral->rollOff10;    
    values[5] = lldex->current[level]->spectral->rollOff25;    
    values[6] = lldex->current[level]->spectral->rollOff50;    
    values[7] = lldex->current[level]->spectral->rollOff75;    
    values[8] = lldex->current[level]->spectral->rollOff90;    
    values[9] = lldex->current[level]->spectral->centroid;
    values[10] = lldex->current[level]->spectral->flux;
    values[11] = lldex->current[level]->spectral->maxPos;                     
    values[12] = lldex->current[level]->spectral->minPos;

    lldex->current[level]->spectral->flush_status = LLDEX_FLUSHED;
    //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)

    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION





// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDspectral_extractor( pLLDspectral obj, pLLDex lldex, int level )
#define FUNCTION "LLDspectral_extractor"
{_FUNCTION_ENTER_
  int i,m;
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (lldex->current[level]->fft == NULL) {
      FEATUM_ERROR(9,"fft data in lldex struct is NULL, but required for spectral!");
      _FUNCTION_RETURN_(0);
    }
    
    LONG_IDX N = lldex->current[level]->fft->nBins;
    pLLDexSpectral currentSpectral = lldex->current[level]->spectral;
    if (currentSpectral == NULL) {
       LLDspectral_createLLDex( lldex->current[level] );
       currentSpectral = lldex->current[level]->spectral;
    } else {
       memzero(currentSpectral, sizeof(sLLDexSpectral));     
    }
     
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

    FLOAT_TYPE_FFT sumA = 0.0, sumB = 0.0;
    FLOAT_TYPE_FFT nA=1.0, nB=1.0;
    
    FLOAT_TYPE normE = 1.0;
    if (lldex->current[level]->energy != NULL) {
        normE = (FLOAT_TYPE)(lldex->current[level]->energy->rmsEnergy);
        normE *= normE;
    }
    if (normE == 0.0) normE = 1.0;
    
    // 0..250:
    FLOAT_TYPE_FFT step = lldex->current[level]->fft->fStep;
    FEATUM_DEBUG(10,"computing energy bands... [step=%f]\n",step);

    FLOAT_TYPE_FFT nf = 0.0; n=1;
    for (nf=step; nf <= 250.0; nf += step) {
      if (n<N) { sumA += mag[n++]; nA += 1.0; }
    }
    // 250+x .. 650:
    for (; nf <= 650.0; nf += step) {
      if (n<N) { sumB += mag[n++]; nB += 1.0; }
    }
    if (nA==0.0) { nA=1.0; }
    if (nB==0.0) { nB=1.0; }
//printf("sumA. %f,  normE: %f, nA: %f\n",sumA,normE,nA);
    currentSpectral->en0_250 = (FLOAT_TYPE)(sumA)/(normE*nA);
    currentSpectral->en0_650 = (FLOAT_TYPE)(sumA+sumB)/(normE*(nA+nB));
    currentSpectral->en250_650 = (FLOAT_TYPE)sumB/(normE*nB);

    // 1000 .. 4000:
    sumA = 0.0; nA=0.0;
    n = (int)round(1000.0/step); if (n==0) n=1;
    for (nf=((FLOAT_TYPE)n)*step; nf <= 4000.0; nf += step) {
      if (n<N) { sumA += mag[n++]; nA += 1.0; }
    }
    if (nA==0.0) { nA=1.0; }
    currentSpectral->en1000_4000 = (FLOAT_TYPE)sumA/(normE*nA);

    FEATUM_DEBUG(10,"computing centroid...\n");
    // centroid
    sumA=0.0; sumB=0.0; nf=0.0;
    for (n=0; n<N; n++) {
      sumA += nf * magSrc[n];
      sumB += magSrc[n];
      nf += step;
    }
    if (sumB != 0.0) 
      currentSpectral->centroid = (FLOAT_TYPE)(sumA/sumB)/NORM_FREQUENCY;
    else 
      currentSpectral->centroid = 0.0;        
      
    FEATUM_DEBUG(10,"computing rollOff...\n");
    //rollOff
    sumA=sumB; sumB=0.0;
//    for (n=1; n<N; n++) {
//      sumA += magSrc[n];
//    }
    currentSpectral->rollOff10 = 0.0;
    currentSpectral->rollOff25 = 0.0;
    currentSpectral->rollOff50 = 0.0;
    currentSpectral->rollOff75 = 0.0;
    currentSpectral->rollOff90 = 0.0;
    for (n=1; n<N; n++) {
      sumB += magSrc[n];
      if ((currentSpectral->rollOff10 == 0.0)&&(sumB >= 0.1*sumA))
        currentSpectral->rollOff10 = ((FLOAT_TYPE)n)*(FLOAT_TYPE)step/NORM_FREQUENCY;
      if ((currentSpectral->rollOff25 == 0.0)&&(sumB >= 0.25*sumA))
        currentSpectral->rollOff25 = ((FLOAT_TYPE)n)*(FLOAT_TYPE)step/NORM_FREQUENCY;
      if ((currentSpectral->rollOff50 == 0.0)&&(sumB >= 0.5*sumA))
        currentSpectral->rollOff50 = ((FLOAT_TYPE)n)*(FLOAT_TYPE)step/NORM_FREQUENCY;
      if ((currentSpectral->rollOff75 == 0.0)&&(sumB >= 0.75*sumA))
        currentSpectral->rollOff75 = ((FLOAT_TYPE)n)*(FLOAT_TYPE)step/NORM_FREQUENCY;
      if ((currentSpectral->rollOff90 == 0.0)&&(sumB >= 0.9*sumA))
        currentSpectral->rollOff90 = ((FLOAT_TYPE)n)*(FLOAT_TYPE)step/NORM_FREQUENCY;
    }

    FEATUM_DEBUG(10,"computing flux...\n");
    // flux  
    pLLDexElement prev = LLDex_getHistory( lldex, level, -1 );
    if (prev != NULL) {
      FLOAT_TYPE_FFT normP=1.0, normC=1.0;
      if (lldex->current[level]->energy != NULL) {
        normC = lldex->current[level]->energy->rmsEnergy;
        if (normC <= 0.0) normC = 1.0;
      }
      FLOAT_TYPE_FFT *magP = prev->fft->magnitudes;
      if (prev->energy != NULL) {
        normP = prev->energy->rmsEnergy;
        if (normP <= 0.0) normP = 1.0;
      }
      sumA = 0.0;
      for (n=1; n<N; n++) {
        sumB = (magSrc[n]/normC - magP[n]/normP);
        sumA += sumB*sumB;
      }
      sumA /= (FLOAT_TYPE_FFT)(N-1);
      currentSpectral->flux = sqrt((FLOAT_TYPE)sumA);
    } else {
      currentSpectral->flux = 0.0;      
    }
    
    // maxPos and minPos:
    sumA=0.0; sumB=0.0;
    int minp =0, maxp = 0;
    for (n=1; n<N; n++) {
      if (magSrc[n] < sumA) { minp = n; sumA = magSrc[n]; }
      if (magSrc[n] > sumB) { maxp = n; sumB = magSrc[n]; }
    }
    currentSpectral->maxPos = (FLOAT_TYPE)maxp / (FLOAT_TYPE)N;
    currentSpectral->minPos = (FLOAT_TYPE)minp / (FLOAT_TYPE)N;

    // done... free used memory
    free(mag);
        
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDspectral_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDspectral_createLLDex"
{_FUNCTION_ENTER_
  pLLDexSpectral p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->spectral != NULL) LLDspectral_freeLLDex(obj);          
    obj->spectral = NULL;
    p = (pLLDexSpectral) calloc(1,sizeof(sLLDexSpectral));
    if (p != NULL) {
      p->freeLLDex = &LLDspectral_freeLLDex;
      ret = 1;
    }
    obj->spectral = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDspectral_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDspectral_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->spectral != NULL) {
      free(obj->spectral);  obj->spectral = NULL;                      
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDspectral feature extractor object
pLLDspectral LLDspectral_destroy( pLLDspectral obj )
#define FUNCTION "LLDspectral_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



