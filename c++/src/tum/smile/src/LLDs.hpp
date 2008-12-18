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

LLD wrapper class, manages all LLD extractors

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __LLDS_HPP
#define __LLDS_HPP

extern "C" {
  #include "LLDs.h"
}
#include "LLDex.hpp"
#include "feature-memory.hpp"

//????
//extern sLLDs_const LLD;  // declared in extractors-globalconfig.c

class cLLDs {

  
  public:
  sLLDs _data;
         cLLDs(cLLDex &l, int freeLLDobjs) {
           LLDs_create( &_data, &(l._data), 0, freeLLDobjs );
         }

         // enable LLD "name", and register extractor obj. if obj = NULL it will be created by calling default constructor!
         // if level == -1 the default defined by the constants will be used
         inline void * setupLLD(const char * name, int enabled, int level) {
           return LLDs_setupLLD( &_data, name, enabled, NULL, level );
         }
         inline void * setupLLD(const char * name, int enabled, void *exObj, int level) {
           return LLDs_setupLLD( &_data, name, enabled, exObj, level );
         }
         
         inline void countEnabled() {
           LLDs_countEnabled( &_data );
         }

         inline void configureFeatureMemoryNames( cFeatureMemory &mem, int ftmem_level ) {
           LLDs_configureFeatureMemoryNames( &_data, &(mem._data), ftmem_level );
         }

         inline int createExtractors() {
           return LLDs_createExtractors( &_data );
         }

         inline int callExtractors(int level) {
           return LLDs_callExtractors( &_data, level );
         }
         
         inline int flushToFtMem( cFeatureMemory &mem, int level ) {
           return LLDs_flushToFtMem( &_data, &(mem._data), level );
         }

         inline int checkDependencies() {
           return LLDs_checkDependencies( &_data );
         }

         inline int setupFeatureMemLevels( cFeatureMemory &mem, cInputFramer &framer, int *framerIDs, LONG_IDX *capacity ){
           return LLDs_setupFeatureMemLevels( &_data, &(mem._data), &(framer._data), framerIDs, capacity, -1 );
         }

         inline int setupFeatureMemLevels( cFeatureMemory &mem, cInputFramer &framer, int *framerIDs, LONG_IDX *capacity, int nConf ){
           return LLDs_setupFeatureMemLevels( &_data, &(mem._data), &(framer._data), framerIDs, capacity, nConf );
         }
         
         ~cLLDs() { LLDs_destroyData( &_data ); }
        
};



#endif // #ifndef __LLDS_HPP
