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
 
 /*  IMPLEMENTATION of the class <LLDex>
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDex"
 
 // this file contains the function definitions and the private class data

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "LLDex.h"

#undef FUNCTION     // use undef only if you define it below for every function

// constructor: create and initialise ftex struct
// if framer != NULL, nIDs is ignored, otherwise it is used instead of the nClients value from framer
pLLDex LLDex_create( pLLDex obj, pInputFramer framer, int nIDs, int *nHistory )
#define FUNCTION "LLDex_create"
{_FUNCTION_ENTER_
  int i;
  
//  if (obj != NULL) obj = LLDex_destroy( obj );
  if (obj == NULL) {
    obj = (pLLDex)calloc(1,sizeof(sLLDex));
  } else {
    memzero(obj,sizeof(sLLDex));       
  }
    
    if (obj == NULL) {
      FEATUM_ERR_FATAL(0,"Error allocating memory!");
       _FUNCTION_RETURN_(NULL);
    }
    if (nIDs <= 0) {
      FEATUM_ERR_FATAL(0,"nIDs must be at least 1 !");
       _FUNCTION_RETURN_(NULL);
    }
             
    
    if (framer) {
      obj->nIDs = framer->nClients;
    } else {
      obj->nIDs = nIDs;
    }
    obj->history = (pLLDexElement **)calloc(1,sizeof(pLLDexElement *)*obj->nIDs);
    obj->historyRbIdx = (int *)calloc(1,sizeof(int) * obj->nIDs);
    obj->nHistory = (int *)calloc(1,sizeof(int) * obj->nIDs);
    if (nHistory != NULL) {
      memcpy(obj->nHistory,nHistory, sizeof(int) * obj->nIDs);
      for (i=0; i< obj->nIDs; i++) {
        obj->history[i] = (pLLDexElement *)calloc(1,sizeof(pLLDexElement)*obj->nHistory[i]); 
      }
    }
    
    obj->current = (pLLDexElement *)calloc(1,sizeof(pLLDexElement)*obj->nIDs); 
  
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION 


// add a frame for id  and return corresponding pLLDexElement
// the oldest element will be freed
pLLDexElement LLDex_addFrame( pLLDex obj, int level, pPcmBuffer pcm )
#define FUNCTION "LLDex_addFrame"
{_FUNCTION_ENTER_
  pLLDexElement l = NULL;
  
  #ifdef COMPILE_CHECKS
  if (level < 0) _FUNCTION_RETURN_(NULL);
  if ((obj != NULL) && (level >= obj->nIDs)) _FUNCTION_RETURN_(NULL);
  #endif
  
  if ((obj != NULL)&&(pcm != NULL)) {
    #ifdef COMPILE_CHECKS
    if (obj->nHistory == NULL) _FUNCTION_RETURN_(NULL);
    if (obj->historyRbIdx == NULL) _FUNCTION_RETURN_(NULL);
    if (obj->history == NULL) _FUNCTION_RETURN_(NULL);
    if (obj->history[level] == NULL) _FUNCTION_RETURN_(NULL);
    #endif
    
    l = LLDex_createElement( pcm );        
    obj->historyRbIdx[level]++;
    obj->historyRbIdx[level] %= obj->nHistory[level];
    // free old object:
    obj->history[level][obj->historyRbIdx[level]] = LLDex_destroyElement(obj->history[level][obj->historyRbIdx[level]]);        
    // assign new object:
    obj->history[level][obj->historyRbIdx[level]] = l;
    obj->current[level] = l;
    _FUNCTION_RETURN_( l );
  } else {
    obj->current[level] = NULL; // TODO : validate if ok?
    _FUNCTION_RETURN_( NULL );         
  }

}
#undef FUNCTION 


// nBack == 0 : current element (= last element that was added)
// nBack == -1 :  element at t - 1
// ...
pLLDexElement LLDex_getHistory( pLLDex obj, int id, int nBack )
#define FUNCTION "LLDex_getHistory"
{_FUNCTION_ENTER_
  pLLDexElement l = NULL;
  if ((obj != NULL)&&(obj->history)) {
    #ifdef COMPILE_CHECKS
    if (id < 0) _FUNCTION_RETURN_(NULL);
    if ((id >= obj->nIDs)) _FUNCTION_RETURN_(NULL);
    if (obj->nHistory == NULL) _FUNCTION_RETURN_(NULL);
    if (obj->history == NULL) _FUNCTION_RETURN_(NULL);
    if (obj->historyRbIdx == NULL) _FUNCTION_RETURN_(NULL);
    #endif

    int n = obj->historyRbIdx[id] - nBack;
    n %= obj->nHistory[id];
    if (obj->history[id]) _FUNCTION_RETURN_( obj->history[id][n] ); 
  }
  _FUNCTION_RETURN_( l );
}
#undef FUNCTION 


// completely destroy ftex obj
pLLDex LLDex_destroy( pLLDex obj )
#define FUNCTION "LLDex_destroy"
{_FUNCTION_ENTER_
  int i,n;
  
  if (obj != NULL) {
    // free history:
    if (obj->history != NULL) {
      for (i=0; i<obj->nIDs; i++) {
        if ((obj->nHistory != NULL)&&(obj->history[i] != NULL)) {
          for (n=0; n < obj->nHistory[i]; n++) {
            LLDex_destroyElement( obj->history[i][n] );
          }
          free( obj->history[i] );
        }
      }
      free( obj->history );
    }

    if (obj->historyRbIdx != NULL) free(obj->historyRbIdx);
    if (obj->nHistory != NULL) free(obj->nHistory);
    if (obj->current != NULL) free(obj->current);

    free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION


// completely destroy ftex obj
pLLDex LLDex_destroyData( pLLDex obj )
#define FUNCTION "LLDex_destroyData"
{_FUNCTION_ENTER_
  int i,n;
  
  if (obj != NULL) {
    // free history:
    if (obj->history != NULL) {
      for (i=0; i<obj->nIDs; i++) {
        if ((obj->nHistory != NULL)&&(obj->history[i] != NULL)) {
          for (n=0; n < obj->nHistory[i]; n++) {
            LLDex_destroyElement( obj->history[i][n] );
          }
          free( obj->history[i] );
        }
      }
      free( obj->history ); obj->history = NULL;
    }

    if (obj->historyRbIdx != NULL) free(obj->historyRbIdx);
    if (obj->nHistory != NULL) free(obj->nHistory);
    if (obj->current != NULL) free(obj->current);
    obj->historyRbIdx = NULL;
    obj->nHistory = NULL;
    obj->current = NULL;
    obj->nIDs = 0;
    
    //free(obj);
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION


pLLDexElement LLDex_createElement( pPcmBuffer b )
#define FUNCTION "LLDex_createElement"
{_FUNCTION_ENTER_

  pLLDexElement l = (pLLDexElement)calloc(1,sizeof(sLLDexElement));
  FEATUM_DEBUG(10,"alloc sizeof(sLLDexElement)=%i\n",sizeof(sLLDexElement));
  
  if (l != NULL) {
    l->pcmAutoFree = 1;
    l->pcm = pcmBuffer_duplicate( b );
    if (b != NULL) 
      l->timestamp = b->timestamp;
  } else {
    FEATUM_ERR_FATAL(0,"Error allocating memory!");     
  }
  _FUNCTION_RETURN_(l);
}
#undef FUNCTION


// free element an all associated data within
pLLDexElement LLDex_destroyElement( pLLDexElement obj )
#define FUNCTION "LLDex_destroyElement"
{_FUNCTION_ENTER_
  int i;
  
  if (obj != NULL) {
    if (obj->pcmAutoFree) obj->pcm = pcmBufferFree( obj->pcm, 0 );

    pLLDexGeneric *elements;
    elements = (pLLDexGeneric *)&(((pLLDexElementGeneric)obj)->el1);
//    elements = (pLLDexGeneric*) ( ((long long)obj) + sizeof(int) + sizeof(FLOAT_TYPE_TIME) + sizeof(pPcmBuffer));
    
    //int nEl = ( sizeof(sLLDexElement) - sizeof(int) - sizeof(FLOAT_TYPE_TIME) - sizeof(pPcmBuffer) )/sizeof(pLLDexGeneric);
    // NOTE: windows gcc does alignment for floats and doubles, thus struct size increases and the above line cannot be used
    //       the line below now should work on all architectures.... hopefully.
    long nEl = (sizeof(sLLDexElement) - ((long)elements - (long)obj) )/sizeof(pLLDexGeneric);
    
    FEATUM_DEBUG(10,"nEl = %i, sizeof(sLLDexElement)=%i\n",nEl,sizeof(sLLDexElement));
    for (i=0; i<nEl; i++) {
      FEATUM_DEBUG(11,"attempting to free i=%i (%i)\n",i,*elements);
      if ((*elements != NULL)&&((*elements)->freeLLDex != NULL)) 
        ((*elements)->freeLLDex)(obj); // free each element using its own destructor
      elements++;  
    }
    // algo:   sLLDexElement as array starting at sizeof(int) + sizeof(pPcmBuffer);
              //   with nElem = sizeof(sLLDexElement) - sizeof(int) + sizeof(pPcmBuffer);
              // call function pointer to free function (always first element in struct)
              
    // TODO::: free all the elements and associated data
       // the LLDs object is required for this purpose.... and a search algortihmm... ugly...
       

    free(obj);
  }      
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION






