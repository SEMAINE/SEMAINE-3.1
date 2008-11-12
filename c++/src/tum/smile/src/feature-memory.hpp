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

This class manages the feautre memory with multiple levels of different capacities

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __FEATURE_MEMORY_HPP
#define __FEATURE_MEMORY_HPP

extern "C" {
  #include "feature-memory.h"
}

class cFtIdentifier {
  public:
       sFtIdentifier _data;
       
       cFtIdentifier() {
         featureMemory_initFtID(&(_data));                
       } 

       cFtIdentifier(int level) {
         featureMemory_initFtID(&(_data));                
         _data.level = level;
       } 
       
       int found() { return _data.found; }
       
       ~cFtIdentifier() {}
};

class cFeatureMemory {

  
  public:
    sFeatureMemory _data;

    cFeatureMemory(int defaultBufferType, int nLevels) {
      featureMemory_create( &_data, defaultBufferType, nLevels );   
    }

    cFeatureMemory(int nLevels) {
      featureMemory_create( &_data, FTMEM_FIXED_BUFFER, nLevels );   
    }

    inline int configureLevel( int level, int capacity, int bufferType, float T, int nFtEls, int prevLevel) {
      return featureMemory_configureLevel( &_data, level, capacity, bufferType, T, nFtEls, prevLevel);
    }
    
    inline int setNFtEls( int level, int nFtEls ) {
      return featureMemory_setNFtEls( &_data, level, nFtEls );
    }
    
    inline int dataMemAlloc( ) {
      return featureMemory_dataMemAlloc( &_data );
    }
    inline int dataMemAlloc( int level ) {
      return featureMemory_dataMemAllocLevel( &_data , level );
    }
    inline int dataMemAllocLevel( int level ) {
      return featureMemory_dataMemAllocLevel( &_data , level );
    }

    inline int dataMemFree( ) {
      return featureMemory_dataMemFree( &_data );
    }

    // nDyn is the number of basenames if the number of values is dynamic
    inline int setupElement( int level, int el, int nVal, int nDyn, char **names, int enabled ) {
      return featureMemory_setupElement( &_data, level, el, nVal, nDyn, names, enabled );
    }

    inline int setupElement( int level, int el, int nVal, int nDyn, char **names ) {
      return featureMemory_setupElement( &_data, level, el, nVal, nDyn, names, 1 );
    }
    
    // build names... if possible (due to dynmaic elements it may not be possible when no frames are present...)
    inline int buildNames( int level, int el ) {
      return featureMemory_buildNames( &_data, level, el );
    }

    pFtMemElement getElement( int level, LONG_IDX vIdx, int el ) {
      return featureMemory_getElement( &_data, level, vIdx, el );
    }
      
    pFtMemElement getElementFRep( pFeatureMemory obj, int level, LONG_IDX vIdx, int el ) {
      return featureMemory_getElementFRep( &_data, level, vIdx, el );
    }
    
    inline LONG_IDX getSpecialIndex( int level, LONG_IDX vIdxConst ) {
      return featureMemory_getSpecialIndex( &_data, level, vIdxConst );
    }

    inline FLOAT_TYPE_TIME vIdxToTime ( int level, int vIdx ) {
      return featureMemory_vIdxToTime ( &_data, level, vIdx );
    }
    
    // find a feature by name, give known values of level, el, n in ftID, otherwise set to -1
    inline int findFeature(const char* name, cFtIdentifier &ftID) {
      return featureMemory_findFeature(&_data, name, &(ftID._data));
    }

// returns pointer to 1 or more (ftID.nVals) features 
// relIdx is relative to the current Index (allFilledIdx)
    inline FLOAT_TYPE_FTMEM * getFeaturesByID(cFtIdentifier &ftID, LONG_IDX relIdx) {
      return featureMemory_getFeaturesByID(&_data, &(ftID._data), relIdx);
    }
    inline FLOAT_TYPE_FTMEM * getFeaturesByID(cFtIdentifier &ftID) { // relIdx = 0
      return featureMemory_getFeaturesByID(&_data, &(ftID._data), 0);
    }

    
    // vIdx can be -1 for currentAllFilledIndex  and -2 for currentReadIndex
    // slow get feature functions...  level0: direct (raw) indexing (always works)
    inline FLOAT_TYPE_FTMEM getFeature( int level, LONG_IDX vIdx, int el, int n ) {
      return featureMemory_getFeature( &_data, level, vIdx, el, n );
    }

    // level1 : two dimensional indexing (i.e. low level feature index + index of functional)
    //   only works if for the requested level the variable prevLevels == 1
    inline FLOAT_TYPE_FTMEM getFeature( int level, LONG_IDX vIdx, int el, int n0, int n1  ) {
      return featureMemory_getFeatureL1( &_data, level, vIdx, el, n0, n1 );
    }

    // level2 : three dimensional indexing (for hierarchical functionals)
    //   only works if for the requested level the variable prevLevels == 2
    inline FLOAT_TYPE_FTMEM getFeature( int level, LONG_IDX vIdx, int el, int n0, int n1, int n2  ) {
      return featureMemory_getFeatureL2( &_data, level, vIdx, el, n0, n1, n2 );
    }
    

    // faster way: get feature indicies once, then always retrieve the features directly from the elements
    //int featureMemory_getFeatureIndexL0( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n );
    // level0 not required, because result == n
    // level1 : two dimensional indexing (i.e. low level feature index + index of functional)
    inline int getFeatureIndex( int level, int el, int n0, int n1 ) {
      return featureMemory_getFeatureIndexL1( &_data, level, el, n0, n1 );
    }

    // level2 : three dimensional indexing (for hierarchical functionals)
    inline int getFeatureIndex( int level, int el, int n0, int n1, int n2 ) {
      return featureMemory_getFeatureIndexL2( &_data, level, el, n0, n1, n2 );
    }

    // the names returned by these functions are the same for all three functions, only the referencing of the features is different
    // get feature name functions...  level0: direct (raw) indexing (always works)
    inline char * getFeatureName( int level, int el, int n ) {
      return featureMemory_getFeatureName( &_data, level, el, n );           
    }

    // level1 : two dimensional indexing (i.e. low level feature index + index of functional)
    //   only works if for the requested level the variable prevLevels == 1
    inline char * getFeatureName( int level, int el, int n0, int n1 ) {
      return featureMemory_getFeatureNameL1( &_data, level, el, n0, n1 );
    }

    // level2 : three dimensional indexing (for hierarchical functionals)
    //   only works if for the requested level the variable prevLevels == 2
    inline char * getFeatureName( int level, int el, int n0, int n1, int n2 ) {
      return featureMemory_getFeatureNameL2( &_data, level, el, n0, n1, n2 );           
    }

    // allocate memory for frame <idx>:
    inline int allocFrame( int level, int vIdx) {
      return featureMemory_allocFrame( &_data, level, vIdx); 
    }
    
// allocate memory for subsequent frame, increase current frame counter
    inline int addFrame( int level) {
      return featureMemory_addFrame( &_data, level);
    }
    
//pFtMemFrame featureMemory_freeFrame( pFtMemFrame obj );
//pFtMemFrame featureMemory_getFrame( pFeatureMemory obj, int level, LONG_IDX absIdx );


// functions to return the size of the structures without directly having to access the featureMemory object
//char *featureMemory_getFeatureName( pFeatureMemory mem, int level, int el, int n );
    inline int getNLevels( pFeatureMemory mem ) {
      return featureMemory_getNLevels( &_data );
    }
    
    inline int getNFtEls( int level ) {
      return featureMemory_getNFtEls( &_data, level );
    }
    
    inline int getNFeatures( int level, int el ) {
      return featureMemory_getNFeatures( &_data, level, el );
    }
    
    inline int getNFeatures_idx( pFeatureMemory mem, int level, int el, LONG_IDX vIdx ) {
      return featureMemory_getNFeatures_idx( &_data, level, el, vIdx );
    }

    inline int getNFeaturesPerFrame( int level ) {
      return featureMemory_getNFeaturesPerFrame( &_data, level );
    }

    //FLOAT_TYPE_FTMEM * featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal, int relidx );
    // allocate and return memory for new values in a feature memory element
    inline FLOAT_TYPE_FTMEM * getValuesForWrite_abs( int level, int el, int nVal, LONG_IDX vIdx, FLOAT_TYPE_TIME timestamp ) {
      return featureMemory_getValuesForWrite_abs( &_data, level, el, nVal, vIdx, timestamp );
    }

    inline FLOAT_TYPE_FTMEM * getValuesForWrite_seq( int level, int el, int nVal, LONG_IDX relidx, FLOAT_TYPE_TIME timestamp ) {
      return featureMemory_getValuesForWrite_seq( &_data, level, el, nVal, relidx, timestamp );
    }
    
    // check the fill status of the feature Memory level <level> and update the status
    inline int checkStatus( int level ) {
      return featureMemory_checkStatus( &_data, level );
    }

    ~cFeatureMemory() { featureMemory_destroyData( &_data ); }
};


        

#endif // #ifndef __FEATURE_MEMORY_H
