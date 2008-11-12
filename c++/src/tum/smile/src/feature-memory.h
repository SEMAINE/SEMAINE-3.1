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

This class manages the feautre memory with multiple levels of different capacities

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __FEATURE_MEMORY_H
#define __FEATURE_MEMORY_H

#include "input-framer.h"

///// featureMemory  - store final feature frames

#define VIDX_CURRENT_WRITE -9990
#define VIDX_CURRENT_READ -9991
#define VIDX_CURRENT_ALLFILLED -9992

#define FTMEM_DEFAULT_BUFFER 0
#define FTMEM_FIXED_BUFFER 1
#define FTMEM_RING_BUFFER  2

// featureMemory by default does NOT have ringbuffer capabilities

// FTmem struct element:
typedef struct {
  int nValues;
  int filled;
  FLOAT_TYPE_FTMEM *values;
} sFtMemElement;
typedef sFtMemElement *pFtMemElement;

typedef struct {
  int nFtEls;   // number of feature elements
  int allFilled;  // are all elements filled?
  FLOAT_TYPE_TIME timestamp;  // seconds
  pFtMemElement *frame;   // [ftelement]
} sFtMemFrame;
typedef sFtMemFrame * pFtMemFrame;

// feature name mapping
typedef struct {
  int nTotal;
  char ** final_names;
  int final_names_alloced;
  
  int nValues;    // either number of features (first level) or multiplier   # of static values or dynamic values (once assigned, otherwise 0)
  int nValuesDyn;  // only for first level: number of dynamic values       0 if only static, non-zero if dynamic

  char **names;   // names (either base names, or postfixes)
  char * dynname;
    
  //int n;          // n here is the same as n in sFtMemElement (only for static elements, elemnts with dynamic size have n=0 here)
  //int nDyn;       // number of dynamic basenames: required for functionals, etc. 
  //int alloced;    // flag that indicates whether the memory for the names has been allocated or not
  //char ** names;
  //char * dynname;
  int options;   // flag options like COMPUTE_FUNCTIONALS, COMPUTE_DELTA, SAVE ....
  // possibly more options in here....?
  int enabled;  // is this feature enabled?  // TODO: later: dynamic allocation of feature mem, only enabled features will be allocated
} sFtMemElementConfig;
typedef sFtMemElementConfig * pFtMemElementConfig;

typedef struct {
  LONG_IDX capacity; // temporal capacity: size of frame memory (actual size of buffer)
  int bufferType;     // 1: fixed capacity buffer : 2: ring buffer
  LONG_IDX curIdx;   // current (virtual) write index for sequential appending of feature frames
  LONG_IDX allFilledIdx;  // (virtual) index up to where all frames are completely filled (data was flushed from lldex struct)
  LONG_IDX readIdx;
  LONG_IDX levelLatency_frames;  // unused for now  (level latency in frames)
  FLOAT_TYPE_TIME levelLatency;  // unsused for now (level latency in seconds)
  FLOAT_TYPE_TIME T;   // period in seconds of subsequent feature frames (frameStep in seconds)
//  LONG_IDX frameStep; // the same as T , but in unit of samples   // OBSOLETE!
  LONG_IDX nFeaturesPerFrame;  // total number of features per frame
  LONG_IDX frameSize;  // ?? total size of feature frame, initialised at beginning ??

  int nPrevLevels;
  int prevLevel;   // set to -1 to disable (i.e. no previous level linked)
  int nextLevel;   // set to -1 to disable (i.e. no next level linked)

  int nFtEls;   // number of feature elements

  pFtMemElementConfig config;   // [ftelement] names, etc for each feature element
  // these indices are relative to the buffer (arrays of size = capacity)
  FLOAT_TYPE_TIME *t;    // timstamp for each frame [index]
  pFtMemElement *ft;   // [ftelement]  (each array will be [capacity] in size)
} sFeatureMemoryLevel;
typedef sFeatureMemoryLevel * pFeatureMemoryLevel;

// main data struct of feature memory
typedef struct {
  int nLevels;       // number of feature frame levels
                     // each level can hold different features at different rates
  int ll_levels;     // number of levels already initialized, i.e. number of levels that contain low level data
  int defaultBufferType;
  pFeatureMemoryLevel level;   // data for each level
} sFeatureMemory;
typedef sFeatureMemory * pFeatureMemory;

typedef struct {
  char *name;
  int isValidated;
  int found;
  int level;
  int el;
  int n;
  int nVals;
} sFtIdentifier;
typedef sFtIdentifier * pFtIdentifier;
        

// virtual index: absolute index counting from beginning (could be either fixed buffer or ring buffer)

//pFeatureMemory featureMemory_create( pFeatureMemory obj, pInputFramer framer, int *framerIDs, int nLevels, LONG_IDX *capacity );
pFeatureMemory featureMemory_create( pFeatureMemory obj, int defaultBufferType, int nLevels );

int featureMemory_configureLevel( pFeatureMemory obj, int level, int capacity, int bufferType, float T, int nFtEls, int prevLevel);
int featureMemory_setNFtEls( pFeatureMemory obj, int level, int nFtEls );
int featureMemory_dataMemAlloc( pFeatureMemory obj );
int featureMemory_dataMemAllocLevel( pFeatureMemory obj, int level );
int featureMemory_dataMemFree( pFeatureMemory obj );


// nDyn is the number of basenames if the number of values is dynamic
int featureMemory_setupElement( pFeatureMemory mem, int level, int el, int nVal, int nDyn, char **names, int enabled );
// build names... if possible (due to dynmaic elements it may not be possible when no frames are present...)
int featureMemory_buildNames( pFeatureMemory mem, int level, int el );

/*
pFtMemElement featureMemory_getCurrentWriteElement( pFeatureMemory obj, int level, int el );
pFtMemElement featureMemory_getCurrentAllFilledElement( pFeatureMemory obj, int level, int el );
pFtMemElement featureMemory_getCurrentReadElement( pFeatureMemory obj, int level, int el );
*/
pFtMemElement featureMemory_getElement( pFeatureMemory obj, int level, LONG_IDX vIdx, int el );
pFtMemElement featureMemory_getElementFRep( pFeatureMemory obj, int level, LONG_IDX vIdx, int el );
LONG_IDX featureMemory_getSpecialIndex( pFeatureMemory obj, int level, LONG_IDX vIdxConst );
FLOAT_TYPE_TIME featureMemory_vIdxToTime ( pFeatureMemory obj, int level, int vIdx );

// initialize an ft identifier before calling findFeature
void featureMemory_initFtID(pFtIdentifier ftID);

// find a feature by name, give known values of level, el, n in ftID, otherwise set to -1
int featureMemory_findFeature(pFeatureMemory obj, const char* name, pFtIdentifier ftID);

// returns pointer to 1 or more (ftID.nVals) features 
// relIdx is relative to the current Index (allFilledIdx)
FLOAT_TYPE_FTMEM * featureMemory_getFeaturesByID(pFeatureMemory obj, pFtIdentifier ftID, LONG_IDX relIdx);

// vIdx can be -1 for currentAllFilledIndex  and -2 for currentReadIndex
// slow get feature functions...  level0: direct (raw) indexing (always works)
FLOAT_TYPE_FTMEM featureMemory_getFeature( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n );
// level1 : two dimensional indexing (i.e. low level feature index + index of functional)
//   only works if for the requested level the variable prevLevels == 1
FLOAT_TYPE_FTMEM featureMemory_getFeatureL1( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n0, int n1 );
// level2 : three dimensional indexing (for hierarchical functionals)
//   only works if for the requested level the variable prevLevels == 2
FLOAT_TYPE_FTMEM featureMemory_getFeatureL2( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n0, int n1, int n2 );

// faster way: get feature indicies once, then always retrieve the features directly from the elements
//int featureMemory_getFeatureIndexL0( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n );
// level0 not required, because result == n
// level1 : two dimensional indexing (i.e. low level feature index + index of functional)
int featureMemory_getFeatureIndexL1( pFeatureMemory obj, int level, int el, int n0, int n1 );
// level2 : three dimensional indexing (for hierarchical functionals)
int featureMemory_getFeatureIndexL2( pFeatureMemory obj, int level, int el, int n0, int n1, int n2 );

// the names returned by these functions are the same for all three functions, only the referencing of the features is different
// get feature name functions...  level0: direct (raw) indexing (always works)
char * featureMemory_getFeatureName( pFeatureMemory obj, int level, int el, int n );
// level1 : two dimensional indexing (i.e. low level feature index + index of functional)
//   only works if for the requested level the variable prevLevels == 1
char * featureMemory_getFeatureNameL1( pFeatureMemory obj, int level, int el, int n0, int n1 );
// level2 : three dimensional indexing (for hierarchical functionals)
//   only works if for the requested level the variable prevLevels == 2
char * featureMemory_getFeatureNameL2( pFeatureMemory obj, int level, int el, int n0, int n1, int n2 );



// allocate memory for frame <idx>:
int featureMemory_allocFrame( pFeatureMemory obj, int level, int vIdx);
// allocate memory for subsequent frame, increase current frame counter
int featureMemory_addFrame( pFeatureMemory obj, int level);

//pFtMemFrame featureMemory_freeFrame( pFtMemFrame obj );
//pFtMemFrame featureMemory_getFrame( pFeatureMemory obj, int level, LONG_IDX absIdx );


// functions to return the size of the structures without directly having to access the featureMemory object
//char *featureMemory_getFeatureName( pFeatureMemory mem, int level, int el, int n );
int featureMemory_getNLevels( pFeatureMemory mem );
int featureMemory_getNFtEls( pFeatureMemory mem, int level );
int featureMemory_getNFeatures( pFeatureMemory mem, int level, int el );
int featureMemory_getNFeatures_idx( pFeatureMemory mem, int level, int el, LONG_IDX vIdx );
int featureMemory_getNFeaturesPerFrame( pFeatureMemory mem, int level );

//FLOAT_TYPE_FTMEM * featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal, int relidx );
// allocate and return memory for new values in a feature memory element
FLOAT_TYPE_FTMEM * featureMemory_getValuesForWrite_abs( pFeatureMemory mem, int level, int el, int nVal, LONG_IDX vIdx, FLOAT_TYPE_TIME timestamp );
FLOAT_TYPE_FTMEM * featureMemory_getValuesForWrite_seq( pFeatureMemory mem, int level, int el, int nVal, LONG_IDX relidx, FLOAT_TYPE_TIME timestamp );

// check the fill status of the feature Memory level <level> and update the status
int featureMemory_checkStatus( pFeatureMemory mem, int level );

pFeatureMemory featureMemory_destroy( pFeatureMemory obj );
pFeatureMemory featureMemory_destroyData( pFeatureMemory obj );

#if 0
// new getFeature functions:
getFeatureByName  relIdx, el
getFeatureByName_global  relIdx   (names must be unique!)
getFeatureByNameAbs  vIdx, el
getFeatureByNameAbs_global  vIdx   (names must be unique!)
getFeatureIndexByName....
#endif

#endif // #ifndef __FEATURE_MEMORY_H
