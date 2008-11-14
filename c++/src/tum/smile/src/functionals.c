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
 
 /*  IMPLEMENTATION of the class <functionals>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "Functionals"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "featum_util.h"       //( optional )
#include "functionals.h"

#undef FUNCTION     // use this undef only if you define it below for every function


#include "functionals_comp.h"

// TODO: timestamp handling!!

/* Constructor: creates a class data object */
// all times and sizes are in floating point with unit seconds
pFunctionals functionals_create( pFunctionals obj, int nIDs )
#define FUNCTION "functionals_create"
{_FUNCTION_ENTER_

  if (obj == NULL) {
    obj = (pFunctionals)calloc(1,sizeof(sFunctionals));
  } else {
    memzero(obj, sizeof(sFunctionals));       
  }
  
  if (obj != NULL) {
    obj->nIDs = nIDs;


    obj->config = (pFunctionalIDConfig)calloc(1,sizeof(sFunctionalIDConfig)*nIDs);
    if (obj->config == NULL) { free(obj);  _FUNCTION_RETURN_(NULL); }

    int i=0;
    while (1) {
      if (functConf[i].name == NULL) break;
      i++;
      if (i<0) { FEATUM_ERROR(2,"variable i overflow while parsing functConf"); break; }
    }
    // i now contains the number of configured functionals
    obj->maxFunctionals = i;
    
    for (i=0; i<obj->nIDs; i++) {
      obj->config[i].functsEnabled = (int *)calloc(1,sizeof(int)*obj->maxFunctionals);
      obj->config[i].functsIdx = (int *)calloc(1,sizeof(int)*obj->maxFunctionals);
      obj->config[i].functs = (sFunctionalsConfig *)&functConf;
    }
     
  }
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


/* Constructor: creates a class data object */
// automatically read ll_levels from featureMemory, and call functionals_create internally
pFunctionals functionals_autoCreate( pFunctionals obj, pFeatureMemory mem )
#define FUNCTION "functionals_autoCreate"
{_FUNCTION_ENTER_
  if (mem != NULL) {
    pFunctionals ret = functionals_create(obj, mem->ll_levels);
    _FUNCTION_RETURN_(ret);
  } else {
    _FUNCTION_RETURN_(NULL);
  }
}
#undef FUNCTION 


// id_list: list of ids to enable the given group of functionals on
// the last element of the list must be -1 to terminate the list!
int functionals_enable( pFunctionals obj, const char *name, int *id_list )
#define FUNCTION "functionals_enable"
{_FUNCTION_ENTER_
  if ( (obj == NULL) || (name == NULL) ) _FUNCTION_RETURN_(0);

  // find functional in global config struct "functConf"
  int i=0;
  while(1) {
    if (functConf[i].name == NULL) {// last element
      FEATUM_ERROR(1,"functional %s not found in functConf struct defined in functionals.c!",name);
      _FUNCTION_RETURN_(0);
    }
    if (!strcmp(functConf[i].name,name)) {
      break;                                     
    }
    i++;
    if (i<0) {
             FEATUM_ERROR_FATAL(2,"variable i overflow, while parsing functConf struct!");
             _FUNCTION_RETURN_(0);
    }
  }
  // variable i now contains the index of the functional "name"
  functConf[i].confId = i;
  
  // for each id in id_list add this functional to the enabled list for that id
  int j=0; int id;
  if (id_list != NULL) {
    while(1) {
      if (id_list[j] == -1) break;  // last element
      
      id = id_list[j];
      if ((obj->config[id].nFunctionals < obj->maxFunctionals)&&(!obj->config[id].functsEnabled[i])) { 
        obj->config[id].functsIdx[i] = obj->config[id].nFunctionalValues;
        obj->config[id].functsEnabled[i] = 1;
        obj->config[id].nFunctionalValues += functConf[i].cnt;
        obj->config[id].nFunctionals++;
        if (functConf[i].requireSorted) obj->config[id].requireSorted = 1;
      }

      j++;
      if (j<0) {
             FEATUM_ERROR_FATAL(2,"variable j overflow, while parsing functConf struct!");
             _FUNCTION_RETURN_(0);
      }
    }
  }
  if (j==0) { // enable for all ids
    for (id=0; id<obj->nIDs; id++) {
      if ((obj->config[id].nFunctionals < obj->maxFunctionals)&&(!obj->config[id].functsEnabled[i])) { 
        obj->config[id].functsIdx[i] = obj->config[id].nFunctionalValues;
        obj->config[id].functsEnabled[i] = 1;
        obj->config[id].nFunctionalValues += functConf[i].cnt;
        obj->config[id].nFunctionals++;
        if (functConf[i].requireSorted) obj->config[id].requireSorted = 1;
      }
    }
  }
  
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION 


/*
 create -> setup IDs -> enable functionals -> finalize

*/

// setup feature memory and names for all enabled functionals on all ids
int functionals_finalizeConfiguration( pFunctionals obj )
#define FUNCTION "functionals_finalizeConfiguration"
{_FUNCTION_ENTER_
  if (obj == NULL ) _FUNCTION_RETURN_(0);
  if (obj->config == NULL ) _FUNCTION_RETURN_(0);

  int id; int f;
  for (id = 0; id < obj->nIDs ; id ++) {
    if (obj->config[id].isConfigured == 1) {

    char **names = (char**)malloc(sizeof(char*) * obj->config[id].nFunctionalValues);

    if (names == NULL) {
      FEATUM_ERROR(0,"Error allocating memory!");
      _FUNCTION_RETURN_(0);               
    }
    obj->config[id].names = names;  // must be freed when functionals object is destroyed..
    int N=0;
    for(f=0; f < obj->maxFunctionals ; f++) {
      if (obj->config[id].functsEnabled[f]) {
        int n;
        for (n=0; n < obj->config[id].functs[f].cnt; n++) {
          names[N++] = strdup(obj->config[id].functs[f].names[n]);
        }
      }
    }
    //------------------
    // TODO: initialize config of olevel: names, etc...
       // read names from input level... for each enabled functional append functional name


    int i;
    int olevel = obj->config[id].olevel;
    int ilevel = obj->config[id].ilevel;
    pFeatureMemory mem = obj->config[id].mem;
    for (i=0; i<mem->level[olevel].nFtEls; i++) {
      if (featureMemory_setupElement( mem, olevel, i, obj->config[id].nFunctionalValues, 0, obj->config[id].names, mem->level[ilevel].config[i].enabled ) == 0) {
        FEATUM_ERROR(0,"error during featureMemory_setupElement! (return code 0)");
        _FUNCTION_RETURN_(0);                                
      }
      if (!featureMemory_buildNames(mem, olevel, i)) {
        FEATUM_DEBUG(8,"building of names for level %i and el %i not yet possible!",olevel,i);
      }
    }

    obj->config[id].isConfigured = 2;
    
    }  else { // if not isConfigured == 1
      if (obj->config[id].isConfigured == 2) {
        FEATUM_WARNING(4,"id %i is already configured (state = 2), skipping",id);
        _FUNCTION_RETURN_(0);                                         
      }
      if (obj->config[id].isConfigured == 0) {
        FEATUM_ERROR(4,"id %i is not set up yet, call setupID method first! skipping now...",id);
        _FUNCTION_RETURN_(0);                                         
      }
    
    }
  }
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION



// initialise functionals extractor <id>  (id = 0...nIDs-1)
// initialise the output level <olevel> in the feature memory
// if capacity == -1 then the capacity will be automatically derived from the input level capacity
// T = frameStep
int functionals_setupID( pFunctionals obj, pFeatureMemory mem, int id, int olevel, int ilevel, float T, float computeframeSize, int capacity )
#define FUNCTION "functionals_setupID"
{_FUNCTION_ENTER_
  if ((obj != NULL)&&(mem != NULL)) {
    #ifdef COMPILE_CHECKS
    if (olevel < 0) _FUNCTION_RETURN_(0);
    if (olevel > mem->nLevels) _FUNCTION_RETURN_(0);
    if (ilevel < 0)   _FUNCTION_RETURN_(0);
    if (ilevel > mem->ll_levels)  {
      FEATUM_WARNING(5,"ilevel >  mem->ll_levels !!  check if this is ok!");
    }
    if (ilevel > mem->nLevels)   _FUNCTION_RETURN_(0);
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id > obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config == NULL)  _FUNCTION_RETURN_(0);    
    #endif
    if (capacity == -1) { // automatic capacity
      capacity = mem->level[ilevel].capacity / (int)(T / mem->level[ilevel].T) + 2;            
    }
                 
    obj->config[id].mem = mem;
    
    if (!featureMemory_configureLevel( mem, olevel, capacity, FTMEM_DEFAULT_BUFFER, T, mem->level[ilevel].nFtEls, ilevel)){
      FEATUM_ERROR(0,"error during featureMemory_configureLevel! (return code 0)");
      _FUNCTION_RETURN_(0);                                
    }

    mem->level[olevel].frameSize = (LONG_IDX)round(computeframeSize / mem->level[ilevel].T );

    obj->config[id].computeframeSize = computeframeSize;
    obj->config[id].computeframeSize_frames = (LONG_IDX)round(computeframeSize / mem->level[ilevel].T);
    obj->config[id].frameStep = T;  // note: no T_frames exists... this would only introduce acumulating round-off errors
    obj->config[id].frameStep_input = mem->level[ilevel].T;
    obj->config[id].olevel = olevel;
    obj->config[id].ilevel = ilevel;

    // allocate memory in feature memory only for olevel levels (ilevel memory should have been already allocated before with info from inputFramer)
    if (!featureMemory_dataMemAllocLevel(mem,olevel)) {
      FEATUM_ERROR(0,"error during featureMemory_dataMemAllocLevel! (return code 0)");
      _FUNCTION_RETURN_(0);                                
    }

    obj->config[id].isConfigured = 1;
    
    

     _FUNCTION_RETURN_(1);
     
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// initialise the output levels in the feature memory automatically based on parameters of the lld levels
// fs: array of framesizes in seconds to compute functionals over
// T : array of framesteps/periods in seconds of functionals sampling
// the following is assumed in this function: id is set to ilevel, for each ilevel ONE olevel exists!
int functionals_autoSetupIDs( pFunctionals obj, pFeatureMemory mem, float *T, float *fs )
#define FUNCTION "functionals_autoSetupOutputLevels"
{_FUNCTION_ENTER_
  if ((mem != NULL)&&(T!=NULL)) {
    int i; //int capacity;
    
    for (i=0; i<mem->ll_levels; i++) {
      //capacity = mem->level[i].capacity / (int)(T[i] / mem->level[i].T) + 2; // capacity estimate...
      functionals_setupID( obj, mem, i, mem->ll_levels+i, i, T[i], fs[i], -1 );  // auto capacity
    }      
  }
}
#undef FUNCTION 


// compute the functionals for configuration <id>
// (save functionals to olevel of id in feature memory mem)
// when this function is called, it computes all functionals possible, up to current idx in ilevel
int functionals_compute( pFunctionals obj, int id )
#define FUNCTION "functionals_compute"
{_FUNCTION_ENTER_

  if ((obj != NULL) && (obj->config != NULL)) {
    #ifdef COMPILE_CHECKS
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config[id].mem == NULL) _FUNCTION_RETURN_(0);
    #endif
    pFeatureMemory mem = obj->config[id].mem;
    
    // check how many frames need to be computed...
    if (mem->level[id].allFilledIdx - obj->config[id].curIdx > obj->config[id].computeframeSize_frames) {   // change allFilledIdx to curIdx to have some functionals earlier than others???
       // now at least one functinals frame can be computed
       int nFrames = (mem->level[id].allFilledIdx - obj->config[id].curIdx) / obj->config[id].computeframeSize_frames;  // TODO: does this always round down??
// TODO::: ....

    }

  }
}
#undef FUNCTION 


/*
sampling of functionals frames:
   at         T*(idx)   to T*(idx)+computeframeSize   
   value t in seconds to input frames: t/T_input
   output frame number: (idx)

idx_o_frames = idx_i_sec / T_output

*/

// TODO: timestamp handling !!!

// compute the functionals for configuration <id>
// (save functionals to olevel of id in feature memory mem)
// when this function is called, it computes only the current (i.e. last) functional frame
// RETURN value: 0 on if no new frame was computed (error or no data), 1 on success, i.e. a new frame was computed
int functionals_computeCurrentOnly( pFunctionals obj, int id )
#define FUNCTION "functionals_computeCurrentOnly"
{_FUNCTION_ENTER_
  int olevel, ilevel;
  
  if ((obj != NULL) && (obj->config != NULL)) {
    #ifdef COMPILE_CHECKS
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config[id].mem == NULL) _FUNCTION_RETURN_(0);
    #endif
    pFeatureMemory mem = obj->config[id].mem;
    olevel = obj->config[id].olevel;
    ilevel = obj->config[id].ilevel;
    

      FEATUM_DEBUG(2,"allFilled: %i - curIdx_frames: %i, computeFrameSize_frames: %i, frameStep: %f", mem->level[ilevel].allFilledIdx, obj->config[id].curIdx_frames, obj->config[id].computeframeSize_frames, obj->config[id].frameStep);
    // check how many frames need to be computed...
    if (mem->level[ilevel].allFilledIdx - obj->config[id].curIdx_frames >= obj->config[id].computeframeSize_frames-1) {   // next frame is ready to be computed
       // compute ....
       FEATUM_DEBUG(11,"compute");
       //LONG_IDX idx_o = mem->level[olevel].curIdx+1; //obj->config[id].curIdx / obj->config[id].frameStep; //????
       LONG_IDX idx_o = ++(mem->level[olevel].curIdx); //obj->config[id].curIdx / obj->config[id].frameStep + 1; //????
       // start and end idx in input frames
       LONG_IDX start_idx, end_idx;
       
       if (idx_o != obj->config[id].curIdx_output+1) {
         FEATUM_WARNING(6,"output index discontinuity detected! idx_o = %i , curIdx_output = %i", idx_o, obj->config[id].curIdx_output);
       }
       
       // TODO: are these indicies correct?? or round to predefined ones derive from output index??
       end_idx = mem->level[ilevel].allFilledIdx;
       start_idx = mem->level[ilevel].allFilledIdx - obj->config[id].computeframeSize_frames + 1;
       
       // call private method to do computations...
       
       if ( functionals_doCompute( obj, &(obj->config[id]), idx_o, start_idx, end_idx ) ) {
         FEATUM_DEBUG(2,"compute success");
         // if successfull.. increase curIdx by frame step
         //mem->level[olevel].curIdx = max_l(idx_o,mem->level[olevel].curIdx);
         //FEATUM_DEBUG(0,"mem->level[%i].curIdx = %i",olevel,mem->level[olevel].curIdx);
         mem->level[olevel].allFilledIdx = mem->level[olevel].curIdx;
         
         obj->config[id].curIdx += obj->config[id].frameStep;
         obj->config[id].curIdx_frames = obj->config[id].curIdx / obj->config[id].frameStep_input;  // was: frameStep_input! 
         obj->config[id].curIdx_output = idx_o; // +1
         _FUNCTION_RETURN_(1);
       } else {
         mem->level[olevel].curIdx--;     
         _FUNCTION_RETURN_(0);       
       }
    }

  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 

// sets the start of the next frame that will be computed to curIdx
int functionals_setFrameStart( pFunctionals obj, int id, LONG_IDX relIdx )
#define FUNCTION "functionals_setFrameStart"
{_FUNCTION_ENTER_
  if ((obj != NULL) && (obj->config != NULL)) {
    #ifdef COMPILE_CHECKS
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config[id].mem == NULL) _FUNCTION_RETURN_(0);
    #endif

    pFeatureMemory mem = obj->config[id].mem;
    int olevel = obj->config[id].olevel;
    int ilevel = obj->config[id].ilevel;
 
    if (mem->level != NULL) {
      obj->config[id].curIdx_frames = mem->level[ilevel].curIdx + relIdx;
      if (obj->config[id].curIdx_frames < 0) obj->config[id].curIdx_frames = 0;
      _FUNCTION_RETURN_(1);
    }
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


// compute the functionals for configuration <id>
// (save functionals to olevel of id in feature memory mem)
// when this function is called, it computes only (and always) , from last compute position to current position
// RETURN value: 0 on error, 1 on success, but when nothing was computed, 2 if something was computed
#define FMIN_FRAMES 1
int functionals_computeNow( pFunctionals obj, int id, int fmin_frames )
#define FUNCTION "functionals_computeNow"
{_FUNCTION_ENTER_
  int olevel, ilevel;
  
  if ((obj != NULL) && (obj->config != NULL)) {
    #ifdef COMPILE_CHECKS
    if (id < 0) _FUNCTION_RETURN_(0);
    if (id >= obj->nIDs) _FUNCTION_RETURN_(0);
    if (obj->config[id].mem == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (fmin_frames < 0) fmin_frames = FMIN_FRAMES;
    pFeatureMemory mem = obj->config[id].mem;
    olevel = obj->config[id].olevel;
    ilevel = obj->config[id].ilevel;
   
    FEATUM_DEBUG(5,"allFilled: %i - curIdx_frames: %i, computeFrameSize_frames: %i, frameStep: %f [ID:%i]", mem->level[ilevel].allFilledIdx, obj->config[id].curIdx_frames, obj->config[id].computeframeSize_frames, obj->config[id].frameStep,id);
    // check how many frames need to be computed...
    if (mem->level[ilevel].allFilledIdx - obj->config[id].curIdx_frames >= fmin_frames-1) {   // next frame is ready to be computed
       // compute ....
      FEATUM_DEBUG(2,"compute");
       LONG_IDX idx_o = ++(mem->level[olevel].curIdx); //obj->config[id].curIdx / obj->config[id].frameStep; //????
       // start and end idx in input frames
       LONG_IDX start_idx, end_idx;
       
       if (idx_o != obj->config[id].curIdx_output+1) {
         FEATUM_WARNING(6,"output index discontinuity detected! idx_o = %i , curIdx_output = %i", idx_o, obj->config[id].curIdx_output);
       }
       
       // TODO: are these indicies correct?? or round to predefined ones derive from output index??
       end_idx = mem->level[ilevel].allFilledIdx;
//       start_idx = mem->level[ilevel].allFilledIdx - obj->config[id].computeframeSize_frames ;  // ??? +1 ???
       start_idx =  obj->config[id].curIdx_frames;
       if (start_idx < 0) start_idx = 0;
       
       // call private method to do computations...
       
       FEATUM_DEBUG(7,"really computing functionals...");
       //FEATUM_MESSAGE(1,"fcomp: %i -> %i",start_idx,end_idx);       
       if ( functionals_doCompute( obj, &(obj->config[id]), idx_o, start_idx, end_idx ) ) {

         FEATUM_DEBUG(7,"really computing functionals... success!");
         // if successfull.. increase curIdx by frame step
         //mem->level[olevel].curIdx = max_l(idx_o,mem->level[olevel].curIdx);
         //FEATUM_DEBUG(0,"mem->level[%i].curIdx = %i",olevel,mem->level[olevel].curIdx);
         mem->level[olevel].allFilledIdx = mem->level[olevel].curIdx;
         
         obj->config[id].curIdx += obj->config[id].frameStep;
         obj->config[id].curIdx_frames = end_idx+1; //obj->config[id].curIdx / obj->config[id].frameStep_input; 
         obj->config[id].curIdx_output = idx_o;
         _FUNCTION_RETURN_(2);
       } else {
         mem->level[olevel].curIdx--;
         _FUNCTION_RETURN_(0);       
       }
    }
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 



// (private function) 
inline FLOAT_TYPE_FTMEM * functionals_getIndexPtr( int nFunctionalValues, FLOAT_TYPE_FTMEM *values, int inputfeatureNum, int funcIdx, int localIdx)
{
  return values + ( inputfeatureNum*nFunctionalValues + funcIdx + localIdx );
}




void quickSort(FLOAT_TYPE_FTMEM *arr, int elements) {
  #define  MAX_LEVELS  300
  FLOAT_TYPE_FTMEM piv, beg[MAX_LEVELS], end[MAX_LEVELS],swap;
  int i=0, L, R;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L];
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L;
      if (end[i]-beg[i]>end[i-1]-beg[i-1]) {
        swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
        swap=end[i]; end[i]=end[i-1]; end[i-1]=swap; }}
    else {
      i--; }
  }
}
      

void functionals_sortBuffer(FLOAT_TYPE_FTMEM *sorted, LONG_IDX bufLen)
#define FUNCTION "functionals_sortBuffer"
{_FUNCTION_ENTER_
  quickSort(sorted,bufLen);
}
#undef FUNCTION


   


// (private function) 
// this function does the actual hard work..... start_idx and end_idx elemets are BOTH included in the computation!
int functionals_doCompute( pFunctionals obj, pFunctionalIDConfig config, int output_index, LONG_IDX start_idx, LONG_IDX end_idx )
#define FUNCTION "functionals_doCompute"
{_FUNCTION_ENTER_
  pFeatureMemoryLevel mem_olevel = &(config->mem->level[config->olevel]);
  pFeatureMemoryLevel mem_ilevel = &(config->mem->level[config->ilevel]);
  int current;

  // compute functionals on data in ilevel from start_idx to end_idx
  LONG_IDX i;
  FEATUM_DEBUG(4,"start_index = %i, end_index = %i, idx_o = %i",start_idx,end_idx,output_index);
  int el, n;
  for (el = 0; el < mem_ilevel->nFtEls; el++) {   // for every feature element0
    if ((mem_ilevel->config != NULL) && (mem_ilevel->config[el].enabled)) {
    int nFeatures_i = featureMemory_getNFeatures_idx( config->mem, config->ilevel, el, start_idx );
    int nFeatures_o = config->nFunctionalValues * nFeatures_i;
    
    if (nFeatures_o > 0) {
    FLOAT_TYPE_TIME timeBegin = featureMemory_vIdxToTime ( config->mem, config->ilevel, start_idx );
    FLOAT_TYPE_TIME timestamp = timeBegin;
    
    FEATUM_DEBUG(10,"allocating nVals = %i (nFunctionalValues = %i)",nFeatures_o, config->nFunctionalValues);
    FLOAT_TYPE_FTMEM *o_values = 
       featureMemory_getValuesForWrite_abs( config->mem, config->olevel, el, 
                                            nFeatures_o,
                                            output_index, timestamp );
       
    if (o_values == NULL) { FEATUM_ERROR(0,"o_values == NULL!!");  _FUNCTION_RETURN_(0); }
    else { //   mem_ilevel->config[el].n
    for (n = 0; n < nFeatures_i ; n++) {  // and every feature therein   // was : mem_ilevel->ft[start_idx][el]->nValues
      //FEATUM_DEBUG(0,"el=%i, n = %i, mem_ilevel->ft[start_idx][el]->nValues = %i",el,n, mem_ilevel->ft[start_idx][el]->nValues);
      FLOAT_TYPE_FTMEM *unsorted = NULL;
      FLOAT_TYPE_FTMEM *sorted = NULL;
      LONG_IDX bufLen = end_idx - start_idx + 1;
      FEATUM_DEBUG(10,"d2 n=%i, el=%i, bufLen = %i, st=%i, end=%i",n,el,bufLen, start_idx,end_idx);

      // fill unsorted buffer: 
      unsorted = (FLOAT_TYPE_FTMEM *)calloc(1,bufLen*sizeof(FLOAT_TYPE_FTMEM));
  
      if (unsorted == NULL) { FEATUM_ERR_FATAL(0,"Error allocating memory"); _FUNCTION_RETURN_(0); }
      LONG_IDX j = 0;
      for (i=start_idx; i<=end_idx; i++) { // for each frame in the given range
          FEATUM_DEBUG(11,"i=%i",i);
          pFtMemElement ftEl = featureMemory_getElement( config->mem, config->ilevel, i, el );
          if ((ftEl != NULL)&&(ftEl->filled)) {
            unsorted[j] = ftEl->values[n];
            j++;
          }
      }  
      if (j != bufLen) {
        FEATUM_WARNING(3,"incomplete frames!! bufLen expected = %i, actual: %i",bufLen,j);
        bufLen = j;      
      }
      FEATUM_DEBUG(10,"d3 n=%i, el=%i",n,el);
      
      // sort buffer:
      if (config->requireSorted == 1) {
        sorted = (FLOAT_TYPE_FTMEM *)malloc(bufLen*sizeof(FLOAT_TYPE_FTMEM));
        if (sorted == NULL) { FEATUM_ERR_FATAL(0,"Error allocating memory"); _FUNCTION_RETURN_(0); }        
        memcpy(sorted,unsorted,bufLen*sizeof(FLOAT_TYPE_FTMEM));
        functionals_sortBuffer(sorted, bufLen);
        //TODO: save and use partially sorted last buffer
      }          

      // compute the functionals....    // WARNING: array index is (obj->en_xxxx - 1)!!!
      sSharedFuncs shared;
      memzero(&shared,sizeof(sSharedFuncs));
      if ((bufLen != config->costableBufLen)||(config->costable == NULL)) {
        // create costable now                  
        if (config->costable != NULL) free(config->costable);
        config->costableBufLen = bufLen;
        config->costable = (FLOAT_TYPE_FTMEM *)malloc(sizeof(FLOAT_TYPE_FTMEM)*bufLen*6); // 6 DCT coeffs
        if (config->costable == NULL) {
          FEATUM_ERROR(0,"Error allocating memory for costable!");
          _FUNCTION_RETURN_(0);                  
        }
        for (i=FIRST_DCT; i < FIRST_DCT+N_DCT; i++) {
          FLOAT_TYPE_FTMEM fi = (FLOAT_TYPE_FTMEM)i;
          int m;
          for (m=0; m<bufLen; m++) {
            config->costable[m+(i-FIRST_DCT)*bufLen] = cos(M_PI*fi/(FLOAT_TYPE_FTMEM)bufLen * (((FLOAT_TYPE_FTMEM)(m+1)) - 0.5) );                  
          }
        }
      }
      shared.costable = config->costable;
      
      if (bufLen <= 0) {
        FEATUM_WARNING(2,"bufLen = %i. feature values will be bogus",bufLen);
      } else {

      for (i=0; i<obj->maxFunctionals; i++) {
        if (config->functsEnabled[i]) {
          FLOAT_TYPE_FTMEM * output = functionals_getIndexPtr( config->nFunctionalValues, o_values, n, config->functsIdx[i], 0 );
          //printf("comp func. %i - %s\n",i,config->functs[i].name);
          //printf("OV: %u -- op: %u  nfo: %i  n=%i\n",(unsigned long)o_values,(unsigned long)output,nFeatures_o,n);
          if ((config->functs[i].requireSorted)&&(config->functs[i].comp_sorted != NULL)) {
            config->functs[i].comp_sorted(sorted,bufLen,output,config->functs[i].cnt,&shared);
          } else if (config->functs[i].comp_unsorted != NULL)  {
            config->functs[i].comp_unsorted(unsorted,bufLen,output,config->functs[i].cnt,&shared);
          } else {
            FEATUM_ERROR(3,"cannot compute functional %i (%s), please check config in functionals.c",config->functs[i].name);       
          }
        }
      }
      
      }
      
      free(unsorted); 
      //        printf("z \n");fflush(stdout);
      if (config->requireSorted == 1) free(sorted);      
      
    } // end for
    } // end else
    } // if != NULL 
    else {  // if at least ONE element cannot be processed, return failure...
      FEATUM_DEBUG(6,"for some shitty reason this function has failed: start_idx=%i, nFeatures_i=%i, ilevel=%i, el=%i ",start_idx,nFeatures_i,config->ilevel,el);
      _FUNCTION_RETURN_(0);
    } 
    } // if (enabled)  
  } // end for(el...)
  // save result in olevel at output_index

  _FUNCTION_RETURN_(1);
}
#undef FUNCTION 


// compute all functionals for all input levels, automatically map output levels
int functionals_autoCompute( pFunctionals obj )
#define FUNCTION "functionals_autoCompute"
{_FUNCTION_ENTER_
  int i;
  if ((obj != NULL) ) {
    for (i=0; i<obj->nIDs; i++) {
      if (!functionals_compute( obj, i)) {
        FEATUM_ERROR(0,"Error computing functionals!");
        _FUNCTION_RETURN_(0);                          
      }
    }  
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 


/* Destructor: frees the class data object and returns NULL */
pFunctionals functionals_destroy( pFunctionals obj )
#define FUNCTION "functionals_destroy"
{_FUNCTION_ENTER_
  functionals_destroyData( obj );
  if (obj != NULL) {
    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 

/* Destructor: frees the class data object and returns NULL */
pFunctionals functionals_destroyData( pFunctionals obj )
#define FUNCTION "functionals_destroyData"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    // TODO: destroy temporary data..???
    
    if (obj->config != NULL) {
      int id;
      for (id=0; id < obj->nIDs ; id++) {
          if (obj->config[id].names != NULL) {
            int n;
            for (n=0; n<obj->config[id].nFunctionalValues; n++) {
              if (obj->config[id].names[n] != NULL) free(obj->config[id].names[n]);
            }                           
            free(obj->config[id].names);
          }    
          if (obj->config[id].functsEnabled != NULL) free(obj->config[id].functsEnabled);
          if (obj->config[id].functsIdx != NULL) free(obj->config[id].functsIdx);
          if (obj->config[id].costable != NULL) { free(obj->config[id].costable); obj->config[id].costable = NULL; }

      }
      free(obj->config);
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 
