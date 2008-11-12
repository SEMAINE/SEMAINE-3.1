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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLD feature extractor : FFT

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDFFT_H
#define __LLDFFT_H

#ifdef FT_LLD_FFT
extern int ft_lld_fft;   // id in ft mem. (= element number/index)
extern int ft_lld_fftLong;   // id in ft mem. (= element number/index)
extern char * lld_fft_ftMem_names[];
extern char * lld_fftLong_ftMem_names[];
#define FT_LLD_FFT_nVal     0      // number of values/names in ft. mem  (0=dynamic size)
#endif


#include "feature-memory.h"
#include "pcm-process.h"

typedef struct LLDexFFT sLLDexFFT;
typedef sLLDexFFT * pLLDexFFT;

#include "LLDex.h"

// disable this, if frameSize is always constant. There will then be a slight performance increase
#define SUPPORT_FRAMESIZE_CHANGE
//#define COMPUTE_PHASE         // disabling this saves computation time...

// FTex struct element:
struct LLDexFFT {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   LONG_IDX nBins, nBlocks;
   FLOAT_TYPE fStep;  // frequency step
   FLOAT_TYPE_FFT *magnitudes;
   #ifdef COMPUTE_PHASE
   FLOAT_TYPE_FFT *phases;
   #endif
   FLOAT_TYPE_FFT *complex;  // raw complex frequncy domain data
};


// private data structure:
typedef struct {
   LONG_IDX frameSize_last;
   #ifdef USE_SLOW_DFT
   FLOAT_TYPE_FFT *ip;  // sintable
   FLOAT_TYPE_FFT *w;   // costable
   #else
   int * ip;
   double * w;
   #endif
} sLLDfft;
typedef sLLDfft * pLLDfft;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDenergy_configure
pLLDfft LLDfft_create( pLLDfft obj );

// custom configuration of extractor parameters  (optional)
// int LLDenergy_configure( pLLDenergy obj );
// use this line if this extractor does not have a configure function:
#define LLDfft_configure(obj)

//int LLDenergy_setupNames( int level, pFeatureMemory mem );

// flushToMem: copy final data from ftex to feature memory element array
int LLDfft_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDfft_extractor( pLLDfft obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDfft_createLLDex( pLLDexElement obj );
//pLLDex LLDenergy_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDfft_freeLLDex( pLLDexElement obj );

// destroy the LLDenergy feature extractor object
pLLDfft LLDfft_destroy( pLLDfft obj );

#endif // #ifndef __LLDFFT_H
