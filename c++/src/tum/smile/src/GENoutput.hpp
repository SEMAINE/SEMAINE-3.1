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


/*
How to use the output module to write a custom pure c++ output plugin:
   1. Build a class that is derived from cGenOutput
    
    class cMyOutput : public cGenOutput {
           cMyOutput( cFeatureMemory &mem, .... ) : cGenOutput(mem) { ... }
           
           ~cMyOutput () { }
    };


How to write a C output plugin (optionally with a c++ interface):
    implement the usual functions required for c objects:
              pXXXOutput objectName_create ( pXXXOutput obj, pFeatureMemory mem, .... );
              objectName_destroy
              objectName_destroyData
    then write functions for opening and closing the output file/stream/etc.
    
    define a callback function:
      int objectName_buildAndWriteHeader( pGenOutput obj )
      
      this function is called whenever the file header 
      (feature names and vector dimensionality) is available.
      No output has (and should not) been written to the output file so far.
      This function should read (pGenOutput)obj->outputDef and write or create
      a header which may be stored in obj->headerString (for string headers)
      or as obj->header [void *] for custom data structs.
      ( be sure to free any memory allocated within the custom data struct )
      
    define a callback function: 
      int objectName_writeFrame( pGenOutput obj, pOutputVector vec )
      
      this is called from genOutput_saveFrame, whenever data is ready to be saved
      
    then add the following generic functions:
      //configure the elements and levels to be saved (i.e. configure output feature vector)
      int ..._configure( pCsvOutput obj, pElID els, int nEls )
      #define FUNCTION "csvOutput_configure"
      {_FUNCTION_ENTER_
        int ret;
        ret = genOutput_configure( (pGenOutput)obj, els, nEls );
        _FUNCTION_RETURN_(ret);
      }
      #undef FUNCTION 
         
         // save next frame
         int ..._saveFrame( pCsvOutput obj )
         #define FUNCTION "csvOutput_saveFrame"
         {_FUNCTION_ENTER_
           int ret;
           ret = genOutput_saveFrame( obj, objectName_writeFrame );
           _FUNCTION_RETURN_(ret);
         }
         #undef FUNCTION 

         // save all frames up to last possible
         int ..._saveAll( pCsvOutput obj )
         #define FUNCTION "csvOutput_saveAll"
         {_FUNCTION_ENTER_
           int ret;
           ret = genOutput_saveAll( obj, objectName_writeFrame );
           _FUNCTION_RETURN_(ret);
         }
         #undef FUNCTION 

   in the destructor ..._destroyData call the genOutput destructor:
          genOutput_destroyData( (pGenOutput)obj );

   in the constructor ..._create  you must set the header callback, if you
   are interested in header information:
       genOutput_setHeaderCallback( (pGenOutput)obj, csvOutput_buildAndWriteHeader );
 
   further, the first line of the constructor must be a call to the genOutput
   constructor:
       obj = (pCsvOutput)genOutput_create((pGenOutput)obj, mem);
       
       
   optionally, write a c++ header with a dummy class as interface to the functions you created.
   the class should be derived from cGenOutput
   
*/ 

#ifndef __GEN_OUTPUT_HPP
#define __GEN_OUTPUT_HPP

extern "C" {
  #include "GENoutput.h"
}
#include "feature-memory.hpp"

class cGenOutput {
  
  public:
    sGenOutput _data;

    // TODO: in derived classes genOutput_create is called twice.... solve this!
    cGenOutput( cFeatureMemory &mem ) {
       genOutput_create( &_data, &(mem._data) );
    }

    int loadSelection( const char *selFile ) {
      return genOutput_loadSelection( &_data, selFile );
    }
    
    // this function will be called from buildVectorDefinition, once all the names are ready
    virtual void setHeaderCallback( int (*_writeHeader)( pGenOutput ) ) {
      genOutput_setHeaderCallback( &_data, _writeHeader );
    }


//    int genOutput_buildVector( pGenOutput obj, pOutputVector vec );
//    int genOutput_buildVectorDefinition( pGenOutput obj );

    virtual int configure( pElID els, int nEls ) {
      return genOutput_configure( &_data, els, nEls );
    }

    virtual int saveFrame( int (*_writeFrame)( pGenOutput, pOutputVector ) ) {
      return genOutput_saveFrame( &_data, _writeFrame );
    }
    
    int setIdxToLastFrame( ) {
      return genOutput_setIdxToLastFrame( &_data );
    }
    pOutputVector getFrame() {
      return genOutput_getFrame( &_data );     
    }
    
    virtual int saveAll(  int (*_writeFrame)( pGenOutput, pOutputVector ) ) {
      return genOutput_saveAll( &_data, _writeFrame );
    }

    
    virtual ~cGenOutput() { genOutput_destroyData( &_data );  }
};



#endif // #ifndef __GEN_OUTPUT_HPP
