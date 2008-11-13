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
 
 /* See the corresponding header file (.h) for documentation! */
 #define MODULE "input-framer"

#include <stdlib.h>
#include <stdio.h>
#include "featum_common.h"
#include "featum_util.h" 
#include "input-framer.h"
 
 // this file contains the function definitions and the private class data
#undef FUNCTION



/* Constructor: creates a class data object */
pInputFramer inputFramer_create(pAudioStream input)
#define FUNCTION "inputFramer_create"
{_FUNCTION_ENTER_
  pInputFramer obj = inputFramer_create2(NULL, input);
  _FUNCTION_RETURN_( obj );
/*
  int i;
  pInputFramer obj = NULL;
  if (input == NULL) _FUNCTION_RETURN_( NULL );

  obj = (pInputFramer)calloc(1,sizeof(sInputFramer));
  if (obj==NULL) _FUNCTION_RETURN_( obj );
  
  obj->input = input;

  obj->nChan = obj->input->nChan;
  obj->sampleType = obj->input->sampleType;
  obj->memOrga = obj->input->memOrga;  
  obj->sampleRate = obj->input->sampleRate;
  obj->nBPS = obj->input->nBPS;
  
  for (i=0; i<MAX_CLIENTS; i++) {
    obj->client[i] = (pInputFramerClient)calloc(1,sizeof(sInputFramerClient));
    obj->client[i]->id = i+1;
    obj->client[i]->windowFunc = WIN_HAMMING;       // default = Hamming Window
    obj->client[i]->preEmphasis = 0.0;              // default = no pre emphasis
    obj->clientLUT[i+1] = i;
  }
  FEATUM_DEBUG(9,"nChan=%i, sampleType=%i, memOrga=%i, MAX_CLIENTS=%i",obj->input->nChan,obj->input->sampleType,obj->input->memOrga,MAX_CLIENTS);
  */
}
#undef FUNCTION

/* Constructor: creates a class data object */
pInputFramer inputFramer_create2(pInputFramer obj, pAudioStream input)
#define FUNCTION "inputFramer_create2"
{_FUNCTION_ENTER_
  int i;
  if (input == NULL) _FUNCTION_RETURN_( NULL );

  if (obj == NULL) {
    obj = (pInputFramer)calloc(1,sizeof(sInputFramer));
  } else {
    memzero(obj,sizeof(sInputFramer));       
  }
  if (obj==NULL) _FUNCTION_RETURN_( obj );
  
  obj->input = input;

  obj->nChan = obj->input->nChan;
  obj->sampleType = obj->input->sampleType;
  obj->memOrga = obj->input->memOrga;  
  obj->sampleRate = obj->input->sampleRate;
  obj->nBPS = obj->input->nBPS;
  
  for (i=0; i<MAX_CLIENTS; i++) {
    obj->client[i] = (pInputFramerClient)calloc(1,sizeof(sInputFramerClient));
    obj->client[i]->id = i+1;
    obj->client[i]->windowFunc = WIN_HAMMING;       // default = Hamming Window
    obj->client[i]->preEmphasis = 0.0;              // default = no pre emphasis
    obj->clientLUT[i+1] = i;
  }
  FEATUM_DEBUG(9,"nChan=%i, sampleType=%i, memOrga=%i, MAX_CLIENTS=%i",obj->input->nChan,obj->input->sampleType,obj->input->memOrga,MAX_CLIENTS);
  _FUNCTION_RETURN_( obj );
}
#undef FUNCTION

int inputFramer_setWindowing(pInputFramer obj, int id, int type)
#define FUNCTION "inputFramer_setWindowing"
{_FUNCTION_ENTER_

  int id2;
  if (obj == NULL) _FUNCTION_RETURN_( 0 );
  if ((id <1)||(id>MAX_CLIENTS)) _FUNCTION_RETURN_( 0 );
  
  id2 = obj->clientLUT[id];
  
  obj->client[id2]->windowFunc = type;
   
  _FUNCTION_RETURN_(1); 
}
#undef FUNCTION


int inputFramer_setPreEmphasis(pInputFramer obj, int id, FLOAT_TYPE preEmphasis)
#define FUNCTION "inputFramer_setPreEmphasis"
{_FUNCTION_ENTER_

  int id2;
  if (obj == NULL) _FUNCTION_RETURN_( 0 );
  if ((id <1)||(id>MAX_CLIENTS)) _FUNCTION_RETURN_( 0 );
  
  id2 = obj->clientLUT[id];
  
  obj->client[id2]->preEmphasis = preEmphasis;
  if ((preEmphasis > 0.0)&&(obj->nChan > 0)) {
    if (obj->client[id2]->preEmphasis_status != NULL) free(obj->client[id2]->preEmphasis_status);
    obj->client[id2]->preEmphasis_status = (WORK_SAMPLE*)calloc(1,sizeof(WORK_SAMPLE)*obj->nChan);
  }
  _FUNCTION_RETURN_(1); 
}
#undef FUNCTION

// sets the audio format, the data read from input should be converted to before returning it via getFrame
// if nothing has been set, the input format will be used. 
void inputFramer_setAudioParameters(pInputFramer obj, int nChan, int sampleType, int memOrga)
#define FUNCTION "inputFramer_setAudioParameters"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    if (nChan > 0) obj->nChan = nChan;
    if (sampleType >= 0) obj->sampleType = sampleType;
    if (memOrga >= 0) obj->memOrga = memOrga;
    FEATUM_DEBUG(9,"nChan=%i, sampleType=%i, memOrga=%i",obj->nChan,obj->sampleType,obj->memOrga);
  }
  _FUNCTION_RETURNV_;
}
#undef FUNCTION


// internal:     
// sort clients, so that they appear in descending order regarding frameStep
// also update maxFrameLength!
int sortClients(pInputFramer obj)
#define FUNCTION "sortClients"
{_FUNCTION_ENTER_
  int i,j; int idx;
  pInputFramerClient tmp;
 
  for (i=0; i<obj->nClients-1; i++) {
    // search for largest
    idx = i;
    for (j=i+1; j<obj->nClients; j++) {
      if (obj->client[j]->frameStep > obj->client[idx]->frameStep)
        idx = j;
    }
    // swap
    if (idx != i) {
      tmp = obj->client[i];
      obj->client[i] = obj->client[idx];
      obj->client[idx] = tmp; 
    }
    if (obj->client[i]->id > 0) 
      obj->clientLUT[obj->client[i]->id] = i;
    else 
      obj->clientLUT[obj->client[i]->id] = -1; 
      
    FEATUM_DEBUG(11,"obj->client[%i]->frameLength=%i, step=%i",i,obj->client[i]->frameLength,obj->client[i]->frameStep);
    if (obj->client[i]->frameLength > obj->maxFrameLength) 
      obj->maxFrameLength = obj->client[i]->frameLength;
  }    
  if (obj->client[i]->id > 0) 
    obj->clientLUT[obj->client[i]->id] = i;
  else 
    obj->clientLUT[obj->client[i]->id] = -1; 

  _FUNCTION_RETURN_(1);
}
#undef FUNCTION


LONG_IDX inputFramer_getClient_frameLength(pInputFramer obj, int id)
#define FUNCTION "inputFramer_getClient_frameLength"
{_FUNCTION_ENTER_
    
  int id2;
  if (obj == NULL) _FUNCTION_RETURN_( 0 );
  if ((id <1)||(id>MAX_CLIENTS)) _FUNCTION_RETURN_( 0 );
  
  id2 = obj->clientLUT[id];
  
  _FUNCTION_RETURN_(obj->client[id2]->frameLength);
}
#undef FUNCTION


LONG_IDX inputFramer_getClient_frameStep(pInputFramer obj, int id)
#define FUNCTION "inputFramer_getClient_frameLength"
{_FUNCTION_ENTER_

  int id2;
  if (obj == NULL) _FUNCTION_RETURN_( 0 );
  if ((id <1)||(id>MAX_CLIENTS)) _FUNCTION_RETURN_( 0 );
  
  id2 = obj->clientLUT[id];
  
  _FUNCTION_RETURN_(obj->client[id2]->frameStep);
}
#undef FUNCTION


// add a framer client with parameters framesize (in seconds) and overlap (in percent)
// powOf2 specifies whether the nearest power of 2 framesize should be chosen
// instead of the strict values (overlap will then be adjusted to ensure the same framestep)
int inputFramer_addClientSecOvl(pInputFramer obj, float framesize, float overlap, int powOf2)
#define FUNCTION "inputFramer_addClientSec"
{_FUNCTION_ENTER_

  LONG_IDX flen, fstep;
  float sr;
  int ret;    

  if (obj == NULL) _FUNCTION_RETURN_(0);
  if (obj->input == NULL) _FUNCTION_RETURN_(0);
  
  sr = (float)(obj->input->sampleRate);
  flen = (LONG_IDX)( framesize * sr );
  fstep = (LONG_IDX)( (1.0-overlap) * framesize * sr );
  
  if (powOf2) {
    // adjust fstep and round flng to nearest power of 2
    // round flng to nearest power of 2
    unsigned long int flng = (unsigned long int)flen;           
    unsigned long int fmask = 0x8000;
    while ( (fmask & flng) == 0) { fmask = fmask >> 1; }
    // fmask now contains the MSB position
    if (fmask > 1) {
      if ( (fmask>>1)&flng ) { flng = fmask<<1; }
      else { flng = fmask; }
    } else {
      flng = 2;           
    }  
    flen = (LONG_IDX)flng;
  }
  
  ret = inputFramer_addClient(obj, flen, fstep);           
  _FUNCTION_RETURN_( ret );
}
#undef FUNCTION

// add a framer client with parameters framesize (in seconds) and framestep (in seconds)
// powOf2 specifies whether the nearest power of 2 framesize should be chosen
// instead of the strict values (framestep will remain unaffected)
int inputFramer_addClientSecStep(pInputFramer obj, float framesize, float framestep, int powOf2)
#define FUNCTION "inputFramer_addClientSec"
{_FUNCTION_ENTER_

  LONG_IDX flen, fstep;
  float sr;
  int ret;    

  if (obj == NULL) _FUNCTION_RETURN_(0);
  if (obj->input == NULL) _FUNCTION_RETURN_(0);
  
  sr = (float)(obj->input->sampleRate);
  
  fstep = (LONG_IDX)( framestep * sr );
  flen = (LONG_IDX)( framesize * sr );    
  FEATUM_DEBUG(10,"original flen: %i",flen);
  
  if (powOf2) {
    // round flng to nearest power of 2
    unsigned long int flng = (unsigned long int)flen;           
    unsigned long int fmask = 0x8000;
    while ( (fmask & flng )== 0) { fmask = fmask >> 1; }
    FEATUM_DEBUG(12,"fmask: %i, flng: %i",fmask,flng);
    // fmask now contains the MSB position
    if (fmask > 1) {
      if ( (fmask>>1)&flng ) { flng = fmask<<1; }
      else { flng = fmask; }
    } else {
      flng = 2;           
    }  
    flen = (LONG_IDX)flng;
    FEATUM_DEBUG(10,"adjusted flen: %i",flen);  
  } 

  ret = inputFramer_addClient(obj, flen, fstep);
  _FUNCTION_RETURN_( ret );
}
#undef FUNCTION


int inputFramer_addClient(pInputFramer obj, int framesize, int framestep)
#define FUNCTION "inputFramer_addClient"
{_FUNCTION_ENTER_

  int i,id;
  if (obj == NULL) _FUNCTION_RETURN_( 0 );

  FEATUM_DEBUG(9," (start)  framesize %i, framestep %i , maxFrameLength: %i, nClients: %i",framesize,framestep,obj->maxFrameLength,obj->nClients);
  i = obj->nClients;

  if (i>=MAX_CLIENTS) _FUNCTION_RETURN_( 0 );
  
  // add client at index i (=id-1)
  obj->client[i]->frameLength = framesize;
  
  if (framesize > obj->maxFrameLength) obj->maxFrameLength = framesize;
  obj->client[i]->frameStep = framestep;

  i = obj->client[i]->id; // return id 
  obj->nClients++;
          
  sortClients(obj);
  // (TODO:) update buffer indicies !!
  // -> dirty but ok (?) : free buffer, it will be re-initialized by next getFrame call
  //                        ( a small amount of input data will be skipped )
  obj->buffer = pcmRingBufferMR_destroy(obj->buffer);
  
  FEATUM_DEBUG(9," (end) id=%i, framesize %i, framestep %i , maxFrameLength: %i, nClients: %i",i,framesize,framestep,obj->maxFrameLength,obj->nClients);
  _FUNCTION_RETURN_(i); 
}
#undef FUNCTION


int inputFramer_deleteClient(pInputFramer obj, int id) 
#define FUNCTION "inputFramer_deleteClient"
{_FUNCTION_ENTER_
  int i;
  if (obj == NULL) _FUNCTION_RETURN_( 0 );
  #ifdef COMPILE_CHECKS
  if (obj->client == NULL) _FUNCTION_RETURN_(0);
  if ( (id < 1) || (id > MAX_CLIENTS) ) _FUNCTION_RETURN_(0);
  #endif 

  i=obj->clientLUT[id];
  FEATUM_DEBUG(9," id=%i, framesize %i, framestep %i , maxFrameLength: %i, nClients (after del.): %i",id, obj->client[i]->frameLength, obj->client[i]->frameStep, obj->maxFrameLength, obj->nClients-1);
  obj->client[i]->frameLength = 0;
  obj->client[i]->frameStep = 0;
  if (obj->client[i]->preEmphasis_status) {
    free(obj->client[i]->preEmphasis_status); 
    obj->client[i]->preEmphasis_status = NULL;
  }
    
  sortClients(obj);  // call this BEFORE decreasing nClients!!
  obj->nClients--;
  
  // (TODO:) update buffer indicies !!
  // -> dirty but ok (?) : free buffer, it will be re-initialized by next getFrame call
  //                        ( a small amount of input data will be skipped )
  obj->buffer = pcmRingBufferMR_destroy(obj->buffer);
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION


// MAIN method, get frame for client ID
// NOTE: the memory must be freed by the application!?
// the last parameter is optional, if not NULL, the given object will be cleared
// and the data will be written to this object (for iterative calls to getFrame)
int inputFramer_getFrame(pInputFramer obj, int id, pPcmBuffer *frame)
#define FUNCTION "inputFramer_getFrame"
{_FUNCTION_ENTER_

  int i,id2;
  if (obj == NULL) _FUNCTION_RETURN_( 0 );
  if ((id <1)||(id>MAX_CLIENTS)) _FUNCTION_RETURN_( 0 );
  if (frame == NULL) _FUNCTION_RETURN_( 0 );
  if (obj->nClients <= 0) _FUNCTION_RETURN_( 0 );
  
  id2 = obj->clientLUT[id];

  // check if buffer is allocated 
  if (obj->buffer == NULL) {
    FEATUM_DEBUG(10,"Creating ringbuffer (2x %i) ...", obj->maxFrameLength);
    #ifdef COMPILE_CHECKS
    if (obj->maxFrameLength == 0) {
      FEATUM_FATAL_ERR(2,"Ringbuffer of size 0 (obj->maxFrameLength == 0) !!");
      _FUNCTION_RETURN_( 0 );
    }
    #endif
    obj->buffer = pcmRingBufferMR_create(NULL, obj->maxFrameLength *2, obj->nClients,
                              obj->input->nChan, obj->input->sampleType, obj->input->memOrga, obj->input->sampleRate);
    #ifdef COMPILE_CHECKS
    if (obj->buffer == NULL) {
      FEATUM_FATAL_ERR(0,"Error allocating memory!");
      _FUNCTION_RETURN_( 0 );                
    }
    #endif
    FEATUM_DEBUG(10,"filling ringbuffer with data from input ...");
//    FEATUM_DEBUG(10,"filling RRR with data from input ...\n");
    // fill with data from input
    obj->buffer->wridx = audioStream_getData(obj->input, obj->buffer->data);
  }
  //FEATUM_DEBUG(10,"Allocated rBuffer ...");
  
  // ... and has enough data:
  if (pcmRingBufferMR_toread(obj->buffer,id2) < obj->client[id2]->frameLength) {
    //load more data
    FEATUM_DEBUG(10,"Re-reading data into ringbuffer\n   (free %i, current fl: %i) ...",pcmRingBufferMR_freespace(obj->buffer),obj->client[id2]->frameLength);
    pPcmBuffer tmp = pcmBufferAllocate(NULL, pcmRingBufferMR_freespace(obj->buffer),
                        obj->nChan, obj->sampleType, obj->memOrga, obj->sampleRate);
    audioStream_getData(obj->input, tmp);
    pcmRingBufferMR_write(obj->buffer, tmp);
    pcmBufferFree(tmp,0);
  }

  
  // copy if...  (synchronisation constraints are fulfilled)
  int copy = 0;  
  FEATUM_DEBUG(10,"is there data to copy for current id? (%i) ",id2);

  if (obj->nClients == 1) {
    copy = 1;       FEATUM_DEBUG(10,"copy=1  only 1 client ");              
  } else {
  
  // TODO: framestep or framelength -> whatever is smaller!!??  ok
  // TODO: always return frames if nClients==1 !  never return frame for nClients <= 0
  if (id2 == 0) {  // master id
    FEATUM_DEBUG(11,"check master id: ri+1 = %i , ri = %i , fs+1 = %i, fl+1 = %i",obj->buffer->rdidx[id2+1],obj->buffer->rdidx[id2],obj->client[id2+1]->frameStep,obj->client[id2+1]->frameLength);
    if (obj->buffer->rdidx[id2+1] >= 
           obj->buffer->rdidx[id2] 
           - min_l(obj->client[id2+1]->frameStep,obj->client[id2+1]->frameLength)/2 ) // LC
    { copy = 1;    FEATUM_DEBUG(10,"copy=1 LC only "); }
  } else if (id2 == obj->nClients-1) { // last id
    FEATUM_DEBUG(11,"check last id: ri-1 = %i , ri = %i , fs = %i, fl = %i",obj->buffer->rdidx[id2-1],obj->buffer->rdidx[id2],obj->client[id2]->frameStep,obj->client[id2]->frameLength);
    if (obj->buffer->rdidx[id2] 
        + min_l(obj->client[id2]->frameStep,obj->client[id2]->frameLength)/2 
              < obj->buffer->rdidx[id2-1]) // UC
    { copy = 1;    FEATUM_DEBUG(10,"copy=1 UC only ");}
  } else { // inbetween
    FEATUM_DEBUG(11,"check inbetween id: ri-1 = %i , ri = %i , fs = %i, fl = %i",obj->buffer->rdidx[id2-1],obj->buffer->rdidx[id2],obj->client[id2]->frameStep,obj->client[id2]->frameLength);
    if ((obj->buffer->rdidx[id2+1] >= obj->buffer->rdidx[id2] - obj->client[id2+1]->frameStep/2 ) // LC
        && (obj->buffer->rdidx[id2] + obj->client[id2]->frameStep/2 < obj->buffer->rdidx[id2-1])) // UC
    { 
      copy = 1;    FEATUM_DEBUG(10,"copy=1 UC + LC ");
    }
  }
  
  }  // if (obj->nClients == 1) 

  // do copy and allocate output frame buffer
  int windowSetup_flag = 0;
  if (copy) {
    FEATUM_DEBUG(10,"copy == 1");
    // frame memory management
    if (*frame == NULL) {
      // allocate new frame
      *frame = pcmBufferAllocate(NULL, obj->client[id2]->frameLength, 
                   obj->nChan, obj->sampleType, obj->memOrga, obj->sampleRate);
      if (obj->client[id2]->windowFunc != WIN_NONE) 
        windowSetup_flag = 1; //pcmProcess_windowingSetup( *frame, 1, obj->client[id2]->windowFunc );
      if ((obj->client[id2]->preEmphasis > 0.0)&&(obj->nChan > 0)) {
        if (obj->client[id2]->preEmphasis_status != NULL) free(obj->client[id2]->preEmphasis_status);
        obj->client[id2]->preEmphasis_status = (WORK_SAMPLE*)calloc(1,sizeof(WORK_SAMPLE)*obj->nChan);
      }
    } else {
      // check if frame is of correct length!
      if ((*frame)->nBlocksAlloc < obj->client[id2]->frameLength) {
        *frame = pcmBufferFree(*frame,1);
        *frame = pcmBufferAllocate(*frame, obj->client[id2]->frameLength, 
                     obj->nChan, obj->sampleType, obj->memOrga, obj->sampleRate);
        if (obj->client[id2]->windowFunc != WIN_NONE)
          windowSetup_flag = 1; //pcmProcess_windowingSetup( *frame, 1, obj->client[id2]->windowFunc );
      } else {
        pcmBuffer_clear(*frame);    
      }
    }
    // actual copy process:
    int blocksread = 0;
    blocksread = pcmRingBufferMR_readStep(obj->buffer, id2, *frame, obj->client[id2]->frameStep);
    if (windowSetup_flag) 
      pcmProcess_windowingSetup( *frame, 1, obj->client[id2]->windowFunc );
    if (obj->client[id2]->preEmphasis > 0.0) 
      pcmProcess_preEmphasis(*frame, obj->client[id2]->preEmphasis, obj->client[id2]->preEmphasis_status);
    if (obj->client[id2]->windowFunc != WIN_NONE) pcmProcess_doWindowing(*frame);
    if (*frame != NULL) (*frame)->timestamp = audioStream_samplesToSeconds(obj->input,(*frame)->timestampSamples);
    
    if (blocksread <= 0) { // EOF!
      FEATUM_DEBUG(5,"End-Of-Input Stream: blocksread == 0!");
      copy = 0; 
      pcmBufferFree(*frame,1);
      (*frame)->nBlocks = 0;
//      *frame = NULL;    //!!!!!!!!!!!!!!!!!
    }
  } else {
    FEATUM_DEBUG(10,"Nothing to copy ... stream locked!");       
  }
  #ifdef DEBUG
  if (*frame != NULL) 
    FEATUM_DEBUG(9," id=%i , (id2=%i),, frame->nBlocks=%i , frame->nBlocksAlloc=%i",id,id2,(*frame)->nBlocks,(*frame)->nBlocksAlloc);
  #endif
  
  _FUNCTION_RETURN_( copy );
}
#undef FUNCTION

/* Destructor: frees the class data object */
// free only the inputFramer class
pInputFramer inputFramer_destroy(pInputFramer obj)
#define FUNCTION "inputFramer_destroy"
{_FUNCTION_ENTER_

  int i;
  if (obj != NULL) {
    // free buffers
    obj->buffer = pcmRingBufferMR_destroy(obj->buffer);  
    for (i=0; i<MAX_CLIENTS; i++) {
      if (obj->client[i]) {
        if (obj->client[i]->preEmphasis_status) free(obj->client[i]->preEmphasis_status); 
        free(obj->client[i]);
      }
    }

    free(obj);
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION

/* Destructor: frees the class data object */
// free only the inputFramer class
pInputFramer inputFramer_destroyData(pInputFramer obj)
#define FUNCTION "inputFramer_destroyData"
{_FUNCTION_ENTER_

  int i;
  if (obj != NULL) {
    // free buffers
    obj->buffer = pcmRingBufferMR_destroy(obj->buffer);  
    for (i=0; i<MAX_CLIENTS; i++) {
      if (obj->client[i]) {
        if (obj->client[i]->preEmphasis_status) free(obj->client[i]->preEmphasis_status); 
        free(obj->client[i]); obj->client[i] = NULL;
      }
    }
  }
  _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION

// also free input audio stream object
pInputFramer inputFramer_destroyWithInput(pInputFramer obj)
#define FUNCTION "inputFramer_destroyWithInput"
{_FUNCTION_ENTER_
  
  if (obj != NULL) {
    obj->input = audioStream_destroy(obj->input);

    //return inputFramer_destroy(obj);
    obj = inputFramer_destroy(obj);
    _FUNCTION_RETURN_( obj );
  }
  _FUNCTION_RETURN_( NULL );
  //return NULL;
}
#undef FUNCTION
