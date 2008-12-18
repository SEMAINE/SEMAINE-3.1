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

This class manages the temporary features ringbuffer, that is used by the feature
extractors to gain access to feature data of past frames. This avoids multiple
internal storage and recomputation of various shared feature data such as FFT
coefficients.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDEX_HPP
#define __LLDEX_HPP

extern "C" {
  #include "LLDex.h"
}
#include "input-framer.hpp"

class cLLDex {

  public:
  sLLDex _data;
  
  // constructor: create and initialise ftex struct
  // if framer != NULL, nIDs is ignored, otherwise it is used instead of the nClients value from framer
  // nHistory is a pointer to an array of size nIDs holding the number of frames to keep for each level
  cLLDex(cInputFramer &framer, int nIDs, int *nHistory ){
    LLDex_create( &_data, &(framer._data), nIDs, nHistory ); 
  }
  
  // add a frame for id/level  and return corresponding pFtexElement
  int addFrame( int level, cPcmBuffer &pcm ) {
    if ( LLDex_addFrame( &_data, level, &(pcm._data) ) != NULL ) return 1;
    else return 0;
  }
  
  // nBack == 0 : current element (= last element that was added)
  // nBack == -1 :  element at t - 1
  // ...
  pLLDexElement getHistory( int id, int nBack ) {
    return LLDex_getHistory( &_data, id, nBack );
  }

  // completely destroy LLDex obj
  ~cLLDex() { LLDex_destroyData( &_data ); }

  // low-level functions:
             
   // create an LLDex element, copy pcmBuffer and store in LLDex object
   pLLDexElement createElement( cPcmBuffer &b ) {
     return LLDex_createElement( &(b._data) );
   }

   // free element an all associated data within
   pLLDexElement destroyElement( pLLDexElement obj ) {
     return LLDex_destroyElement( obj );
   }
         
};


#endif // #ifndef __LLDEX_HPP
