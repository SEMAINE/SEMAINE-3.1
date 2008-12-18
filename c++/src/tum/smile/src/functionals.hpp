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

This class computes statistical functionals operating on a featureMemory object

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __FUNCTIONALS_HPP
#define __FUNCTIONALS_HPP

extern "C" {
  #include "functionals.h"
}
#include "feature-memory.hpp"

class cFunctionals {
  sFunctionals _data;
  
  public:

    cFunctionals( int nIDs ) {
      functionals_create( &_data, nIDs );
    }

    cFunctionals() {
      functionals_create( &_data, 1 );
    }


    // equivalent to functionals_autoCreate
    cFunctionals( cFeatureMemory &mem ) {
      functionals_create( &_data, mem._data.ll_levels );
    }

    int enable(const char *name, int *id_list) {
      return functionals_enable( &_data, name, id_list);
    }

    // initialise functionals extractor <id>  (id = 0...nIDs-1)
    // initialise the output level <olevel> in the feature memory
    // if capacity == -1 then the capacity will be automatically derived from the input level capacity
    inline int setupID( cFeatureMemory &mem, int id, int olevel, int ilevel, float T, float computeframeSize, int capacity ) {
      return functionals_setupID( &_data, &(mem._data), id, olevel, ilevel, T, computeframeSize, capacity );
    }

    // execute this before(!) calling any of the setupOutputLevels functions
    inline int finalizeConfiguration( ) {
      return functionals_finalizeConfiguration( &_data );
    }

    // initialise the output levels in the feature memory automatically based on parameters of the lld levels
    inline int autoSetupIDs( cFeatureMemory &mem, float *T, float *fs ) {
      return functionals_autoSetupIDs( &_data, &(mem._data), T, fs );
    }


    // compute the functionals on level <ilevel> of feature memory <mem>
    // save functionals to olevel in feature memory mem
    // when this function is called, it computes all functionals possible, up to current idx in ilevel
    inline int compute( int id ) {
      return functionals_compute( &_data, id );
    }

    // this function computes functionals only for curIdx in ilevel
    inline int computeCurrentOnly( int id ) {
      return functionals_computeCurrentOnly( &_data, id );
    }
    

    // ignore frameSize , compute functionals right now
    inline int computeNow( int id, int fmin_frames ) {
      return functionals_computeNow( &_data, id, fmin_frames );
    }

    // set frame start for computeNow() function to current position
    inline int setFrameStart( int id ) {
      return functionals_setFrameStart( &_data, id, 0 );       
    }

    // set frame start for computeNow() function to current position +- relIdx
    inline int setFrameStart( int id, LONG_IDX relIdx ) {
      return functionals_setFrameStart( &_data, id, relIdx );
    }
    
    // compute all functionals for all input levels, automatically map output levels
    inline int autoCompute() {
      return functionals_autoCompute( &_data );
    }

    ~cFunctionals() { functionals_destroyData( &_data ); }
      
};


#endif // #ifndef __FUNCTIONALS_HPP
