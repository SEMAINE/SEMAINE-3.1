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

LLD wrapper class, manages all LLD extractors

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDS_H
#define __LLDS_H

#include "feature-memory.h"
#include "LLDex.h"

#define MAX_DEPENDS            5
#define TOTAL_NUM_LLDS         12  // 11


typedef struct {
   int enabled;
   int priority;  // lower priority gets processed first
   int level;    // framesize level the extractor operates on
   char *name;
   
   char **ftmem_names;   // name of single values in feature memory
   int ftmem_nVals;    // number of values in feature memory
   int ftmem_nValsDyn;    // number of values in feature memory
   int * ftmem_element;  // element index in feature memory
   
   int (*flushToFtMem)(int, pFeatureMemory, pLLDex);
   int (*extractor)(void *, pLLDex, int);
   int (*createLLDex)(pLLDexElement);
   int (*freeLLDex)(pLLDexElement);
   void * (*LLDcreate)(void *);
   void * (*LLDdestroy)(void *);
   void * LLDobj;
   int nDepends;
   const char *depends[MAX_DEPENDS];
//   int ftMemIdx;
} sLLDconfig;
typedef sLLDconfig *pLLDconfig;

typedef struct {
   int nExtractors;
   int nExtractorLevels;
   int *nExtractors_enabled;    // for each level...
   int *nExtractors_flushable;  // for each level...
   pLLDex LLDex;     
   int freeLLDex, freeLLDobjs;
   sLLDconfig config[TOTAL_NUM_LLDS+1];
//   sLLDconfig config[];
} sLLDs;
typedef sLLDs *pLLDs;

typedef struct {
   int nExtractors;
   int nExtractorLevels;
   int *nExtractors_enabled;    // for each level...
   int *nExtractors_flushable;  // for each level...
   pLLDex LLDex;     
   int freeLLDex, freeLLDobjs;
//   pLLDconfig config;
   sLLDconfig config[TOTAL_NUM_LLDS+1];
} sLLDs_const;
typedef sLLDs_const *pLLDs_const;
/****************************************************************************/










/****************************************************************************/ 


#define LLD_DFLT_ENABLE 0

#define LLD_LEVEL0 0
#define LLD_LEVEL1 1   

extern sLLDs_const LLD;  // declared in extractors-globalconfig.c


// set freeLLDobjs to 1 to automatically allocate AND free the LLD objects by calling the generic constructor      
pLLDs LLDs_create( pLLDs obj, pLLDex l, int freeLLDex, int freeLLDobjs );

// enable LLD "name", and register extractor obj. if obj = NULL it will be created by calling default constructor!
// if level == -1 the default defined by the constants will be used
void * LLDs_setupLLD( pLLDs obj, const char * name, int enabled, void *exObj, int level );
void LLDs_countEnabled( pLLDs obj );

void LLDs_configureFeatureMemoryNames( pLLDs obj, pFeatureMemory mem, int ftmem_level );

int LLDs_createExtractors( pLLDs obj );
int LLDs_callExtractors( pLLDs obj, int level );
int LLDs_flushToFtMem( pLLDs obj, pFeatureMemory mem, int level );

/*int LLDs_register( pLLDs obj, char * name, 
                     void (*flushToFtMem)(pFeautreMemory, pLLDex),
                     void (*extractor)(void *, pLLDex),
                     pLLDex (*freeLLDex)(void *)
                  );   
*/
int LLDs_checkDependencies( pLLDs obj );
pLLDs LLDs_destroy( pLLDs obj );
pLLDs LLDs_destroyData( pLLDs obj );

int LLDs_setupFeatureMemLevels( pLLDs llds, pFeatureMemory mem, pInputFramer framer, int *framerIDs, LONG_IDX *capacity, int nConf );





#endif // #ifndef __LLDS_H
