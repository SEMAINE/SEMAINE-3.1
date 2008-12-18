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

LLD feature extractor : Linear-Predictive Coding a_j and k_j coefficients
This code is incomplete and untested... (TODO.....)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDLPC_H
#define __LLDLPC_H

#include "feature-memory.h"
#include "pcm-process.h"

#ifdef FT_LLD_LPC            
extern int ft_lld_lpc;
extern char * lld_lpc_ftMem_names[]; 
#define FT_LLD_LPC_nVal     0      // number of values/names in ft. mem  (0=dynamic size)
#endif

typedef struct LLDexLpc sLLDexLpc;
typedef sLLDexLpc * pLLDexLpc;

#include "LLDex.h"

// FTex struct element:
struct LLDexLpc {
   MANDATORY_LLDEX_HEADER        // this line is mandatory in all LLDex structs

   // user defined portion below here:
   int lpcOrder;
   FLOAT_TYPE *k;  // reflection coefficients (1..lpcOrder)
   FLOAT_TYPE *a;  // lpc coeffs (1..lpcOrder-1)
   FLOAT_TYPE *r;  // acf coeffs (1..lpcOrder)
};

#define DEFAULT_lpcOrder  12

// private data structure:
typedef struct {
  int lpcOrder;
  LONG_IDX blocksize;
  FLOAT_TYPE *E; // prediction energy
  FLOAT_TYPE *a; // a coeff matrix
} sLLDlpc;
typedef sLLDlpc * pLLDlpc;

   
// constructor : must always have only one argument, creates object
// all configuration will be done AFTER object creation via LLDlpc_configure
pLLDlpc LLDlpc_create( pLLDlpc obj );

// custom configuration of extractor parameters  (optional)
// use this line if this extractor does not have a configure function:
//#define LLDlpc_configure(obj)
int LLDlpc_configure( pLLDlpc obj, int lpcOrder );

// flushToMem: copy final data from ftex to feature memory element array
int LLDlpc_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex );
// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDlpc_extractor( pLLDlpc obj, pLLDex lldex, int level );  // level = frame id

// create the lldex object and store pointer to it in obj (besides returning the pointer)
int LLDlpc_createLLDex( pLLDexElement obj );
//pLLDex LLDlpc_createLLDex( void * obj );

// free the lldex object l and return NULL
int LLDlpc_freeLLDex( pLLDexElement obj );

// destroy the LLDlpc feature extractor object
pLLDlpc LLDlpc_destroy( pLLDlpc obj );

#endif // #ifndef __LLDLPC_H
