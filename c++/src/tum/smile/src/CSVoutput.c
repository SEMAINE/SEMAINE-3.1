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
 
 /*  IMPLEMENTATION of the class <CSVoutput>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "CSVoutput"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "featum_util.h"       //( optional )
#include "CSVoutput.h"

#undef FUNCTION     // use undef only if you define it below for every function


int csvOutput_buildAndWriteHeader( pGenOutput obj );

/* Constructor: creates a class data object */
// usage: pClassName testObj = NULL;
//        testObj = testClass_create( testObj, ... );
pCsvOutput csvOutput_create( pCsvOutput obj, pFeatureMemory mem, const char *filename )
#define FUNCTION "csvOutput_create"
{_FUNCTION_ENTER_

  obj = (pCsvOutput)genOutput_create((pGenOutput)obj, mem);

  if (obj != NULL) {
    obj->filedesc = NULL;
    obj->header = NULL;
    obj->headerLocation = 0;
    if (filename != NULL) {
      csvOutput_setOutputFile(obj, filename, 0);  // append not supported here, because dim is not known
    } 
    genOutput_setHeaderCallback( (pGenOutput)obj, csvOutput_buildAndWriteHeader );
  }
  _FUNCTION_RETURN_(obj);
}
#undef FUNCTION 


// configure or change the current output file  (append=1 : append to file instead of overwriting)
int csvOutput_setOutputFile( pCsvOutput obj, const char *filename, int append )
#define FUNCTION "csvOutput_setOutputFile"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(filename != NULL)) {
    if (obj->streamName) free(obj->streamName);
    obj->streamName = strdup(filename);
    if (obj->filedesc != NULL) {
      fclose(obj->filedesc); obj->filedesc = NULL;
    }

    if (!append) {
      obj->filedesc = fopen(filename,"w");
      if (obj->filedesc == NULL) {
        FEATUM_ERROR(0,"Error opening output feature file %s for writing (append=%i)",filename,append);
        _FUNCTION_RETURN_(0);
      }   
      obj->writePtr_vec = 0;
      
      _FUNCTION_RETURN_(1);
    } else {  // check if vectorsize is the same AND determine current file position
           /* append not yet fully supported: sanity checks (vector dimensionality ...) are missing */
      int ret = csvOutput_reopenOutputFile(obj);
      _FUNCTION_RETURN_(ret);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


int csvOutput_buildAndWriteHeader( pGenOutput obj )
#define FUNCTION "csvOutput_buildAndWriteHeader"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    int i,j; int sPos=0;
    obj->headerString = (char *)calloc(1,sizeof(char)*obj->outputDef.n * (obj->outputDef.maxNameLength+4) + 2);
    if (obj->headerString == NULL) _FUNCTION_RETURN_(0)

    strcpy( obj->headerString, "timestamp" ); sPos = 9; // length of "timestamp" string
    obj->headerString[sPos++] = CSV_DELIM_CHAR;       

    for (i=0; i<obj->outputDef.n; i++) {
      //append string:
      for (j=0; j<strlen(obj->outputDef.names[i]); j++) {
        obj->headerString[sPos++] = obj->outputDef.names[i][j];
      }
     // append CSV delimiter
     obj->headerString[sPos++] = CSV_DELIM_CHAR;       
    }
    obj->headerString[--sPos] = 0;

    fprintf(obj->filedesc,"%s\n",obj->headerString); // csv header line
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

//configure the elements and levels to be saved (i.e. configure output feature vector)
int csvOutput_configure( pCsvOutput obj, pElID els, int nEls )
#define FUNCTION "csvOutput_configure"
{_FUNCTION_ENTER_
  int ret;
  ret = genOutput_configure( (pGenOutput)obj, els, nEls );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 


int csvOutput_writeFrame( pGenOutput obj, pOutputVector vec )
#define FUNCTION "csvOutput_writeFrame"
{_FUNCTION_ENTER_

  if ((obj != NULL) && (vec != NULL)) {
    int i;
    
    fprintf(obj->filedesc,"%f%c",vec->timestamp,CSV_DELIM_CHAR); // timestamp
    for (i=0; i < vec->n - 1 ; i++) {
      fprintf(obj->filedesc,"%f%c",vec->data[i],CSV_DELIM_CHAR);      
    }
    fprintf(obj->filedesc,"%f\n",vec->data[i],CSV_DELIM_CHAR);
    
    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// save next frame
int csvOutput_saveFrame( pCsvOutput obj )
#define FUNCTION "csvOutput_saveFrame"
{_FUNCTION_ENTER_
  int ret;
  ret = genOutput_saveFrame( obj, csvOutput_writeFrame );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 

// save all frames up to last possible
int csvOutput_saveAll( pCsvOutput obj )
#define FUNCTION "csvOutput_saveAll"
{_FUNCTION_ENTER_
  int ret;
  ret = genOutput_saveAll( obj, csvOutput_writeFrame );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 

// closes the current output file, subsequent files to saveFrame will fail
// use this function only if you have to close the file before destroying the object
int csvOutput_closeOutputFile( pCsvOutput obj )
#define FUNCTION "csvOutput_closeOutputFile"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->filedesc != NULL) {
      fclose(obj->filedesc); obj->filedesc = NULL;
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// reopens the current output file (only if it has been previously closed with closeOutputFile)
// sets write pointer to end of file
int csvOutput_reopenOutputFile( pCsvOutput obj )
#define FUNCTION "csvOutput_reopenOutputFile"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if ((obj->filedesc == NULL)&&(obj->streamName != NULL)) {
      obj->filedesc = fopen(obj->streamName, "a+"); 
      if (obj->filedesc != NULL) {
        _FUNCTION_RETURN_(1);
      }
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

/* Destructor: frees the class data object, closes the output file and returns NULL */
pCsvOutput csvOutput_destroyData( pCsvOutput obj )
#define FUNCTION "csvOutput_destroyData"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    csvOutput_closeOutputFile( obj );
    genOutput_destroyData( (pGenOutput)obj );
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

/* Destructor: frees the class data object, closes the output file and returns NULL */
pCsvOutput csvOutput_destroy( pCsvOutput obj )
#define FUNCTION "csvOutput_destroy"
{_FUNCTION_ENTER_
  csvOutput_destroyData(obj);
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 




