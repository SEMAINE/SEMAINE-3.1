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
#define MODULE "pcmProcess"
#include "featum_common.h"

 // Functions for simple processing and analysis of pcmBuffers
 // see pcm-process.h for more detailed documentation
 
 // NOTE: all the functions in this module only support MEMORGA_INTERLV
 //           (except for the windowing functions)
 // all functions only support sample type WORK_SAMPLE (which can be defined)
 
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI  3.141592653589793
#endif
#include <stdlib.h>
#include <string.h>

#include "featum_util.h"       //( optional )
#include "pcm-process.h"


#undef FUNCTION

// NOTE on windowing functions:
// In order to use a different window function on a pcmBuffer where previously 
// another window function has been applied to, you must free the corresponding
// uData entry containing the coefficients so that they will be recomputed for
// the new window function. 
// use pcmBuffer_uDataFree(pPcmBuffer b, int entryNo) for this purpose

// set up window coefficients for pcmBuffer b
// set recomp = 1 to force recomputing coefficients, even if they have been allocated already
//  (e.g. when chaning the type of the windowing function
void pcmProcess_windowingSetup( pPcmBuffer b, int recomp, int type ) 
#define FUNCTION "pcmProcess_windowingSetup"
{_FUNCTION_ENTER_

  int i;
  if (b == NULL) _FUNCTION_RETURNV_;
  #ifdef COMPILE_CHECKS
  if (b->nBlocks <= 0) _FUNCTION_RETURNV_;
  #endif
  if (pcmBuffer_uDataAllocf(b, uDataWINDOW, sizeof(FLOAT_TYPE)*b->nBlocksAlloc+sizeof(LONG_IDX)) ) {
    ((LONG_IDX *)(b->uData->entry[uDataWINDOW]))[0] = b->nBlocks; recomp = 1;
  } else {
     if ( ((LONG_IDX *)(b->uData->entry[uDataWINDOW]))[0] != b->nBlocks ) { 
       ((LONG_IDX *)(b->uData->entry[uDataWINDOW]))[0] = b->nBlocks; recomp = 1;   
     }
  }  
  if (recomp) {
    FLOAT_TYPE *coeffs = (FLOAT_TYPE *)(b->uData->entry[uDataWINDOW]+sizeof(LONG_IDX));
    FLOAT_TYPE N = (FLOAT_TYPE) (b->nBlocks-1);
    FLOAT_TYPE n;
    switch (type) {
      case WIN_HAMMING:
        for (n=0.0; n <= N; n += 1.0) {
//          *coeffs = 0.53836 - 0.46164 * cos( 2.0 * M_PI * (FLOAT_TYPE)n / N );
          *coeffs = 0.54 - 0.46 * cos( 2.0 * M_PI * (FLOAT_TYPE)n / N );
          coeffs++;
        }
        break;
      case WIN_HANNING:
        for (n=0.0; n <= N; n += 1.0) {
          *coeffs = 0.5 * (1.0 - cos( 2 * M_PI * (FLOAT_TYPE)n / N ) );
          coeffs++;
        }
        break;
      case WIN_NONE:
        for (i=0; i < b->nBlocks; i++) {
          *coeffs = 1.0; coeffs++;
        }      
          
      default:
       FEATUM_ERROR(1,"unknown window type: %i",type);
    }     
  }
  
  _FUNCTION_RETURNV_;
}
#undef FUNCTION

// apply a windowing function to pcmBuffer b
// the function coefficients must be set up first by calling pcmProcess_<name>WinSetup
void pcmProcess_doWindowing( pPcmBuffer b ) 
#define FUNCTION "pcmProcess_doWindowing"
{_FUNCTION_ENTER_

  LONG_IDX n; int c;

  if ((b->uData == NULL) || (b->uData->entry[uDataWINDOW] == NULL) ) {
    FEATUM_ERROR(6,"windowing coefficients have not been initialised !!\n   use pcmProcess_windowingSetup first!!");
    _FUNCTION_RETURNV_;
  }
  #ifdef COMPILE_CHECKS
  if (b->sampleType != WORK_SAMPLETYPE) {
    FEATUM_ERROR(1,"b->sampleType (%i) != WORK_SAMPLETYPE (%i) !!",b->sampleType,WORK_SAMPLETYPE);
    _FUNCTION_RETURNV_;             
  }
  #endif
  
  { // works for both memOrgas :)
    void *d; void *D;
    FLOAT_TYPE *w;
    for (c=0; c < b->nChan; c++) {
      d = (void *)b->data + c*b->chanSize;
      D = d + b->nBlocks * b->blockSize;
      w = (FLOAT_TYPE *)(b->uData->entry[uDataWINDOW]+sizeof(LONG_IDX));
      for (; d < D; d += b->blockSize )
      {
        *(WORK_SAMPLE *)(d) = (WORK_SAMPLE)( (FLOAT_TYPE)(*(WORK_SAMPLE *)d) * (*w) );
        w++;
      }
    }  
  }  
     
  _FUNCTION_RETURNV_;
}
#undef FUNCTION

// apply a simple first order pre emphasis filter to the sample data
void pcmProcess_preEmphasis( pPcmBuffer b, FLOAT_TYPE preEmphasis, WORK_SAMPLE *status ) 
#define FUNCTION "pcmProcess_preEmphasis"
{_FUNCTION_ENTER_

  LONG_IDX n; int c;

  #ifdef COMPILE_CHECKS
  if (b->sampleType != WORK_SAMPLETYPE) {
    FEATUM_ERROR(1,"b->sampleType (%i) != WORK_SAMPLETYPE (%i) !!",b->sampleType,WORK_SAMPLETYPE);
    _FUNCTION_RETURNV_;             
  }
  #endif
  
  { // works for both memOrgas :)
    FEATUM_DEBUG(9,"Doing preEmphasis");
    void *dcur; void *dlast; void *D;
    for (c=0; c < b->nChan; c++) {
      dcur = (void *)b->data + c*b->chanSize;
      D = dcur + b->nBlocks * b->blockSize;
      
      // first sample is special
      WORK_SAMPLE last = *(WORK_SAMPLE *)(dcur);
//      *(WORK_SAMPLE *)(dcur) -= (WORK_SAMPLE)( (FLOAT_TYPE)(status[c]) * preEmphasis );
//      dcur += b->blockSize;
      for (; dcur < D; dcur += b->blockSize )
      {
        WORK_SAMPLE cur = *(WORK_SAMPLE *)(dcur);
        *(WORK_SAMPLE *)(dcur) -= (WORK_SAMPLE)( (FLOAT_TYPE)(last) * preEmphasis );
        last = cur;
      }
//      status[c] = last;
    }  
  }  
     
  _FUNCTION_RETURNV_;
}
#undef FUNCTION

//double maxi = 0.0;

// compute normalised rms energy
FLOAT_TYPE pcmProcess_energy( pPcmBuffer b ) 
#define FUNCTION "pcmProcess_energy"
{_FUNCTION_ENTER_

  LONG_IDX i; double ret = 0;
  // TODO: check for desired format, memorga, channels, bps, etc. 
  // if not supported, then use copy_buffer to convert
  FEATUM_DEBUG(11,"b->nBlocks = %i",b->nBlocks);
  
  if (b != NULL) {
    if (b->nBlocks == 0) _FUNCTION_RETURN_( 0.0 );
    double sum = 0; //FLOAT_TYPE max = (FLOAT_TYPE)((WORK_SAMPLE *)b->data)[0];
    for (i=0; i < b->nBlocks; i++) {
      sum +=  (double)((SAMPLEFP *)b->data)[i] * (double)((SAMPLEFP *)b->data)[i];
//      sum +=  (FLOAT_TYPE)((WORK_SAMPLE *)b->data)[i] * (FLOAT_TYPE)((WORK_SAMPLE *)b->data)[i];
//      if ((FLOAT_TYPE)((WORK_SAMPLE *)b->data)[i] > max) max = (FLOAT_TYPE)((WORK_SAMPLE *)b->data)[i];
    }
    ret = sqrt( (FLOAT_TYPE)(sum) / (FLOAT_TYPE)b->nBlocks );
    if (ret == 0.0) {
       FEATUM_DEBUG(1,"0.0 output : nBlocks = %i",b->nBlocks);
    }
    _FUNCTION_RETURN_( ret );
  } else {
    FEATUM_DEBUG(1,"NULL input");
    _FUNCTION_RETURN_( 0.0 ); 
  }
}
#undef FUNCTION


double pcmProcess_logEnergy( pPcmBuffer p )
#define FUNCTION "pcmProcess_logEnergy"
{
       
}
#undef FUNCTION


#ifdef USE_SLOW_DFT
// sign: 1 forward, -1 backward
void slow_rdft(int N, int sign, FLOAT_TYPE_FFT *input_pcm, FLOAT_TYPE_FFT *ip, FLOAT_TYPE_FFT *w){
#define FUNCTION "slow_rdft"
{_FUNCTION_ENTER_
  if (*ip == -99.0) { // create cos and sin tables
  
  }
  int n,k;
  
  /*
  <case1> RDFT
            R[k] = sum_j=0^n-1 a[j]*cos(2*pi*j*k/n), 0<=k<=n/2
            I[k] = sum_j=0^n-1 a[j]*sin(2*pi*j*k/n), 0<k<n/2
        <case2> IRDFT (excluding scale)
            a[k] = (R[0] + R[n/2]*cos(pi*k))/2 + 
                   sum_j=1^n/2-1 R[j]*cos(2*pi*j*k/n) + 
                   sum_j=1^n/2-1 I[j]*sin(2*pi*j*k/n), 0<=k<n
                   */
  
  // compute the dft  O(N^2)                 
  FLOAT_TYPE_FFT *out = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT)*N);
  if (sign == 1) {  // forward transform  (real to complex)

    // 0-th coefficient
    *out = 0.0;
    for (k=0; k<N; k++) {
      *out += input_pcm[k];
    }   
  
    for (n=1; n<(N/2)-1; n++) {
      FLOAT_TYPE_FFT *re = out+(n*2);
      FLOAT_TYPE_FFT *im = out+(n*2)+1;
      *re = 0.0; *im = 0.0;
      for (k=0; k<N; k++) {
        *re += input_pcm[k] * cos(M_PI*2.0*(FLOAT_TYPE_FFT)n*(FLOAT_TYPE_FFT)k/(FLOAT_TYPE_FFT)N);
        *im += input_pcm[k] * (-1.0) sin(M_PI*2.0*(FLOAT_TYPE_FFT)n*(FLOAT_TYPE_FFT)k/(FLOAT_TYPE_FFT)N);
      }   
    }         
    // nyquist coefficient (only real output)
    FLOAT_TYPE_FFT *re = out+(N/2);
    *re = 0.0;
    for (k=0; k<N; k++) {
      *re += input_pcm[k] * ();
    }   
    
           
  } else {
         
         
  }

  memcpy
  
  _FUNCTION_RETURNV_;  
}
#undef FUNCTION
#endif


// the array complex_interleaved must be at least p->nBlocks elements in size!
int pcmProcess_fft( pPcmBuffer pcm, FLOAT_TYPE_FFT *complex_interleaved, int *ip, FLOAT_TYPE_FFT *w )
#define FUNCTION "pcmProcess_fft"
{_FUNCTION_ENTER_
  LONG_IDX i;
  if (pcm != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (complex_interleaved == NULL) { FEATUM_ERROR(0,"complex_interleaved == NULL!"); _FUNCTION_RETURN_(0);  }
    if (ip == NULL) { FEATUM_ERROR(0,"int *ip == NULL!"); _FUNCTION_RETURN_(0);  }
    if (w == NULL) { FEATUM_ERROR(0,"double *w == NULL!"); _FUNCTION_RETURN_(0);  }
    if (pcm->nChan > 1) {
      FEATUM_ERROR(0,"more than 1 channel not yet supported for FFT ! aborting fft computation!");
      _FUNCTION_RETURN_(0);
    }
    #endif

    FLOAT_TYPE_FFT *input_pcm = complex_interleaved;
    FLOAT_TYPE_FFT * out = input_pcm;
    WORK_SAMPLE * in = pcm->data;
    for (i=0; i<pcm->nBlocks; i++) {
        *out = (FLOAT_TYPE_FFT)(*in);
        out++; in++;
    }

    // do the dft (inplace)
    FEATUM_DEBUG(10,"before rdft");
    rdft(pcm->nBlocks, 1, input_pcm, ip, w);
    FEATUM_DEBUG(10,"done with rdft");
  
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);  
    
}
#undef FUNCTION

// magnitudes and phases must be size pcm->nBlocks/2 
int pcmProcess_fft_magphase( pPcmBuffer pcm, FLOAT_TYPE_FFT *magnitudes, FLOAT_TYPE_FFT * phases, int *ip, FLOAT_TYPE_FFT *w )
#define FUNCTION "pcmProcess_fft_magphase"
{_FUNCTION_ENTER_
  LONG_IDX i;
  if (pcm != NULL) {

    FLOAT_TYPE_FFT *input_pcm;    
    input_pcm = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*pcm->nBlocks);
    if (input_pcm == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!");
      _FUNCTION_RETURN_(0);
    }
    
    
    if (pcmProcess_fft( pcm, input_pcm, ip, w )) {
      // save output to lldex (mag + phase..)
      // output is stored in input_pcm array by the rdft function
      if (magnitudes != NULL) {
        for (i=2; (i < (pcm->nBlocks-1))&&(i<pcm->nBlocksAlloc-1); i += 2) {
           magnitudes[i/2] = sqrt(input_pcm[i]*input_pcm[i] + input_pcm[i+1]*input_pcm[i+1]);
        }
        magnitudes[0] = input_pcm[0];
        //magnitudes[(pcm->nBlocks/2)-1] = input_pcm[1];
      }
    
      if (phases != NULL) {
                
        for (i=2; i < (pcm->nBlocks); i += 2) {
           phases[i/2] = atan(input_pcm[i+1]/input_pcm[i]);
        }
        // TODO... ???
        phases[0] = 0.0;
        phases[(pcm->nBlocks/2)-1] = 0.0;
      }
    } else {
      FEATUM_ERROR(0,"pcmProcess_fft failed!");
      free(input_pcm);
      _FUNCTION_RETURN_(0);       
    }  
    
    free(input_pcm);
  
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);  
}
#undef FUNCTION

// complex must be of size pcm->nBlocks
// magnitudes and phases must be at least size pcm->nBlocks/2
int pcmProcess_fft_All( pPcmBuffer pcm, FLOAT_TYPE_FFT *complex, FLOAT_TYPE_FFT *magnitudes, FLOAT_TYPE_FFT * phases, int *ip, FLOAT_TYPE_FFT *w )
#define FUNCTION "pcmProcess_fft_All"
{_FUNCTION_ENTER_
  LONG_IDX i; int no_complex = 0;
  if (pcm != NULL) {

    FLOAT_TYPE_FFT *input_pcm = complex;
    if (input_pcm == NULL) {
      no_complex = 1;
      input_pcm = (FLOAT_TYPE_FFT *)malloc(sizeof(FLOAT_TYPE_FFT)*pcm->nBlocks);
      if (input_pcm == NULL) {
        FEATUM_ERROR(0,"Error allocating memory!");
        _FUNCTION_RETURN_(0);
      }
    }
    
    
    if (pcmProcess_fft( pcm, input_pcm, ip, w )) {
      // save output to lldex (mag + phase..)
      // output is stored in input_pcm array by the rdft function
      if (magnitudes != NULL) {
        for (i=2; i < (pcm->nBlocks); i += 2) {
           magnitudes[i/2] = sqrt(input_pcm[i]*input_pcm[i] + input_pcm[i+1]*input_pcm[i+1]);
        }
        magnitudes[0] = input_pcm[0];
        //magnitudes[(pcm->nBlocks/2)-1] = input_pcm[1];
      }
    
      if (phases != NULL) {
                
        for (i=2; i < (pcm->nBlocks); i += 2) {
           phases[i/2] = atan(input_pcm[i+1]/input_pcm[i]);
        }
        // TODO... ???
        phases[0] = 0.0;
        phases[(pcm->nBlocks/2)-1] = 0.0;
      }
    } else {
      FEATUM_ERROR(0,"pcmProcess_fft failed!");
      if (no_complex) free(input_pcm);
      _FUNCTION_RETURN_(0);       
    }  
    
    if (no_complex) free(input_pcm);
  
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);  
}
#undef FUNCTION



// ....



