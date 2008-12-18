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

Cepstral Mean Subtraction for MFCC, depends on FFT and MFCC extractor

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDMFCCZ_H
#define __LLDMFCCZ_H

#include "feature-memory.h"
#include "pcm-process.h"

#define DEFAULT_alpha 0.995

#ifdef FT_LLD_MFCCZ            
extern int ft_lld_mfccz;
extern char * lld_mfccz_ftMem_names[]; 
#endif

typedef struct LLDexMfccz sLLDexMfccz;
typedef sLLDexMfccz * pLLDexMfccz;

#include "LLDex.h"

// FTex struct element:
struct LLDexMfccz {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   int nMFCC;
   FLOAT_TYPE *mfccz;    // normalised mfcc
};

// private data structure:
typedef struct {
   int nMfcc;
   int nVec;
   FLOAT_TYPE alpha;
   FLOAT_TYPE *means;
   int nMeans0;
   FLOAT_TYPE *means0;
} sLLDmfccz;
typedef sLLDmfccz * pLLDmfccz;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDmfccz_configure
pLLDmfccz LLDmfccz_create( pLLDmfccz obj );

// custom configuration of extractor parameters  (optional)
// int LLDmfccz_configure( pLLDmfcc obj );
// use this line if this extractor does not have a configure function:
//#define LLDmfccz_configure(obj)
int LLDmfccz_configure( pLLDmfccz obj, FLOAT_TYPE alpha, const char * initFile );

// flushToMem: copy final data from ftex to feature memory element array
int LLDmfccz_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDmfccz_extractor( pLLDmfccz obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDmfccz_createLLDex( pLLDexElement obj );

// free the lldex object l and return NULL
int LLDmfccz_freeLLDex( pLLDexElement obj );

// destroy the LLDmfcc feature extractor object
pLLDmfccz LLDmfccz_destroy( pLLDmfccz obj );

#endif // #ifndef __LLDMFCCZ_H
