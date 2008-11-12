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
 
 /*  IMPLEMENTATION of the feature extractor module LLDtime
     See the corresponding header file (.h) for documentation!  */

#define MODULE "LLDtime"
 
 // this file contains the function definitions and the private class data

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "feature-memory.h"
//#include "LLDtime.h"
#include "LLDs.h"

#undef FUNCTION     // use undef only if you define it below for every function


// default constructor 
pLLDtime LLDtime_create( pLLDtime obj )
#define FUNCTION "LLDtime_create"
{_FUNCTION_ENTER_
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION 


// custom configuration of extractor parameters
/*  this is only a template function:
int LLDtime_configure( pLLDtime obj )
#define FUNCTION "LLDtime_create"
{_FUNCTION_ENTER_
  if (obj != NULL) {
     _FUNCTION_RETURN_( 1 );
  }
  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION 
*/


// setup names in feature memory object
/*
int LLDtime_setupFtmem( int level, pFeatureMemory mem )
#define FUNCTION "LLDtime_setupNames"
{_FUNCTION_ENTER_
  int ret;
  ret = featureMemory_setupElement( mem, level, FT_LLD_TIME, FT_LLD_TIME_nVal, (char **)&lld_time_ftMem_names );
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION 
*/  

//int featureMemory_flushToFtMem( pFeatureMemory mem, int level, int el, int nVal )

// flushToMem: copy final data from ftex to feature memory element array
int LLDtime_flushToFtMem( int level, pFeatureMemory mem, pLLDex lldex )
#define FUNCTION "LLDtime_flushToFtMem"
{_FUNCTION_ENTER_
 
  if (lldex == NULL)  _FUNCTION_RETURN_(0);
  #ifdef COMPILE_CHECKS_LOOPS
  if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
  if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
  #endif
  if (lldex->current[level]->time == NULL) _FUNCTION_RETURN_(0);
  if (lldex->current[level]->time->flush_status == LLDEX_FLUSHED) _FUNCTION_RETURN_(2);
  
  FLOAT_TYPE_FTMEM *values 
    = featureMemory_getValuesForWrite_seq( mem, level, ft_lld_time, 
                                           FT_LLD_TIME_nVal, 0, 
                                           lldex->current[level]->timestamp );


  if (values != NULL) {
   values[0] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->time->zcr; 
   values[1] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->time->dc; 
   values[2] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->time->min;
   values[3] 
      = (FLOAT_TYPE_FTMEM)lldex->current[level]->time->max;

   lldex->current[level]->time->flush_status = LLDEX_FLUSHED;
   //(TODO: assign timestamps...??   should be done in featureMemory_getValuesWrite...)

    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// extractor:  do the main extraction , this function is called once per frame
// if data from multiple past frames is required, it is accessible via the lldex object
int LLDtime_extractor( pLLDtime obj, pLLDex lldex, int level )
#define FUNCTION "LLDtime_extractor"
{_FUNCTION_ENTER_
  if (lldex != NULL) {
    #ifdef COMPILE_CHECKS_LOOPS
    if (level >= lldex->nIDs) _FUNCTION_RETURN_(0);
    if (level < 0) _FUNCTION_RETURN_(0);
    if (lldex->current[level] == NULL) _FUNCTION_RETURN_(0);
    #endif
    if (lldex->current[level]->time == NULL) {
       LLDtime_createLLDex( lldex->current[level] );                               
    }

    /* embedded processing here, so we only loop through data once... */
    // TODO: check for correct data format!
    pPcmBuffer b = lldex->current[level]->pcm;
    if (b != NULL) {
      if (b->nBlocks == 0) _FUNCTION_RETURN_( 0 );

        LONG_IDX n; int nZc = 0; long dc = 0;
        WORK_SAMPLE max = ((WORK_SAMPLE *)b->data)[0]; WORK_SAMPLE min = ((WORK_SAMPLE *)b->data)[0];
        WORK_SAMPLE s;
        for (n=1; n < b->nBlocks-1; n++) {
          s = ((WORK_SAMPLE *)b->data)[n];
          if ( 
/*
               (  ( ((WORK_SAMPLE *)b->data)[n-1] > 0 ) && ( s < 0 ) )
               || (  ( ((WORK_SAMPLE *)b->data)[n-1] < 0 ) && ( s > 0 ) )
               || ( ( s == 0 ) && ( (((WORK_SAMPLE *)b->data)[n-1]<0) && (((WORK_SAMPLE *)b->data)[n+1]>0) ) )
               || ( ( s == 0 ) && ( (((WORK_SAMPLE *)b->data)[n-1]>0) && (((WORK_SAMPLE *)b->data)[n+1]<0) ) )
*/
               (  ((WORK_SAMPLE *)b->data)[n-1]*s < 0  )
               || ( ( s == 0 ) && ( ((WORK_SAMPLE *)b->data)[n-1]*((WORK_SAMPLE *)b->data)[n+1] < 0 ) )
             ) {
                 nZc++;
               }
          dc += s;
          if ( s > max ) { max = s; }
          if ( s < min ) { min = s; }
        }  
        /* last sample */
        s = ((WORK_SAMPLE *)b->data)[n];
        dc += s;
        if ( s > max ) { max = s; }
        if ( s < min ) { min = s; }
        
        lldex->current[level]->time->zcr = ((FLOAT_TYPE)nZc)/((FLOAT_TYPE)(b->nBlocks))*(FLOAT_TYPE)(b->sampleRate)/1000.0;
        lldex->current[level]->time->dc = ((FLOAT_TYPE)dc)/((FLOAT_TYPE)(b->nBlocks));
        lldex->current[level]->time->min = ((FLOAT_TYPE)min);
        lldex->current[level]->time->max = ((FLOAT_TYPE)max);
        _FUNCTION_RETURN_(1);
      
    }
  }

  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// create LLDex object for this extractor
int LLDtime_createLLDex( pLLDexElement obj )
#define FUNCTION "LLDtime_createLLDex"
{_FUNCTION_ENTER_
  pLLDexTime p = NULL;
  int ret = 0;
  
  if (obj != NULL) {
    if (obj->time != NULL) LLDtime_freeLLDex(obj);          
    p = (pLLDexTime) calloc(1,sizeof(sLLDexTime));
    if (p != NULL) {
      p->freeLLDex = &LLDtime_freeLLDex;
      ret = 1;
    }
    obj->time = p;
  }
  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION


// free the lldex object l and return NULL
int LLDtime_freeLLDex( pLLDexElement obj )
#define FUNCTION "LLDtime_freeLLDex"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if (obj->time != NULL) {
      free(obj->time);  obj->time = NULL;
      _FUNCTION_RETURN_(1);
    }   
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// destroy the LLDtime feature extractor object
pLLDtime LLDtime_destroy( pLLDtime obj )
#define FUNCTION "LLDtime_destroy"
{_FUNCTION_ENTER_
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION



