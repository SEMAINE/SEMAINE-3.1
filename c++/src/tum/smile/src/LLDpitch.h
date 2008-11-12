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

LLD feature extractor : voice pitch (F0)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDPITCH_H
#define __LLDPITCH_H

#ifdef FT_LLD_PITCH         
extern int ft_lld_pitch;
extern char * lld_pitch_ftMem_names[];
#define FT_LLD_PITCH_nVal   3      //  // number of values/names in ft. mem
#endif


#include "feature-memory.h"
#include "pcm-process.h"

typedef struct LLDexPitch sLLDexPitch;
typedef sLLDexPitch * pLLDexPitch;

#include "LLDex.h"

// maximum detectable pitch in Hz
#define MAX_PITCH 500.0

// FTex struct element:
struct LLDexPitch {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   FLOAT_TYPE F0freq;
   int f0_index;
   FLOAT_TYPE F0strength;
   FLOAT_TYPE F0quality;   
};


// private data structure:
typedef struct {
   LONG_IDX frameSize_last;
   int * ip;
   double * w;
} sLLDpitch;
//typedef struct LLDpitch sLLDpitch;
typedef sLLDpitch * pLLDpitch;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDpitch_configure
pLLDpitch LLDpitch_create( pLLDpitch obj );

// custom configuration of extractor parameters  (optional)
// int LLDpitch_configure( pLLDpitch obj );
// use this line if this extractor does not have a configure function:
#define LLDpitch_configure(obj)

//int LLDpitch_setupNames( int level, pFeatureMemory mem );

// flushToMem: copy final data from ftex to feature memory element array
int LLDpitch_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDpitch_extractor( pLLDpitch obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDpitch_createLLDex( pLLDexElement obj );
//pLLDex LLDpitch_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDpitch_freeLLDex( pLLDexElement obj );

// destroy the LLDpitch feature extractor object
pLLDpitch LLDpitch_destroy( pLLDpitch obj );

#endif // #ifndef __LLDPITCH_H
