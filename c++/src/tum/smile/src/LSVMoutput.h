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

This class saves features from featureMemory in LSVM format

input can be joined from multiple levels in the featureMemory
However, the levels must have the same frameStep!

Merging data from asynchronus level might be supported in the future
 (then frames from levels with greater frameStep will be duplicated, etc.)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LSVM_OUTPUT_H
#define __LSVM_OUTPUT_H

#include <stdio.h>
#include "feature-memory.h"
#include "GENoutput.h"

typedef sGenOutput sLsvmOutput;
typedef sLsvmOutput* pLsvmOutput;



pLsvmOutput lsvmOutput_create( pLsvmOutput obj, pFeatureMemory mem, const char *filename, int append );

// configure or change the current output file  (append=1 : append to file instead of overwriting)
int lsvmOutput_setOutputFile( pLsvmOutput obj, const char *filename, int append );

//configure the elements and levels to be saved (i.e. configure output feature vector)
int lsvmOutput_configure( pLsvmOutput obj, pElID els, int nEls );

// save current frame (at featureMemory current index)
int lsvmOutput_saveFrame( pLsvmOutput obj );

// save current frame (at featureMemory current index), append class information
int lsvmOutput_saveFrameExt( pLsvmOutput obj, const char* classlabel );

// save all frames up to last possible
// passing of instance name and class label is not possible here!!
int lsvmOutput_saveAll( pLsvmOutput obj );

// closes the current output file, subsequent files to saveFrame will fail
// use this function only if you have to close the file before destroying the object
int lsvmOutput_closeOutputFile( pLsvmOutput obj );

// reopens the current output file (only if it has been previously closed with closeOutputFile
int lsvmOutput_reopenOutputFile( pLsvmOutput obj );

/* Destructor: frees the class data object, closes the output file and returns NULL */
pLsvmOutput lsvmOutput_destroyData( pLsvmOutput obj );
pLsvmOutput lsvmOutput_destroy( pLsvmOutput obj );

#endif // #ifndef __LSVM_OUTPUT_H
