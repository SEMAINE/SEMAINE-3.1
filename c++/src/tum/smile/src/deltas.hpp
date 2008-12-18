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

Computation of first order differentials
Takes an input level, clones it , and outputs the differentials for all the enabled elements
in the output level.
Higher order differentials can be computed by subsequently applying this class.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __DELTAS_HPP
#define __DELTAS_HPP

extern "C" {
  #include "deltas.h"
}
#include "feature-memory.hpp"

// all times and sizes are in floating point with unit seconds

class cDeltas {
  sDeltas _data;
  
  public:

         
    cDeltas( int nIDs ) {
      deltas_create( &_data, nIDs );
    }

    cDeltas() {
      deltas_create( &_data, 1 );
    }

    // initialise functionals extractor <id>  (id = 0...nIDs-1)
    // initialise the output level <olevel> in the feature memory
    // if capacity == -1 then the capacity will be automatically derived from the input level capacity
    inline int setupID( cFeatureMemory &mem, int id, int olevel, int ilevel, int nFramesContext ) {
      return deltas_setupID( &_data, &(mem._data), id, olevel, ilevel, nFramesContext );
    }

    // compute the deltas on level <ilevel> of feature memory <mem>
    // save functionals to olevel in feature memory mem
    // when this function is called, it computes all deltass possible, up to current idx in ilevel
    inline int compute( int id ) {
      return deltas_compute( &_data, id );
    }
    
    // this function computes deltas only for curIdx in ilevel
    inline int computeNext( int id ) {
      return deltas_computeNext( &_data, id );
    }

    // directly compute for index oIdx
    inline int doCompute( int id, LONG_IDX oIdx ) {
      return deltas_doCompute( &_data, id, oIdx );
    }

    // compute all deltas for all ids
    inline int autoCompute() {
      return deltas_autoCompute( &_data );
    }

    // compute deltas on last (missing) frames at end of stream, 
    // for all input levels. automatically map output levels
    inline int autoComputeLast( ) {
      return deltas_autoComputeLast( &_data );
    }

    ~cDeltas() { deltas_destroyData( &_data ); }

};

#endif // #ifndef __DELTAS_HPP
