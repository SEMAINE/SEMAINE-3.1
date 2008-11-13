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

This class saves features from featureMemory in HTK format

input can be joined from multiple levels in the featureMemory
However, the levels must have the same frameStep!

Merging data from asynchronus level might be supported in the future
 (then frames from levels with greater frameStep will be duplicated, etc.)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __HTK_OUTPUT_H
#define __HTK_OUTPUT_H

#include <stdio.h>
#include <inttypes.h>
#include "feature-memory.h"
#include "GENoutput.h"

	
typedef struct {
  uint32_t nSamples;
  uint32_t samplePeriod;
  uint16_t sampleSize;
  uint16_t parmKind;
} sHTKheader;
typedef sHTKheader * pHTKheader;


/* The class data structure */
/*
typedef struct {
  char *filename;
  FILE * filedesc;         // contains descriptor of open file, or -1 if no file is open
  pFeatureMemory mem;
  int is_vax;
  sHTKheader header;
  float samplePeriod;
  LONG_IDX idx_save;   // max. index in featureMemory that has been saved  (used to determine if we have new data to save)
  LONG_IDX fileIdx_frames;  // current write position in file (in frames or vectors)
  int outputVectorDimensionality;
  int nEls;     // number of elements to save
  pElID els;    // indicies of elements to save
} sHtkOutput;
*/
typedef sGenOutput sHtkOutput;
typedef sHtkOutput* pHtkOutput;


/* Constructor: creates a class data object */
// usage: pClassName testObj = NULL;
//        testObj = testClass_create( testObj, ... );
pHtkOutput htkOutput_create( pHtkOutput obj,  pFeatureMemory mem, const char *filename );

// configure or change the current output file  (append=1 : append to file instead of overwriting)
int htkOutput_setOutputFile( pHtkOutput obj, const char *filename, int append );

//configure the elements and levels to be saved (i.e. configure output feature vector)
int htkOutput_configure( pHtkOutput obj, pElID els, int nEls );

// save current frame (at featureMemory current index)
int htkOutput_saveFrame( pHtkOutput obj );

// save all frames up to last possible
int htkOutput_saveAll( pHtkOutput obj );

// save arbitrary frame given by frameIndex (however, frameIndex must be smaller than the max. index saved)
//int htkOutput_saveFrame_abs( pHtkOutput obj, LONG_IDX frameIndex );

// closes the current output file, subsequent files to saveFrame will fail
// use this function only if you have to close the file before destroying the object
int htkOutput_closeOutputFile( pHtkOutput obj );

// reopens the current output file (only if it has been previously closed with closeOutputFile
int htkOutput_reopenOutputFile( pHtkOutput obj );

/* Destructor: frees the class data object, closes the output file and returns NULL */
pHtkOutput htkOutput_destroyData( pHtkOutput obj );
pHtkOutput htkOutput_destroy( pHtkOutput obj );

#endif // #ifndef __HTK_OUTPUT_H