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
 
 /*  IMPLEMENTATION of the class <featureMemory>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "feature-memory"
 
 // TODOOOO: implement a compatible featureMemoryRB  with ringbuffer capabilities & absolute indicies (possibly warping)
 
 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function

#ifdef FTMEM_NO_FUNCTION_ENTRY_DEBUG
#undef _FUNCTION_ENTER_
#undef _FUNCTION_RETURN_
#undef _FUNCTION_RETURNV_
#define _FUNCTION_ENTER_
#define _FUNCTION_RETURN_(retval) {return retval;}
#define _FUNCTION_RETURNV_ {return;}
#endif


    /*
      Two different methods for allocating featureMemory frames can be used:
        a) addFrame to add a new frame, allocate it AND increase curIdx
           (for this method curIdx must be initialized with -1 )
        b) implict by getValuesForWrite : no change to curIdx whatsoever is made,
           only when an unallocated frame is accessed via these functions for the FIRST(!) time
           the frame is automatically allocated
           for this method to work, curIdx must be initialised with 0 !!
    */


// (ok) TODO: somewhere add number of features for each element and compute true frameSize!
// TODO: check if the filled and allFilled flags and indicies are set correctly!!
// (ok) TODO: check timestamp handling!



// virtual index: absolute index counting from beginning (could be either fixed buffer or ring buffer)



// *capacity array must be at least nLevels*sizeof(LONG_IDX) in size or NULL!
pFeatureMemory featureMemory_create( pFeatureMemory obj, int defaultBufferType, int nLevels )
#define FUNCTION "featureMemory_create"
{_FUNCTION_ENTER_
  int i;
//  if ((obj == NULL)){
    FEATUM_DEBUG(10,"checks 1 passed");
    #ifdef COMPILE_CHECKS
    if (nLevels <= 0) {
      FEATUM_ERROR(1,"cannot create featureMemory with 0 levels");
      _FUNCTION_RETURN_(NULL);
    }
    #endif
   
    if (obj == NULL) {
      obj = (pFeatureMemory)calloc(1,sizeof(sFeatureMemory));
    } else {
      memzero(obj,sizeof(sFeatureMemory));       
    }

    if (obj != NULL) {
      obj->level = (pFeatureMemoryLevel)calloc(1,sizeof(sFeatureMemoryLevel)*nLevels);
      obj->nLevels = nLevels;
      if ((defaultBufferType != FTMEM_FIXED_BUFFER)&&(defaultBufferType != FTMEM_RING_BUFFER)) defaultBufferType = FTMEM_FIXED_BUFFER;

      obj->defaultBufferType = defaultBufferType;
      if (obj->level != NULL) {
          obj->ll_levels = 0;
          for (i=0; i<nLevels; i++) {
              obj->level[i].capacity = 0; 
              obj->level[i].nFtEls = 0;  // call featureMemory_memAlloc to allocate actual data memory and set this number
              obj->level[i].ft = NULL;   // [ftelement][index]
              obj->level[i].t = NULL;            
              obj->level[i].config = NULL;
              obj->level[i].curIdx = -1;  // no frame has been initialised
              obj->level[i].allFilledIdx = -1;
              obj->level[i].readIdx = -1;              
              obj->level[i].prevLevel = -1;
              obj->level[i].nextLevel = -1;
              obj->level[i].nPrevLevels = 0;
              obj->level[i].nFeaturesPerFrame = -1;
          }
      }
    }
//  } else {
//    FEATUM_DEBUG(10,"ERR: obj = %i, ",(long)obj);
//  }  
  _FUNCTION_RETURN_(obj);
}
#undef FUNCTION 

/*
// *capacity array must be at least nLevels*sizeof(LONG_IDX) in size or NULL!
pFeatureMemory featureMemory_create( pFeatureMemory obj, int defaultBufferType, pInputFramer framer, int *framerIDs, int nLevels, LONG_IDX *capacity )
#define FUNCTION "featureMemory_create"
{_FUNCTION_ENTER_
  int i;
// TODO: check if number of framer IDs < nLevels... if so, then add extra levels without frameStep, etc.
  if ((obj == NULL)){
  //&&(framerIDs != NULL)&&(framer != NULL)) {
//                 &&(capacity != NULL)
    FEATUM_DEBUG(10,"checks 1 passed");
    #ifdef COMPILE_CHECKS
    if (nLevels <= 0) {
      FEATUM_ERROR(1,"cannot create featureMemory with 0 levels");
      _FUNCTION_RETURN_(NULL);
    }
    #endif
    obj = (pFeatureMemory)calloc(1,sizeof(sFeatureMemory));
    if (obj != NULL) {
      obj->level = (pFeatureMemoryLevel)calloc(1,sizeof(sFeatureMemoryLevel)*nLevels);
      obj->nLevels = nLevels;
      if ((defaultBufferType != FTMEM_FIXED_BUFFER)||(defaultBufferType != FTMEM_RING_BUFFER)) defaultBufferType = FTMEM_FIXED_BUFFER;
      obj->defaultBufferType = defaultBufferType;
      if (obj->level != NULL) {
        if ((framerIDs != NULL)&&(framer != NULL)&&(capacity != NULL)) {
          int minlevels;
          // if featureMemory has more levels than low-level input levels (from the framer) than assume the extra levels are for functionals, etc. 
          if (nLevels > framer->nClients) {
            minlevels =  framer->nClients;
          } else {
            minlevels = nLevels;       
          }
          for (i=0; i<minlevels; i++) {
            obj->level[i].capacity = capacity[i];
            if (framer->client != NULL) {
              obj->level[i].T = (float)audioStream_samplesToSeconds( framer->input, framer->client[framer->clientLUT[framerIDs[i]]]->frameStep );
              //obj->level[i].frameStep = framer->client[framer->clientLUT[framerIDs[i]]]->frameStep;
              obj->level[i].frameSize = 0; //framer->client[framer->clientLUT[framerIDs[i]]]->frameLength;  //????
              FEATUM_DEBUG(10,"obj->level[%i / %i].frameStep == %i",i,framerIDs[i],obj->level[i].frameStep);
            }
            obj->level[i].nFtEls = 0;  // call featureMemory_memAlloc to allocate actual data memory
            obj->level[i].ft = NULL;   // [index][ftelement]
            obj->level[i].t = NULL;            
            obj->level[i].config = NULL;
            obj->level[i].curIdx = -1;  // no frame has been initialised
            obj->level[i].allFilledIdx = -1;
            obj->level[i].readIdx = -1;            
            obj->level[i].prevLevel = -1;
            obj->level[i].nextLevel = -1;
          }               
          for (i=minlevels; i<nLevels; i++) {
            //obj->level[i].capacity = capacity[i]; // could also be zero
            obj->level[i].nFtEls = 0;  // call featureMemory_memAlloc to allocate actual data memory
            obj->level[i].ft = NULL;   // [index][ftelement]
            obj->level[i].t = NULL;            
            obj->level[i].config = NULL;
            obj->level[i].curIdx = -1;  // no frame has been initialised
            obj->level[i].allFilledIdx = -1;
            obj->level[i].readIdx = -1;
            obj->level[i].prevLevel = -1;
            obj->level[i].nextLevel = -1;
          }
          obj->ll_levels = minlevels;
        } else {
          obj->ll_levels = 0;
          if (capacity != NULL) {
            for (i=0; i<nLevels; i++) {
              //obj->level[i].capacity = capacity[i]; // could also be zero
              obj->level[i].nFtEls = 0;  // call featureMemory_memAlloc to allocate actual data memory
              obj->level[i].ft = NULL;   // [index][ftelement]
              obj->level[i].t = NULL;            
              obj->level[i].config = NULL;
              obj->level[i].curIdx = -1;  // no frame has been initialised
              obj->level[i].allFilledIdx = -1;
              obj->level[i].readIdx = -1;              
              obj->level[i].prevLevel = -1;
              obj->level[i].nextLevel = -1;
            }
          } // end: if (capacity == 0) ...
        }
      }
    }
  } else {
    FEATUM_DEBUG(10,"ERR: obj = %i, capacity = %i, framerIDs = %i",(unsigned int)obj, (unsigned int)capacity,(unsigned int)framerIDs);
  }  
  _FUNCTION_RETURN_(obj);
}
#undef FUNCTION 
*/


// T can be zero, if the frames are not periodic
// for no previous level set prevLevel to -1
int featureMemory_configureLevel( pFeatureMemory obj, int level, int capacity, int bufferType, float T, int nFtEls, int prevLevel)
#define FUNCTION "featureMemory_configureLevel"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(0);
    #endif
    if ((obj->level != NULL)&&(level < obj->nLevels)) {
      obj->level[level].capacity = capacity;
      obj->level[level].nFtEls = nFtEls;
      if (bufferType == FTMEM_DEFAULT_BUFFER) bufferType = obj->defaultBufferType;
      else if ((bufferType != FTMEM_FIXED_BUFFER)&&(bufferType != FTMEM_RING_BUFFER)) { bufferType = obj->defaultBufferType; }
      obj->level[level].bufferType = bufferType;
      if (prevLevel >= obj->nLevels) prevLevel = -1;
      obj->level[level].prevLevel = prevLevel;
      if (prevLevel >= 0) {
        obj->level[level].nPrevLevels = obj->level[prevLevel].nPrevLevels + 1;
        obj->level[prevLevel].nextLevel = level;
      }
      obj->level[level].T = T;
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// set only number of feature elements (effective only after configureLevel!)
int featureMemory_setNFtEls( pFeatureMemory obj, int level, int nFtEls )
#define FUNCTION "featureMemory_setNFtEls"
{
  if ((obj != NULL)&&(nFtEls > 0)&&(level>=0)) {
    if (level < obj->nLevels) {
      obj->level[level].nFtEls = nFtEls;
       _FUNCTION_RETURN_(1);
    }
  }  
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// call this before memAlloc to set up the additional levels, e.g. those for functionals
/*
int featureMemory_configureExtraLevel( pFeatureMemory obj, int level, float T, int capacity)
#define FUNCTION "featureMemory_configureExtraLevel"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(0);
    #endif
    if ((obj->level != NULL)&&(level < obj->nLevels)) {
      obj->level[level].capacity = capacity;
      obj->level[level].T = T;
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 
*/

int featureMemory_dataMemAllocLevel( pFeatureMemory obj, int level )
#define FUNCTION "featureMemory_dataMemAllocLevel"
{_FUNCTION_ENTER_
  LONG_IDX i,el;
  if (obj != NULL) {
    if ((level >= 0) && (level < obj->nLevels) ) {
      i = level;
      if ((obj->level != NULL)&&(obj->level[i].ft == NULL)) {
        if (obj->level[i].nFtEls == 0) { FEATUM_ERROR(2,"nFtEls == 0 on level %i : probably not configured!",i); }
        else {
        FEATUM_DEBUG(10,"level %i -> allocating capacity = %i",i,obj->level[i].capacity);
        obj->level[i].ft = (pFtMemElement *)calloc(1,(LONG_IDX)sizeof(pFtMemElement) * obj->level[i].nFtEls);
        if (obj->level[i].ft != NULL) {
          for (el=0; el<obj->level[i].nFtEls; el++ ) {
            obj->level[i].ft[el] = (pFtMemElement)calloc(1,sizeof(sFtMemElement) * obj->level[i].capacity);   
//            FEATUM_DEBUG(12,"allocating frame %i of %i (x %i)",n,obj->level[i].capacity,obj->level[i].nFtEls);            
//            if (n<29988)
//            obj->level[i].ft[n] = (pFtMemElement *)calloc(1,sizeof(pFtMemElement) * obj->level[i].nFtEls);
            FEATUM_DEBUG(12," ptr=%i",(long)(obj->level[i].ft[el]));
            if (obj->level[i].ft[el] == NULL) {
              FEATUM_ERR_FATAL(0,"Error allocating memory!");
              _FUNCTION_RETURN_(0);
            }
            // the memory of the elements will be allocated at a later point in time...?
            // during addElement or getElement ?
          }
        } else {
          FEATUM_ERR_FATAL(0,"Error allocating memory!");
          _FUNCTION_RETURN_(0);
        }
        
        if (obj->level[i].config == NULL)
          obj->level[i].config = (pFtMemElementConfig)calloc(1,sizeof(sFtMemElementConfig) * obj->level[i].nFtEls);
        if (obj->level[i].t == NULL)
          obj->level[i].t = (FLOAT_TYPE_TIME *)calloc(1,sizeof(FLOAT_TYPE_TIME)*obj->level[i].capacity);        
          
        }
      } else {
        FEATUM_WARNING(2,"Memory for obj->level[%i]->ft already allocated or obj->level[%i] == NULL",level);
      }  
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// nFtElems: number of feature elements for each level
//int featureMemory_memAlloc( pFeatureMemory obj, LONG_IDX *nFtElems )
int featureMemory_dataMemAlloc( pFeatureMemory obj )
#define FUNCTION "featureMemory_dataMemAlloc"
{_FUNCTION_ENTER_
  LONG_IDX i,n;
  if (obj != NULL) {
    for (i=0; i<obj->nLevels; i++) {
      if (featureMemory_dataMemAllocLevel( obj, i ) == 0) {
        _FUNCTION_RETURN_(0);                                           
      }
    }
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

/*
// nFtElems: number of feature elements for each level
//int featureMemory_memAlloc( pFeatureMemory obj, LONG_IDX *nFtElems )
int featureMemory_dataMemAlloc( pFeatureMemory obj )
#define FUNCTION "featureMemory_dataMemAlloc"
{_FUNCTION_ENTER_
  LONG_IDX i,n;
  if ((obj != NULL)&&(nFtElems != NULL)) {
    for (i=0; i<obj->nLevels; i++) {
      if ((obj->level != NULL)&&(obj->level[i].ft == NULL)) {
        if (obj->level[i].nFtEls == 0) { FEATUM_ERROR(2,"nFtEls == 0 on level %i : probably not configured!",i);
        else {
        FEATUM_DEBUG(11,"allocating capacity = %i",obj->level[i].capacity);
        obj->level[i].ft = (pFtMemElement **)calloc(1,(LONG_IDX)sizeof(pFtMemElement *) * obj->level[i].capacity);

        if (obj->level[i].ft != NULL) {
          for (n=0; n<obj->level[i].capacity; n++ ) {
            FEATUM_DEBUG(12,"allocating frame %i of %i (x %i)",n,obj->level[i].capacity,obj->level[i].nFtEls);            
//            if (n<29988)
              obj->level[i].ft[n] = (pFtMemElement *)calloc(1,sizeof(pFtMemElement) * obj->level[i].nFtEls);
            FEATUM_DEBUG(12," ptr=%i",(int)(obj->level[i].ft[n]));
            if (obj->level[i].ft[n] == NULL) {
              FEATUM_ERR_FATAL(0,"Error allocating memory!");
              //_FUNCTION_RETURN_(0);
            }
            // the memory of the elements will be allocated at a later point in time...?
            // during addElement or getElement ?
          }
        } else {
          FEATUM_ERR_FATAL(0,"Error allocating memory!");
          _FUNCTION_RETURN_(0);
        }
        
        if (obj->level[i].config == NULL)
          obj->level[i].config = (pFtMemElementConfig)calloc(1,sizeof(sFtMemElementConfig) * obj->level[i].nFtEls);
        if (obj->level[i].t == NULL)
          obj->level[level].t = (FLOAT_TYPE_TIME *)calloc(sizeof(FLOAT_TYPE_TIME)*obj->level[i].capacity);        
          
        }
      } else {
        FEATUM_WARNING(2,"Memory for obj->level[i]->ft already allocated or obj->level[i] == NULL");
      }  
    }
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 
*/

// this function will free the complete data memory and all feature memory elements
int featureMemory_dataMemFree( pFeatureMemory obj )
#define FUNCTION "featureMemory_dataMemFree"
{_FUNCTION_ENTER_
  int i,j,el,n;
  if (obj != NULL) {
    for (i=0; i<obj->nLevels; i++) {
      if (obj->level != NULL) {
        if (obj->level[i].ft != NULL) {
                              
          for (el=0; el<obj->level[i].nFtEls; el++ ) {
            if (obj->level[i].ft[el] != NULL) {  
              for (n=0; n<obj->level[i].capacity; n++) {
//                if (obj->level[i].ft[n][j] != NULL) { 
                if (obj->level[i].ft[el][n].values != NULL) free(obj->level[i].ft[el][n].values);
              }
              free(obj->level[i].ft[el]);
            }  
          } // end for (el...)
          free(obj->level[i].ft); obj->level[i].ft = NULL;
          if (obj->level[i].config != NULL) {
            for (j=0; j<obj->level[i].nFtEls; j++) {
/*              if ((obj->level[i].config[j].alloced)&&(obj->level[i].config[j].names != NULL)&&(obj->level[i].config[j].n>0)) {
                for (n=0; n< obj->level[i].config[j].n; n++) {
                  if (obj->level[i].config[j].names[n] != NULL) free(obj->level[i].config[j].names[n]);
                }
                free(obj->level[i].config[j].names);
              }*/
              // TODO:: check if the above code is necessary or if we are not allowed to free because **names array was assigned a pointer to a predefined constant in extractors-globalconfig.c
              if (obj->level[i].config[j].dynname != NULL) free(obj->level[i].config[j].dynname);
              if ((obj->level[i].config[j].final_names != NULL)&&(obj->level[i].config[j].nTotal >0)) {
                for (n=0; n<obj->level[i].config[j].nTotal; n++) {
                  if (obj->level[i].config[j].final_names != NULL) free(obj->level[i].config[j].final_names[n]);
                }
                free(obj->level[i].config[j].final_names);
              }
            }
            free(obj->level[i].config); obj->level[i].config = NULL;
          } 
          if ( obj->level[i].t != NULL) {
            free(obj->level[i].t); obj->level[i].t = NULL;   
          }

        }
        //obj->level[i].nFtEls = 0;
      }
    }
  }
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION 


// nDyn is the number of basenames if the number of values is dynamic
// setup names in feature memory object
// nDyn is the number of basnames if the number of values is dynamic
#ifdef OPTIMIZE_SIZE
int featureMemory_setupElement( pFeatureMemory mem, int level, int el, int nVal, int nDyn, char **names, int enabled )
#else
inline int featureMemory_setupElement( pFeatureMemory mem, int level, int el, int nVal, int nDyn, char **names, int enabled )
#endif
#define FUNCTION "featureMemory_setupElement"
{_FUNCTION_ENTER_
  int n;
  if ((mem != NULL)&&(level >= 0)) {
    FEATUM_DEBUG(3,"el=%i, level=%i, nVal=%i, enabled=%i",el,level,nVal,enabled);
    #ifdef COMPILE_CHECKS
    if ((level < mem->nLevels) && (mem->level != NULL) && (mem->level[level].config != NULL)) {
    #endif
        FEATUM_DEBUG(3,"mem->level[level].config = %i",(long)(mem->level[level].config));
        mem->level[level].config[el].nValues = nVal;
        mem->level[level].config[el].nValuesDyn = nDyn;
        mem->level[level].config[el].names = names;  // TODO: ??? correct ???
        mem->level[level].config[el].enabled = enabled;
        if (mem->level[level].config[el].final_names != NULL) {
          for (n=0; n<mem->level[level].config[el].nTotal; n++) { 
            if (mem->level[level].config[el].final_names[n] != NULL) free(mem->level[level].config[el].final_names[n]); 
          }
          free(mem->level[level].config[el].final_names);
          mem->level[level].config[el].final_names = NULL;
        }
        mem->level[level].config[el].nTotal = -1;
        // TODO: initialize final_names and nTotal for non-dynmaic first levels in link chain...??
         
        _FUNCTION_RETURN_(1);   
    #ifdef COMPILE_CHECKS
    } else if (mem->level[level].config == NULL) {
      FEATUM_ERROR(1,"you must first run featureMemory_dataMemAlloc to allocate memory...!");
    }
    #endif
  }
  _FUNCTION_RETURN_(0);   
}
#undef FUNCTION 

inline LONG_IDX vIdxToRealIdx( pFeatureMemoryLevel objl, LONG_IDX vIdx ) 
#define FUNCTION "vIdxToRealIdx"
{//_FUNCTION_ENTER_
  if (objl->bufferType == FTMEM_RING_BUFFER) {
    //_FUNCTION_RETURN_(vIdx % objl->capacity);                       
    return (vIdx % objl->capacity);                       
  } else {
    //_FUNCTION_RETURN_(vIdx);                    
    return vIdx;
  }
  return 0;
//  _FUNCTION_RETURN_(0);       
}
#undef FUNCTION 

inline int vIdxInRange(pFeatureMemoryLevel objl, LONG_IDX vIdx)
#define FUNCTION "vIdxInRange"
{_FUNCTION_ENTER_
  if (vIdx >= 0) 
  {
    if (objl->capacity <= 0) { return 0; }

    if ((objl->bufferType == FTMEM_FIXED_BUFFER) && (vIdx < objl->capacity)) {
      _FUNCTION_RETURN_(1);
    }

    if (objl->bufferType == FTMEM_RING_BUFFER) {  // ring buffer
      // for ringbuffers the current write index determines the ringbuffer position, 
      // when new data (frames) is added old data (capacity frames back) gets overwritten
      // thus vIdx must be in the range:
//              return objl->curIdx;
      if ((vIdx > objl->curIdx - objl->capacity) && (vIdx <= objl->curIdx)) {
//printf("vIDx .xxxx. %i  ci: %i  cap: %i\n",vIdx,objl->curIdx,objl->capacity); fflush(stdout);                
        _FUNCTION_RETURN_(1);
      }
    }            
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

/*
pFtMemElement featureMemory_getCurrentWriteElement( pFeatureMemory obj, int level, int el )
#define FUNCTION "featureMemory_getCurrentWriteElement"
{_FUNCTION_ENTER_
  int i;
  if ((obj != NULL)&&(level>=0)&&(el>=0)) {
    if (level >= obj->nLevels) {
      FEATUM_ERROR(1,"level %i > obj->nLevels %i",level,obj->nLevels);
      _FUNCTION_RETURN_(0);
    }
    if ((obj->level != NULL)&&(obj->level[level].ft != NULL) 
         && (obj->level[level].ft[vIdxToRealIdx(&(obj->level[level]),obj->level[level].curIdx)] != NULL)) {
      if (el < obj->level[level].nFtEls) {
        _FUNCTION_RETURN_( obj->level[level].ft[vIdxToRealIdx(&(obj->level[level]),obj->level[level].curIdx)][el] );
      }  else {
        FEATUM_ERROR(1,"Element Nr. %i does not exist (out of bounds: nFts = %i", el , obj->level[level].nFts);
        _FUNCTION_RETURN_(NULL);
      }
    } else if (obj->level[level].ft == NULL) {
       FEATUM_ERROR(1," obj->level[level].ft == NULL, check if you have called featureMemory_memAlloc!");    
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 


pFtMemElement featureMemory_getCurrentAllFilledElement( pFeatureMemory obj, int level, int el )
#define FUNCTION "featureMemory_getCurrentAllFilledElement"
{_FUNCTION_ENTER_
  int i;
  if ((obj != NULL)&&(level>=0)&&(el>=0)) {
    if (level >= obj->nLevels) {
      FEATUM_ERROR(1,"level %i > obj->nLevels %i",level,obj->nLevels);
      _FUNCTION_RETURN_(NULL);
    }
    if ((obj->level != NULL)&&(obj->level[level].ft != NULL) 
         && (obj->level[level].allFilledIdx >= 0) && (obj->level[level].ft[vIdxToRealIdx(obj->level[level],obj->level[level].allFilledIdx)] != NULL)) {
      if (el < obj->level[level].nFtEls) {
        _FUNCTION_RETURN_( obj->level[level].ft[vIdxToRealIdx(obj->level[level],obj->level[level].allFilledIdx)][el] );
      }  else {
        FEATUM_ERROR(1,"Element Nr. %i does not exist (out of bounds: nFts = %i", el , obj->level[level].nFts);
        _FUNCTION_RETURN_(NULL);
      }
    } else if (obj->level[level].ft == NULL) {
       FEATUM_WARNING(1,"probably allFilledIdx == -1, no frames filled yet, returning NULL");
//       FEATUM_ERROR(1," obj->level == NULL or obj->level[level].ft == NULL, check if you have called featureMemory_dataMemAlloc!");    
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

pFtMemElement featureMemory_getCurrentReadElement( pFeatureMemory obj, int level, int el )
#define FUNCTION "featureMemory_getCurrentAllFilledElement"
{_FUNCTION_ENTER_
  int i;
  if ((obj != NULL)&&(level>=0)&&(el>=0)) {
    if (level >= obj->nLevels) {
      FEATUM_ERROR(1,"level %i > obj->nLevels %i",level,obj->nLevels);
      _FUNCTION_RETURN_(NULL);
    }
    if ((obj->level != NULL)&&(obj->level[level].ft != NULL) 
         && (obj->level[level].readIdx >= 0) && (obj->level[level].ft[vIdxToRealIdx(obj->level[level],obj->level[level].readIdx)] != NULL)) {
      if (el < obj->level[level].nFtEls) {
        _FUNCTION_RETURN_( obj->level[level].ft[vIdxToRealIdx(obj->level[level],obj->level[level].readIdx)][el] );
      }  else {
        FEATUM_ERROR(1,"Element Nr. %i does not exist (out of bounds: nFts = %i", el , obj->level[level].nFts);
        _FUNCTION_RETURN_(NULL);
      }
    } else if (obj->level[level].ft == NULL) {
       FEATUM_WARNING(1,"probably readIdx == -1, no frames filled yet, returning NULL");
//       FEATUM_ERROR(1," obj->level == NULL or obj->level[level].ft == NULL, check if you have called featureMemory_dataMemAlloc!");    
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 
*/

// if the element is not filled it is returned, the calling code must check whether ret->filled is set or not
// special constants are defined for vIdx to access the current write, read and allFilled elements
pFtMemElement featureMemory_getElement( pFeatureMemory obj, int level, LONG_IDX vIdx, int el )
#define FUNCTION "featureMemory_getElement"
{ //_FUNCTION_ENTER_

  #ifdef COMPILE_CHECKS_LOOPS
  if ((obj != NULL)&&(level>=0)&&(el>=0)) {
  #else
  if (obj != NULL) {
  #endif
    if (level >= obj->nLevels) {
      FEATUM_ERR_FATAL(0,"level %i > obj->nLevels %i",level,obj->nLevels);
      //_FUNCTION_RETURN_(NULL);
      return NULL;
    }
    if (obj->level != NULL) {
      if (vIdx <= VIDX_CURRENT_WRITE) {  // special index constants
         switch(vIdx) {
           case VIDX_CURRENT_WRITE : vIdx = obj->level[level].curIdx; break;
           case VIDX_CURRENT_READ  : vIdx = obj->level[level].readIdx; break;
           case VIDX_CURRENT_ALLFILLED : vIdx = obj->level[level].allFilledIdx; break;
         }
      }
      if ((obj->level[level].ft != NULL) && (vIdxInRange(&(obj->level[level]),vIdx))) {
        int rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
        if (el < obj->level[level].nFtEls) {
          if (obj->level[level].ft[el] != NULL) {
            //_FUNCTION_RETURN_( &(obj->level[level].ft[el][rIdx]) );
            return ( &(obj->level[level].ft[el][rIdx]) );
          }  else {
            FEATUM_ERROR(1,"Element Nr. %i is NULL (out of bounds: nFts = %i)", el , obj->level[level].nFtEls);
            //_FUNCTION_RETURN_(NULL);
            return NULL;
          }
        } // end if (el < bj->level[level].nFtEls)
      }
    }
  }
  return NULL;
//  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

// special constants are defined for vIdx to access the current write, read and allFilled elements
// if the requested element is out of range this special "FRep" function returns the first/last possible element!
// this function uses 0..allFilledIdx as range, therefore, only filled frames should be returned (if available!)
pFtMemElement featureMemory_getElementFRep( pFeatureMemory obj, int level, LONG_IDX vIdx, int el )
#define FUNCTION "featureMemory_getElementFRep"
{_FUNCTION_ENTER_

  #ifdef COMPILE_CHECKS_LOOPS
  if ((obj != NULL)&&(level>=0)&&(el>=0)) {
  #else
  if (obj != NULL) {
  #endif
    if (level >= obj->nLevels) {
      FEATUM_ERR_FATAL(0,"level %i > obj->nLevels %i",level,obj->nLevels);
      _FUNCTION_RETURN_(NULL);
    }
    if (obj->level != NULL) {
      if (vIdx <= VIDX_CURRENT_WRITE) {  // special index constants
         switch(vIdx) {
           case VIDX_CURRENT_WRITE : vIdx = obj->level[level].curIdx; break;
           case VIDX_CURRENT_READ  : vIdx = obj->level[level].readIdx; break;
           case VIDX_CURRENT_ALLFILLED : vIdx = obj->level[level].allFilledIdx; break;
         }
      }
      if (vIdx < 0) { vIdx = 0; }
      if (vIdx > obj->level[level].allFilledIdx) { vIdx = obj->level[level].allFilledIdx; }
//printf("A VIDX:.:::: %i\n",vIdx); fflush(stdout);        
      // todo: test this code::: for ringbuffers... return last possible element in ringbuffer?
      if (!vIdxInRange(&(obj->level[level]),vIdx) && (obj->level[level].bufferType == FTMEM_RING_BUFFER) ) {
        // search for next element in range!
//        printf("OOR\n"); fflush(stdout);
        vIdx = obj->level[level].curIdx + obj->level[level].capacity + 1;
      }
//printf("VIDX:.:::: %i  [level %i] [afi=%i] [ci=%i]\n",vIdx,level,obj->level[level].allFilledIdx,obj->level[level].curIdx); fflush(stdout);      
      if ((obj->level[level].ft != NULL) && (vIdxInRange(&(obj->level[level]),vIdx))) {
        int rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
//printf(" -> ridx : %i\n ",rIdx); fflush(stdout);
        if (el < obj->level[level].nFtEls) {
          if (obj->level[level].ft[el] != NULL) {
            _FUNCTION_RETURN_( &(obj->level[level].ft[el][rIdx]) );
          }  else {
            FEATUM_ERROR(1,"Element Nr. %i is NULL (out of bounds: nFts = %i)", el , obj->level[level].nFtEls);
            _FUNCTION_RETURN_(NULL);
          }
        } // end if (el < bj->level[level].nFtEls)
      }
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 


// get special index, specified via constants: VIDX_CURRENT_WRITE, VIDX_CURRENT_READ, VIDX_CURRENT_ALLFILLED 
LONG_IDX featureMemory_getSpecialIndex( pFeatureMemory obj, int level, LONG_IDX vIdxConst )
#define FUNCTION "featureMemory_getSpecialIndex"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    #endif
         switch(vIdxConst) {
           case VIDX_CURRENT_WRITE : _FUNCTION_RETURN_( obj->level[level].curIdx );
           case VIDX_CURRENT_READ  : _FUNCTION_RETURN_( obj->level[level].readIdx );
           case VIDX_CURRENT_ALLFILLED : _FUNCTION_RETURN_( obj->level[level].allFilledIdx );
         }
    #ifdef COMPILE_CHECKS_LOOPS
    }
    #endif
  }
  _FUNCTION_RETURN_(-10);  // unsafe but clean error handling
}
#undef FUNCTION 


// returns -1 on error, otherwise the timestamp in seconds as floating point
FLOAT_TYPE_TIME featureMemory_vIdxToTime ( pFeatureMemory obj, int level, int vIdx )
#define FUNCTION "featureMemory_vIdxToTime"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    #endif
    if (vIdxInRange(&(obj->level[level]),vIdx)) {
      if (obj->level[level].t != NULL) {
        _FUNCTION_RETURN_(obj->level[level].t[ vIdxToRealIdx(&(obj->level[level]),vIdx) ]);
      }
    }
    #ifdef COMPILE_CHECKS_LOOPS
    }}
    #endif

  }
  _FUNCTION_RETURN_((FLOAT_TYPE_TIME)(-1));
}
#undef FUNCTION 



// vIdx can be -1 to -3 for special indicies
// slow get feature functions...  level0: direct (raw) indexing (always works)
FLOAT_TYPE_FTMEM featureMemory_getFeature( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n )
#define FUNCTION "featureMemory_getFeature"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    if ((el >= 0) && (el<obj->level[level].nFtEls) && (n>=0)) {
    #endif
    if (vIdxInRange(&(obj->level[level]),vIdx)) {
      int rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
      if ( (obj->level[level].ft[el] != NULL) ) {
        FLOAT_TYPE_FTMEM *values = obj->level[level].ft[el][rIdx].values;
        if (values != NULL) {
          int nValues = obj->level[level].ft[el][rIdx].nValues;
          if (n<nValues) {
             _FUNCTION_RETURN_(values[n]);
          }
        } // END  if (values != NULL)
      }
    }
    #ifdef COMPILE_CHECKS_LOOPS
    }}
    #endif
  }
  _FUNCTION_RETURN_((FLOAT_TYPE_FTMEM)0);
}
#undef FUNCTION 

// level1 : two dimensional indexing (i.e. low level feature index + index of functional)
//   only works if for the requested level the variable prevLevels == 1
FLOAT_TYPE_FTMEM featureMemory_getFeatureL1( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n0, int n1 )
#define FUNCTION "featureMemory_getFeatureL1"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    if ((el >= 0) && (el<obj->level[level].nFtEls)) {
    if (obj->level[level].config != NULL) {
    #endif
    if (vIdxInRange(&(obj->level[level]),vIdx)) {
      if (obj->level[level].nPrevLevels != 1) _FUNCTION_RETURN_((FLOAT_TYPE_FTMEM)0);
      int rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
      if ( (obj->level[level].ft[el] != NULL) ) {
//           && (obj->level[level].ft[rIdx][el] != NULL) ) {
        FLOAT_TYPE_FTMEM *values = obj->level[level].ft[el][rIdx].values;
        if (values != NULL) {
          int nValues = obj->level[level].ft[el][rIdx].nValues;
          int n;
          n = n0*obj->level[level].config[el].nValues + n1;
          if ((n>=0)&&(n<nValues)) {
             _FUNCTION_RETURN_(values[n]);
          }
        } // END  if (values != NULL) 
      }
    }
    #ifdef COMPILE_CHECKS_LOOPS
    }}}
    #endif

  }
  _FUNCTION_RETURN_((FLOAT_TYPE_FTMEM)0);
}
#undef FUNCTION 

// level2 : three dimensional indexing (for hierarchical functionals)
//   only works if for the requested level the variable prevLevels == 2
FLOAT_TYPE_FTMEM featureMemory_getFeatureL2( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n0, int n1, int n2 )
#define FUNCTION "featureMemory_getFeatureL2"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    if ((el >= 0) && (el<obj->level[level].nFtEls)) {
    if (obj->level[level].config != NULL) {
    #endif
    if (vIdxInRange(&(obj->level[level]),vIdx)) {
      if (obj->level[level].nPrevLevels != 2) _FUNCTION_RETURN_((FLOAT_TYPE_FTMEM)0);
      int rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
      if ( (obj->level[level].ft[el] != NULL) ) {
//           && (obj->level[level].ft[rIdx][el] != NULL) ) {
        FLOAT_TYPE_FTMEM *values = obj->level[level].ft[el][rIdx].values;
        if (values != NULL) {
          int nValues = obj->level[level].ft[el][rIdx].nValues;
          int n;
          n = n0*obj->level[obj->level[level].prevLevel].config[el].nValues + n1*obj->level[level].config[el].nValues + n2;
          if ((n>=0)&&(n<nValues)) {
             _FUNCTION_RETURN_(values[n]);
          }
        } // END  if (values != NULL) 
      }
    }
    #ifdef COMPILE_CHECKS_LOOPS
    }}}
    #endif

  }
  _FUNCTION_RETURN_((FLOAT_TYPE_FTMEM)0);
}
#undef FUNCTION 


// faster way: get feature indicies once, then always retrieve the features directly from the elements
//int featureMemory_getFeatureIndexL0( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n );
// level0 not required, because result == n
// level1 : two dimensional indexing (i.e. low level feature index + index of functional)
int featureMemory_getFeatureIndexL1( pFeatureMemory obj, int level, int el, int n0, int n1 )
#define FUNCTION "featureMemory_getFeatureIndexL1"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    if ((el >= 0) && (el<obj->level[level].nFtEls)) {
    if (obj->level[level].config != NULL) {
    #endif
      if (obj->level[level].nPrevLevels != 1) _FUNCTION_RETURN_(0);
      int n;
      n = n0*obj->level[level].config[el].nValues + n1;
      _FUNCTION_RETURN_(n);
    #ifdef COMPILE_CHECKS_LOOPS
    }}}}
    #endif

  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// level2 : three dimensional indexing (for hierarchical functionals)
int featureMemory_getFeatureIndexL2( pFeatureMemory obj, int level, int el, int n0, int n1, int n2 )
#define FUNCTION "featureMemory_getFeatureIndexL1"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    if ((el >= 0) && (el<obj->level[level].nFtEls)) {
    if (obj->level[level].config != NULL) {
    #endif
      if (obj->level[level].nPrevLevels != 1) _FUNCTION_RETURN_(0);
      int n;
      n = n0*obj->level[obj->level[level].prevLevel].config[el].nValues + n1*obj->level[level].config[el].nValues + n2;
      _FUNCTION_RETURN_(n);
    #ifdef COMPILE_CHECKS_LOOPS
    }}}}
    #endif

  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

int buildNames( pFeatureMemory mem, int level, int el );

// public wrapper for private buildNames function
int featureMemory_buildNames( pFeatureMemory mem, int level, int el )
#define FUNCTION "featureMemory_buildNames"
{_FUNCTION_ENTER_
  int ret = 0;
  if ((mem != NULL)&&(mem->level != NULL)&&(mem->level[level].config != NULL)) {
    ret = buildNames(mem,level,el); 
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 

// private function
int buildNames( pFeatureMemory mem, int level, int el )
#define FUNCTION "buildNames"
{_FUNCTION_ENTER_
  int i,j,ret;
  if ( (mem->level[level].config[el].nValuesDyn != 0) 
       && (mem->level[level].nPrevLevels > 0) ) { // dynamic size not supported (yet?) for subsequent linked levels (only for first level in chain!)
    FEATUM_ERROR(1,"dynamic number of features not supported (yet?) for subsequent linked levels (only for first level in chain!)");
    _FUNCTION_RETURN_(0);
  }
  FEATUM_DEBUG(10,"entering buildNames......");
  
  pFtMemElementConfig config = &(mem->level[level].config[el]);
  // find first level & compute total number of values
  FEATUM_DEBUG(10,"level %i, nPrevLevels %i",level,mem->level[level].nPrevLevels);
  if (mem->level[level].nPrevLevels > 0) {
                                   
    if (mem->level[level].prevLevel >= 0) {
//      if (mem->level[level].nPrevLevels == 1) { // do something...
      if ( (mem->level[mem->level[level].prevLevel].config[el].nTotal <= 0)
          &&(mem->level[mem->level[level].prevLevel].config[el].final_names == NULL) ) {
        ret = buildNames(mem, mem->level[level].prevLevel, el); // build previous level
        if (ret == 0) {
          FEATUM_ERROR(4,"previous buildNames failed");
                _FUNCTION_RETURN_(0);
        }
      }
      if (mem->level[mem->level[level].prevLevel].config[el].final_names == NULL) {
        FEATUM_ERROR(6,"final names still NULL... not proceeding");
        _FUNCTION_RETURN_(0);                                                                   
      }
//      } else {  // > 1  ==> recurse down to 1
//        buildNames(mem, mem->level[level].prevLevel, el);  
//      }
      // ---- now build this level----
      FEATUM_DEBUG(5," prev nTotal: %i, nValues (current) = %i",mem->level[mem->level[level].prevLevel].config[el].nTotal,config->nValues);

      config->nTotal = mem->level[mem->level[level].prevLevel].config[el].nTotal * config->nValues;
      if (config->nTotal <= 0) {
        FEATUM_ERROR(4,"nTotal (%i) <= 0 even though previous level has been built! ",config->nTotal);
        _FUNCTION_RETURN_(0);
      }
      // create names:
      if (config->final_names != NULL) {
        FEATUM_ERROR(1,"final_names already allocated?? why are we here?");
        _FUNCTION_RETURN_(0); 
      }      
      config->final_names = (char **)calloc(1,sizeof(char *)*config->nTotal);
      if (config->final_names == NULL) {
        FEATUM_ERROR(1,"Error allocating memory 1!");
        _FUNCTION_RETURN_(0);
      }
      for (i=0; i<mem->level[mem->level[level].prevLevel].config[el].nTotal; i++) { 
        int len0 = strlen(mem->level[mem->level[level].prevLevel].config[el].final_names[i]);
        for (j=0; j<config->nValues; j++) {
          int len1 = strlen(config->names[j]);
          char *st = NULL;
          st = (char *)calloc(1,sizeof(char) * (len0+len1+1+1) );

          strncpy(st, mem->level[mem->level[level].prevLevel].config[el].final_names[i], len0+1);
          FEATUM_DEBUG(15,"MULTI_LEVEL name: i=%i j=%i \"%s\" sfx: \"%s\"",i,j,st, config->names[j]);
          
          strncat(st, "-", 1);
          strncat(st, config->names[j], len1);
//#warning feature name handling is currently broken due to BUGGY BLOODY M*CROSFT WINDOWS!!!!
          config->final_names[i*(config->nValues)+j] = st;
          FEATUM_DEBUG(14,"MULTI_LEVEL name: i=%i j=%i \"%s\" sfx: \"%s\" (l1:%i, l0:%i)",i,j,config->final_names[i*(config->nValues)+j], config->names[j],len1,len0);

//          strncat(st, config->names[j], len1+1);
          
//          char * appd = config->final_names[i*(config->nValues)+j] + len0;
//          *appd = '-'; // delimiter
          //appd++;
//          *appd = 0;
          /*
          if (el < 0) {
            if (config->names[j] != NULL ) 
              strncpy(appd, config->names[j], len1+1); // len1
          appd += len1;
          *appd = 0;
          }
          */
          //FEATUM_DEBUG(8,"MULTI_LEVEL b name: i=%i j=%i \"%s\" sfx: \"%s\"",i,j,config->final_names[i*(config->nValues)+j], config->names[j]);
        }
      }
      _FUNCTION_RETURN_(1);
    }
  } else { // already on first level in link chain
    if ((config->nTotal > 0) && (config->final_names != NULL)) { // nothing to do...
      _FUNCTION_RETURN_(1);                    
    } else {
  
    // check for dynamic features:
    if (config->nValuesDyn > 0) { 
 
      FEATUM_DEBUG(9,"setting up names for dynamic feature (level %i, el %i)",level,el);
      // retrieve actual nValues from feature frame... use allFilledIdx
      if (mem->level[level].allFilledIdx >= 0) {
        int rIdx;
        if (vIdxInRange(&(mem->level[level]),mem->level[level].allFilledIdx)) {
          rIdx = vIdxToRealIdx(&(mem->level[level]),mem->level[level].allFilledIdx);
        } else { // there seems to be a problem with data filling... try curIdx instead
          if (vIdxInRange(&(mem->level[level]),mem->level[level].curIdx)) {
            rIdx = vIdxToRealIdx(&(mem->level[level]),mem->level[level].curIdx);
          } else {  // serious trouble...
            FEATUM_ERROR(2,"something is seriously wrong with allFilledIdx AND curIdx, both are out of range!");
            _FUNCTION_RETURN_(0);
          }               
        }
        // try rIdx:
        if ((mem->level[level].ft != NULL)&&(mem->level[level].ft[el] != NULL)
             &&(mem->level[level].ft[el][rIdx].values != NULL)
             &&(mem->level[level].ft[el][rIdx].nValues > 0) ) {
//        if ((mem->level[level].ft != NULL)&&(mem->level[level].ft[rIdx] != NULL)
//             &&(mem->level[level].ft[rIdx][el] != NULL)&&(mem->level[level].ft[rIdx][el]->values != NULL)
//             &&(mem->level[level].ft[rIdx][el]->nValues > 0) ) {
             config->nValues = mem->level[level].ft[el][rIdx].nValues;
             config->nTotal = mem->level[level].ft[el][rIdx].nValues;
        } else {  // setup of names is currently not possible
          FEATUM_ERROR(8,"setup of names is currently not possible (level=%i, el=%i, rIdx=%i)",level,el,rIdx);
        }
      } else { // cannot setup names yet...
        FEATUM_ERROR(8,"cannot setup names yet... allFilledIdx=%i on level=%i and el=%i",mem->level[level].allFilledIdx,level,el);
        _FUNCTION_RETURN_(0);
      }
      config->nTotal = config->nValues;
      
      // init names:
      if (config->final_names != NULL) {
        FEATUM_ERROR(1,"final_names already allocated?? why are we here?");
        _FUNCTION_RETURN_(0); 
      }              
      config->final_names = (char **)calloc(1,sizeof(char *)*config->nTotal);
      if (config->final_names == NULL) {
        FEATUM_ERROR(1,"Error allocating memory 2!");
        _FUNCTION_RETURN_(0);
      }
      
      // setup dynamic names
      for (i=0; i<config->nTotal; i++) {
        int curN = i % config->nValuesDyn;
        int len0 = strlen(config->names[curN]);
        config->final_names[i] = (char *)calloc(1,sizeof(char)*(len0+12+1));
        strncpy(config->final_names[i], config->names[curN], len0);
        char *appd = config->final_names[i] + len0;
        snprintf(appd,12,"%i",i/config->nValuesDyn);
        FEATUM_DEBUG(9,"final name: (%i) : %s",i,config->final_names[i]);
/*        FEATUM_DEBUG(9,"final name: (0) : %s",config->final_names[0]);
        FEATUM_DEBUG(9,"final name: (1) : %s",config->final_names[1]);
        FEATUM_DEBUG(9,"final name: (2) : %s",config->final_names[2]);        */
      }       
   
      _FUNCTION_RETURN_(1);   
 
    } else { // static

      config->nTotal = config->nValues;
      // copy names:
      config->final_names = (char **)calloc(1,sizeof(char *)*config->nTotal);
      
      if (config->final_names == NULL) {
        FEATUM_ERROR(1,"Error allocating memory 3!");
        _FUNCTION_RETURN_(0);
      }
      for (i=0; i<config->nTotal; i++) { 
        config->final_names[i] = strdup(config->names[i]);
      }
      
      _FUNCTION_RETURN_(1);
    }
    }
  }
  
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// set all values to -1
void featureMemory_initFtID(pFtIdentifier ftID) {
  if (ftID != NULL) {
    ftID->name = NULL; 
    ftID->isValidated = 0;
    ftID->found =0;
    ftID->level = -1;
    ftID->el = -1;
    ftID->n = -1;
    ftID->nVals = 0;
  }  
}


// internal function only! it performs no checks...
int featureMemory_findFeature_EL(pFeatureMemory obj, const char* name, pFtIdentifier ftID)
#define FUNCTION "featureMemory_findFeature_EL"
{_FUNCTION_ENTER_
  // search only on one level:
  int level = ftID->level;
  int el = ftID->el;
  int ret = 0;

  if (ftID->n >= 0) _FUNCTION_RETURN_(1);
  
  pFtMemElementConfig config = &(obj->level[level].config[el]);
  if ((config->nTotal <= 0)&&(config->final_names == NULL)) {
    FEATUM_DEBUG(10,"about to build feature names...");
    if (buildNames(obj,level,el) == 0) { //error
      FEATUM_ERROR(4,"could not build names...!");
      _FUNCTION_RETURN_(0);         
    }  
  }
  //FEATUM_DEBUG(7,"searching '%s' on level %i, el %i\n",name,level,el);
  int n;
  for (n=0; n<config->nTotal; n++) {
    //FEATUM_DEBUG(9,"  n %i :: '%s'\n",n,config->final_names[n]);
    if (!strcmp(config->final_names[n],name)) {
      ftID->n = n;
      ftID->nVals = obj->level[level].config->nTotal-n;
      ftID->found = 1;
      printf(" found '%s' at level %i, el %i, n %i\n",name,ftID->level,ftID->el,n); fflush(stdout);
      ret = 1;
      break; // stop, we found the feature!                                          
    } 
  }
  if (ret == 0) ftID->n = -1;
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION

// internal function only! it performs no checks...
int featureMemory_findFeature_L(pFeatureMemory obj, const char* name, pFtIdentifier ftID){
  // search only on one level:
  int level = ftID->level;
  int ret = 0;
  if (ftID->el >= 0) {  // search for one el only
    ret = featureMemory_findFeature_EL(obj, name, ftID);
  } else { // search on all 0..nFtEls elements
    int el;
    for (el=0; el < obj->level[level].nFtEls; el++) {
      ftID->el = el;
      ret = featureMemory_findFeature_EL(obj, name, ftID);
      if (ret == 1) break;  // we found the feature... stop!
    }
    if (ret == 0) ftID->el = -1;
  }
  return ret;
}

// find a feature by name, give known values of level, el, n in ftID, otherwise set to -1
int featureMemory_findFeature(pFeatureMemory obj, const char* name, pFtIdentifier ftID)
#define FUNCTION "featureMemory_findFeature"
{_FUNCTION_ENTER_
  int ret = 0;    
  if ((obj != NULL)&&(obj->level != NULL)&&(ftID != NULL)&&(name != NULL)) {
    if (ftID->level >= 0) {  // search only on level:
      ret = featureMemory_findFeature_L(obj, name, ftID);       
    } else {
      int level;
      for (level=0; level<obj->nLevels; level++) {
        ftID->level = level;
        ret = featureMemory_findFeature_L(obj, name, ftID);
        if (ret == 1) break;
      }       
      if (ret == 0) ftID->level = -1;
    }
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION

/*
// vIdx can be -1 to -3 for special indicies
// slow get feature functions...  level0: direct (raw) indexing (always works)
FLOAT_TYPE_FTMEM featureMemory_getFeature( pFeatureMemory obj, int level, LONG_IDX vIdx, int el, int n )
#define FUNCTION "featureMemory_getFeature"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)) {
    #ifdef COMPILE_CHECKS_LOOPS
    if ( (level>=0) && (level<obj->nLevels) ) {
    if ((el >= 0) && (el<obj->level[level].nFtEls) && (n>=0)) {
    #endif
    if (vIdxInRange(&(obj->level[level]),vIdx)) {
      int rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
      if ( (obj->level[level].ft[el] != NULL) ) {
        FLOAT_TYPE_FTMEM *values = obj->level[level].ft[el][rIdx].values;
        if (values != NULL) {
          int nValues = obj->level[level].ft[el][rIdx].nValues;
          if (n<nValues) {
             _FUNCTION_RETURN_(values[n]);
          }
        } // END  if (values != NULL)
      }
    }
    #ifdef COMPILE_CHECKS_LOOPS
    }}
    #endif
  }
  _FUNCTION_RETURN_((FLOAT_TYPE_FTMEM)0);
}
#undef FUNCTION 
*/

// returns pointer to 1 or more (ftID.nVals) features 
// relIdx is relative to the current Index (allFilledIdx)
FLOAT_TYPE_FTMEM * featureMemory_getFeaturesByID(pFeatureMemory obj, pFtIdentifier ftID, LONG_IDX relIdx)
#define FUNCTION "featureMemory_getFeatureByID"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(obj->level != NULL)&&(ftID != NULL)) {
    if (ftID->isValidated) {  // perform less checks...
      int level = ftID->level;
      LONG_IDX vIdx = obj->level[level].allFilledIdx+relIdx;
      int el = ftID->el;
      if (vIdxInRange(&(obj->level[level]),vIdx)) {
        LONG_IDX rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
        FLOAT_TYPE_FTMEM *values = obj->level[level].ft[el][rIdx].values;
        if (values != NULL) {
          _FUNCTION_RETURN_(values+ftID->n);
        }
      }
    
    
    } else {           
           
    int level = ftID->level;
    if ((level < 0)||(level > obj->nLevels)) {
      FEATUM_ERROR(4,"level out of range (%i)\n",level);
      _FUNCTION_RETURN_(NULL);           
    }
    LONG_IDX vIdx = obj->level[level].allFilledIdx+relIdx;
    if (vIdxInRange(&(obj->level[level]),vIdx)) {
      LONG_IDX rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
      int el = ftID->el;
      if ((el < 0)||(el > obj->level[level].nFtEls)) {
        FEATUM_ERROR(4,"el out of range (%i)\n",el);
        _FUNCTION_RETURN_(NULL);           
      }
      if ( (obj->level[level].ft[el] != NULL) ) {
        FLOAT_TYPE_FTMEM *values = obj->level[level].ft[el][rIdx].values;
        if (values != NULL) {
          int nValues = obj->level[level].ft[el][rIdx].nValues;
          int n = ftID->n;
          if (n<nValues) {
            FLOAT_TYPE_FTMEM * ret = values+n;
            if (n+ftID->nVals > nValues) {
              ftID->nVals = nValues-n;                   
            }
            ftID->isValidated = 1;
            _FUNCTION_RETURN_(ret);

          }
        } // END  if (values != NULL)
      }
    }
    
    } // end else...

  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 


// the names returned by these functions are the same for all three functions, only the referencing of the features is different
// get feature name functions...  level0: direct (raw) indexing (always works)
char * featureMemory_getFeatureName( pFeatureMemory obj, int level, int el, int n )
#define FUNCTION "featureMemory_getFeatureName"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (level >= obj->nLevels) _FUNCTION_RETURN_(NULL);
    if (obj->level == NULL) _FUNCTION_RETURN_(NULL);
    if (el < 0) _FUNCTION_RETURN_(NULL);
    if (el >= obj->level[level].nFtEls) _FUNCTION_RETURN_(NULL);
    if (obj->level[level].config == NULL) _FUNCTION_RETURN_(NULL);
    if (n < 0) _FUNCTION_RETURN_(NULL);
    #endif
    pFtMemElementConfig config = &(obj->level[level].config[el]);
    if ((config->nTotal <= 0)&&(config->final_names == NULL)) {
      FEATUM_DEBUG(10,"about to build feature names...");
      if (buildNames(obj,level,el) == 0) { //error
        FEATUM_ERROR(4,"could not build names...!");
        _FUNCTION_RETURN_(NULL);         
      }  
    }

    config = &(obj->level[level].config[el]);
    if ((config->nTotal > 0)&&(n < config->nTotal)) {
      FEATUM_DEBUG(9,"returning final name: (%i) : %s",n,config->final_names[n]);
      _FUNCTION_RETURN_( config->final_names[n] );
    }
    
  }
  _FUNCTION_RETURN_(NULL);   
}
#undef FUNCTION 

// level1 : two dimensional indexing (i.e. low level feature index + index of functional)
//   only works if for the requested level the variable prevLevels == 1
char * featureMemory_getFeatureNameL1( pFeatureMemory obj, int level, int el, int n0, int n1 )
#define FUNCTION "featureMemory_getFeatureNameL1"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (level >= obj->nLevels) _FUNCTION_RETURN_(NULL);
    if (obj->level == NULL) _FUNCTION_RETURN_(NULL);
    if (el < 0) _FUNCTION_RETURN_(NULL);
    if (el >= obj->level[level].nFtEls) _FUNCTION_RETURN_(NULL);
    if (obj->level[level].config == NULL) _FUNCTION_RETURN_(NULL);
    if (n0 < 0) _FUNCTION_RETURN_(NULL);
    if (n1 < 0) _FUNCTION_RETURN_(NULL);
    #endif
    if (obj->level[level].nPrevLevels != 1) _FUNCTION_RETURN_(0);
    pFtMemElementConfig config = &(obj->level[level].config[el]);
    if ((config->nTotal <= 0)&&(config->final_names == NULL)) {
      if (buildNames(obj,level,el) == 0) { //error
        _FUNCTION_RETURN_(NULL);         
      }  
    }

    int n;
    n = n0*obj->level[level].config[el].nValues + n1;
    if ((config->nTotal > 0)&&(n < config->nTotal)) {
      _FUNCTION_RETURN_( config->final_names[n] );
    }
    
  }
  _FUNCTION_RETURN_(NULL);   
}
#undef FUNCTION 


// level2 : three dimensional indexing (for hierarchical functionals)
//   only works if for the requested level the variable prevLevels == 2
char * featureMemory_getFeatureNameL2( pFeatureMemory obj, int level, int el, int n0, int n1, int n2 )
#define FUNCTION "featureMemory_getFeatureNameL2"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (level >= obj->nLevels) _FUNCTION_RETURN_(NULL);
    if (obj->level == NULL) _FUNCTION_RETURN_(NULL);
    if (el < 0) _FUNCTION_RETURN_(NULL);
    if (el >= obj->level[level].nFtEls) _FUNCTION_RETURN_(NULL);
    if (obj->level[level].config == NULL) _FUNCTION_RETURN_(NULL);
    if (n0 < 0) _FUNCTION_RETURN_(NULL);
    if (n1 < 0) _FUNCTION_RETURN_(NULL);
    if (n2 < 0) _FUNCTION_RETURN_(NULL);
    #endif
    if (obj->level[level].nPrevLevels != 2) _FUNCTION_RETURN_(0);
    pFtMemElementConfig config = &(obj->level[level].config[el]);
    if ((config->nTotal <= 0)&&(config->final_names == NULL)) {
      if (buildNames(obj,level,el) == 0) { //error
        _FUNCTION_RETURN_(NULL);         
      }  
    }

    int n;
    n = n0*obj->level[obj->level[level].prevLevel].config[el].nValues + n1*obj->level[level].config[el].nValues + n2;
    if ((config->nTotal > 0)&&(n < config->nTotal)) {
      _FUNCTION_RETURN_( config->final_names[n] );
    }
    
  }
  _FUNCTION_RETURN_(NULL);   
}
#undef FUNCTION 
// feature names can be assigned only after the first frame was completely filled!
// the first call to any getFeatureName function checks whether the name cache has been set up, if not it will set it up

/*
#ifdef OPTIMIZE_SIZE
char *featureMemory_getFeatureName( pFeatureMemory mem, int level, int el, int n )
#else
inline char *featureMemory_getFeatureName( pFeatureMemory mem, int level, int el, int n )
#endif
#define FUNCTION "featureMemory_getFeatureName"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (level >= mem->nLevels) _FUNCTION_RETURN_(NULL);
    if (mem->level == NULL) _FUNCTION_RETURN_(NULL);
    if (el < 0) _FUNCTION_RETURN_(NULL);
    if (el >= mem->level[level].nFts) _FUNCTION_RETURN_(NULL);
    if (mem->level[level].config == NULL) _FUNCTION_RETURN_(NULL);
    if (n < 0) _FUNCTION_RETURN_(NULL);
    if ((mem->level[level].config[el].n > 0)&&(n >= mem->level[level].config[el].n)) _FUNCTION_RETURN_(NULL);
    #endif
    if (mem->level[level].config[el].n == 0) { // dynamic feature memory elements
      if (mem->level[level].config[el].names != NULL) {
        // append number to basename string:
                  //mem->level[level].config[el].names[0]
        int curbase = n % mem->level[level].config[el].nDyn;
        char *base;
        if (mem->level[level].config[el].dynname != NULL) {
          base = mem->level[level].config[el].dynname;
          free(base);
          base = (char *) malloc(sizeof(char)*(strlen(mem->level[level].config[el].names[curbase])+8)); 
        } else {
          base = (char *) malloc(sizeof(char)*(strlen(mem->level[level].config[el].names[curbase])+8)); 
        }
        strncpy(base,mem->level[level].config[el].names[curbase],sizeof(char)*(strlen(mem->level[level].config[el].names[curbase])+8));
        char *append = base + strlen(mem->level[level].config[el].names[curbase]);
        snprintf(append,6,"%i",n/mem->level[level].config[el].nDyn);
        mem->level[level].config[el].dynname = base;
        return base;
      } else _FUNCTION_RETURN_(NULL);
    } else {
      _FUNCTION_RETURN_( mem->level[level].config[el].names[n] );
    }
  }
  _FUNCTION_RETURN_(NULL);   
}
#undef FUNCTION 

*/

/*
pFtMemElement featureMemory_getCurrentElement( pFeatureMemory obj, int level, int el )
#define FUNCTION "featureMemory_getCurrentElement"
{_FUNCTION_ENTER_
  int i;
  if ((obj != NULL)&&(level>=0)&&(el>=0)) {
    if (level >= obj->nLevels) {
      FEATUM_ERR_FATAL(0,"level %i > obj->nLevels %i",level,obj->nLevels);
      _FUNCTION_RETURN_(0);
    }
    if ((obj->level != NULL)&&(obj->level[level].ft != NULL) 
         && (obj->level[level].ft[obj->level[level].curIdx] != NULL)) {
      if (el < obj->level[level].nFts) {
        _FUNCTION_RETURN_( obj->level[level].ft[obj->level[level].curIdx][el] );
      }  else {
        FEATUM_ERROR(1,"Element Nr. %i does not exist (out of bounds: nFts = %i", el , obj->level[level].nFts);
        _FUNCTION_RETURN_(NULL);
      }
    } else if (obj->level[level].ft == NULL) {
       FEATUM_ERROR(1," obj->level[level].ft == NULL, check if you have called featureMemory_memAlloc!");    
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 
*/


// OBSOLETE
// allocate memory for frame with index <idx> on level <level>
int featureMemory_clearFrame( pFeatureMemory obj, int level, int vIdx)
#define FUNCTION "featureMemory_clearFrame"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(level>=0)) {
    if (vIdxInRange(&(obj->level[level]),vIdx)) {
      LONG_IDX rIdx = vIdxToRealIdx( &(obj->level[level]),vIdx);
      int el;
      for (el=0; el<obj->level[level].nFtEls; el++) {
        if (obj->level[level].ft[el][rIdx].values != NULL) free(obj->level[level].ft[el][rIdx].values);
        memzero(&(obj->level[level].ft[el][rIdx]), sizeof(sFtMemElement));
      }
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// OBSOLETE
// allocate memory for frame with index <idx> on level <level>
int featureMemory_allocFrame( pFeatureMemory obj, int level, int vIdx)
#define FUNCTION "featureMemory_allocFrame"
{_FUNCTION_ENTER_
  /*
  int i;
  pFtMemElement * tmp = NULL;
  FEATUM_DEBUG(5," level= %i  obj = %i",level,(long)obj);
  if ((obj != NULL)&&(level>=0)) {
    #ifdef COMPILE_CHECKS
    if (obj->level == NULL) {
      FEATUM_ERROR(1,"obj->level == NULL");
      _FUNCTION_RETURN_(0);
    }
    if (level >= obj->nLevels) {
      FEATUM_ERROR(1,"level %i > obj->nLevels %i",level,obj->nLevels);
      _FUNCTION_RETURN_(0);
    }
    if (level < 0) {
      FEATUM_ERROR(1,"invalid level: level=%i (must be >= 0)",level);
      _FUNCTION_RETURN_(0);
    }
    if (!vIdxInRange(&(obj->level[level]),vIdx)) {
      FEATUM_ERROR(1,"invalid index vIdx=%i -> out of range (no more space left? capacity = %i",vIdx,obj->level[level].capacity);
      _FUNCTION_RETURN_(0);
    }
    #endif
    if (obj->level[level].ft != NULL) {
      int rIdx = vIdxToRealIdx(&(obj->level[level]),vIdx);
      FEATUM_DEBUG(5,"adding new empty frame w. rIdx %i (capacity = %i)\n",rIdx,obj->level[level].capacity);

      
        if (obj->level[level].ft[rIdx] != NULL) {
          tmp = obj->level[level].ft[rIdx];
          for (i=0; i<obj->level[level].nFtEls; i++) {
            FEATUM_DEBUG(8,"i=%i, level=%i, nftels=%i (tmp=%i)",i,level,obj->level[level].nFtEls,(long)tmp);
            if (tmp[i] == NULL) {
//                       printf("here1\n");
              tmp[i] = (pFtMemElement)calloc(1,sizeof(sFtMemElement));  // tmp[i] = 
//                       printf("here2\n"); fflush(stdout);
              if (tmp[i] == NULL) {
                FEATUM_ERR_FATAL(0,"Error allocating memory for sFtMemElement");
                 _FUNCTION_RETURN_(0);
              }   
            }           
          }          
        }
      //}
      _FUNCTION_RETURN_(1);
    } else {
       FEATUM_ERROR(1," obj->level[level].ft == NULL, check if you have called featureMemory_memAlloc!");    
    }
  }
  */
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION 

// allocate memory for subsequent frame, increase current frame counter
int featureMemory_addFrame( pFeatureMemory obj, int level)
#define FUNCTION "featureMemory_addFrame"
{_FUNCTION_ENTER_
  int ret = 0;
  if ((obj != NULL)&&(obj->level != NULL)) {
    obj->level[level].curIdx++;  // at the beginning curIdx must be initialised with -1, so the first frame allocated will be the one with index 0
    //ret = featureMemory_allocFrame( obj, level, obj->level[level].curIdx);
    if (obj->level[level].bufferType == FTMEM_RING_BUFFER) {
      ret = featureMemory_clearFrame( obj, level, obj->level[level].curIdx);
    } else { ret = 1; }
    _FUNCTION_RETURN_(ret);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


/*
inline pFtMemFrame featureMemory_freeFrame( pFtMemFrame obj )
#define FUNCTION "featureMemory_freeFrame"
{_FUNCTION_ENTER_
  if (obj != NULL) free(obj);          
}
#undef FUNCTION           


// the frame must be freed by the calling code using simple free(...);
pFtMemFrame featureMemory_getFrame( pFeatureMemory obj, int level, LONG_IDX absIdx )
#define FUNCTION "featureMemory_getElement"
{_FUNCTION_ENTER_
  pFtMemFrame ret;
  
  #ifdef COMPILE_CHECKS
  if ((obj != NULL)&&(level>=0)&&(absIdx > 0)) {
  #else
  if (obj != NULL) {
  #endif
    if (level >= obj->nLevels) {
      FEATUM_ERR_FATAL(0,"level %i > obj->nLevels %i",level,obj->nLevels);
      _FUNCTION_RETURN_(NULL);
    }
    #ifdef COMPILE_CHECKS
    if ((obj->level != NULL)&&(obj->level[level].ft != NULL) && (absIdx < obj->level[level].capacity)
         && (obj->level[level].ft[absIdx] != NULL)) {
    #else
    if ((obj->level != NULL)&&(obj->level[level].ft != NULL) 
         && obj->level[level].ft[absIdx] != NULL)) {
    #endif
      ret = (pFtMemFrame)calloc(1,sizeof(sFtMemFrame));
      if (ret != NULL) {
        ret->nFts = obj->level[level].nFts;
        // TODO: compute timestamp(samples) and timestampSec
        ret->timestampSec = (float)absIdx * obj->level[level].T;
        ret->timestamp = obj->level[level].frameStep * absIdx;
        ret->frame = obj->level[level].ft[absIdx];
        if (absIdx < obj->level[level].allFilledIdx) ret->allFilled = 1;
        _FUNCTION_RETURN_( ret );
      } else {
        FEATUM_ERR_FATAL(0,"Error allocating memory!");
        _FUNCTION_RETURN_(NULL);
      }
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 
*/

/*
// setup names in feature memory object
// nDyn is the number of basnames if the number of values is dynamic
#ifdef OPTIMIZE_SIZE
int featureMemory_setupElement( pFeatureMemory mem, int level, int el, int nVal, int nDyn, char **names, int enabled )
#else
inline int featureMemory_setupElement( pFeatureMemory mem, int level, int el, int nVal, int nDyn, char **names, int enabled )
#endif
#define FUNCTION "featureMemory_setupElement"
{_FUNCTION_ENTER_
  if ((mem != NULL)&&(level >= 0)) {
    FEATUM_DEBUG(3,"el=%i, level=%i, nVal=%i, enabled=%i",el,level,nVal,enabled);
    #ifdef COMPILE_CHECKS
    if ((level < mem->nLevels) && (mem->level != NULL)) {
    #endif
        FEATUM_DEBUG(3,"mem->level[level].config = %i",(int)(mem->level[level].config));
        mem->level[level].config[el].n = nVal;
        mem->level[level].config[el].nDyn = nDyn;
        mem->level[level].config[el].names = names;  // TODO: ??? correct ???
        mem->level[level].config[el].enabled = enabled;
        _FUNCTION_RETURN_(1);   
    #ifdef COMPILE_CHECKS
    }
    #endif
  }
  _FUNCTION_RETURN_(0);   
}
#undef FUNCTION 
*/

// TODO: function to return numbered feature names for dynamic features:
// calling application must free the returned pointer -> SOLVED? do not free pointer!
// char *featureMemory_getFeatureName(
/*
#ifdef OPTIMIZE_SIZE
char *featureMemory_getFeatureName( pFeatureMemory mem, int level, int el, int n )
#else
inline char *featureMemory_getFeatureName( pFeatureMemory mem, int level, int el, int n )
#endif
#define FUNCTION "featureMemory_getFeatureName"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (level >= mem->nLevels) _FUNCTION_RETURN_(NULL);
    if (mem->level == NULL) _FUNCTION_RETURN_(NULL);
    if (el < 0) _FUNCTION_RETURN_(NULL);
    if (el >= mem->level[level].nFts) _FUNCTION_RETURN_(NULL);
    if (mem->level[level].config == NULL) _FUNCTION_RETURN_(NULL);
    if (n < 0) _FUNCTION_RETURN_(NULL);
    if ((mem->level[level].config[el].n > 0)&&(n >= mem->level[level].config[el].n)) _FUNCTION_RETURN_(NULL);
    #endif
    if (mem->level[level].config[el].n == 0) { // dynamic feature memory elements
      if (mem->level[level].config[el].names != NULL) {
        // append number to basename string:
                  //mem->level[level].config[el].names[0]
        int curbase = n % mem->level[level].config[el].nDyn;
        char *base;
        if (mem->level[level].config[el].dynname != NULL) {
          base = mem->level[level].config[el].dynname;
          free(base);
          base = (char *) malloc(sizeof(char)*(strlen(mem->level[level].config[el].names[curbase])+8)); 
        } else {
          base = (char *) malloc(sizeof(char)*(strlen(mem->level[level].config[el].names[curbase])+8)); 
        }
        strncpy(base,mem->level[level].config[el].names[curbase],sizeof(char)*(strlen(mem->level[level].config[el].names[curbase])+8));
        char *append = base + strlen(mem->level[level].config[el].names[curbase]);
        snprintf(append,6,"%i",n/mem->level[level].config[el].nDyn);
        mem->level[level].config[el].dynname = base;
        return base;
      } else _FUNCTION_RETURN_(NULL);
    } else {
      _FUNCTION_RETURN_( mem->level[level].config[el].names[n] );
    }
  }
  _FUNCTION_RETURN_(NULL);   
}
#undef FUNCTION 
*/

#ifdef OPTIMIZE_SIZE
int featureMemory_getNLevels( pFeatureMemory mem ) 
#else
inline int featureMemory_getNLevels( pFeatureMemory mem ) 
#endif
#define FUNCTION "featureMemory_getNLevels"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    _FUNCTION_RETURN_(mem->nLevels);
  }
  _FUNCTION_RETURN_(0);   
}
#undef FUNCTION 


// get the number of feature elements per vector on level <level>
#ifdef OPTIMIZE_SIZE
int featureMemory_getNFtEls( pFeatureMemory mem, int level )
#else
inline int featureMemory_getNFtEls( pFeatureMemory mem, int level )
#endif
#define FUNCTION "featureMemory_getNFtEls"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(0);
    if (level >= mem->nLevels) _FUNCTION_RETURN_(0);
    if (mem->level == NULL) _FUNCTION_RETURN_(0);
    #endif
    _FUNCTION_RETURN_(mem->level[level].nFtEls);
  }
  _FUNCTION_RETURN_(0);   
}
#undef FUNCTION 


//get the number of features for a certain feature element <el> on level <level>
#ifdef OPTIMIZE_SIZE
int featureMemory_getNFeatures( pFeatureMemory mem, int level, int el )
#else
inline int featureMemory_getNFeatures( pFeatureMemory mem, int level, int el )
#endif
#define FUNCTION "feattureMemory_getNFeatures"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(0);
    if (level >= mem->nLevels) _FUNCTION_RETURN_(0);
    if (mem->level == NULL) _FUNCTION_RETURN_(0);
    if (el < 0) _FUNCTION_RETURN_(0);
    if (el >= mem->level[level].nFtEls) _FUNCTION_RETURN_(0);
    if (mem->level[level].config == NULL) _FUNCTION_RETURN_(0);
    #endif
    _FUNCTION_RETURN_( mem->level[level].config[el].nTotal );
  }
  _FUNCTION_RETURN_(0);   
}
#undef FUNCTION 

//get the number of features for a certain feature element <el> on level <level> by reading it from data memory at index <vIdx>
#ifdef OPTIMIZE_SIZE
int featureMemory_getNFeatures_idx( pFeatureMemory mem, int level, int el, LONG_IDX vIdx )
#else
inline int featureMemory_getNFeatures_idx( pFeatureMemory mem, int level, int el, LONG_IDX vIdx )
#endif
#define FUNCTION "feattureMemory_getNFeatures_idx"
{_FUNCTION_ENTER_
    /*
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(0);
    if (level >= mem->nLevels) _FUNCTION_RETURN_(0);
    if (mem->level == NULL) _FUNCTION_RETURN_(0);
    if (el < 0) _FUNCTION_RETURN_(0);
    if (el >= mem->level[level].nFtEls) _FUNCTION_RETURN_(0);
    if (mem->level[level].config == NULL) _FUNCTION_RETURN_(0);
    #endif*/
    pFtMemElement elem = featureMemory_getElement(mem,level,vIdx,el);
    if (elem != NULL) {
      _FUNCTION_RETURN_( elem->nValues );
    } 
    /*
    if (vIdxInRange(&(mem->level[level]), vIdx)) {
      LONG_IDX rIdx = vIdxToRealIdx(&(mem->level[level]), vIdx);
      _FUNCTION_RETURN_( mem->level[level].ft[rIdx][el]->nValues );
    } else { _FUNCTION_RETURN_(0); }
    */
  //}
  _FUNCTION_RETURN_(0);   
}
#undef FUNCTION 


//get the number of features for a complete feature frame on level <level>
#ifdef OPTIMIZE_SIZE
int featureMemory_getNFeaturesPerFrame( pFeatureMemory mem, int level )
#else
inline int featureMemory_getNFeaturesPerFrame( pFeatureMemory mem, int level )
#endif
#define FUNCTION "featureMemory_getNFeaturesPerFrame"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    #ifdef COMPILE_CHECKS
    if (level < 0) _FUNCTION_RETURN_(0);
    if (level >= mem->nLevels) _FUNCTION_RETURN_(0);
    if (mem->level == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (mem->level[level].nFeaturesPerFrame == -1) {  // not initialized
      int i; 
      if (mem->level[level].config == NULL) _FUNCTION_RETURN_(0);
      mem->level[level].nFeaturesPerFrame = 0;
      for (i=0; i<mem->level[level].nFtEls; i++) {
        if (mem->level[level].config[i].enabled) {
          if (mem->level[level].config[i].nTotal >= 0) {
            mem->level[level].nFeaturesPerFrame += mem->level[level].config[i].nTotal;
          } else {
            FEATUM_WARNING(3,"Cannot yet determine total number of features due to dynamic feature elements! Call a getName function first...");
            _FUNCTION_RETURN_(0);         
          }
        }
      }
    }
    _FUNCTION_RETURN_( mem->level[level].nFeaturesPerFrame );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


//   flushToFtMem: rename to getValWrite  (get values of ft element to write into and setup nVal etc.)
//                 this function should be available in two variants: a) sequential using curIdx + relidx  
//                                                                    b) random, by specifying absIdx
//                    the above two functions will internally call allocFrame


// getValuesForWrite_abs: do checks, allocate memory for values array, this function takes an absolute frame index
//       and return pointer to values array to be filled by custom flushToFtMem function
// IMPORTANT: the frame must be filled with data after this call because the filled flag is set to one by this function!
#ifdef OPTIMIZE_SIZE
FLOAT_TYPE_FTMEM * featureMemory_getValuesForWrite_abs( pFeatureMemory mem, int level, int el, int nVal, LONG_IDX vIdx, FLOAT_TYPE_TIME timestamp )
#else
inline FLOAT_TYPE_FTMEM * featureMemory_getValuesForWrite_abs( pFeatureMemory mem, int level, int el, int nVal, LONG_IDX vIdx, FLOAT_TYPE_TIME timestamp )
#endif
#define FUNCTION "featureMemory_getValuesForWrite_abs"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= mem->nLevels) _FUNCTION_RETURN_(NULL);
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (mem->level == NULL) _FUNCTION_RETURN_(NULL);
    #endif
    if (mem->level[level].ft == NULL) {
      FEATUM_DEBUG(9,".ft == NULL");
      _FUNCTION_RETURN_(NULL);
    }
    if (!vIdxInRange(&(mem->level[level]), vIdx)) {
      FEATUM_ERROR(8,"vIdx = %i out of range for level %i",vIdx,level);
      _FUNCTION_RETURN_(NULL);
    }
    int rIdx = vIdxToRealIdx ( &(mem->level[level]), vIdx );
    if (mem->level[level].ft[el] == NULL) {
      FEATUM_DEBUG(9,".ft[el=%i] == NULL",el);
      _FUNCTION_RETURN_(NULL);
    }
    /*
    if (mem->level[level].ft[absidx][el] == NULL) {
      FEATUM_DEBUG(9,".ft[absidx=%i][%i] == NULL",absidx,el);
      _FUNCTION_RETURN_(NULL);
    }
    */
    pFtMemElement current = &(mem->level[level].ft[el][rIdx]);
    FEATUM_DEBUG(11," level = %i, rIdx = %i, el=%i  (vIdx=%i) (current %i)", level, rIdx, el,vIdx,(long)current);

/*    if (current == NULL) { // frame not allocated? use addFrame to do so...
      FEATUM_DEBUG(10,"frame ft[rIdx=%i][el=%i] on level %i not allocated.. calling allocFrame",rIdx,el,level);
      if (!featureMemory_allocFrame(mem, level, vIdx)) {
        FEATUM_ERROR(0,"error allocating frame!");
        _FUNCTION_RETURN_(NULL);                                   
      }
      current = mem->level[level].ft[rIdx][el];
    }*/
    #ifdef COMPILE_CHECKS_LOOPS
    if (mem->level[level].t != NULL) {
    #endif
      mem->level[level].t[rIdx] = timestamp;
    #ifdef COMPILE_CHECKS_LOOPS
    }
    #endif
    if (current->values == NULL) {
      current->nValues = nVal;
      current->values 
        = (FLOAT_TYPE_FTMEM *)calloc(1,sizeof(FLOAT_TYPE_FTMEM)* nVal);
    } else {
      if (current->nValues != nVal) {
        free(current->values);
        current->values = (FLOAT_TYPE_FTMEM *)calloc(1,sizeof(FLOAT_TYPE_FTMEM)* nVal);
        current->nValues = nVal;        
      } else {
        memzero(current->values,current->nValues * sizeof(FLOAT_TYPE_FTMEM) );      
      }    
    }
    current->filled = 1;
    FEATUM_DEBUG(11,"getValues_abs... nVal=%i , level = %i, vIdx = %i, el=%i (mfcc=%i)",nVal,  level, vIdx,el,ft_lld_mfcc);
    FEATUM_DEBUG(11,"returning pointer to values...:");
    //TODO: assign timestamps...

    _FUNCTION_RETURN_( current->values );
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION


// getValuesForWrite_seq: do checks, allocate memory for values array, this function takes a frame index relative to the current sequential processing index
//       and return pointer to values array to be filled by custom flushToFtMem function
// IMPORTANT: the frame must be filled with data after this call because the filled flag is set to one by this function!
#ifdef OPTIMIZE_SIZE
FLOAT_TYPE_FTMEM * featureMemory_getValuesForWrite_seq( pFeatureMemory mem, int level, int el, int nVal, LONG_IDX relidx, FLOAT_TYPE_TIME timestamp )
#else
inline FLOAT_TYPE_FTMEM * featureMemory_getValuesForWrite_seq( pFeatureMemory mem, int level, int el, int nVal, LONG_IDX relidx, FLOAT_TYPE_TIME timestamp )
#endif
#define FUNCTION "featureMemory_getValuesForWrite_seq"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM *ret = NULL;

  if (mem != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= mem->nLevels) _FUNCTION_RETURN_(NULL);
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (mem->level == NULL) _FUNCTION_RETURN_(NULL);
    #endif
//    LONG_IDX vIdx = mem->level[level].curIdx + relidx;
    LONG_IDX vIdx;
    if (mem->level[level].curIdx < 0) {
      vIdx = relidx;
    } else {
      vIdx = mem->level[level].curIdx + relidx;
    }
    if (!vIdxInRange( &(mem->level[level]), vIdx )) {
      FEATUM_DEBUG(9,"invalid relidx (%i) .. computed vIdx(%i) is out of range! (curIdx=%i for level=%i)",relidx,vIdx,mem->level[level].curIdx,level);
      _FUNCTION_RETURN_(NULL);
    } else {
      FEATUM_DEBUG(11," nVal=%i , curIdx = %i, level = %i, vIdx = %i",nVal, mem->level[level].curIdx, level, vIdx);
      ret = featureMemory_getValuesForWrite_abs( mem, level, el, nVal, vIdx, timestamp );
    }
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// flushToMem: do checks and return pointer to values array to be filled by custom flushToFtMem function
// the values portion of the feature element will be allocated by this function
#if 0
#ifdef OPTIMIZE_SIZE
FLOAT_TYPE_FTMEM * featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal, int relidx, FLOAT_TYPE_TIME timestamp )
#else
inline FLOAT_TYPE_FTMEM * featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal, int relidx, FLOAT_TYPE_TIME timestamp )
#endif
#define FUNCTION "featureMemory_flushToFtMem"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= mem->nLevels) _FUNCTION_RETURN_(NULL);
    if (level < 0) _FUNCTION_RETURN_(NULL);
    if (mem->level == NULL) _FUNCTION_RETURN_(NULL);
    #endif
    if (mem->level[level].ft == NULL) {
      FEATUM_DEBUG(9,".ft == NULL");
      _FUNCTION_RETURN_(NULL);
    }  
    if (relidx > 0) relidx = 0;   // future frames not allowed!
    int vIdx = mem->level[level].curIdx + relidx;
    if (vIdx < 0) vIdx = 0;  // ????
    int rIdx;
    if (vIdxInRange( &(mem->level[level]), vIdx )) {
      rIdx = vIdxToRealIdx ( &(mem->level[level]) , vIdx );
    } else {
      FEATUM_ERROR(9,"vIdx (%i) out of range for level %i!",vIdx,level);
      _FUNCTION_RETURN_(NULL);
    } 
    if (mem->level[level].ft[el] == NULL) {
      FEATUM_DEBUG(9,".ft[el=%i] == NULL",el);
      _FUNCTION_RETURN_(NULL);
    }
/*    if (mem->level[level].ft[rIdx][el] == NULL) {
      _FUNCTION_RETURN_(NULL);
    }*/
/*    FEATUM_DEBUG(11,"checking alloc... level = %i, rIdx = %i, vIdx = %i", level, rIdx,vIdx);
    if (mem->level[level].ft[rIdx][el] == NULL) { // frame not allocated? use addFrame to do so...
      FEATUM_DEBUG(9,".ft[%i][%i] == NULL , use addFrame first, to allocate frame!",rIdx,el);
       _FUNCTION_RETURN_(NULL);    // TODO: auto alloc frame here... no more need for addFrame
    }*/
    if (mem->level[level].ft[el][rIdx].values == NULL) {
      mem->level[level].ft[el][rIdx].nValues = nVal;
      mem->level[level].ft[el][rIdx].values 
        = (FLOAT_TYPE_FTMEM *)calloc(1,sizeof(FLOAT_TYPE_FTMEM)* nVal);
    }
    mem->level[level].ft[el][rIdx].filled = 1;
    //    FEATUM_DEBUG(11,"returning pointer to values...:");
    //TODO: assign timestamps...
    
    _FUNCTION_RETURN_( mem->level[level].ft[el][rIdx].values );

    /*
    mem->level[level].ft[mem->level[level].curIdx][FT_LLD_ENERGY]->values[0] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->energy->rmsEnergy; 
    mem->level[level].ft[mem->level[level].curIdx][FT_LLD_ENERGY]->values[1] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->energy->logEnergy; 
    */
    //lldex->current[level]->energy->flush_status = LLDEX_FLUSHED;
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION
#endif


// check the fill status of the feature Memory level <level> and update the status
int featureMemory_checkStatus( pFeatureMemory mem, int level )
#define FUNCTION "featureMemory_checkStatus"
{_FUNCTION_ENTER_
  LONG_IDX idx;
  LONG_IDX vIdx,rIdx;
  int el;

  if (mem != NULL) {
    #ifdef COMPILE_CHECKS
    if (mem->level == NULL) { _FUNCTION_RETURN_(0); }
    if (level >= mem->nLevels) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    #endif
    vIdx = mem->level[level].curIdx;
    if ( !vIdxInRange(&(mem->level[level]), vIdx) ) {
      FEATUM_ERROR(0,"the current index (vIdx=%i) in feature Memory level %i is out of range (capacity = %i)!", vIdx, level, mem->level[level].capacity);
      _FUNCTION_RETURN_(0);
    }

    // start at allFilledIdx:
    FEATUM_DEBUG(8,"CHECK_STATUS: first = %i, last = %i",mem->level[level].allFilledIdx+1, mem->level[level].curIdx);
//    FEATUM_MESSAGE(2,"CHECK_STATUS: first = %i, last = %i",mem->level[level].allFilledIdx+1, mem->level[level].curIdx);
    for (idx=mem->level[level].allFilledIdx+1; idx <= mem->level[level].curIdx; idx++) { 
      int idx_filled = 1;
      for (el = 0; el < mem->level[level].nFtEls; el++ ) {
          if (mem->level[level].config[el].enabled) {
            pFtMemElement elem = featureMemory_getElement(mem,level,idx,el);
//            idx_filled &= mem->level[level].ft[rIdx][el]->filled;
            if (elem != NULL) {
              idx_filled &= elem->filled;
              FEATUM_DEBUG(11,"el %i , rIdx %i, filled=%i",el,idx,elem->filled);
            } else {
              idx_filled = 0;       
            }
          }
      }  
      if (idx_filled) mem->level[level].allFilledIdx = idx;
    }
    FEATUM_DEBUG(8,"CHECK_STATUS: allFilled: %i",mem->level[level].allFilledIdx);

    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the object and free all allocated data memory and elements
pFeatureMemory featureMemory_destroy( pFeatureMemory obj )
#define FUNCTION "featureMemory_destroy"
{_FUNCTION_ENTER_
  int i;
  featureMemory_destroyData(obj);
  if (obj != NULL) {
/*      featureMemory_dataMemFree(obj);
      if (obj->level != NULL) {
//        for (i=0; i<obj->nLevels; i++) {
//          if (obj->level[i]->config != NULL) free(obj->level[i]->config);
//          // TODO:...
//        }               // ^done by featureMemory_memFree
        free(obj->level);
      }*/
      free(obj);
  }
  
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

// destroy the object and free all allocated data memory and elements
pFeatureMemory featureMemory_destroyData( pFeatureMemory obj )
#define FUNCTION "featureMemory_destroyData"
{_FUNCTION_ENTER_
  int i;
  if (obj != NULL) {
      featureMemory_dataMemFree(obj);
      if (obj->level != NULL) {
        free(obj->level); obj->level = NULL;
      }
  }
  
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 



