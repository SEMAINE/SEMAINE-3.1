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
 
 /*   Audio stream wrapper class */
 /* See the corresponding header file (.h) for documentation! */
 
 
 // this file contains the function definitions and the private class data
#define MODULE "audio-stream"
#include "featum_common.h"

#include <stdlib.h>
#include <stdio.h>
#include "featum_util.h"       
#include "audio-stream.h"
#include "wave-input.h"
#include "live-input.h"


#undef FUNCTION
//pAudioStream audioStream_create(void *input, int type);
//pAudioStream audioStream_createGeneric(void *input, int type, 
//int (*getframe_method)(void *, pPcmBuffer), int nChan, int sampleType, int memOrga);

/* Constructor: creates a class data object */
pAudioStream audioStream_createGeneric(void *input, int type, 
                 int (*getframe_method)(void *, pPcmBuffer), int nChan, int sampleType, int memOrga, int sampleRate, int nBPS)
#define FUNCTION "createGeneric"
{_FUNCTION_ENTER_
   pAudioStream obj=NULL;
   
   if (input == NULL) _FUNCTION_RETURN_( NULL );
   
   obj = (pAudioStream)calloc(1,sizeof(sAudioStream));
   if (obj == NULL) _FUNCTION_RETURN_( NULL );
   
   obj->inputObj = input;
   if (type == TYPE_GENERIC) {
     if (getframe_method == NULL) {
       free(obj); _FUNCTION_RETURN_( NULL );         
     }
     obj->getframe_method = getframe_method;
     obj->type = TYPE_GENERIC;       
   } else {
     obj->getframe_method = NULL;
     obj->type = type;
   }
   obj->nChan = nChan;
   obj->sampleType = sampleType;
   obj->memOrga = memOrga;
   obj->sampleRate = sampleRate;
   obj->nBPS = nBPS;
   FEATUM_DEBUG(4,"nChan=%i nBPS=%i sampleRate=%i \n",obj->nChan, obj->nBPS, obj->sampleRate);

   _FUNCTION_RETURN_( obj );
}
#undef FUNCTION

/* Constructor: creates a class data object */
pAudioStream audioStream_create( void *input, int type) 
#define FUNCTION "create"
{_FUNCTION_ENTER_

   pAudioStream obj=NULL;
   
   if (input == NULL) _FUNCTION_RETURN_( NULL );
   
   if (type == TYPE_WAVEINPUT) {
     obj = audioStream_createGeneric(input, type, NULL, 
                               ((pWaveInput)input)->buffer.nChan, 
                               ((pWaveInput)input)->buffer.sampleType, 
                               ((pWaveInput)input)->buffer.memOrga,
                               waveInput_getSampleRate((pWaveInput)input),
                               ((pWaveInput)input)->buffer.nBPS);
   #ifdef LIVE_REC  
   } else if (type == TYPE_LIVEINPUT) {
     obj = audioStream_createGeneric(input, type, NULL, 
                               ((pLiveInput)input)->buffer.nChan, 
                               ((pLiveInput)input)->buffer.sampleType, 
                               ((pLiveInput)input)->buffer.memOrga,
                               liveInput_getSampleRate((pLiveInput)input),
                               ((pLiveInput)input)->buffer.nBPS);
     
   #endif  
   } else {
     // TODO:: more types (e.g. TCPIP, streams...)
     obj = audioStream_createGeneric(input, type, NULL, 2, SAMPLETYPE_I16, MEMORGA_INTERLV, 44100, 2);
   }

   _FUNCTION_RETURN_( obj );
}
#undef FUNCTION


int audioStream_getData( pAudioStream obj, pPcmBuffer data )
#define FUNCTION "getData"
{_FUNCTION_ENTER_
  int ret;
  if (obj != NULL) {
    if (obj->type == TYPE_GENERIC) {
       // call function pointer function:
       if (obj->getframe_method != NULL) {
         _FUNCTION_RETURN_( obj->getframe_method(obj->inputObj,data) );
       } else { _FUNCTION_RETURN_(0); }
    } else {
      switch (obj->type) {
        #ifdef LIVE_REC
        case TYPE_LIVEINPUT:
             FEATUM_DEBUG(10,"TYPE_LIVEINPUT");
             ret = liveInput_getDataSequential((pLiveInput)(obj->inputObj),data);
             _FUNCTION_RETURN_( ret );
             break;    
        #endif         
        case TYPE_WAVEINPUT:
             FEATUM_DEBUG(10,"TYPE_WAVEINPUT");
             ret = waveInput_getDataSequential((pWaveInput)(obj->inputObj),data);
             _FUNCTION_RETURN_( ret );
             break;             
        case TYPE_TCPIP:
             FEATUM_FATAL_ERR(0,"TCP/IP audio streams not yet implemented!");
             _FUNCTION_RETURN_( 0 );
             break;                 
        case TYPE_STREAM:
             FEATUM_FATAL_ERR(0,"Audio streams TYPE_STREAM not yet implemented!");
             _FUNCTION_RETURN_( 0 );
             break;
        default:
             FEATUM_ERR_FATAL(0,"Audio stream type is unknown! (%i)",obj->type);                
             _FUNCTION_RETURN_( 0 );
      }       
    }    
  } else {
    _FUNCTION_RETURN_( 0 );
  }
}
#undef FUNCTION

#ifdef OMPTIMIZE_SIZE
LONG_IDX audioStream_secondsToSamples( pAudioStream obj, FLOAT_TYPE se )
#else
inline LONG_IDX audioStream_secondsToSamples( pAudioStream obj, FLOAT_TYPE se )
#endif
#define FUNCTION "waveInput_secondsToSamples"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    _FUNCTION_RETURN_( (LONG_IDX) ( (FLOAT_TYPE)obj->sampleRate * se ) );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

#ifdef OMPTIMIZE_SIZE
FLOAT_TYPE audioStream_samplesToSeconds( pAudioStream obj, LONG_IDX sa )
#else
inline FLOAT_TYPE audioStream_samplesToSeconds( pAudioStream obj, LONG_IDX sa )
#endif
#define FUNCTION "waveInput_samplesToSeconds"
{_FUNCTION_ENTER_

  if (obj != NULL) {
     _FUNCTION_RETURN_ ( (FLOAT_TYPE)sa / (FLOAT_TYPE)obj->sampleRate );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


/* Destructor: frees the class data object */
pAudioStream audioStream_destroy( pAudioStream obj )
#define FUNCTION "destroy"
{_FUNCTION_ENTER_

  if (obj != NULL)
    free(obj);
        
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION
