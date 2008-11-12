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




#ifndef __ARFF_OUTPUT_HPP
#define __ARFF_OUTPUT_HPP

extern "C" {
  #include "ARFFoutput.h"
}
#include "feature-memory.hpp"
#include "GENoutput.hpp"


class cArffOutput : public cGenOutput {

  public:
    // _data is declared in base class cGenOutput !!
    
    cArffOutput( cFeatureMemory &mem, const char * filename ) : cGenOutput(mem) {
      arffOutput_create( (pArffOutput)(&_data), &(mem._data), filename, 0 );            
    }

    cArffOutput( cFeatureMemory &mem, const char * filename, int append ) : cGenOutput(mem) {
      arffOutput_create( (pArffOutput)(&_data), &(mem._data), filename, append );            
    }
    
    int setOutputFile( const char *filename, int append ) {
      return arffOutput_setOutputFile( (pArffOutput)(&_data), filename, append );
    }
    
    int setClassesList( const char *classlist ) {
      return arffOutput_setClassesList( &_data, classlist );
    }
    
    // the saveFrame functions must be mapped to according c functions of the specific class
    // do NOT use the inherited saveFrame methods! They do not pass the callback reference
    // to the writeFrame method!
    
    inline int saveFrame() {
      return arffOutput_saveFrame( (pArffOutput)(&_data) );
    }

    inline int saveFrameExt( const char *label ) {
      return arffOutput_saveFrameExt( (pArffOutput)(&_data), label, NULL );
    }

    inline int saveFrameExt( const char *label, const char *instance ) {
      return arffOutput_saveFrameExt( (pArffOutput)(&_data), label, instance );
    }
    
    inline int saveAll() {
      return arffOutput_saveAll( (pArffOutput)(&_data) );
    }
    

    int closeOutputFile() {
      return arffOutput_closeOutputFile( (pArffOutput)(&_data) );
    }
    
    int reopenOutputFile() {
      return arffOutput_reopenOutputFile( (pArffOutput)(&_data) );
    }
    
    ~cArffOutput () {  arffOutput_destroyData( (pArffOutput)(&_data) ); }
};


#endif // #ifndef __ARFF_OUTPUT_HPP
