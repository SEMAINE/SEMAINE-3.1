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

LLD feature extractor : energy and log-Energy

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDENERGY_H
#define __LLDENERGY_H

#ifdef FT_LLD_ENERGY
extern int ft_lld_energy;   // id in ft mem. (= element number/index)
extern char * lld_energy_ftMem_names[];
#define FT_LLD_ENERGY_nVal     2      // number of values/names in ft. mem
#endif


#include "feature-memory.h"
#include "pcm-process.h"


// FTex struct element:
struct LLDexEnergy {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   FLOAT_TYPE rmsEnergy;
   FLOAT_TYPE logEnergy;
};

typedef struct LLDexEnergy sLLDexEnergy;
typedef sLLDexEnergy * pLLDexEnergy;

#include "LLDex.h"



// private data structure:
typedef void sLLDenergy;
typedef sLLDenergy * pLLDenergy;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDenergy_configure
pLLDenergy LLDenergy_create( pLLDenergy obj );

// custom configuration of extractor parameters  (optional)
// int LLDenergy_configure( pLLDenergy obj );
// use this line if this extractor does not have a configure function:
#define LLDenergy_configure(obj)

// flushToMem: copy final data from ftex to feature memory element array
int LLDenergy_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDenergy_extractor( pLLDenergy obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDenergy_createLLDex( pLLDexElement obj );
//pLLDex LLDenergy_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDenergy_freeLLDex( pLLDexElement obj );

// destroy the LLDenergy feature extractor object
pLLDenergy LLDenergy_destroy( pLLDenergy obj );

#endif // #ifndef __LLDENERGY_H
