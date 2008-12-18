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

This class saves features from featureMemory in CSV format

input can be joined from multiple levels in the featureMemory
However, the levels must have the same frameStep!

Merging data from asynchronus level might be supported in the future
 (then frames from levels with greater frameStep will be duplicated, etc.)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __CSV_OUTPUT_H
#define __CSV_OUTPUT_H

#include <stdio.h>
#include "feature-memory.h"
#include "GENoutput.h"

	
typedef struct {
} sCSVheader;
typedef sCSVheader * pCSVheader;

typedef sGenOutput sCsvOutput;
typedef sCsvOutput* pCsvOutput;


/* Constructor: creates a class data object */
// usage: pClassName testObj = NULL;
//        testObj = testClass_create( testObj, ... );
pCsvOutput csvOutput_create( pCsvOutput obj, pFeatureMemory mem, const char *filename );

// set or change the current output file  (append=1 : append to file instead of overwriting)
int csvOutput_setOutputFile( pCsvOutput obj, const char *filename, int append );

//configure the elements and levels to be saved (i.e. configure output feature vector)
int csvOutput_configure( pCsvOutput obj, pElID els, int nEls );

// save current frame (at featureMemory current index)
int csvOutput_saveFrame( pCsvOutput obj );

// save current frame (at featureMemory current index)
int csvOutput_saveAll( pCsvOutput obj );

// closes the current output file, subsequent files to saveFrame will fail
// use this function only if you have to close the file before destroying the object
int csvOutput_closeOutputFile( pCsvOutput obj );

// reopens the current output file (only if it has been previously closed with closeOutputFile
int csvOutput_reopenOutputFile( pCsvOutput obj );

/* Destructor: frees the class data object, closes the output file and returns NULL */
pCsvOutput csvOutput_destroyData( pCsvOutput obj );
pCsvOutput csvOutput_destroy( pCsvOutput obj );

#endif // #ifndef __CSV_OUTPUT_H
