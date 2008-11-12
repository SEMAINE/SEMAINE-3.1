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

This class saves features from featureMemory in ARFF format

input can be joined from multiple levels in the featureMemory
However, the levels must have the same frameStep!

Merging data from asynchronus level might be supported in the future
 (then frames from levels with greater frameStep will be duplicated, etc.)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __ARFF_OUTPUT_H
#define __ARFF_OUTPUT_H

#include <stdio.h>
#include "feature-memory.h"
#include "GENoutput.h"

typedef sGenOutput sArffOutput;
typedef sArffOutput* pArffOutput;



pArffOutput arffOutput_create( pArffOutput obj, pFeatureMemory mem, const char *filename, int append );

// configure or change the current output file  (append=1 : append to file instead of overwriting)
int arffOutput_setOutputFile( pArffOutput obj, const char *filename, int append );

//configure the elements and levels to be saved (i.e. configure output feature vector)
int arffOutput_configure( pArffOutput obj, pElID els, int nEls );

// set the list of classes (i.e. type ot the @attribute class)
int arffOutput_setClassesList( pArffOutput obj, const char *classlist );

// save current frame (at featureMemory current index)
int arffOutput_saveFrame( pArffOutput obj );

// save current frame (at featureMemory current index), append class information
int arffOutput_saveFrameExt( pArffOutput obj, const char* classlabel, const char *instancelabel );

// save all frames up to last possible
// passing of instance name and class label is not possible here!!
int arffOutput_saveAll( pArffOutput obj );

// closes the current output file, subsequent files to saveFrame will fail
// use this function only if you have to close the file before destroying the object
int arffOutput_closeOutputFile( pArffOutput obj );

// reopens the current output file (only if it has been previously closed with closeOutputFile
int arffOutput_reopenOutputFile( pArffOutput obj );

/* Destructor: frees the class data object, closes the output file and returns NULL */
pArffOutput arffOutput_destroyData( pArffOutput obj );
pArffOutput arffOutput_destroy( pArffOutput obj );

#endif // #ifndef __ARFF_OUTPUT_H
