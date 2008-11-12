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

LLD feature extractor : Mel-Frequency Cepstral Coefficients (FFT->Mel->Power->DCT->liftering)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDSPECTRAL_H
#define __LLDSPECTRAL_H

#include "feature-memory.h"
#include "pcm-process.h"

#define NORM_FREQUENCY  1000.0

#ifdef FT_LLD_SPECTRAL            
extern int ft_lld_spectral;
extern char * lld_spectral_ftMem_names[]; 
#define FT_LLD_SPECTRAL_nVal   13      //  // number of values/names in ft. mem
#endif

typedef struct LLDexSpectral sLLDexSpectral;
typedef sLLDexSpectral * pLLDexSpectral;

#include "LLDex.h"

// FTex struct element:
struct LLDexSpectral {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   FLOAT_TYPE en0_250;
   FLOAT_TYPE en0_650;
   FLOAT_TYPE en250_650;
   FLOAT_TYPE en1000_4000;   
   FLOAT_TYPE flux;
   FLOAT_TYPE rollOff10;
   FLOAT_TYPE rollOff25;
   FLOAT_TYPE rollOff50;
   FLOAT_TYPE rollOff75;
   FLOAT_TYPE rollOff90;   
   FLOAT_TYPE centroid;
   FLOAT_TYPE maxPos;
   FLOAT_TYPE minPos;
};

// private data structure:
typedef struct {
  LONG_IDX blocksize;

} sLLDspectral;
typedef sLLDspectral * pLLDspectral;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDspectral_configure
pLLDspectral LLDspectral_create( pLLDspectral obj );

// custom configuration of extractor parameters  (optional)
// int LLDspectral_configure( pLLDspectral obj );
// use this line if this extractor does not have a configure function:
#define LLDspectral_configure(obj)
//int LLDspectral_configure( pLLDspectral obj, int nMel, int nMFCC, int cepLifter, int firstMFCC );

// flushToMem: copy final data from ftex to feature memory element array
int LLDspectral_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDspectral_extractor( pLLDspectral obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDspectral_createLLDex( pLLDexElement obj );
//pLLDex LLDspectral_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDspectral_freeLLDex( pLLDexElement obj );

// destroy the LLDspectral feature extractor object
pLLDspectral LLDspectral_destroy( pLLDspectral obj );

#endif // #ifndef __LLDSPECTRAL_H
