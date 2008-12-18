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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LLD feature extractor : pitch from voice cepstralF0

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDCEPSTRALF0_H
#define __LLDCEPSTRALF0_H

// some defaults:
#define CEPS_F0_THRESH   10       // factor that peak must be higher than mean value
#define CEPS_MAX_PITCH   600.0    
#define N_F0_CANDIDATES  1

#include "feature-memory.h"
#include "pcm-process.h"

#ifdef FT_LLD_CEPSTRALF0     
extern int ft_lld_cepstralF0;
extern char * lld_cepstralF0_ftMem_names[];
#define FT_LLD_CEPSTRALF0_nVal   N_F0_CANDIDATES*2      //  // number of values/names in ft. mem
#endif

typedef struct LLDexCepstralF0 sLLDexCepstralF0;
typedef sLLDexCepstralF0 * pLLDexCepstralF0;

#include "LLDex.h"

// FTex struct element:
struct LLDexCepstralF0 {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:

   int numCeps;
   int f0Index[N_F0_CANDIDATES];
   FLOAT_TYPE F0strength[N_F0_CANDIDATES];
   FLOAT_TYPE F0freq[N_F0_CANDIDATES];
};


// private data structure:
typedef struct {
  FLOAT_TYPE_FFT maxPitch;
  FLOAT_TYPE_FFT threshold;
} sLLDcepstralF0;
//typedef struct LLDcepstralF0 sLLDcepstralF0;
typedef sLLDcepstralF0 * pLLDcepstralF0;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDcepstralF0_configure
pLLDcepstralF0 LLDcepstralF0_create( pLLDcepstralF0 obj );

// custom configuration of extractor parameters  (optional)
// use this line if this extractor does not have a configure function:
//#define LLDcepstralF0_configure(obj)
int LLDcepstralF0_configure(pLLDcepstralF0 obj, FLOAT_TYPE_FFT maxPitch, FLOAT_TYPE_FFT threshold );

//int LLDcepstralF0_setupNames( int level, pFeatureMemory mem );

// flushToMem: copy final data from ftex to feature memory element array
int LLDcepstralF0_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDcepstralF0_extractor( pLLDcepstralF0 obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDcepstralF0_createLLDex( pLLDexElement obj );
//pLLDex LLDcepstralF0_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDcepstralF0_freeLLDex( pLLDexElement obj );

// destroy the LLDcepstralF0 feature extractor object
pLLDcepstralF0 LLDcepstralF0_destroy( pLLDcepstralF0 obj );

#endif // #ifndef __LLDCEPSTRALF0_H
