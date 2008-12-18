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

LLD feature extractor : cepstrum  (complex and real)
  (complex cepstrum not yet tested... )

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDCEPSTRUM_H
#define __LLDCEPSTRUM_H

#include "feature-memory.h"
#include "pcm-process.h"

#ifdef FT_LLD_CEPSTRUM     
extern int ft_lld_cepstrum;
extern char * lld_cepstrum_ftMem_names[];
#define FT_LLD_CEPSTRUM_nVal   0      //  // number of values/names in ft. mem
#endif

typedef struct LLDexCepstrum sLLDexCepstrum;
typedef sLLDexCepstrum * pLLDexCepstrum;

#include "LLDex.h"

// FTex struct element:
struct LLDexCepstrum {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:

   int numCeps;
   FLOAT_TYPE_FFT *cepstrum;
};


// private data structure:
typedef struct {
   int numCeps;
   int complexCepstrum;
   LONG_IDX frameSize_last;
   int * ip;
   FLOAT_TYPE_FFT * w;
} sLLDcepstrum;
//typedef struct LLDcepstrum sLLDcepstrum;
typedef sLLDcepstrum * pLLDcepstrum;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDcepstrum_configure
pLLDcepstrum LLDcepstrum_create( pLLDcepstrum obj );

// custom configuration of extractor parameters  (optional)
// use this line if this extractor does not have a configure function:
//#define LLDcepstrum_configure(obj)
int LLDcepstrum_configure(pLLDcepstrum obj, int numCeps, int complex );

// flushToMem: copy final data from ftex to feature memory element array
int LLDcepstrum_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDcepstrum_extractor( pLLDcepstrum obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDcepstrum_createLLDex( pLLDexElement obj );

// free the lldex object l and return NULL
int LLDcepstrum_freeLLDex( pLLDexElement obj );

// destroy the LLDcepstrum feature extractor object
pLLDcepstrum LLDcepstrum_destroy( pLLDcepstrum obj );

#endif // #ifndef __LLDCEPSTRUM_H
