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

Simple computation of first order differentials
Takes an input level, clones it , and outputs the differentials for all the enabled elements
in the output level.
Higher order differentials can be computed by subsequently applying this class.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __DELTAS_H
#define __DELTAS_H

#include "feature-memory.h"


// NOTE:: ilevel is identical to the level number in featureMemory of the input features

typedef struct {
  pFeatureMemory mem;

  int nFramesContext;   // number of context frames on one side (symmetrical)

  float curIdx;       // current input index in seconds
  LONG_IDX curIdx_output;   // current output index in frames
  LONG_IDX curIdx_frames;  // current input index in frames (last frame of input context window)

  int olevel;    // mapping from functional extractor ID to output level
  int ilevel;    // mapping from functional extractor ID to feature Mem input level

  char **names;
  
} sDeltasConfig;
typedef sDeltasConfig *pDeltasConfig;

/* The class data structure */
typedef struct {
  int nIDs;
  pDeltasConfig config;
} sDeltas;
typedef sDeltas* pDeltas;


/* Constructor: creates a class data object */
// all times and sizes are in floating point with unit seconds
pDeltas deltas_create( pDeltas obj, int nIDs );

// initialise functionals extractor <id>  (id = 0...nIDs-1)
// initialise the output level <olevel> in the feature memory
// if capacity == -1 then the capacity will be automatically derived from the input level capacity
int deltas_setupID( pDeltas obj, pFeatureMemory mem, int id, int olevel, int ilevel, int nFramesContext );

// compute the functionals on level <ilevel> of feature memory <mem>
// save functionals to olevel in feature memory mem
// when this function is called, it computes all deltass possible, up to current idx in ilevel
int deltas_compute( pDeltas obj, int id );
// this function computes deltas only for curIdx in ilevel
int deltas_computeNext( pDeltas obj, int id );

// directly compute for index oIdx
int deltas_doCompute( pDeltas obj, int id, LONG_IDX oIdx );

// compute all deltas for all ids
int deltas_autoCompute( pDeltas obj );

// compute deltas on last (missing) frames at end of stream, 
// for all input levels. automatically map output levels
int deltas_autoComputeLast( pDeltas obj );

/* Destructor: frees the class data object and returns NULL */
pDeltas deltas_destroy( pDeltas obj );
pDeltas deltas_destroyData( pDeltas obj );

#endif // #ifndef __FUNCTIONALS_H
