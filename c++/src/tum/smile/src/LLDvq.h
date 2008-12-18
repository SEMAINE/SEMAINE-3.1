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

LLD feature extractor : Voice Quality
(TODO.....)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDVQ_H
#define __LLDVQ_H

#include "feature-memory.h"
#include "pcm-process.h"

#ifdef FT_LLD_VQ            
extern int ft_lld_vq;
extern char * lld_vq_ftMem_names[]; 
#define FT_LLD_VQ_nVal   3 
#endif

typedef struct LLDexVq sLLDexVq;
typedef sLLDexVq * pLLDexVq;

#include "LLDex.h"

// FTex struct element:
struct LLDexVq {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   FLOAT_TYPE hnr;
   FLOAT_TYPE jitter;
   FLOAT_TYPE shimmer;
   
};


// private data structure:
typedef struct {
  LONG_IDX blocksize;
} sLLDvq;
typedef sLLDvq * pLLDvq;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDvq_configure
pLLDvq LLDvq_create( pLLDvq obj );

// custom configuration of extractor parameters  (optional)
// int LLDvq_configure( pLLDvq obj );
// use this line if this extractor does not have a configure function:
#define LLDvq_configure(obj)
//int LLDvq_configure( pLLDvq obj, int nMel, int nVQ, int cepLifter, int firstVQ );

// flushToMem: copy final data from ftex to feature memory element array
int LLDvq_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDvq_extractor( pLLDvq obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDvq_createLLDex( pLLDexElement obj );
//pLLDex LLDvq_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDvq_freeLLDex( pLLDexElement obj );

// destroy the LLDvq feature extractor object
pLLDvq LLDvq_destroy( pLLDvq obj );

#endif // #ifndef __LLDVQ_H
