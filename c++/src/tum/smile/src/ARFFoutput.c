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
 
 /*  IMPLEMENTATION of the class <ARFFoutput>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "ARFFoutput"
 
 // this file contains the function definitions and the private class data

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "ARFFoutput.h"

#undef FUNCTION     // use undef only if you define it below for every function


int arffOutput_buildAndWriteHeader( pGenOutput obj );

/* Constructor: creates a class data object */
// usage: pClassName testObj = NULL;
//        testObj = testClass_create( testObj, ... );
pArffOutput arffOutput_create( pArffOutput obj, pFeatureMemory mem, const char *filename, int append )
#define FUNCTION "arffOutput_create"
{_FUNCTION_ENTER_

  obj = (pArffOutput)genOutput_create((pGenOutput)obj, mem);

  if (obj != NULL) {
    obj->filedesc = NULL;
    obj->header = NULL;
    obj->headerLocation = 0;
    if (filename != NULL) {
      if (file_exists(filename)) obj->append = 1;
      arffOutput_setOutputFile(obj, filename, append);  
    } 
    if (!(obj->append)) 
      genOutput_setHeaderCallback( (pGenOutput)obj, arffOutput_buildAndWriteHeader );
  }
  _FUNCTION_RETURN_(obj);
}
#undef FUNCTION 

// configure or change the current output file  (append=1 : append to file instead of overwriting)
int arffOutput_setOutputFile( pArffOutput obj, const char *filename, int append )
#define FUNCTION "arffOutput_setOutputFile"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(filename != NULL)) {
    if (obj->streamName) free(obj->streamName);
    obj->streamName = strdup(filename);
    if (obj->filedesc != NULL) {
      fclose(obj->filedesc); obj->filedesc = NULL;
    }

    if (!append) {
      obj->filedesc = fopen(filename,"w");
//printf("Not append\n");      
      if (obj->filedesc == NULL) {
        FEATUM_ERROR(0,"Error opening output feature file %s for writing (append=%i)",filename,append);
        _FUNCTION_RETURN_(0);
      }   
      obj->writePtr_vec = 0;
      
      _FUNCTION_RETURN_(1);
    } else {  // check if vectorsize is the same AND determine current file position
           /* append not yet fully supported: sanity checks (vector dimensionality ...) are missing */
//printf("appending!\n");
      int ret = arffOutput_reopenOutputFile(obj);
      _FUNCTION_RETURN_(ret);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

int arffOutput_setClassesList( pArffOutput obj, const char *classlist )
#define FUNCTION "arffOutput_setClassesList"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    obj->userdata3 = (void *)classlist;  // a compiler warning here is ok!
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

int arffOutput_buildAndWriteHeader( pGenOutput obj )
#define FUNCTION "arffOutput_buildAndWriteHeader"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    int i,j; int sPos=0;
        
    fprintf(obj->filedesc,"@relation \"feaTUM.arff\"\n\n");  // TODO : make this string configurable
    fprintf(obj->filedesc,"@attribute filename string\n");
    fprintf(obj->filedesc,"@attribute timestamp numeric\n");  // timestamp for every frame
    
    for (i=0; i<obj->outputDef.n; i++) {
      fprintf(obj->filedesc,"@attribute %s numeric\n",obj->outputDef.names[i]);
    }

    if (obj->userdata3 != NULL) {
      fprintf(obj->filedesc,"@attribute class %s\n\n",(const char *)(obj->userdata3)); // TODO: make the class list configurable
    } else {
      fprintf(obj->filedesc,"@attribute class {null}\n\n"); // TODO: make the class list configurable
    }
    fprintf(obj->filedesc,"@data\n\n");

    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 



//configure the elements and levels to be saved (i.e. configure output feature vector)
int arffOutput_configure( pArffOutput obj, pElID els, int nEls )
#define FUNCTION "arffOutput_configure"
{_FUNCTION_ENTER_
  int ret;
  ret = genOutput_configure( (pGenOutput)obj, els, nEls );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 


// userdata1:  class label
// userdata2:  instance name
int arffOutput_writeFrame( pGenOutput obj, pOutputVector vec )
#define FUNCTION "arffOutput_writeFrame"
{_FUNCTION_ENTER_

  if ((obj != NULL) && (vec != NULL)) {
    int i;

    if (obj->userdata2 != NULL) {
      fprintf(obj->filedesc,"'%s'%c",(char *)obj->userdata2,ARFF_DELIM_CHAR); // name
    } else {
      fprintf(obj->filedesc,"'%s'%c",obj->streamName,ARFF_DELIM_CHAR); // name
    }

    
    fprintf(obj->filedesc,"%f%c",vec->timestamp,ARFF_DELIM_CHAR); // timestamp
    for (i=0; i < vec->n - 1 ; i++) {
      fprintf(obj->filedesc,"%f%c",vec->data[i],ARFF_DELIM_CHAR);      
    }
    if (obj->userdata1 != NULL) {// class label
      fprintf(obj->filedesc,"%f%c%s\n",vec->data[i],ARFF_DELIM_CHAR,(char *)obj->userdata1);
    } else {
      fprintf(obj->filedesc,"%f%cnull\n",vec->data[i],ARFF_DELIM_CHAR);
    }

    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// save next frame
int arffOutput_saveFrame( pArffOutput obj )
#define FUNCTION "arffOutput_saveFrame"
{_FUNCTION_ENTER_
  int ret;
  ret = arffOutput_saveFrameExt( obj, "null", NULL );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 

// save next frame, append class information
int arffOutput_saveFrameExt( pArffOutput obj, const char *classlabel, const char *instancelabel )
#define FUNCTION "arffOutput_saveFrameExt"
{_FUNCTION_ENTER_
  int ret;
  if (obj != NULL) {
    obj->userdata1 = (void *)classlabel;
    obj->userdata2 = (void *)instancelabel;        
  }
  ret = genOutput_saveFrame( obj, arffOutput_writeFrame );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 


// save all frames up to last possible
// passing of instance name and class label is not possible here!!
int arffOutput_saveAll( pArffOutput obj )
#define FUNCTION "arffOutput_saveAll"
{_FUNCTION_ENTER_
  int ret;
  obj->userdata1 = NULL;
  ret = genOutput_saveAll( obj, arffOutput_writeFrame );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 

// closes the current output file, subsequent files to saveFrame will fail
// use this function only if you have to close the file before destroying the object
int arffOutput_closeOutputFile( pArffOutput obj )
#define FUNCTION "arffOutput_closeOutputFile"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->filedesc != NULL) {
      //fprintf(obj->filedesc,"\n");
      fclose(obj->filedesc); obj->filedesc = NULL;
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// reopens the current output file (only if it has been previously closed with closeOutputFile)
// sets write pointer to end of file
int arffOutput_reopenOutputFile( pArffOutput obj )
#define FUNCTION "arffOutput_reopenOutputFile"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if ((obj->filedesc == NULL)&&(obj->streamName != NULL)) {
      // TODO: possibly count number of lines, and update writePtr_vec ....
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
pArffOutput arffOutput_destroyData( pArffOutput obj )
#define FUNCTION "arffOutput_destroyData"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    arffOutput_closeOutputFile( obj );
    genOutput_destroyData( (pGenOutput)obj );
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

/* Destructor: frees the class data object, closes the output file and returns NULL */
pArffOutput arffOutput_destroy( pArffOutput obj )
#define FUNCTION "arffOutput_destroy"
{_FUNCTION_ENTER_
  arffOutput_destroyData(obj);
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 




