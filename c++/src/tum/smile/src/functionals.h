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

This class computes statistical functionals operating on a featureMemory object

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
 
#ifndef __FUNCTIONALS_H
#define __FUNCTIONALS_H

#include "feature-memory.h"

typedef struct {
  FLOAT_TYPE_FTMEM max,min;
  FLOAT_TYPE_FTMEM mean;  // arithmetic
  FLOAT_TYPE_FTMEM centroid_numerator;
  FLOAT_TYPE_FTMEM centroid_numerator2;
  FLOAT_TYPE_FTMEM quartile1;
  FLOAT_TYPE_FTMEM quartile2;
  FLOAT_TYPE_FTMEM quartile3;
  FLOAT_TYPE_FTMEM *costable;
} sSharedFuncs;
typedef sSharedFuncs * pSharedFuncs;


#define UNSORTED 0
#define SORTED   1
typedef struct  {
  const char *name;
  void (*comp_unsorted)(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt,  pSharedFuncs shared);
  void (*comp_sorted)(FLOAT_TYPE_FTMEM *sorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt,  pSharedFuncs shared);
  int cnt;
  int requireSorted;
  int confId; 
  const char **names;
} sFunctionalsConfig;
typedef sFunctionalsConfig * pFunctionalsConfig;




// NOTE:: ilevel is identical to the level number in featureMemory of the input features

typedef struct {
  int isConfigured;
  pFeatureMemory mem;
  float computeframeSize;   // frame size over which to compute the functionals (seconds)
  int computeframeSize_frames;   // frame size in input frames ...
  float frameStep;         // output frameStep for one value (usually the same as frameStep_input ??)
  float frameStep_input;  // frameStep of input level (used...??)
  float curIdx;       // current input index in seconds
  LONG_IDX curIdx_output;   // current output index in frames
  LONG_IDX curIdx_frames;  // current input index in frames
  int olevel;    // mapping from functional extractor ID to output level
  int ilevel;    // mapping from functional extractor ID to feature Mem input level

  int requireSorted;   
  FLOAT_TYPE_FTMEM *costable;
  int costableBufLen;
  
  int nFunctionals;
  int nFunctionalValues;   // number of values and names
  sFunctionalsConfig *functs;
  int *functsEnabled;
  int *functsIdx;
    
  char **names;   // names for all enabled functionals
  
} sFunctionalIDConfig;
typedef sFunctionalIDConfig *pFunctionalIDConfig;

/* The class data structure */
typedef struct {

  int nIDs;
  int maxFunctionals;
  pFunctionalIDConfig config;

} sFunctionals;
typedef sFunctionals* pFunctionals;



/* Constructor: creates a class data object */
// all times and sizes are in floating point with unit seconds
pFunctionals functionals_create( pFunctionals obj, int nIDs );

/* Constructor: creates a class data object */
// automatically read nLevels from featureMemory, and call functionals_create internally
pFunctionals functionals_autoCreate( pFunctionals obj, pFeatureMemory mem );

// enable the functional "name" for each ID in id_list
// id_list should have -1 as the last element to terminate the list!!
// if id_list is NULL or contains only "-1" then the functional will be enabled 
// for ALL ids
int functionals_enable(pFunctionals obj, const char *name, int *id_list);

// execute this before(!) calling any of the setupID functions
int functionals_finalizeConfiguration( pFunctionals obj );

// initialise functionals extractor <id>  (id = 0...nIDs-1)
// initialise the output level <olevel> in the feature memory
// if capacity == -1 then the capacity will be automatically derived from the input level capacity
int functionals_setupID( pFunctionals obj, pFeatureMemory mem, int id, int olevel, int ilevel, float T, float computeframeSize, int capacity );

// initialise the output levels in the feature memory automatically based on parameters of the lld levels
int functionals_autoSetupIDs( pFunctionals obj, pFeatureMemory mem, float *T, float *fs );

// compute the functionals on level <ilevel> of feature memory <mem>
// save functionals to olevel in feature memory mem
// when this function is called, it computes all functionals possible, up to current idx in ilevel
int functionals_compute( pFunctionals obj, int id );
int functionals_computeCurrentOnly( pFunctionals obj, int id );
// ignore frameSize , compute functionals right now
int functionals_computeNow( pFunctionals obj, int id, int fmin_frames );
// sets the start of the next frame that will be computed to curIdx
int functionals_setFrameStart( pFunctionals obj, int id, LONG_IDX relIdx );

// compute all functionals for all input levels, automatically map output levels
int functionals_autoCompute( pFunctionals obj );

/* Destructor: frees the class data object and returns NULL */
pFunctionals functionals_destroy( pFunctionals obj );
pFunctionals functionals_destroyData( pFunctionals obj );

#endif // #ifndef __FUNCTIONALS_H
