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
 
 /*  IMPLEMENTATION of the class <HTKoutput>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "HTKoutput"
 
 // this file contains the function definitions and the private class data

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "HTKoutput.h"

#undef FUNCTION     // use undef only if you define it below for every function


// various helper functions


int IsVAXOrder ()
{
	short x;
	unsigned char *pc;
	pc = (unsigned char *) &x;
	*pc = 1; *( pc + 1 ) = 0;			// store bytes 1 0
	return x==1;					// does it read back as 1?
}

void Swap32 ( uint32_t *p )
{
	uint8_t temp,*q;
	q = (uint8_t*) p;
	temp = *q; *q = *( q + 3 ); *( q + 3 ) = temp;
	temp = *( q + 1 ); *( q + 1 ) = *( q + 2 ); *( q + 2 ) = temp;
}

void Swap16 ( uint16_t *p )
{
	uint8_t temp,*q;
	q = (uint8_t*) p;
	temp = *q; *q = *( q + 1 ); *( q + 1 ) = temp;
}

void SwapFloat( float *p )
{
	uint8_t temp,*q;
	q = (uint8_t*) p;
	temp = *q; *q = *( q + 3 ); *( q + 3 ) = temp;
	temp = *( q + 1 ); *( q + 1 ) = *( q + 2 ); *( q + 2 ) = temp;
}

void prepareHeader( pHTKheader h )
{
  if ( IsVAXOrder () )
  {
    Swap32 ( &(h->nSamples) );
    Swap32 ( &(h->samplePeriod) );
    Swap16 ( &(h->sampleSize) );
    Swap16 ( &(h->parmKind) );
  }     
}

// class implementation:

pHtkOutput htkOutput_create( pHtkOutput obj, pFeatureMemory mem, const char *filename )
#define FUNCTION "htkOutput_create"
{_FUNCTION_ENTER_

  obj = (pHtkOutput)genOutput_create((pHtkOutput)obj, mem);

  if (obj != NULL) {
    obj->filedesc = NULL;
    obj->header = NULL;
    obj->headerLocation = 0;
    if (filename != NULL) {
      htkOutput_setOutputFile(obj, filename, 0);  // append not supported here, because dim is not known
    } 
    genOutput_setHeaderCallback( (pGenOutput)obj, NULL );
    if ( IsVAXOrder () ) { obj->userflag1 = 1; }
    
    obj->header = (pHTKheader)calloc(1,sizeof(sHTKheader));
    if (obj->header == NULL) {
      FEATUM_ERROR(1,"error allocating memory for HTK header");
    }
  }
  _FUNCTION_RETURN_(obj);
}
#undef FUNCTION 

// configure or change the current output file  (append=1 : append to file instead of overwriting)
int htkOutput_setOutputFile( pHtkOutput obj, const char *filename, int append )
#define FUNCTION "htkOutput_setOutputFile"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(filename != NULL)) {
    if (obj->streamName) free(obj->streamName);
    obj->streamName = strdup(filename);
    if (obj->filedesc != NULL) {
      fclose(obj->filedesc); obj->filedesc = NULL;
    }

    if (obj->header == NULL) {
      FEATUM_ERROR(2,"memory for HTK header is not allocated! Cannot set output file!");
      _FUNCTION_RETURN_(0);                
    }
      
    if (!append) {
      obj->filedesc = fopen(filename,"wb");
      if (obj->filedesc == NULL) {
        FEATUM_ERROR(0,"Error opening output feature file %s (append=%i)",filename,append);
        _FUNCTION_RETURN_(0);
      }   
      obj->writePtr_vec = 0;
      pHTKheader h = (pHTKheader)(obj->header);
      h->nSamples = 0;
      h->samplePeriod = 0;  // TODO: sample period
      h->sampleSize = (uint16_t)(obj->outputDef.n);
      h->parmKind = 9;  // TODO: support others like MFCC, MELSPEC, etc.
      // adjust endianness
      sHTKheader header;  // local copy, due to prepareHeader!
                          // obj->header should remain in host byte order!
      memcpy(&header, obj->header, sizeof(sHTKheader));
      prepareHeader(&header);
      // write header:
      if (!fwrite(&header, sizeof(sHTKheader), 1, obj->filedesc)) {
        FEATUM_ERROR(1,"Error writing to feature file %s!",filename);                         
        _FUNCTION_RETURN_(0);
      }
      _FUNCTION_RETURN_(1);
    } else {  // check if vectorsize is the same AND determine current file position
      int ret = htkOutput_reopenOutputFile(obj);
      _FUNCTION_RETURN_(ret);
    }
    
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

//configure the elements and levels to be saved (i.e. configure output feature vector)
int htkOutput_configure( pHtkOutput obj, pElID els, int nEls )
#define FUNCTION "htkOutput_configure"
{_FUNCTION_ENTER_
  int ret;
  ret = genOutput_configure( (pGenOutput)obj, els, nEls );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 


int htkOutput_writeFrame( pGenOutput obj, pOutputVector vec )
#define FUNCTION "htkOutput_writeFrame"
{_FUNCTION_ENTER_

  if ((obj != NULL) && (vec != NULL)) {
    if (vec->data == NULL) { _FUNCTION_RETURN_(0); }
    int i;
    
    float * tmp = (float *)calloc(1,sizeof(float)*vec->n);
    if (tmp == NULL) {
      FEATUM_ERROR(1,"error allocating memory");
      _FUNCTION_RETURN_(0);        
    }
    for (i=0; i<vec->n; i++) {
      tmp[i] = (float)vec->data[i];
      if ( obj->userflag1 ) { // VAX byte order
        SwapFloat(&(tmp[i]));
      }
    }

    size_t written = fwrite(tmp,sizeof(float),vec->n,obj->filedesc);
    free(tmp);
    if (written != vec->n) {
      FEATUM_ERROR(0,"Error writing frame to output feature file! Disk full?");
      _FUNCTION_RETURN_(0);            
    }
    
    _FUNCTION_RETURN_(1);
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// save next frame
int htkOutput_saveFrame( pHtkOutput obj )
#define FUNCTION "htkOutput_saveFrame"
{_FUNCTION_ENTER_
  int ret;
  ret = genOutput_saveFrame( obj, htkOutput_writeFrame );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 

// save all frames up to last possible
int htkOutput_saveAll( pHtkOutput obj )
#define FUNCTION "htkOutput_saveAll"
{_FUNCTION_ENTER_
  int ret;
  ret = genOutput_saveAll( obj, htkOutput_writeFrame );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 


// closes the current output file, subsequent files to saveFrame will fail
// use this function only if you have to close the file before destroying the object
int htkOutput_closeOutputFile( pHtkOutput obj )
#define FUNCTION "htkOutput_closeOutputFile"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (obj->filedesc != NULL) {
      // update header with updated nSamples
      if (obj->header != NULL) {
        pHTKheader h = (pHTKheader)(obj->header);
        h->nSamples = obj->writePtr_vec;
        h->samplePeriod = (uint32_t)(round(obj->samplePeriod * 1000.0))*10000; // convert to 100ns HTK time-units
        h->sampleSize = obj->outputDef.n * (sizeof(float));
        prepareHeader( h );
        fseek(obj->filedesc, 0, SEEK_SET); // seek to beginning of file
        fwrite( obj->header, sizeof(sHTKheader), 1, obj->filedesc );
      }
      fclose(obj->filedesc); obj->filedesc = NULL;
      memzero(obj->header,sizeof(sHTKheader));
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// reopens the current output file (only if it has been previously closed with closeOutputFile)
// sets write pointer to end of file
int htkOutput_reopenOutputFile( pHtkOutput obj )
#define FUNCTION "htkOutput_reopenOutputFile"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    sHTKheader header;
    if ((obj->filedesc == NULL)&&(obj->streamName != NULL)) {
      memzero(obj->header,sizeof(sHTKheader));
      obj->filedesc = fopen(obj->streamName, "ab+"); 
      if (obj->filedesc != NULL) {
       // seek to beginning, read header
       fseek(obj->filedesc, 0, SEEK_SET);
       if (fread(&header,sizeof(sHTKheader),1,obj->filedesc) != sizeof(sHTKheader) ) {
         FEATUM_ERROR(0,"Error reading header from feature file! (append=1)");
         fclose(obj->filedesc); obj->filedesc = NULL;
         _FUNCTION_RETURN_(0);
       } else {
         prepareHeader(&header);
         if (header.sampleSize != obj->outputDef.n) {
           FEATUM_ERROR(1,"cannot append to a file with different vector size (file: %i, output: %i)", header.sampleSize, obj->outputDef.n);
           fclose(obj->filedesc); obj->filedesc = NULL;
           _FUNCTION_RETURN_(0);           
         } else {
           fseek(obj->filedesc, 0, SEEK_END);
           memcpy(obj->header,&header,sizeof(sHTKheader));
         }  
         obj->writePtr_vec = header.nSamples;
       }
      }
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

/* Destructor: frees the class data object, closes the output file and returns NULL */
pHtkOutput htkOutput_destroyData( pHtkOutput obj )
#define FUNCTION "htkOutput_destroyData"
{_FUNCTION_ENTER_

  htkOutput_closeOutputFile( obj );
  genOutput_destroyData( (pGenOutput)obj );

  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

/* Destructor: frees the class data object, closes the output file and returns NULL */
pHtkOutput htkOutput_destroy( pHtkOutput obj )
#define FUNCTION "htkOutput_destroy"
{_FUNCTION_ENTER_

  htkOutput_destroyData( obj );
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 




