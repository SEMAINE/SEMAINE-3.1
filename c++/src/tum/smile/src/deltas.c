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
 
 /*  IMPLEMENTATION of the class <deltas>: computation of delta (differential) feature coefficients
     See the corresponding header file (.h) for documentation!  */

#define MODULE "Deltas"
 
 // this file contains the function definitions and the private class data

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "deltas.h"

#undef FUNCTION     // use this undef only if you define it below for every function


#define nDeltas 1

/* Constructor: creates a class data object */
pDeltas deltas_create( pDeltas obj, int nIDs )
#define FUNCTION "deltas_create"
{_FUNCTION_ENTER_

  if (obj == NULL) {
    obj = (pDeltas)calloc(1,sizeof(sDeltas));
  } else {
    memzero(obj,sizeof(sDeltas));       
  }
  if (obj != NULL) {
      obj->config = (pDeltasConfig)calloc(1,sizeof(sDeltasConfig)*nIDs);
      if (obj->config == NULL) { free(obj);  _FUNCTION_RETURN_(NULL); }
  }
  obj->nIDs = nIDs;
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


// initialise deltas extractor <id>  (id = 0...nIDs-1)
// initialise the output level <olevel> in the feature memory
// the capacity will be automatically derived from the input level capacity
// beginOrder and endOrder set the range of differential orders to be computed and saved
int deltas_setupID( pDeltas obj, pFeatureMemory mem, int id, int olevel, int ilevel, int nFramesContext )
#define FUNCTION "deltas_setupID"
{_FUNCTION_ENTER_
  int i;
  if ((obj != NULL)&&(mem != NULL)) {
    FEATUM_DEBUG(10,"start");
    #ifdef COMPILE_CHECKS
    if (olevel < 0) _FUNCTION_RETURN_(0);
    if (olevel >= mem->nLevels) {
      FEATUM_ERROR(1,"capacity of feature memory is too small (nLevels=%i),\n increase this in the call of featureMemory_create to at least olevel=%i!",mem->nLevels,olevel);
      _FUNCTION_RETURN_(0);
    }
    if (ilevel < 0) _FUNCTION_RETURN_(0);
    if (ilevel >= mem->ll_levels)  {
      FEATUM_WARNING(10,"ilevel >  mem->ll_levels !!  check if this is ok!");
    }
    if (ilevel >= mem->nLevels)  {
      FEATUM_ERROR(1,"ilevel (%i) > mem->nLevels (%i)",ilevel,mem->nLevels);
      _FUNCTION_RETURN_(0);
    }
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config == NULL)  _FUNCTION_RETURN_(0);    
    #endif
    FEATUM_ERROR(10,"after checks");
    obj->config[id].mem = mem;
    
    if (!featureMemory_configureLevel( mem, olevel, mem->level[ilevel].capacity, FTMEM_DEFAULT_BUFFER, mem->level[ilevel].T, mem->level[ilevel].nFtEls, ilevel)){
      FEATUM_ERROR(0,"error during featureMemory_configureLevel! (return code 0)");
      _FUNCTION_RETURN_(0);                                
    }

    mem->level[olevel].frameSize = mem->level[ilevel].frameSize;
    obj->config[id].nFramesContext = nFramesContext;
    obj->config[id].olevel = olevel;
    obj->config[id].ilevel = ilevel;

    // allocate memory in feature memory only for olevel levels (ilevel memory should have been already allocated before with info from inputFramer)

    if (!featureMemory_dataMemAllocLevel(mem,olevel)) {
      FEATUM_ERROR(0,"error during featureMemory_dataMemAllocLevel! (return code 0)");
      _FUNCTION_RETURN_(0);                                
    }


    if (obj->config[id].names == NULL) {
      obj->config[id].names = (char**)calloc(1,sizeof(char*)*nDeltas);
      for (i=0; i<nDeltas; i++) {
        obj->config[id].names[i] = (char *)calloc(1,sizeof(char)*8);
        snprintf(obj->config[id].names[i], 8, "De");
      }
    }

    // setup names in feature memory
    for (i=0; i<mem->level[olevel].nFtEls; i++) {
      if (featureMemory_setupElement( mem, olevel, i, nDeltas, 0, obj->config[id].names, mem->level[ilevel].config[i].enabled ) == 0) {
        FEATUM_ERROR(0,"error during featureMemory_setupElement! (return code 0)");
        _FUNCTION_RETURN_(0);                                
      }
      
      if (!featureMemory_buildNames(mem, olevel, i)) {
        FEATUM_DEBUG(8,"building of names for level %i and el %i not yet possible!",olevel,i);
      }
    }
     _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// compute all deltas, i.e. repetitatively call computeNext, until this function returns an error
// this function returns 1 if deltas were computed for at least one frame, and 0 if no deltas were computed
int deltas_compute( pDeltas obj, int id )
#define FUNCTION "deltas_compute"
{_FUNCTION_ENTER_

  int status = 1;
  int flag = 0;
  while (status) {
    status = deltas_computeNext(obj, id);      
    if (status) flag = 1;
  }
  _FUNCTION_RETURN_(flag);
  
}
#undef FUNCTION 


// compute the deltas for configuration <id>
// (save deltas to olevel of id in feature memory mem)
// when this function is called, it computes only the current (i.e. last) functional frame
// RETURN value: 0 on error, 1 on success, but when nothing was computed, 2 if something was computed
int deltas_computeNext( pDeltas obj, int id )
#define FUNCTION "deltas_computeNext"
{_FUNCTION_ENTER_

// nFramesContext == 0 is a special case... it's the 'incorrect(?)' simple differential, where 2 neighboring values are subtracted
// nFramesContext == 1 would correspond to a normal differential with left and right 
inline LONG_IDX oIdxToIidx0 (LONG_IDX oIdx, int nFramesContext) {
  // computes first index for diff. window over input frames         
  if (nFramesContext == 0) { return oIdx-1; }
  return oIdx - (nFramesContext);
}

inline LONG_IDX oIdxToIidxL (LONG_IDX oIdx, int nFramesContext) {
  // computes last index for diff. window over input frames
  return oIdx + (nFramesContext);
}

  int olevel, ilevel, nFramesContext;
  
  if ((obj != NULL) && (obj->config != NULL)) {
    #ifdef COMPILE_CHECKS
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config[id].mem == NULL) _FUNCTION_RETURN_(0);
    #endif
    pFeatureMemory mem = obj->config[id].mem;
    olevel = obj->config[id].olevel;
    ilevel = obj->config[id].ilevel;
    nFramesContext = obj->config[id].nFramesContext;
//FEATUM_WARNING(0,"here1");
    FEATUM_DEBUG(10,"allFilled: %i - curIdx_frames: %i", mem->level[ilevel].allFilledIdx, obj->config[id].curIdx_frames);
    // check if current output frame can be computed...

    LONG_IDX ilFill = mem->level[ilevel].allFilledIdx;
    LONG_IDX olComp = mem->level[olevel].curIdx + 1;  // compute the next frame
    
    LONG_IDX il0 = oIdxToIidx0( olComp, nFramesContext );
    LONG_IDX ilL = oIdxToIidxL( olComp, nFramesContext );
    // both il0 and ilL must be in range 
//FEATUM_WARNING(0,"here1b: ilL %i, ilFill %i, il0 %i, olComp %i",ilL,ilFill,il0,olComp);    
    if ((ilL <= ilFill)&&(il0 <= ilFill)&&(olComp <= ilL)&&(olComp>il0)) {
//FEATUM_WARNING(0,"here2");
      // we now compute the differential:
      mem->level[olevel].curIdx++;
      int ret = deltas_doCompute( obj, id, olComp );        
      if (ret == 0) { // error
        mem->level[olevel].curIdx--;
        FEATUM_WARNING(4,"Error computing deltas... ");
      } else { // success : update indicies and return success
//        mem->level[olevel].curIdx = olComp; 
        if (mem->level[olevel].allFilledIdx == olComp-1)
          mem->level[olevel].allFilledIdx = olComp;
        else {
          FEATUM_WARNING(2,"Missing frames on level %i, allFilledIdx cannot be updated: stuck at %i (new curIdx = %i)",olevel,mem->level[olevel].allFilledIdx, olComp);
        }
        _FUNCTION_RETURN_(1);
      }
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// compute the last frames, that did not get processed during normal stream processing due to the context window
int deltas_computeLast( pDeltas obj, int id )
#define FUNCTION "deltas_computeLast"
{_FUNCTION_ENTER_

  int olevel, ilevel, nFramesContext;
  
  if ((obj != NULL) && (obj->config != NULL)) {
    #ifdef COMPILE_CHECKS
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config[id].mem == NULL) _FUNCTION_RETURN_(0);
    #endif
    pFeatureMemory mem = obj->config[id].mem;
    olevel = obj->config[id].olevel;
    ilevel = obj->config[id].ilevel;
    nFramesContext = obj->config[id].nFramesContext;
    FEATUM_DEBUG(10,"allFilled: %i - curIdx_frames: %i", mem->level[ilevel].allFilledIdx, obj->config[id].curIdx_frames);
    // check if current output frame can be computed...

    LONG_IDX ilFill = mem->level[ilevel].allFilledIdx;
    LONG_IDX olComp = mem->level[olevel].curIdx + 1;  // compute the next frame
    int flag = 0;
    while (ilFill >= olComp) {
//FEATUM_WARNING(0,"here1b: ilL %i, ilFill %i, il0 %i, olComp %i",ilL,ilFill,il0,olComp);    
//    if ((ilL <= ilFill)&&(il0 <= ilFill)&&(olComp <= ilL)&&(olComp>il0)) {
//FEATUM_WARNING(0,"here2");
      // we now compute the differential:
      mem->level[olevel].curIdx++;
      int ret = deltas_doCompute( obj, id, olComp );        
      if (ret == 0) { // error
        mem->level[olevel].curIdx--;
        FEATUM_WARNING(4,"Error computing deltas... ");
      } else { // success : update indicies and return success
        //mem->level[olevel].curIdx = olComp; 
        if (mem->level[olevel].allFilledIdx == olComp-1)
          mem->level[olevel].allFilledIdx = olComp;
        else {
          FEATUM_WARNING(2,"Missing frames on level %i, allFilledIdx cannot be updated: stuck at %i (new curIdx = %i)",olevel,mem->level[olevel].allFilledIdx, olComp);
        }
    //    _FUNCTION_RETURN_(1);
        flag = 1;
      }
      olComp = mem->level[olevel].curIdx + 1;  // compute the next frame
    }
    _FUNCTION_RETURN_(flag);  // return 1 (success), if at least one frame was computed successfully
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


int deltas_doCompute( pDeltas obj, int id, LONG_IDX oIdx )
#define FUNCTION "deltas_doCompute"
{_FUNCTION_ENTER_
  int olevel,ilevel,nFramesContext;
  int i,el,n;
  if ((obj != NULL) && (obj->config != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config[id].mem == NULL) _FUNCTION_RETURN_(0);
    #endif
    pFeatureMemory mem = obj->config[id].mem;
    olevel = obj->config[id].olevel;
    ilevel = obj->config[id].ilevel;
    nFramesContext = obj->config[id].nFramesContext;

    FLOAT_TYPE_FTMEM weight;
    if (nFramesContext == 0) {
      weight = 1.0;
    } else {
      FLOAT_TYPE_FTMEM weight = 0.0;
      for (i=1; i<=nFramesContext; i++) {
        weight += (FLOAT_TYPE_FTMEM)(i * i);
      }
      weight *= 2.0;
    }
    
    // for (el....)
    int el;
    for (el=0; el < mem->level[ilevel].nFtEls; el++) {
    if (mem->level[ilevel].config[el].enabled) {
    
    pFtMemElement elem = featureMemory_getElement(mem, ilevel, oIdx, el);
    if (elem == NULL) { 
      FEATUM_WARNING(4,"element %i on level %i not initialised for vIdx = %i",el,ilevel,oIdx);
      _FUNCTION_RETURN_(0);
    }
    // output frame/element
    FLOAT_TYPE_FTMEM * values = featureMemory_getValuesForWrite_abs( mem, olevel, el, elem->nValues, oIdx, featureMemory_vIdxToTime(mem, ilevel, oIdx) );
    if (values == NULL) {
      FEATUM_ERROR(3,"values == NULL [olevel=%i]  [el=%i]  [oIdx=%i]",olevel,el,oIdx);   
      _FUNCTION_RETURN_(0);        
    }
    int nValues = elem->nValues;

    if (nFramesContext == 0) { // special case
      pFtMemElement elem0 = featureMemory_getElement(mem, ilevel, oIdx-1, el);
      pFtMemElement elem1 = elem; //featureMemory_getElement(mem, ilevel, oIdx, el);
      if (elem0 == NULL) { //_FUNCTION_RETURN_(0);
        for (n=0; n<nValues; n++) {
          values[n] = (elem1->values[n]);
        }
      }
      for (n=0; n<nValues; n++) {
        elem->values[n] = (elem1->values[n] - elem0->values[n]);
      }  
    } else if (nFramesContext > 0) {
      LONG_IDX i;
      for (i=1; i<=nFramesContext; i++) {
        pFtMemElement elem0 = featureMemory_getElementFRep(mem, ilevel, oIdx-i, el);
        pFtMemElement elem1 = featureMemory_getElementFRep(mem, ilevel, oIdx+i, el);
        if ((elem0 != NULL)&&(elem1 != NULL)&&(elem0->filled)&&(elem1->filled)) {
          for (n=0; n<nValues; n++) {
            values[n] += (FLOAT_TYPE_FTMEM)(i) * (elem1->values[n] - elem0->values[n]);
          }  
        } else if ((elem0 == NULL)&&(elem1 != NULL)&&(elem1->filled)) { //_FUNCTION_RETURN_(0);
          for (n=0; n<nValues; n++) {
            values[n] += (FLOAT_TYPE_FTMEM)(i) * (elem1->values[n]);
          }  
        } else if ((elem0 != NULL)&&(elem0->filled)&&(elem1 == NULL)) { //_FUNCTION_RETURN_(0);
          for (n=0; n<nValues; n++) {
            values[n] += (FLOAT_TYPE_FTMEM)(i) * (-1.0)* (elem0->values[n]);
          }  
        } else {
          // do nothing if both are NULL...       
        } 
      }
      for (n=0; n<nValues; n++) {
        values[n] /= weight;
      }
    }
    
    }  // end if (enabled...)
    }  // end for (el...)
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// compute all deltas for all input levels, automatically map output levels
int deltas_autoCompute( pDeltas obj )
#define FUNCTION "deltas_autoCompute"
{_FUNCTION_ENTER_
  int i;
  int flag = 0;
  if ((obj != NULL) ) {
    for (i=0; i<obj->nIDs; i++) {
      if (!deltas_compute( obj, i)) {
        FEATUM_ERROR(9,"Error computing deltas for id %i !",i);
      } else { flag = 1; }
    }  
    _FUNCTION_RETURN_(flag);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// compute deltas on last (missing) frames at end of stream, 
// for all input levels. automatically map output levels
int deltas_autoComputeLast( pDeltas obj )
#define FUNCTION "deltas_autoComputeLast"
{_FUNCTION_ENTER_
  int i;
  int flag = 0;
  if ((obj != NULL) ) {
    for (i=0; i<obj->nIDs; i++) {
      if (!deltas_computeLast( obj, i)) {
        FEATUM_ERROR(9,"Error computing deltas for id %i!",i);
      } else { flag = 1; }
    }  
    _FUNCTION_RETURN_(flag);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


/* Destructor: frees the class data object and returns NULL */
pDeltas deltas_destroy( pDeltas obj )
#define FUNCTION "deltas_destroy"
{_FUNCTION_ENTER_
  int i,id;
  deltas_destroyData(obj);
  if (obj != NULL) {
/*    // TODO: destroy temporary data.. ??
    if (obj->config != NULL) {
      for (id=0; id<obj->nIDs; id++) {
      if (obj->config[id].names != NULL) { // free if already allocated ????
        for (i=0;i<nDeltas; i++) { 
            if (obj->config[id].names[i] != NULL) free(obj->config[id].names[i]);
        }
        free( obj->config[id].names );
      }
      }
      free(obj->config);
    }*/
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 


/* Destructor: frees the class data object and returns NULL */
pDeltas deltas_destroyData( pDeltas obj )
#define FUNCTION "deltas_destroyData"
{_FUNCTION_ENTER_
  int i,id;
  if (obj != NULL) {
    // TODO: destroy temporary data.. ??
    if (obj->config != NULL) {
      for (id=0; id<obj->nIDs; id++) {
      if (obj->config[id].names != NULL) { // free if already allocated ????
        for (i=0;i<nDeltas; i++) { 
            if (obj->config[id].names[i] != NULL) free(obj->config[id].names[i]);
        }
        free( obj->config[id].names );
      }
      }
      free(obj->config); obj->config = NULL;
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 
