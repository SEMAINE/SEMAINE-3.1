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
 
#ifndef __CSV_OUTPUT_HPP
#define __CSV_OUTPUT_HPP



extern "C" {
  #include "CSVoutput.h"
}
#include "feature-memory.hpp"
#include "GENoutput.hpp"


class cCsvOutput : public cGenOutput {

  public:
    // _data is declared in base class cGenOutput !!
    
    cCsvOutput( cFeatureMemory &mem, const char * filename ) : cGenOutput(mem) {
      csvOutput_create( (pCsvOutput)(&_data), &(mem._data), filename );            
    }
    
    int setOutputFile( const char *filename, int append ) {
      return csvOutput_setOutputFile( (pCsvOutput)(&_data), filename, append );
    }
    
    
    // the saveFrame functions must be mapped to according c functions of the specific class
    // do NOT use the inherited saveFrame methods! They do not pass the callback reference
    // to the writeFrame method!
    
    virtual inline int saveFrame() {
      return csvOutput_saveFrame( (pCsvOutput)(&_data) );
    }

    virtual inline int saveAll() {
      return csvOutput_saveAll( (pCsvOutput)(&_data) );
    }

    virtual inline int closeOutputFile() {
      return csvOutput_closeOutputFile( (pCsvOutput)(&_data) );
    }
    
    virtual inline int reopenOutputFile() {
      return csvOutput_reopenOutputFile( (pCsvOutput)(&_data) );
    }
    
    ~cCsvOutput () {  csvOutput_destroyData( (pCsvOutput)(&_data) ); }
};


#endif // #ifndef __CSV_OUTPUT_HPP


