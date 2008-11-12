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

Generic output interface:
        what does a general output class do?

- configure the feature selection during or right after object creation 
- building of feature name array
- generation of output vectors
- selection and validation of input frames from feature memory


input can be joined from multiple levels in the featureMemory
However, the levels must have the same frameStep!

Merging data from asynchronus level might be supported in the future
 (then frames from levels with greater frameStep will be duplicated, etc.)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __GEN_OUTPUT_H
#define __GEN_OUTPUT_H

#include <stdio.h>
#include "feature-memory.h"

#define CSV_DELIM_CHAR ';'
#define ARFF_DELIM_CHAR ','
#define LSVM_DELIM_CHAR ' '
#define MAX_FTNAME_LENGTH 255    // maximum length of feature name (used for computation of max. header string size)

#define FLOAT_TYPE_OUTPUT double

typedef struct {
  int level;   // featureMemory level
  int el;      // featureElement number  ( global variable ft_lld_<name> )
  // feature selection range:
  int nStart;  // feature value start index
  int nEnd;    // feature value end index  (-1 for full length)
  int n;       // alternatively: number of features counting from start index
               // will be always converted to nEnd internally  (set to zero if you want to give nEnd directly!!)
  int enabled; // used internally mainly, by default set it to 1!
  int nValues; // set internally: total number of values for level:el
} sElID;   // element identifier
typedef sElID * pElID;

typedef struct {
  int n;          // number of features in output vector (after selection, if applicable)
  int nFull;      // full vector length, when using feature selection, otherwise same as n
  int maxNameLength;  // maximum string length of names (including the \0 character)
  char **names;   // names of the features in the output vector
  char **namesFull; // all names (without feature selection)
  // NOTE: names and namesFull point to the same array, if no feature selection was loaded,
  //       this has to be considered when freeing the arrays! (only free one of them...)
} sOutputVecDef;
typedef sOutputVecDef * pOutputVecDef;

typedef struct {
  int n;
  int filled;
  LONG_IDX vIdx;
  FLOAT_TYPE_TIME timestamp;
  FLOAT_TYPE_TIME duration;
  pOutputVecDef metadata;
  FLOAT_TYPE_OUTPUT *data;
} sOutputVector;
typedef sOutputVector * pOutputVector;

typedef struct{
   int n;
   char **names;
} sOutputSelectionStr;  // list of names of selected features (more flexible..)
typedef sOutputSelectionStr *pOutputSelectionStr;

typedef struct{
   int nFull;   // n here is the full length of the "unselected" feature vector
   int nSel;
   int *enabled;  // flag : 0 or 1  when feature is disabled/enabled respectively
} sOutputSelectionIdx;  
typedef sOutputSelectionIdx *pOutputSelectionIdx;

//typedef struct GenOutput sGenOutput;

/* The class data structure */
typedef struct {
  int configured;       // 0 = not configured, 1 = configured, but no names , 2 = all ok
  char *streamName;     // filename, url, etc.
  FILE * filedesc;      // contains descriptor of open file, or -1 if no file is open
  int append;           // 1 : append mode, 0: no append
  
  pFeatureMemory mem;   // featureMemory, where all the features are read from  (Note: concatting features from multiple feature memories is not supported)
  float samplePeriod;   // if samplePeriod of frames is constant, then this variable is > 0, else it is 0

  char * headerString;  // header string (if applicable)
  void * header;        // generic header
  long headerLocation;  // location of header in file (usually this is 0)
  long headerSize;      // size (length) of header in bytes
  int (*writeHeader)( void * );   // callback, that is called once the names and vecsize are ready
  
  LONG_IDX vIdxSaved;   // max. index in featureMemory that has been saved  (used to determine if we have new data to save)
  LONG_IDX writePtr_vec;// current write position in file (in frames or vectors)
  LONG_IDX writePtr;    // current write position (for files...) in bytes

  int vecSize;          // total vector size in bytes
  
  int nEls;             // number of elements to save
  pElID els;            // indicies of elements to save

  sOutputVecDef outputDef;  // names and dimensionality
  sOutputVector outputVec;  // memory reserved for output vector
  int outputSelType;       // type of feature selection mechanism: 0=none, 1=index, 2=stringList
  sOutputSelectionStr outputSelStr;  // list of names of selected features (more flexible..)
  sOutputSelectionIdx outputSelIdx;  // list of indicies relative to full feature vector
    
  int userflag1;
  int userflag2;
  void * userdata1;      // user defined pointer variable
  void * userdata2;      // user defined pointer variable
  void * userdata3;      // user defined pointer variable
  void * userdata4;      // user defined pointer variable

  void * classdata;      // class data for specific output module
} sGenOutput;
typedef sGenOutput* pGenOutput;




pGenOutput genOutput_create( pGenOutput obj, pFeatureMemory mem );

// this function will be called from buildVectorDefinition, once all the names are ready
int genOutput_setHeaderCallback( pGenOutput obj, int (*_writeHeader)( pGenOutput ) );

int genOutput_loadSelection( pGenOutput obj, const char *selFile );
int genOutput_buildVector( pGenOutput obj, pOutputVector vec );
int genOutput_buildVectorDefinition( pGenOutput obj );
int genOutput_configure( pGenOutput obj, pElID els, int nEls );
int genOutput_saveFrame( pGenOutput obj, int (*_writeFrame)( pGenOutput, pOutputVector ) );
pOutputVector genOutput_getFrame( pGenOutput obj );
int genOutput_saveAll( pGenOutput obj, int (*_writeFrame)( pGenOutput, pOutputVector ) );
// sets the current save index to the last frame available in the feature memory!
int genOutput_setIdxToLastFrame( pGenOutput obj );

pGenOutput genOutput_destroyData( pGenOutput obj );
pGenOutput genOutput_destroy( pGenOutput obj );

/***** helper functions *******/
#define EL_ENABLED 1
#define EL_DISABLED 0
sElID outputObject( int level, int el, int nStart, int nEnd, int n, int enabled );
sElID outputObjectFull( int level, int el );

#endif // #ifndef __GEN_OUTPUT_H
