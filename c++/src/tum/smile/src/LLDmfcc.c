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
 
 /*  IMPLEMENTATION of the feature extractor module LLDmfcc
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDmfcc"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "featum_util.h"       //( optional )
#include "feature-memory.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function

FLOAT_TYPE_FFT melfloor = 1.0; //0.00000001;

// default constructor 
pLLDmfcc LLDmfcc_create( pLLDmfcc obj )
#define FUNCTION "LLDmfcc_create"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    obj = (pLLDmfcc)calloc(1,sizeof(sLLDmfcc));
    if (obj == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!"); 
    }        
    obj->nMel = DEFAULT_nMel;
    obj->nMFCC = DEFAULT_nMFCC;
    obj->cepLifter = DEFAULT_cepLifter;  
    obj->firstMFCC = DEFAULT_firstMFCC;
    obj->usePower = DEFAULT_usePower;
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


int LLDmfcc_configure( pLLDmfcc obj, int nMel, int nMFCC, int cepLifter, int firstMFCC, int usePower )
#define FUNCTION "LLDmfcc_configure"
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
    obj->usePower = usePower;
    _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 



// flushToMem: copy final data from ftex to feature memory element array
int LLDmfcc_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDmfcc_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->mfcc == NULL) _FUNCTION_RETURN_(0);  // extractor failed, do not flush, because there is no data
  if (lldex->current[level]->mfcc->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  

  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_mfcc, 
                                           lldex->current[level]->mfcc->nMFCC, 0, 
                                           lldex->current[level]->timestamp );

  if (values != NULL) {
    // flush the liftered mfcc in lldex->current[level]->mfcc->mfcc
    int i;

    FLOAT_TYPE *mfccarr = lldex->current[level]->mfcc->mfcc;
    for (i=0; i< lldex->current[level]->mfcc->nMFCC; i++) {
      values[i] = (FLOAT_TYPE_FTMEM)(mfccarr[i]);
    }   
   
    lldex->current[level]->mfcc->flush_status = LLDEX_FLUSHED;

    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


/************** mel functions *************************************/

// Hertz to Mel
inline FLOAT_TYPE_FFT Mel(FLOAT_TYPE_FFT fhz) {
      return 1127.0*log(1.0+fhz/700.0);
}

// Mel to Hertz
inline FLOAT_TYPE_FFT Hertz(FLOAT_TYPE_FFT fmel) {
      return 700.0*(exp(fmel/1127.0)-1.0);
}

// convert frequency (hz) to FFT bin number
inline LONG_IDX FtoN(FLOAT_TYPE_FFT fhz, FLOAT_TYPE_FFT baseF)
{
  return (LONG_IDX)round(fhz/baseF);
}

// convert bin number to frequency in Mel
inline FLOAT_TYPE_FFT NtoFmel(LONG_IDX N, FLOAT_TYPE_FFT baseF)
{
  return Mel( (FLOAT_TYPE_FFT)N*baseF );
}


// blocksize is size of fft block, i.e. pcmFrameLength/2
int LLDmfcc_recomputeFilters( pLLDmfcc obj, LONG_IDX blocksize, long int sampleRate )
#define FUNCTION "LLDmfcc_recomputeFilters"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (blocksize < obj->nMel) {
      FEATUM_ERROR(0,"nMel is greater than number of FFT bins! This does not work... not computing mfcc filters! ");
      _FUNCTION_RETURN_(0);
    }
    if (obj->filterCoeffs != NULL) { free(obj->filterCoeffs);   }
    if (obj->chanMap != NULL) { free(obj->chanMap);   }
    if (obj->filterCfs != NULL) { free(obj->filterCfs);   }

    obj->blocksize = blocksize;
    obj->filterCoeffs = (FLOAT_TYPE_FFT*)calloc(1,sizeof(FLOAT_TYPE_FFT) * blocksize);
    obj->chanMap = (LONG_IDX*)malloc(sizeof(LONG_IDX) * blocksize);
    obj->filterCfs = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT) * (obj->nMel+2));

    FLOAT_TYPE_FFT N = (FLOAT_TYPE_FFT) (blocksize*2);
    FLOAT_TYPE_FFT Fs = (FLOAT_TYPE_FFT) (sampleRate);
    FLOAT_TYPE_FFT baseF = Fs/N;
    FLOAT_TYPE_FFT M = (FLOAT_TYPE_FFT) (obj->nMel);
    
    FLOAT_TYPE_FFT LoF_hz = 0.0;
    FLOAT_TYPE_FFT HiF_hz = Fs/2.0; // Hertz(NtoFmel(blocksize+1,baseF));

    FLOAT_TYPE_FFT LoF = Mel(LoF_hz);  // Lo Cutoff Freq (mel)
    FLOAT_TYPE_FFT HiF = Mel(HiF_hz);  // Hi Cutoff Freq (mel)
    LONG_IDX nLoF = FtoN(LoF_hz,baseF);  // Lo Cutoff Freq (fft bin)
    LONG_IDX nHiF = FtoN(HiF_hz,baseF);  // Hi Cutoff Freq (fft bin)

    if (nLoF > blocksize) nLoF = blocksize;
    if (nHiF > blocksize) nHiF = blocksize;
    if (nLoF < 1) nLoF = 1;
    if (nHiF < 1) nHiF = 1;      

    int m,n;
    // compute mel center frequencies
    FLOAT_TYPE_FFT mBandw = (HiF-LoF)/(M+1.0); // half bandwidth of mel bands
    for (m=0; m<=obj->nMel+1; m++) {
	  obj->filterCfs[m] = LoF+(FLOAT_TYPE_FFT)m*mBandw;
    }
    
    // compute channel mapping table:
    m = 0;
    for (n=0; n<blocksize; n++) {
	  if ( (n<nLoF)||(n>nHiF) ) obj->chanMap[n] = -3;
	  else {
  	    while (obj->filterCfs[m] < NtoFmel(n,baseF)) { m++; }
	    obj->chanMap[n] = m-2;
      }
	}
    
    // compute filter weights (falling slope only):
    m = 0;
    FLOAT_TYPE_FFT nM;
    for (n=nLoF;n<nHiF;n++) {
	  nM = NtoFmel(n,baseF);
	  while (nM > obj->filterCfs[m+1] && (m<=obj->nMel)) m++;
	  obj->filterCoeffs[n] = ( obj->filterCfs[m+1] - nM )/(obj->filterCfs[m+1] - obj->filterCfs[m]); 
    }
    
    obj->nLoF = nLoF;
    obj->nHiF = nHiF;
    
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDmfcc_extractor( pLLDmfcc obj, pLLDex lldex, int level )
#define FUNCTION "LLDmfcc_extractor"
{_FUNCTION_ENTER_
  int i,m;
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (lldex->current[level]->fft == NULL) {
      FEATUM_ERROR(9,"fft data in lldex struct is NULL, but required for mfcc!");
      _FUNCTION_RETURN_(0);
    }
    
    LONG_IDX N = lldex->current[level]->fft->nBins;
    
    // initialize filters
    if ((obj->filterCoeffs == NULL)||(N != obj->blocksize)) {
      if (!LLDmfcc_recomputeFilters( obj, N, 
                  lldex->current[level]->fft->fStep * (FLOAT_TYPE)(lldex->current[level]->fft->nBlocks) )) {
        _FUNCTION_RETURN_(0);
      }
    }
     
    if (obj->costable == NULL) {
      int nM = obj->nMel; FLOAT_TYPE fnM = (FLOAT_TYPE)(obj->nMel);
      obj->costable = (FLOAT_TYPE *)malloc(sizeof(FLOAT_TYPE)*nM*obj->nMFCC);
      if (obj->costable == NULL) {
        FEATUM_ERROR(0,"Error allocating memory for costable!");
        _FUNCTION_RETURN_(0);                  
      }
      for (i=obj->firstMFCC; i < obj->firstMFCC+obj->nMFCC; i++) {
          FLOAT_TYPE fi = (FLOAT_TYPE)i;
          for (m=0; m<nM; m++) {
            obj->costable[m+(i-obj->firstMFCC)*nM] = cos(M_PI*fi/fnM * (((FLOAT_TYPE)(m+1)) - 0.5) );                  
          }
      }
    }
    if (obj->sintable == NULL) {
      obj->sintable = (FLOAT_TYPE *)malloc(sizeof(FLOAT_TYPE)*obj->nMFCC);
      if (obj->sintable == NULL) {
        FEATUM_ERROR(0,"Error allocating memory for sintable!");
        _FUNCTION_RETURN_(0);                  
      }
      FLOAT_TYPE L = (FLOAT_TYPE)(obj->cepLifter);
      if (L > 0.0) {
        for (i=obj->firstMFCC; i < obj->firstMFCC+obj->nMFCC; i++) {
          obj->sintable[i-obj->firstMFCC] = (1.0 + L/2.0 * sin(M_PI*((FLOAT_TYPE)i)/L)); 
        }                
      } else {
        for (i=obj->firstMFCC; i < obj->firstMFCC+obj->nMFCC; i++) {
          obj->sintable[i-obj->firstMFCC] = 1.0; 
        }               
      }
    }           
          
    // copy & square the fft magnitude

    FLOAT_TYPE_FFT *mag;
    if (!obj->usePower)
      mag = lldex->current[level]->fft->magnitudes;
    else {
      mag = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT)*N);
      if (mag == NULL) {
        FEATUM_ERROR(0,"Error allocating memory");
        _FUNCTION_RETURN_(0);        
      }
      int n;
      FLOAT_TYPE_FFT *magSrc = lldex->current[level]->fft->magnitudes;
      for (n=0; n<N; n++) {
        mag[n] = magSrc[n]*magSrc[n];
      } 
    }
    
    pLLDexMfcc currentMfcc = lldex->current[level]->mfcc;
    if (currentMfcc == NULL) {
       LLDmfcc_createLLDex( lldex->current[level] );
       currentMfcc = lldex->current[level]->mfcc;
    }
    if (currentMfcc->nMel == 0) {
       currentMfcc->nMel = obj->nMel;
       currentMfcc->nMFCC = obj->nMFCC;
       currentMfcc->melBand = (FLOAT_TYPE *) malloc(sizeof(FLOAT_TYPE)*obj->nMel);
       currentMfcc->mfccUL = (FLOAT_TYPE *) malloc(sizeof(FLOAT_TYPE)*obj->nMFCC);
       currentMfcc->mfcc = (FLOAT_TYPE *) malloc(sizeof(FLOAT_TYPE)*obj->nMFCC);
    }
    
    // do the mel filtering by multiplying with the filters and summing up
    FLOAT_TYPE * outp =  currentMfcc->melBand;
    memzero(outp, lldex->current[level]->mfcc->nMel*sizeof(FLOAT_TYPE));

    FLOAT_TYPE_FFT a;
    int m,n;
    for (n=obj->nLoF; n<obj->nHiF; n++) {
      m = obj->chanMap[n];
      a = mag[n] * obj->filterCoeffs[n]; 
      if (m>-2) {
        if (m>-1) outp[m] += (FLOAT_TYPE)( a );
        if (m < obj->nMel-1) 
            outp[m+1] += (FLOAT_TYPE)( mag[n] - a ); 
      }
    }

    #ifdef DATASAVE_MFCC
    if (!datasave_appendvec_d("melspec.dat",lldex->current[level]->mfcc->melBand,lldex->current[level]->mfcc->nMel)) { FEATUM_ERROR(0,"error writing to mfcc.dat"); }
    #endif    

    for (m=0; m<obj->nMel; m++) {
      FLOAT_TYPE * outc = outp + m; //cache current output  
      if (obj->usePower) {
	    *outc *= 32767.0*32767.0;   
      } else {
        *outc *= 32767.0;
      }
      // the above is for HTK compatibility....
	  // HTK does not scale the input sample values to -1 / +1
      // thus, we must multiply by the max 16bit sample value again.
      
      if (*outc < melfloor) *outc = melfloor;
	  *outc = log(*outc); // compute log mel spectrum
    }

    // done... free used memory if we allocated it
    if ((obj->usePower)&&(mag!=NULL)) free(mag);
    
    // compute dct of mel data & cepstral liftering:
    outp =  currentMfcc->mfccUL;
    FLOAT_TYPE * outpL =  currentMfcc->mfcc;
    FLOAT_TYPE * mel = currentMfcc->melBand;
    FLOAT_TYPE factor = sqrt(2.0/(FLOAT_TYPE)(obj->nMel));
    int nM = obj->nMel; 
    for (i=obj->firstMFCC; i < obj->firstMFCC+obj->nMFCC; i++) {
       FLOAT_TYPE * outc = outp + (i-obj->firstMFCC); *outc = 0.0;
       for (m=0; m<obj->nMel; m++) {
         //*outc +=  mel[m] * cos(M_PI*(FLOAT_TYPE)i/(FLOAT_TYPE)(obj->nMel) * ((FLOAT_TYPE)(m+1) - 0.5) );
         *outc +=  mel[m] * obj->costable[m+(i-obj->firstMFCC)*nM];
       }
       *outc *= factor; 
       // do cepstral liftering:
       //FLOAT_TYPE L = (FLOAT_TYPE)(obj->cepLifter);
       //outpL[i-obj->firstMFCC]  =  *outc * (1.0 + L/2.0 * sin(M_PI*(FLOAT_TYPE)i/L));
       outpL[i-obj->firstMFCC]  =  *outc * obj->sintable[i-obj->firstMFCC];
    }

    #ifdef DATASAVE_MFCC
    if (!datasave_appendvec_d("mfcc.dat",lldex->current[level]->mfcc->mfcc,lldex->current[level]->mfcc->nMFCC)) { FEATUM_ERROR(0,"error writing to mfcc.dat"); }
    #endif    
        
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDmfcc_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDmfcc_createLLDex"
{_FUNCTION_ENTER_
  pLLDexMfcc p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->mfcc != NULL) LLDmfcc_freeLLDex(obj);          
    obj->mfcc = NULL;
    p = (pLLDexMfcc) calloc(1,sizeof(sLLDexMfcc));
    if (p != NULL) {
      p->freeLLDex = &LLDmfcc_freeLLDex;
      ret = 1;
    }
    obj->mfcc = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDmfcc_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDmfcc_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->mfcc != NULL) {
      if (obj->mfcc->melBand != NULL) free(obj->mfcc->melBand);
      if (obj->mfcc->mfccUL != NULL) free(obj->mfcc->mfccUL);
      if (obj->mfcc->mfcc != NULL) free(obj->mfcc->mfcc);
      free(obj->mfcc); 
      obj->mfcc = NULL;
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDmfcc feature extractor object
pLLDmfcc LLDmfcc_destroy( pLLDmfcc obj )
#define FUNCTION "LLDmfcc_destroy"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->filterCoeffs != NULL) { free(obj->filterCoeffs);  }
    if (obj->filterCfs != NULL) { free(obj->filterCfs);  }
    if (obj->chanMap != NULL) { free(obj->chanMap);  }
    if (obj->sintable != NULL) { free(obj->sintable); }
    if (obj->costable != NULL) { free(obj->costable); }
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



