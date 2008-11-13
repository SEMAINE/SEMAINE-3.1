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
 
 /*  IMPLEMENTATION of the class <classTemplate>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDs"
 
 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function

// in LLDs.h: 
// extern sLLDs LLD;  // declared in extractors-globalconfig.c

pLLDs LLDs_create( pLLDs obj, pLLDex l, int freeLLDex, int freeLLDobjs )
#define FUNCTION "LLDs_create"
{_FUNCTION_ENTER_
  int i;
  if (obj == NULL) {
    obj = (pLLDs) calloc(1,sizeof(sLLDs));       
  } else {
    memzero(obj, sizeof(sLLDs));       
  }
  
  //printf("acdef %i - b %i\n",sizeof(LLD),sizeof(sLLDs));
  if (obj == NULL) {
    FEATUM_ERR_FATAL(0,"Error allocating memory!");
    _FUNCTION_RETURN_(NULL);        
  }


    // copy default configuration:
    memcpy(obj,&LLD,sizeof(sLLDs));
//  printf("keytag1  %i  %i\n",(long)(LLD.config[0].LLDdestroy),(long)&LLDenergy_destroy); 
//  (long)(obj->config[i].LLDcreate)
/*    obj->nExtractors = LLD.nExtractors;
    obj->nExtractorLevels = LLD.nExtractorLevels;
    obj->nExtractors_enabled = NULL;
    obj->nExtractors_flushable = NULL;*/
    
//    obj->config = (pLLDconfig *)calloc(1,sizeof(pLLDconfig *)*LLD.nExtractors);

    // TODO: sort extractors according to priority here:  ?? does this break any enumerations by the FT_LLD_ constants ??
//    for (i=0; i< obj->nExtractors; i++) {
//FEATUM_DEBUG(11,"5 %i %i - %i %i",i,obj->nExtractors);//,(int)(obj->config[i]),(int)(LLD.config[i]));
//      obj->config[i] = LLD.config[i]; 
//      obj->config[i] = &(LLD.config[i]); 
// FEATUM_DEBUG(11,"5b %i %i",i,obj->nExtractors);   
//    }
    // obj->config = (sLLDconfig *)&(LLD.config);
     
FEATUM_DEBUG(12,"6");
//    obj->nExtractors = (sizeof(LLD) - 3*sizeof(int) - sizeof(pLLDex)) / sizeof(pLLDconfig);
//    FEATUM_DEBUG(8,"sizeof(LLD) = %i",sizeof(LLD.config));
    FEATUM_DEBUG(8,"preset obj->nExtractors = %i",obj->nExtractors);
    obj->LLDex = l; 
FEATUM_DEBUG(12,"7");

    obj->freeLLDex = freeLLDex;
FEATUM_DEBUG(12,"8");

    obj->freeLLDobjs = freeLLDobjs;
FEATUM_DEBUG(12,"9");

  
  FEATUM_DEBUG(12,"10");

  _FUNCTION_RETURN_(obj);
}
#undef FUNCTION 


// enable LLD "name", and register extractor obj. if obj = NULL it will be created by calling default constructor!
// if level == -1 the default defined by the constants will be used
// NEW: returns a pointer to the extractor object (can be used for further config...)
void * LLDs_setupLLD( pLLDs obj, const char * name, int enabled, void *exObj, int level )
#define FUNCTION "LLDs_setupLLD"
{_FUNCTION_ENTER_
  int i;
  if (obj != NULL) {   
    for (i=0; i<obj->nExtractors; i++) {     // ?? check &&(obj->freeLLDobjs) ??
      if ((obj->config[i].name!= NULL)&&!strcmp(obj->config[i].name,name) && (level == obj->config[i].level)) {
        if (obj->config[i].LLDobj != NULL) {
           // free??                          
           (obj->config[i].LLDdestroy)(obj->config[i].LLDobj);
        }
//        printf("here1\n");
        if (exObj == NULL) {
          exObj = (obj->config[i].LLDcreate)(obj->config[i].LLDobj);
        }
        
//printf("%i <-> %i :: %i %i\n",sizeof(sLLDs), sizeof(LLD),(long)LLD.config, (long)&(LLD.config));
// TODO: remove const from LLD struct!
//        printf("here2 i=%i\n",i); fflush(stdout);

        obj->config[i].LLDobj = exObj;
//                printf("here2b\n"); fflush(stdout);
        obj->config[i].enabled = enabled;
//                printf("here3\n"); fflush(stdout);
/*        */
        
//      featureMemory_setupNames(  mem, ftmem_level, obj->config[i].ftmem_element, obj->config[i].ftmem_nVal, , enabled )
        _FUNCTION_RETURN_(exObj);
      }
    }
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 


// call this after all setupLLD calls
void LLDs_countEnabled( pLLDs obj )
#define FUNCTION "LLDs_countEnabled"
{_FUNCTION_ENTER_
//TODO: nExtractors_enabled and nExtractors_flushable for each level !!
  int j;
  if (obj != NULL) {
    // count extractor levels:
    int nExLevels = obj->nExtractorLevels-1;
    for (j=0; j< obj->nExtractors; j++) {
       if ((obj->config[j].enabled) && (obj->config[j].level > nExLevels)) nExLevels = obj->config[j].level;
    }
    nExLevels++;
    if (obj->nExtractorLevels != nExLevels) {
      if (obj->nExtractors_enabled != NULL) { free(obj->nExtractors_enabled); obj->nExtractors_enabled = NULL; }
      if (obj->nExtractors_flushable != NULL) { free(obj->nExtractors_flushable); obj->nExtractors_flushable = NULL; }
      obj->nExtractorLevels = nExLevels;
    }
    if (obj->nExtractorLevels > 0) {
      if (obj->nExtractors_enabled == NULL) obj->nExtractors_enabled = (int *)calloc(1,sizeof(int)*obj->nExtractorLevels);
      if (obj->nExtractors_flushable == NULL) obj->nExtractors_flushable = (int *)calloc(1,sizeof(int)*obj->nExtractorLevels);
    }
          
    for (j=0; j< obj->nExtractorLevels; j++) {
        
    int i, en_cnt=0, fl_cnt=0;    
    for (i=0; i< obj->nExtractors; i++) {
      if ((obj->config[i].enabled)&&(obj->config[i].level == j)) {
         if ((obj->config[i].flushToFtMem != NULL)) { // if nVal == 0 then a dynamic (but flushable extractor is to be set up
           if (obj->config[i].ftmem_element != NULL) {
             *(obj->config[i].ftmem_element) = fl_cnt; 
             fl_cnt++;
           }  
         }
         en_cnt++;
      }
    }
    obj->nExtractors_enabled[j] = en_cnt;
    obj->nExtractors_flushable[j] = fl_cnt;
    
    }
  }
  _FUNCTION_RETURNV_;
}
#undef FUNCTION 


void LLDs_configureFeatureMemoryNames( pLLDs obj, pFeatureMemory mem, int ftmem_level )
#define FUNCTION "LLDs_configureFeatureMemoryNames"
{_FUNCTION_ENTER_
  int i;
  if ((obj != NULL)&&(mem != NULL)) {
    for (i=0; i< obj->nExtractors; i++) {
          // if nVals == 0 ==>>  setup a dynamic element:  only one name is given (the base name to wich the feature number will be appended)
      if ((obj->config[i].enabled)&&(obj->config[i].ftmem_element != NULL)&&(obj->config[i].ftmem_names != NULL)&&(obj->config[i].flushToFtMem != NULL)&&(obj->config[i].level == ftmem_level)) {
        int nDyn = 0;
        if ((obj->config[i].ftmem_nVals == 0)&&(obj->config[i].ftmem_nValsDyn == 0)) { nDyn = 1; }
        else if (obj->config[i].ftmem_nValsDyn > 0) { nDyn = obj->config[i].ftmem_nValsDyn; }
        featureMemory_setupElement( mem, ftmem_level, *(obj->config[i].ftmem_element), obj->config[i].ftmem_nVals, nDyn, obj->config[i].ftmem_names, 1 );    
      } 
    }
  }
  _FUNCTION_RETURNV_;
}
#undef FUNCTION 


// a lot of dependency checking  O(N^2)  where N is the number of LLD extractors
// only to be called at initialisation
int LLDs_checkDependencies( pLLDs obj )
#define FUNCTION "LLDs_checkDependencies"
{_FUNCTION_ENTER_

  int i,j,d;
  int failed = 0;
  
  if (obj != NULL) {
    for (i=0; i<obj->nExtractors; i++) {
      // check if the extractor is enabled and has dependencies
      if ((obj->config[i].enabled)&&(obj->config[i].nDepends > 0)) {
        // now, for each dependency                                                       
        for (d=0; d < obj->config[i].nDepends; d++) {
           int found = 0, enabled = 0;
            // scan through all extractors to see if the dependency is matched
           for (j=0; j < obj->nExtractors; j++) {
              if (!strcmp(obj->config[j].name,obj->config[i].depends[d])) {
                found = 1;
                if (obj->config[j].enabled) enabled = 1;
                break;
              }
           } // end for(j) loop
           if (!found) {
             FEATUM_ERR_FATAL(0,"LLD extractor \"%s\" depends on missing extractor \"%s\"!",obj->config[i].name,obj->config[i].depends[d]);
             failed = 1;         
           } else {
             if (!enabled) {
               FEATUM_WARNING(0,"LLD extractor \"%s\" depends on disabled extractor \"%s\" -> enabling \"%s\"!",obj->config[i].name,obj->config[i].depends[d]);
               //failed = 0;
               obj->config[j].enabled = 1;
               // TODO:::: !!!! LLDobj must be created after this!!!
             }
           }  

        } // end for(d) loop
      }
    }// end for(i) loop
  } else { failed = 1; }

  if (!failed) {
    // sort extractors according to their priority
                 
  }
  
  _FUNCTION_RETURN_(!failed);
}
#undef FUNCTION 


// initialize the LLDobjs, i.e. call generic constructor, if the object has not been initialised before or during the
// setupLLD call
int LLDs_createExtractors( pLLDs obj )
#define FUNCTION "LLDs_createExtractors"
{_FUNCTION_ENTER_

  int i;
  
  if (obj != NULL) {
    for (i=0; i<obj->nExtractors; i++) {
      if ((obj->config[i].enabled)&&(obj->config[i].LLDobj == NULL)&&(obj->freeLLDobjs)) {
        if (obj->config[i].LLDcreate != NULL) obj->config[i].LLDobj = (obj->config[i].LLDcreate)(NULL);
      }
    } 
  }   
   
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION   



int LLDs_callExtractors( pLLDs obj, int level )
#define FUNCTION "LLDs_callExtractors"
{_FUNCTION_ENTER_

  int i;
  // TODO: consider extractors priority... i.e. call those with lower numerical value for priority first
  // maybe sort extractors first.... during check dependencies or in extra function...?
  if (obj != NULL) {
    for (i=0; i<obj->nExtractors; i++) {
      FEATUM_DEBUG(8,"extractor %i (%s): en=%i, level=%i, obj=%i, exf=%i",i,obj->config[i].name,obj->config[i].enabled,obj->config[i].level,(long)(obj->config[i].LLDobj),(long)(obj->config[i].extractor));
      if ((obj->config[i].level == level)&&(obj->config[i].enabled)&&(obj->config[i].extractor != NULL)) {
        if (! (obj->config[i].extractor)(obj->config[i].LLDobj, obj->LLDex, level)) {
          FEATUM_ERROR(6,"Failed running feature extractor #%i (%s)!!",i,obj->config[i].name);
        } else {
          FEATUM_DEBUG(8,"successfully ran extractor for  %s",obj->config[i].name);
        }
      }
    }
  }
 _FUNCTION_RETURN_(1);
}
#undef FUNCTION    


int LLDs_flushToFtMem( pLLDs obj, pFeatureMemory mem, int level )
#define FUNCTION "LLDs_flushToFtMem"
{_FUNCTION_ENTER_
  int i;
  
  if (obj != NULL) {
    FEATUM_DEBUG(7,"nExtractors = %i",obj->nExtractors);
    #ifdef COMPILE_CHECKS
    if (obj->LLDex == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (obj->LLDex->current[level] != NULL) {
      featureMemory_addFrame( mem, level );
//      obj->LLDex->current[level]->pcm->timestamp;
//      obj->LLDex->current[level]->pcm->timestampSec;
    }
    FEATUM_DEBUG(7,"flushing... ci = %i",mem->level[level].curIdx);
    for (i=0; i<obj->nExtractors; i++) {
      FEATUM_DEBUG(8,"flushing extractor #%i (en= %i), level= %i",i,obj->config[i].enabled,obj->config[i].level);
      if ((obj->config[i].level == level)&&(obj->config[i].enabled)&&(obj->config[i].flushToFtMem != NULL)) {
        FEATUM_DEBUG(8,"  level=%i",level);
        if ((obj->LLDex->current[level] != NULL)) {
          (obj->config[i].flushToFtMem)(level, mem, obj->LLDex); //*flushToFtMem)(int, pFeatureMemory, pLLDex);
        }
      }
    }
    // update fill status
    featureMemory_checkStatus(mem, level);   // WARNING: this line can make things really slow for long files, 
                                             //          if some features are not filled at all
  } else {
    FEATUM_WARNING(2,"LLDs object is NULL");
    _FUNCTION_RETURN_(0);
  }

 _FUNCTION_RETURN_(1);
}
#undef FUNCTION    


pLLDs LLDs_destroy( pLLDs obj )
#define FUNCTION "LLDs_destroy"
{_FUNCTION_ENTER_
  int i;
  if (obj != NULL) {
    if (obj->freeLLDex) LLDex_destroy(obj->LLDex);
    if (obj->freeLLDobjs) {
      for (i=0; i<obj->nExtractors; i++) {
        if (obj->config[i].LLDdestroy != NULL) (obj->config[i].LLDdestroy)(obj->config[i].LLDobj);
      }  
    }  
//    if (obj->config != NULL) free(obj->config);
    free(obj);
  }  
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 


pLLDs LLDs_destroyData( pLLDs obj )
#define FUNCTION "LLDs_destroyData"
{_FUNCTION_ENTER_
  int i;
  if (obj != NULL) {
    if (obj->freeLLDex) { obj->LLDex = LLDex_destroy(obj->LLDex); obj->LLDex = NULL; }
    if (obj->freeLLDobjs) {
      for (i=0; i<obj->nExtractors; i++) {
            //printf("keytag2  %i  %i\n",(long)(obj->config[0].LLDdestroy),(long)&LLDenergy_destroy);  fflush(stdout);
//            LLDenergy_destroy(obj->config[i].LLDobj);
        if (obj->config[i].LLDdestroy != NULL) obj->config[i].LLDobj = (obj->config[i].LLDdestroy)(obj->config[i].LLDobj);
      }  
    }  
    if (obj->nExtractors_flushable != NULL) free(obj->nExtractors_flushable);
    if (obj->nExtractors_enabled != NULL) free(obj->nExtractors_enabled);
//    if (obj->config != NULL) free(obj->config);
  }  

  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION 


int LLDs_setupFeatureMemLevels( pLLDs llds, pFeatureMemory mem, pInputFramer framer, int *framerIDs, LONG_IDX *capacity )
#define FUNCTION "LLDs_setupFeatureMemLevels"
{_FUNCTION_ENTER_
   int i;
   if ((framerIDs != NULL)&&(framer != NULL)&&(capacity != NULL)&&(mem != NULL)&&(llds != NULL)) {
          int minlevels;
          // if featureMemory has more levels than low-level input levels (from the framer) than assume the extra levels are for functionals, etc. 
          if (mem->nLevels > framer->nClients) {
            minlevels =  framer->nClients;
          } else {
            minlevels = mem->nLevels;       
          }
          mem->ll_levels = minlevels;
          FEATUM_DEBUG(5,"ftMem status: ll_levels=%i, minlevels=%i, framer->nClients=%i, nLevels=%i", mem->ll_levels, minlevels, framer->nClients, mem->nLevels);

          if (framer->nClients != llds->nExtractorLevels) {
            FEATUM_WARNING(1,"framer->nClients (%i) != llds->nExtractorLevels (%i)",framer->nClients,llds->nExtractorLevels);
          }
          // TODO: synchronise llds->nExtractorLevels and framer->nClients! (or at least check synchronisation...)
          for (i=0; i<minlevels; i++) {
 
            FLOAT_TYPE_TIME T; 
            LONG_IDX frameSize;
            if (framer->client != NULL) {
              T = (float)audioStream_samplesToSeconds( framer->input, framer->client[framer->clientLUT[framerIDs[i]]]->frameStep );
              //obj->level[i].frameStep = framer->client[framer->clientLUT[framerIDs[i]]]->frameStep;
              frameSize = framer->client[framer->clientLUT[framerIDs[i]]]->frameLength;  //????
              FEATUM_DEBUG(10,"obj->level[%i / %i].frameSize == %i",i,framerIDs[i],mem->level[i].frameSize);
            }
            
            FEATUM_DEBUG(4,"configuring level %i",i);
            featureMemory_configureLevel( mem, i, capacity[i], FTMEM_DEFAULT_BUFFER, T, 0, -1);

            mem->level[i].frameSize = frameSize;

/*            obj->level[i].ft = NULL;   // [index][ftelement]
            obj->level[i].t = NULL;            
            obj->level[i].config = NULL;
            obj->level[i].curIdx = -1;  // no frame has been initialised
            obj->level[i].allFilledIdx = -1;
            obj->level[i].readIdx = -1;            
            obj->level[i].prevLevel = -1;
            obj->level[i].nextLevel = -1;*/
          }
          for (i=0; i< llds->nExtractorLevels; i++) {
            FEATUM_DEBUG(4,"allocating level %i",i);
            mem->level[i].nFtEls = llds->nExtractors_flushable[i];
            featureMemory_dataMemAllocLevel(mem,i);
          }
  }   

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION 
