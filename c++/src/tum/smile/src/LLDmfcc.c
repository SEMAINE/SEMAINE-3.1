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
 
 /*  IMPLEMENTATION of the feature extractor module LLDmfcc
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDmfcc"
 
 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
//#include "LLDmfcc.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function

FLOAT_TYPE melfloor = 0.00000001;

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
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


int LLDmfcc_configure( pLLDmfcc obj, int nMel, int nMFCC, int cepLifter, int firstMFCC )
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
    _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 

// custom configuration of extractor parameters
/*  this is only a template function:
int LLDmfcc_configure( pLLDmfcc obj )
#define FUNCTION "LLDmfcc_create"
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
int LLDmfcc_setupFtmem( int level, pFeatureMemory mem )
#define FUNCTION "LLDmfcc_setupNames"
{_FUNCTION_ENTER_
  int ret;
  ret = featureMemory_setupElement( mem, level, FT_LLD_MFCC, FT_LLD_MFCC_nVal, (char **)&lld_mfcc_ftMem_names );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 
*/  

//int featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal )

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
  
/*  FLOAT_TYPE_FTMEM *values = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_mfcc, lldex->current[level]->mfcc->nMFCC, 0 );
  if (values != NULL) {
    // flush the liftered mfcc in lldex->current[level]->mfcc->mfcc
    int i;
    FLOAT_TYPE *mfccarr = lldex->current[level]->mfcc->mfcc;
    for (i=0; i< lldex->current[level]->mfcc->nMFCC; i++) {
      values[i] = (FLOAT_TYPE_FTMEM)(mfccarr[i]);
    }   
   
    lldex->current[level]->mfcc->flush_status = LLDEX_FLUSHED;
    //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)

    _FUNCTION_RETURN_(1);
  }
  */
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
    //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)

    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


/************** mel functions *************************************/

#if 0
void compute_melFilters(s_mel_descriptor *mel)
{
  int m,n;
  if (mel == NULL) return;

  double N = (double) (mel->numFreq);
  double Fs = (double) (mel->sampleRate);
  double M = MEL_BANDS;

  
  double B(double fhz) {
    return 1125.0*log(1.0+fhz/700.0);
  }
  double Binv(double fmel) {
    return 700.0*(exp(fmel/1125.0)-1.0);
  }
  
  double BfI = B(0.0);
  double BfH = B(Fs/2.0-1.0);
  
  int fofm(int m) {
    return ( (int)round( (2.0*N/Fs)*Binv(BfI + (double)m*(BfH-BfI)/((double)M+1.0) )));    
  }
  
         
  #ifdef __DEBUGGING_PRINT_MELf
    printf("MEL fof %i : %i  -- %f\n",0,fofm(0),N);
  #endif
  for (m=1; m<MEL_BANDS+1; m++) {
    #ifdef __DEBUGGING_PRINT_MELf
      printf("MEL fof %i : %i\n",m,fofm(m));
    #endif
    for (n=0; (n<fofm(m-1))&&(n<mel->numFreq); n++) {
      mel->mel_filters[m-1][n] = 0.0;
    }  
    for (n=fofm(m-1); (n<fofm(m))&&(n<mel->numFreq); n++) {
      mel->mel_filters[m-1][n] = ( 2.0*(double)(n-fofm(m-1)) )/(double)( (fofm(m+1)-fofm(m-1))*(fofm(m)-fofm(m-1)) ); 
    }
    for (n=fofm(m); (n<=fofm(m+1))&&(n<mel->numFreq); n++) {
      mel->mel_filters[m-1][n] = ( 2.0*(double)(fofm(m+1)-n) )/(double)( (fofm(m+1)-fofm(m-1))*(fofm(m+1)-fofm(m)) ); 
    }
    for (n=fofm(m+1)+1; n<mel->numFreq; n++) {
      mel->mel_filters[m-1][n] = 0.0;
    }
  }
}


#endif

// blocksize is size of fft block, i.e. pcmFrameLength/2
int LLDmfcc_recomputeFilters( pLLDmfcc obj, int blocksize, long int sampleRate )
#define FUNCTION "LLDmfcc_recomputeFilters"
{_FUNCTION_ENTER_
    double B(double fhz) {
//      return 1125.0*log(1.0+fhz/700.0);
      return 2595.0*log10(1.0+fhz/700.0);
    }
    double Binv(double fmel) {
//      return 700.0*(exp(fmel/1125.0)-1.0);
//      return 700.0*(exp10(fmel/2595.0)-1.0);
      return 700.0*(pow(10.0,fmel/2595.0)-1.0);

    }


  if (obj != NULL) {
    if (blocksize < obj->nMel) {
      FEATUM_ERROR(0,"nMel is greater than number of FFT bins! This does not work... not computing mfcc filters! ");
      _FUNCTION_RETURN_(0);
    }
    if (obj->filters != NULL) { free(obj->filters);   }
    if (obj->filters_start != NULL) { free(obj->filters_start);   }
    if (obj->filters_end != NULL) { free(obj->filters_end);   }

    obj->blocksize = blocksize;
    obj->filters = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE) * obj->nMel * blocksize);
    obj->filters_start = (LONG_IDX*)malloc(sizeof(LONG_IDX) * obj->nMel);
    obj->filters_end = (LONG_IDX*)malloc(sizeof(LONG_IDX) * obj->nMel);

    double N = (double) (blocksize);
    double Fs = (double) (sampleRate);
    double M = (double) (obj->nMel);
    double BfI = B(0.0);
    double BfH = B(Fs/2.0-1.0);
    int fofm(int m) {
      return ( (int)round( (2.0*N/Fs)*Binv(BfI + (double)m*(BfH-BfI)/((double)M+1.0) )));    
    }
  
      
    int m,n;
    for (m=1; m<obj->nMel+1; m++) {
      FLOAT_TYPE_FFT *filter = obj->filters + ((m-1)*blocksize);
      //    printf("MEL fof %i : %i\n",m,fofm(m));
      
      int fofm_mm1 = obj->filters_start[m-1] = fofm(m-1);
      int fofm_mp1 = obj->filters_end[m-1] = fofm(m+1);
      int fofm_m = fofm(m);
      if (obj->filters_end[m-1] >= obj->blocksize) obj->filters_end[m-1] = obj->blocksize;

      for (n=0; (n<fofm_mm1)&&(n<blocksize); n++) {
        filter[n] = 0.0;
      }  
      for (n=fofm_mm1; (n<fofm_m)&&(n<obj->blocksize); n++) {
        filter[n] = ( 2.0*(double)(n-fofm_mm1) )/(double)( (fofm_mp1-fofm_mm1) ); 
//        filter[n] = ( 2.0*(double)(n-fofm_mm1) )/(double)( (fofm_mp1-fofm_mm1)*(fofm_m-fofm_mm1) ); 
      }
      for (n=fofm_m; (n<=fofm_mp1)&&(n<obj->blocksize); n++) {
        filter[n] = ( (double)(fofm_mp1-n) )/(double)( (fofm_mp1-fofm_m) ); 
//        filter[n] = ( 2.0*(double)(fofm_mp1-n) )/(double)( (fofm_mp1-fofm_mm1)*(fofm_mp1-fofm_m) ); 
      }
      for (n=fofm_mp1+1; n<obj->blocksize; n++) {
        filter[n] = 0.0;
      }
    }
    
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
    if ((obj->filters == NULL)||(N != obj->blocksize)) {
      if (!LLDmfcc_recomputeFilters( obj, N, 
                  lldex->current[level]->fft->fStep * (FLOAT_TYPE)(lldex->current[level]->fft->nBlocks) )) {
        //LLDmfcc_freeLLDex( lldex->current[level] ); 
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
    FLOAT_TYPE_FFT *magSrc = lldex->current[level]->fft->magnitudes;
    if (magSrc == NULL) _FUNCTION_RETURN_(0);
    FLOAT_TYPE_FFT *mag = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT)*N);
    if (mag == NULL) {
      FEATUM_ERROR(0,"Error allocating memory");
      _FUNCTION_RETURN_(0);        
    }
    int n;
    for (n=0; n<N; n++) {
      mag[n] = magSrc[n]; //*magSrc[n];
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
    //memzero(outp, lldex->current[level]->mfcc->nMel*sizeof(FLOAT_TYPE));
    for (m=0; m<obj->nMel; m++) {
      FLOAT_TYPE * outc = outp + m; //current output  
      FLOAT_TYPE_FFT * filter = obj->filters + (m*obj->blocksize);
      *outc = 0.0;
      for (n=obj->filters_start[m]; n<obj->filters_end[m]; n++) {
        *outc += (FLOAT_TYPE)( mag[n] * filter[n] );
      }
      if (*outc < 0.0000001) *outc = 0.0000001;
      // TODO: MelSpec output at this point!
    #ifdef DATASAVE_MFCC
    }
    if (!datasave_appendvec_d("melspec.dat",lldex->current[level]->mfcc->melBand,lldex->current[level]->mfcc->nMel)) { FEATUM_ERROR(0,"error writing to mfcc.dat"); }

    for (m=0; m<obj->nMel; m++) {
      FLOAT_TYPE * outc = outp + m; //current output  
      if (*outc < melfloor) *outc = melfloor;
      *outc = log(*outc);
    }
    #else
      //if ((*outc > -0.00001)&&(*outc <= 0.0)) *outc = -0.00001;
      *outc = log(*outc);
      //*outc /= (FLOAT_TYPE)(obj->blocksize);
    }
    #endif    
      
    
    // done... free used memory
    free(mag);
    
    // compute dct of mel data:
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
//      printf(" ..> melBand = %i\n",(long)obj->mfcc->melBand);
      if (obj->mfcc->melBand != NULL) free(obj->mfcc->melBand);
//      printf(" ..> mfccUL = %i\n",(long)obj->mfcc->mfccUL);
      if (obj->mfcc->mfccUL != NULL) free(obj->mfcc->mfccUL);
//      printf(" ..> mfcc = %i\n",(long)obj->mfcc->mfcc);
      if (obj->mfcc->mfcc != NULL) free(obj->mfcc->mfcc);
//      printf("here...\n"); fflush(stdout);
      free(obj->mfcc); 
//      printf("here2...\n"); fflush(stdout);
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
    if (obj->ip != NULL) { free(obj->ip); }
    if (obj->w != NULL) { free(obj->w);  }
    if (obj->filters != NULL) { free(obj->filters);  }
    if (obj->filters_start != NULL) { free(obj->filters_start);  }
    if (obj->filters_end != NULL) { free(obj->filters_end);  }
    if (obj->sintable != NULL) { free(obj->sintable); }
    if (obj->costable != NULL) { free(obj->costable); }
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



