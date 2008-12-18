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

LLD feature extractor : Mel-Frequency Cepstral Coefficients (FFT->Mel->Power->DCT->liftering)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDMFCC_H
#define __LLDMFCC_H

#include "feature-memory.h"
#include "pcm-process.h"

#ifdef FT_LLD_MFCC            
extern int ft_lld_mfcc;
extern char * lld_mfcc_ftMem_names[]; 
#endif

typedef struct LLDexMfcc sLLDexMfcc;
typedef sLLDexMfcc * pLLDexMfcc;

#include "LLDex.h"

// FTex struct element:
struct LLDexMfcc {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   int nMel;
   FLOAT_TYPE *melBand;
   int nMFCC;
   FLOAT_TYPE *mfccUL;    // UNliftered MFCCs
   FLOAT_TYPE *mfcc;      // final (liftered) MFCCs
};

#define DEFAULT_nMel       26
#define DEFAULT_firstMFCC  0
#define DEFAULT_nMFCC      13
#define DEFAULT_cepLifter  22
#define DEFAULT_usePower   0
extern FLOAT_TYPE_FFT melfloor; // = 0.00000001;

// private data structure:
typedef struct {
  int nMel,nMFCC,cepLifter,firstMFCC;
  int usePower;
  LONG_IDX blocksize;

  FLOAT_TYPE_FFT * filterCoeffs;   // array containing mel filters
  FLOAT_TYPE_FFT * filterCfs;   // center frequencies in mel
  LONG_IDX *chanMap;
  LONG_IDX nLoF, nHiF;
  
  FLOAT_TYPE *sintable;
  FLOAT_TYPE *costable;
} sLLDmfcc;
typedef sLLDmfcc * pLLDmfcc;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDmfcc_configure
pLLDmfcc LLDmfcc_create( pLLDmfcc obj );

// custom configuration of extractor parameters  (optional)
int LLDmfcc_configure( pLLDmfcc obj, int nMel, int nMFCC, int cepLifter, int firstMFCC, int usePower );

// flushToMem: copy final data from ftex to feature memory element array
int LLDmfcc_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDmfcc_extractor( pLLDmfcc obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDmfcc_createLLDex( pLLDexElement obj );
//pLLDex LLDmfcc_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDmfcc_freeLLDex( pLLDexElement obj );

// destroy the LLDmfcc feature extractor object
pLLDmfcc LLDmfcc_destroy( pLLDmfcc obj );

#endif // #ifndef __LLDMFCC_H
