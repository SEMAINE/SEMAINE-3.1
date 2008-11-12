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
 
 
 /* NOTE::   live input is currently only supported for LINUX !!! */
 
 /*  IMPLEMENTATION of the class <liveInput>
     See the corresponding header file (.h) for documentation!  */
 #define MODULE "live-input"
 
 // this file contains the function definitions and the private class data

//!!!! TODO: check correct handling of obj->curPosBlocks !!! 
// so far it is only changed in rb_copy, which is not really the most approriate position
// probably it should be changed after n bytes have been READ from the file
// since curPosBlocks is the file block pointer and should have nothing to do with the ringbuffer


#include <stdio.h>
#include <strings.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include "featum_common.h"

#ifdef LIVE_REC

#ifndef __WINDOWS 
#include <string.h>  // for memcpy function
#ifndef USE_PORTAUDIO
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#endif
#endif 

#include "featum_util.h"       //( optional )
#include "live-input.h"
#include "pcm-data.h"

#undef FUNCTION

#ifdef __WINDOWS
//#error  live-input.c module currently has only LINUX support (/dev/dsp interface) 
#ifndef USE_PORTAUDIO
#define USE_PORTAUDIO
#warning you must use USE_PORTAUDIO when compiling for windows
#endif
#endif

#include "portaudio.h"

// private functions:
pPcmBuffer liveInput_readRawDataFromDevice( pLiveInput obj, pPcmBuffer dataread, LONG_IDX length );
int liveInput_rb_copy( pLiveInput obj, pPcmBuffer data, LONG_IDX lng );

// get current system time
long liveInput_getTime(void)
#define FUNCTION "liveInput_getTime"
{_FUNCTION_ENTER_
  // TODO::::
    
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION

//TODO: check for space in ringbuffer.... if no space, discard data
int liveInput_writeToRingBuffer( pLiveInput obj, pPcmBuffer tmpBuf ) 
#define FUNCTION "liveInput_writeToRingBuffer"
{
#ifndef USE_PORTAUDIO
_FUNCTION_ENTER_
#endif

  #ifdef COMPILE_CHECKS
  if (tmpBuf == NULL) {
    #ifndef USE_PORTAUDIO
    FEATUM_ERROR(1,"tmpBuf supplied == NULL!\n");
    _FUNCTION_RETURN_(0);
    #else
    return 0;
    #endif           
  }
  if (tmpBuf->nBlocks == 0) {
    #ifndef USE_PORTAUDIO
    FEATUM_WARNING(5,"supplied buffer has zero bytes!!\n");
    _FUNCTION_RETURN_(0);           
    #else
    return 0;
    #endif
  }
  #endif
  
  #ifdef USE_PORTAUDIO
  // wait for buffer lock
  while (obj->rbLock) {
    Pa_Sleep(1);
  }
  obj->rbLock = 1;
  #endif
  
  LONG_IDX copied = 0;
  LONG_IDX tocopy = tmpBuf->nBlocks;
  #ifndef USE_PORTAUDIO
  FEATUM_DEBUG(11,"readblocks = %i (to copy)",readblocks);  
  FEATUM_DEBUG(12,"obj->maxBufferSizeBlocks = %i , obj->buffer.nBlocksAlloc = %i", obj->maxBufferSizeBlocks, obj->buffer.nBlocksAlloc);      
  FEATUM_DEBUG(12,"obj->idxCurEnd = %i",obj->idxCurEnd);
  #endif
  
  // check for space in buffer and if necessary, limit "tocopy" length
  LONG_IDX free = obj->bufferSizeBlocks - (obj->idxWr-obj->idxRd);
  if (tocopy > free) {
    #ifndef USE_PORTAUDIO
    FEATUM_WARNING(4,"DROPPED %i blocks while writing to ringbuffer",tocopy-free);
    #endif           
    obj->nSkipped += tocopy-free;
    tocopy = free;
  }
  if (tocopy <= 0) {
    obj->rbLock = 0;
    #ifndef USE_PORTAUDIO
    _FUNCTION_RETURN_(0);  // TODO: maybe timeout counter here, after when 0 will be returned?
    #else
    return 0;
    #endif
  }
  
  obj->idxWarpedWr = (obj->idxWr % obj->bufferSizeBlocks);
  if (obj->bufferSizeBlocks - obj->idxWarpedWr < tocopy) { // one step copy
     copied = pcmBuffer_copy(tmpBuf, &(obj->buffer), 0, tocopy , obj->idxWarpedWr);
     #ifndef USE_PORTAUDIO
     FEATUM_DEBUG(11,"1step copied = %i",copied);
     #endif
  } else { 
     copied = pcmBuffer_copy(tmpBuf, &(obj->buffer), 0, obj->bufferSizeBlocks - obj->idxWarpedWr , obj->idxWarpedWr);
     #ifndef USE_PORTAUDIO
     FEATUM_DEBUG(11,"2step copied S1 = %i",copied);
     #endif
  }
  obj->idxWr += copied; obj->idxWarpedWr = (obj->idxWr % obj->bufferSizeBlocks);

  if (tocopy-copied > 0) {
    copied = pcmBuffer_copy(tmpBuf, &(obj->buffer), copied, tocopy, obj->idxWarpedWr );
    obj->idxWr += copied; obj->idxWarpedWr = (obj->idxWr % obj->bufferSizeBlocks);
    #ifndef USE_PORTAUDIO
    FEATUM_DEBUG(11,"2step copied S2 = %i",copied);
    #endif
  }
      
  // stupid correction (nBlocks gets altered by pcmBuffer_copy...)
  obj->buffer.nBlocks = obj->buffer.nBlocksAlloc;

  #ifdef USE_PORTAUDIO
  obj->rbLock = 0;
  #endif
            
  #ifndef USE_PORTAUDIO
  FEATUM_DEBUG(12,"(at end) obj->idxCurEnd = %i",obj->idxCurEnd);
  #endif
  
  #ifdef USE_PORTAUDIO
  return 1;
  #else
  _FUNCTION_RETURN_(1);
  #endif
}
#undef FUNCTION



#ifdef USE_PORTAUDIO



/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int liveInput_pa_recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
  pLiveInput obj = (pLiveInput)userData;

  if (obj == NULL) { return paAbort; }
  if (obj->abort) { return paAbort; }
  if (obj->bufWrapper == NULL) { return paAbort; }
//printf("fpb: %i\n",framesPerBuffer);
  pPcmBuffer b = pcmBufferMakeWrapper( obj->bufWrapper, (LONG_IDX)framesPerBuffer, obj->parameters.nChan, 
                               obj->parameters.sampleType, MEMORGA_INTERLV, obj->parameters.sampleRate, inputBuffer);
  if (b!=NULL) {
    if (liveInput_writeToRingBuffer(obj,b)) {
      obj->bufUpdate = 1;                                        
    }
  }

  return paContinue;
}


int liveInput_startRecording (pLiveInput obj)
#define FUNCTION "liveInput_startRecording"
{_FUNCTION_ENTER_

    PaStreamParameters  inputParameters;
    PaError             err = paNoError;

    if (obj == NULL) _FUNCTION_RETURN_(0);
    if (obj->paFrames == 0) _FUNCTION_RETURN_(0);
    if (Pa_IsStreamActive( obj->stream ) == 1) {
      FEATUM_WARNING(3,"stream is already active!");
      _FUNCTION_RETURN_(1);                       
    }
    
    FEATUM_DEBUG(4,"nChan=%i nBPS=%i sampleRate=%i\n",obj->parameters.nChan, obj->parameters.nBPS, obj->parameters.sampleRate);
    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    inputParameters.channelCount = obj->parameters.nChan;
    switch (obj->parameters.sampleType) {
      case SAMPLETYPE_I8: inputParameters.sampleFormat = paInt8; break;
      case SAMPLETYPE_I16: inputParameters.sampleFormat = paInt16; break;
      case SAMPLETYPE_I24: inputParameters.sampleFormat = paInt24; break;
      case SAMPLETYPE_I32: inputParameters.sampleFormat = paInt32; break;
      case SAMPLETYPE_FLOAT: inputParameters.sampleFormat = paFloat32; break;
      default:
        FEATUM_ERROR_FATAL(1,"invalid number of bits requested: %i (allowed: 8, 16, 24, 32)\n",obj->parameters.nBPS);
    }
    inputParameters.suggestedLatency =
      Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    obj->bufWrapper  = pcmBufferMakeWrapper( obj->bufWrapper, 0, 1, SAMPLETYPE_I16, 0, 0, NULL);
    /* Setup recording stream -------------------------------------------- */
    err = Pa_OpenStream(
              &(obj->stream),
              &inputParameters,
              NULL,                  /* &outputParameters, */
              obj->parameters.sampleRate,
              obj->paFrames,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              liveInput_pa_recordCallback,
              obj );
    if( err != paNoError ) {
        FEATUM_ERROR_FATAL(0,"error opening portaudio recording stream (code %i) (check samplerate! -> %i)\n",err,obj->parameters.sampleRate);
        _FUNCTION_RETURN_(0);
    }

    err = Pa_StartStream( obj->stream );
    if( err != paNoError ) {
        FEATUM_ERROR_FATAL(0,"cannot start portaudio stream (code %i)\n",err);
        _FUNCTION_RETURN_(0);
    }
    obj->absTimeStart = liveInput_getTime();
    obj->streamStatus = LI_STREAM_STARTED;
    
    _FUNCTION_RETURN_(1);
}
#undef FUNCTION

int liveInput_stopRecording( pLiveInput obj ) 
#define FUNCTION "liveInput_stopRecording"
{_FUNCTION_ENTER_
  PaError             err = paNoError;
  if (obj == NULL) _FUNCTION_RETURN_(0);
  if (obj->streamStatus == LI_STREAM_STOPPED) _FUNCTION_RETURN_(0);
  
  obj->streamStatus =  LI_STREAM_STOPPED;
  // TODO: timestamp!
  obj->absTimeStop = liveInput_getTime(); 

  err = Pa_CloseStream( obj->stream );
  if( err != paNoError ) {
    FEATUM_ERROR(2,"cannot close portaudio stream (code %i)\n",err);
    _FUNCTION_RETURN_(0);
  }
  
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION


#else  // don't USE_PORTAUDIO  // fallback to simple recorder (/dev/dsp)
//
// setup the soundcard for recording  (Linux, OSS, /dev/dsp, very cheap interface)
//  
int liveInput_startRecording (pLiveInput obj)
#define FUNCTION "liveInput_startRecording"
{_FUNCTION_ENTER_
	int tmp;

    if (obj == NULL) { _FUNCTION_RETURN_(0); }
   	if (obj->fd > 0) {
      FEATUM_WARNING(3,"device already opened, obj->fd > 0 !");
      close(obj->fd);
    }
   	
	obj->fd = open (obj->devicename, O_RDONLY);
	if (obj->fd == -1)
	{
		FEATUM_ERR_FATAL (0,"Cannot open device %s\n",obj->devicename);
		_FUNCTION_RETURN_(0);
	}
	
	ioctl (obj->fd, SNDCTL_DSP_RESET, 0);
    ioctl (obj->fd, SNDCTL_DSP_GETBLKSIZE, &(obj->blkSize));
    FEATUM_DEBUG(5,"audio blocksize: %i",obj->blkSize);
    // TODO: adjust obj buffer size to this value...?
    
	if (obj->blkSize < 4 || obj->blkSize > 65535)
	{
		FEATUM_ERR_FATAL (1, "Invalid audio buffer size\n");
		_FUNCTION_RETURN_(0);
	}
    	
	int bits = obj->parameters.nBPS*8;
	if (ioctl (obj->fd, SNDCTL_DSP_SAMPLESIZE, &(bits)) < 0)
	{
		FEATUM_ERR_FATAL (1, "nBits = %i not available (?)\n",bits);
		_FUNCTION_RETURN_(0);
    }
	
	if (obj->parameters.nChan == 2) {
		int tmp = 1;
		if (ioctl (obj->fd, SNDCTL_DSP_STEREO, &tmp) < 0) {
          FEATUM_ERR_FATAL (1, "failed setting stereo (2ch) on audio device!\n");
		  _FUNCTION_RETURN_(0);
        }
	} else if (obj->parameters.nChan == 1) {
		int tmp = 0;
		if (ioctl (obj->fd, SNDCTL_DSP_STEREO, &tmp) < 0) {
          FEATUM_ERR_FATAL (1, "failed setting mono (1ch) on audio device!\n");
		  _FUNCTION_RETURN_(0);
        }
    } else {
      FEATUM_ERR_FATAL (0, "Only channels = 1 | 2 is currently supported with /dev/dsp interface!\n");
      _FUNCTION_RETURN_(0);
    }

    if (ioctl (obj->fd, SNDCTL_DSP_SPEED, &(obj->parameters.sampleRate)) < 0) {
      FEATUM_ERR_FATAL (1, "failed setting sample rate to %i!\n",obj->parameters.sampleRate);
	  _FUNCTION_RETURN_(0);
    }

	if (ioctl (obj->fd, SNDCTL_DSP_SYNC, NULL) < 0)
	{
		FEATUM_ERR_FATAL (1, "Unable to sync dsp\n");
		_FUNCTION_RETURN_(0);
	}

    // TODO: record timestamp here:
    obj->absTimeStart = liveInput_getTime(); 
    
    obj->streamStatus = LI_STREAM_STARTED;
    _FUNCTION_RETURN_(1);
}
#undef FUNCTION

int liveInput_stopRecording( pLiveInput obj ) 
#define FUNCTION "liveInput_stopRecording"
{_FUNCTION_ENTER_
 
  if (obj != NULL) {
    if (obj->fd > 0) close(obj->fd); 
    obj->fd = -1;
    obj->streamStatus =  LI_STREAM_STOPPED;
    // TODO: timestamp!
    obj->absTimeStop = liveInput_getTime(); 

    _FUNCTION_RETURN_(1);        
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

int liveInput_getBlkSize( pLiveInput obj )
#define FUNCTION "liveInput_getBlkSize"
{_FUNCTION_ENTER_

  if (obj->fd == -1) {
              
    obj->fd = open (obj->devicename, O_RDONLY);
    if (obj->fd == -1)
    {
      FEATUM_ERR_FATAL (0,"Cannot open device %s\n",obj->devicename);
      _FUNCTION_RETURN_(0);
    }
	
    ioctl (obj->fd, SNDCTL_DSP_RESET, 0);
    ioctl (obj->fd, SNDCTL_DSP_GETBLKSIZE, &(obj->blkSize));
    FEATUM_DEBUG(5,"audio blocksize: %i",obj->blkSize);
    // TODO: adjust obj buffer size to this value...?
    
    if (obj->blkSize < 4 || obj->blkSize > 65535)
    {
      FEATUM_ERR_FATAL (1, "Invalid audio buffer size\n");
      _FUNCTION_RETURN_(0);
    }
  
    close(obj->fd);
    obj->fd = -1;
    _FUNCTION_RETURN_( obj->blkSize );
    
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


#endif // USE_PORTAUDIO (else)

int liveInput_listDevices(void)
#define FUNCTION "liveInput_listDevices"
{_FUNCTION_ENTER_

  #ifdef USE_PORTAUDIO
  PaError err = Pa_Initialize();
  if( err != paNoError ) {
    FEATUM_ERROR_FATAL(0,"error initialising portaudio library (code %i)\n",err);
    _FUNCTION_RETURN_(0);
  }
     
  // query devices:
  int numDevices;

  numDevices = Pa_GetDeviceCount();
  FEATUM_MESSAGE(0, "There are %i audio devices:\n", numDevices );
  if( numDevices <= 0 )
  {
     printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
     err = numDevices;
     _FUNCTION_RETURN_(0);
  }

  const PaDeviceInfo *deviceInfo;
  printf("  The default device is #%i\n",Pa_GetDefaultInputDevice());

  int i;
  for( i=0; i<numDevices; i++ )
  {
     deviceInfo = Pa_GetDeviceInfo( i );
     printf("  -> Device #%i: \"%s\"\n       #inputChan=%i #outputChan=%i\n",i,deviceInfo->name, deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels); fflush(stdout);
  }
  
  err = Pa_Terminate();
  if( err != paNoError ) {
    FEATUM_ERROR(2,"PortAudio error: %s\n", Pa_GetErrorText( err ) );
  }
   
  #else
  FEATUM_MESSAGE(0,"List of devices not supported for OSS sound interface");
   _FUNCTION_RETURN_(0);
  #endif
  _FUNCTION_RETURN_(1);
}
#undef FUNCTION

pLiveInput liveInput_create( pLiveInput obj, const char *devicename, int deviceId, LONG_IDX maxBuf, int samplerate, int sampleType, int channels, int status)
#define FUNCTION "liveInput_create"
{_FUNCTION_ENTER_

     LONG_IDX bufsize;
     int alloced = 0;
     
     #ifndef USE_PORTAUDIO
     if (devicename == NULL) {
       FEATUM_ERR_FATAL(0,"devicename == NULL (defaulting to /dev/dsp) !");
       devicename = "/dev/dsp";
     }
     #endif
     
     #ifdef USE_PORTAUDIO
     PaError err = Pa_Initialize();
     if( err != paNoError ) {
        FEATUM_ERROR_FATAL(0,"error initialising portaudio library (code %i)\n",err);
        _FUNCTION_RETURN_(0);
     }
     #endif

     if (obj == NULL) {
       obj = (pLiveInput)calloc(1,sizeof(sLiveInput)); 
       if (obj != NULL) alloced = 1;
     } else {
       //_FUNCTION_RETURN_(obj);
       memzero(obj,sizeof(sLiveInput));
     }
     
     if (obj == NULL) { 
       FEATUM_ERR_FATAL(0,"error allocating object for pLiveInput");
       _FUNCTION_RETURN_(NULL);      
     }

     #ifndef USE_PORTAUDIO
     obj->fd = -1;
     liveInput_getBlkSize( obj );
     // ??
     //if (maxBuf > obj->blkSize) maxBuf = obj->blkSize;
     //if (maxBuf < 2*obj->blkSize) maxBuf = 2*obj->blkSize;
     #endif
     
     #ifdef USE_PORTAUDIO
     if (deviceId < 0) {
       deviceId = Pa_GetDefaultInputDevice();
     }
     const PaDeviceInfo *deviceInfo;
     deviceInfo = Pa_GetDeviceInfo( deviceId );
     if ((deviceInfo != NULL)&&(deviceInfo->name != NULL)) {
       strncpy((char *)&(obj->devicename),deviceInfo->name,sizeof(obj->devicename));
       FEATUM_MESSAGE(2,"Recording from device \"%s\" [id=%i]",deviceInfo->name, deviceId);
     }
     #else
     strncpy((char *)&(obj->devicename),devicename,sizeof(obj->devicename));
     #endif
     obj->parameters.sampleType = sampleType; // pcmBitsToSampleType( bits );
     obj->parameters.nBPS = pcmSampleTypeToNBPS( obj->parameters.sampleType );
     obj->parameters.sampleRate = samplerate;
     obj->parameters.nChan = channels;
     obj->parameters.blockSize = channels * obj->parameters.nBPS;
     obj->parameters.byteOrder = BYTEORDER_LE;    // TODO ... (host byte order?)
     obj->parameters.nBlocks = -1;
     FEATUM_DEBUG(4,"nChan=%i nBPS=%i sampleRate=%i  bits=%i\n",obj->parameters.nChan, obj->parameters.nBPS, obj->parameters.sampleRate, pcmSampleTypeToBits(sampleType));

     /*
     switch(obj->parameters.nBPS) {
       case 2: sampleType = SAMPLETYPE_I16; break;
       case 1: sampleType = SAMPLETYPE_I8; break;
       case 3: sampleType = SAMPLETYPE_I24; break;
       case 4: sampleType = SAMPLETYPE_FP; break;  //??
       default:
          FEATUM_ERR_FATAL(0,"bytes per sample value of %i not supported!",obj->parameters.nBPS);               
          if (alloced) free(obj);
          _FUNCTION_RETURN_( NULL );
     }
     obj->parameters.sampleType = sampleType;
     */
     FEATUM_DEBUG(10,"max buf (blocks) set by calling application: %i",maxBuf);

     #ifdef USE_PORTAUDIO
     // force minimal buffering for portaudio callback system
     if (maxBuf < 200) maxBuf = 200;
     obj->paFrames = maxBuf/4 ; //* obj->parameters.blockSize; // this value is in bytes
     printf("paFrames: %i  (maxBufblocks: %i)\n",obj->paFrames,maxBuf);
     #endif
     
     // allocate wave buffer
     if (maxBuf <= 0) { 
       obj->bufferSizeBytes = 0;
       obj->bufferSizeBlocks = 0;
     } else {
       obj->bufferSizeBlocks = maxBuf; 
       obj->bufferSizeBytes = obj->bufferSizeBlocks * (obj->parameters.blockSize);
     }

     FEATUM_DEBUG(10,"max buf (bytes) : %i",obj->bufferSizeBytes);

     if (obj->bufferSizeBytes != 0) {
       pcmBufferAllocate( &(obj->buffer), obj->bufferSizeBlocks, obj->parameters.nChan, 
                                sampleType, MEMORGA_INTERLV, obj->parameters.sampleRate);
     } else { // if maxBuf is set to zero: disable buffering
       obj->buffer.nBlocksAlloc = 0;       
       obj->buffer.nBytesAlloc = 0;
       obj->buffer.data = NULL;
       obj->buffer.sampleType = sampleType;
       obj->buffer.memOrga = MEMORGA_INTERLV;
       obj->buffer.sampleRate = obj->parameters.sampleRate;
       obj->buffer.nChan = obj->parameters.nChan;
       obj->buffer.nBPS = obj->parameters.nBPS;
     }
     obj->nReadTotal = 0;
     obj->nSkipped = 0;
     
     obj->idxWr = 0; obj->idxRd = 0;
     obj->idxWarpedWr = 0; obj->idxWarpedRd = 0;

     
     if (status == 1) {
       // open device:
       if (!liveInput_startRecording(obj)) {
         FEATUM_ERROR_FATAL(1,"failed to start audio recording!");
         pcmBufferFree(&(obj->buffer),1);
         if (alloced) free(obj);
         _FUNCTION_RETURN_(NULL); 
       }

       #ifndef USE_PORTAUDIO    // only do this for the /dev/dsp interface ...
       // load audio data to fill buffer (if buffer is enabled)
       if (obj->buffer.nBlocksAlloc >0) {
         FEATUM_DEBUG(10,"number of blocks TO read into wave ring-buffer: %i",obj->buffer.nBlocksAlloc);
         if (!liveInput_readRawDataFromDevice( obj, &(obj->buffer), obj->buffer.nBlocksAlloc )) {
           FEATUM_ERROR_FATAL(0,"Error reading data from device %s",devicename);
           if (alloced) free(obj);
           if (obj->fd > 0) close(obj->fd);
           _FUNCTION_RETURN_( NULL );
         }
         obj->idxWr = obj->buffer.nBlocks;
       }
       FEATUM_DEBUG(10,"number of blocks read into wave ring-buffer: %i",obj->buffer.nBlocks);
       #endif
     }
     
     _FUNCTION_RETURN_( obj );
}
#undef FUNCTION


int liveInput_setStatus( pLiveInput obj, int status )
#define FUNCTION "liveInput_setStatus"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    if ((status == 0)&&(obj->streamStatus == LI_STREAM_STARTED)) {
       FEATUM_DEBUG(6,"stopping recording (status set to %i) [%i]",status,obj->streamStatus);
       if (!liveInput_stopRecording(obj)) {
         FEATUM_ERROR_FATAL(1,"failed to stop audio recording!");
         _FUNCTION_RETURN_(0); 
       }
    } else if ((status == 1)&&(obj->streamStatus == LI_STREAM_STOPPED)) { // start recording
       FEATUM_DEBUG(6,"starting recording (status set to %i) [%i]",status,obj->streamStatus);
       if (!liveInput_startRecording(obj)) {
         FEATUM_ERROR_FATAL(1,"failed to start audio recording!");
         _FUNCTION_RETURN_(0); 
       }
       FEATUM_DEBUG(6,"started recording (status set to %i) [%i]",status,obj->streamStatus);
    }
    _FUNCTION_RETURN_( 1 );
  }

  _FUNCTION_RETURN_( 0 );
}
#undef FUNCTION


long liveInput_getSampleRate( pLiveInput obj )
#define FUNCTION "liveInput_getSampleRate"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    _FUNCTION_RETURN_( obj->parameters.sampleRate );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


LONG_IDX liveInput_secondsToSamples( pLiveInput obj, FLOAT_TYPE se )
#define FUNCTION "liveInput_secondsToSamples"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    _FUNCTION_RETURN_( (LONG_IDX) ( (FLOAT_TYPE)obj->parameters.sampleRate * se ) );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

FLOAT_TYPE liveInput_samplesToSeconds( pLiveInput obj, LONG_IDX sa )
#define FUNCTION "liveInput_samplesToSeconds"
{_FUNCTION_ENTER_

  if (obj != NULL) {
     _FUNCTION_RETURN_ ( (FLOAT_TYPE)sa / (FLOAT_TYPE)obj->parameters.sampleRate );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION




// allocates a pcmBuffer Object with same properties as wave file and returns pointer to it
// start and length in blocks
pPcmBuffer liveInput_readRawDataFromDevice( pLiveInput obj, pPcmBuffer dataread, LONG_IDX length )
#define FUNCTION "liveInput_readRawDataFromDevice"
{_FUNCTION_ENTER_

  #ifdef USE_PORTAUDIO
  FEATUM_WARNING(1,"readRawDataFromDevice is not supported with portaudio interface, you must use the buffering\n");
  _FUNCTION_RETURN_(0);
  #else

  LONG_IDX nRead = -1;
  char *buf; char *buf0;
  LONG_IDX i,j,n;
  
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if ((length <= 0)&&(dataread==NULL)) _FUNCTION_RETURN_(NULL);
    #endif
    
    if (dataread == NULL)
      dataread = pcmBufferAllocate( dataread, length, obj->parameters.nChan, obj->parameters.sampleType, MEMORGA_INTERLV, obj->parameters.sampleRate );
    else {
      if (dataread->nBlocksAlloc <= 0) { _FUNCTION_RETURN_(NULL); }
      dataread->nChan = obj->parameters.nChan;
      dataread->nBPS = obj->parameters.nBPS;
      dataread->memOrga = MEMORGA_INTERLV;
      dataread->blockSize = obj->parameters.blockSize;
      dataread->chanSize = obj->parameters.nBPS;
      length = dataread->nBlocksAlloc;
    } 
    
    if (dataread == NULL) {
      FEATUM_ERR_FATAL(0,"Error allocating PCM buffer");
      _FUNCTION_RETURN_(NULL);
    }

    if (dataread->nBlocksAlloc < length) {
      FEATUM_WARNING(6,"pcmBuffer is smaller than it should be (nBlocksAlloc<length!)\n");
      length = dataread->nBlocksAlloc;
    }
    
    if (length > 0) {
      FEATUM_DEBUG(11,"TO_read(bytes) = %i",dataread->blockSize*length);
      nRead = read(obj->fd, dataread->data, dataread->blockSize*length);
      FEATUM_DEBUG(11,"nRead = %i",nRead);
    }
    if (nRead >= 0) {
      dataread->nBlocks = abs(nRead / (dataread->blockSize));
    } else { dataread->nBlocks = 0; }
         
    _FUNCTION_RETURN_(dataread); 
  }
  _FUNCTION_RETURN_(NULL);
  #endif

}
#undef FUNCTION


int liveInput_reloadBuffer( pLiveInput obj ) 
#define FUNCTION "liveInput_reloadBuffer"
{_FUNCTION_ENTER_

  FEATUM_DEBUG(11,"idxRd: %i  idxWr: %i\n",obj->idxRd, obj->idxWr);
  LONG_IDX toread = obj->bufferSizeBlocks - (obj->idxWr-obj->idxRd);
  if (toread < 0) {
     FEATUM_ERROR_FATAL(0,"toread == 0, something is seriously wrong with the liveInput buffer!");          
     _FUNCTION_RETURN_(0);
  }
  FEATUM_DEBUG(11,"toread: %i\n",toread);

  if (toread > 0) {
  
    #ifdef USE_PORTAUDIO
    int status = 0;
    // check for maxBufferSize > 0, then do NOT call liveInput_readRawDataFromDevice, 
    // instead wait for buffer to be filled by callback and then return...
    obj->bufUpdate = 0;
    obj->rbLock = 0;
    status = Pa_IsStreamActive( obj->stream );
    //if (status != 1) Pa_Sleep(1);
    int cnt = 0;
    while ((obj->bufUpdate == 0)&&(status==1) &&(cnt<200) ) { //&&( status == 1 )
      Pa_Sleep(1);
      FEATUM_DEBUG(9,"wait for data loop...%i\n",cnt);
      status = Pa_IsStreamActive( obj->stream );
      cnt++;
    }
    if (cnt>=200) {
      FEATUM_ERROR(2,"timeout (cnt=200) while waiting for data from portaudio device...");  
      _FUNCTION_RETURN_(0);             
    }
    if (status != 1) {
      FEATUM_ERROR(6,"portaudio input stream broken (no more active) (status=%i)!\n",status);
      _FUNCTION_RETURN_(0);
    }
    while (obj->rbLock) {
      Pa_Sleep(1);
    }
    obj->rbLock = 1;
    _FUNCTION_RETURN_(1);  
    #else
    
    pPcmBuffer tmpBuf = NULL;
    tmpBuf = liveInput_readRawDataFromDevice( obj, NULL, toread );
    liveInput_writeToRingBuffer( obj, tmpBuf ) ;
    pcmBufferFree(tmpBuf,0);

    #endif
  }

  _FUNCTION_RETURN_(1);
}
#undef FUNCTION



/* gets data from current position, data must be allocated by calling function
   data->nBlocksAlloc blocks will be read, if possible
   after reading in the data, data->nBlocks will be set to the actual number of blocks
   read
*/
int liveInput_getDataSequential( pLiveInput obj, pPcmBuffer data ) 
#define FUNCTION "liveInput_getDataSequential"
{_FUNCTION_ENTER_

  int i;
  
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (data == NULL) _FUNCTION_RETURN_(0);
    #endif
    // TODO: check EOF!
    
    FEATUM_DEBUG(11,"idxWr: %i  idxRd: %i  nReadTotal: %i  nSkipped: %i  bufSize: %i",obj->idxWr,obj->idxRd,obj->nReadTotal,obj->nSkipped, obj->bufferSizeBlocks);
    FEATUM_DEBUG(9,"data->nBlocksAlloc=%i, data->nBlocks=%i",data->nBlocksAlloc, data->nBlocks);
    
    if (obj->bufferSizeBlocks == 0) { // read directly from file if buffer is disabled
      #ifndef USE_PORTAUDIO
      data = liveInput_readRawDataFromDevice( obj, data, 0 );
      int ret = 0;
      if (data != NULL) {
        obj->nReadTotal += data->nBlocks;
        ret = data->nBlocks; 
      }
      _FUNCTION_RETURN_(ret);
      #else
      FEATUM_ERROR_FATAL(0,"Buffersize >0 is required for portaudio recording!");
      _FUNCTION_RETURN_(0);
      #endif
    } else {
      
      LONG_IDX tocopy = data->nBlocksAlloc-data->nBlocks;
      LONG_IDX copied = 0;
     
      if (tocopy > 0) {
      #ifdef USE_PORTAUDIO
      while (obj->rbLock) {
        Pa_Sleep(1);
        FEATUM_DEBUG(9,"waiting for infinity...");
      }
      obj->rbLock = 1;
      #endif

      do {
        // copy part of data that is available in the buffer, try to copy as much as possible
        copied += liveInput_rb_copy(obj,data,tocopy-copied);
        if (data->nBlocks < tocopy) {  // reload
          FEATUM_DEBUG(10,"reload during copy (missing=%i)",tocopy-data->nBlocks);
          if (liveInput_reloadBuffer(obj) == 0) _FUNCTION_RETURN_(0);
        }
        FEATUM_DEBUG(11,"tocopy=%i, data->nBlocks=%i",tocopy,data->nBlocks);
      } while(data->nBlocks < tocopy);

      #ifdef USE_PORTAUDIO
      obj->rbLock = 0;
      #endif
      }
    
      _FUNCTION_RETURN_(tocopy);
    }
  } else {
    _FUNCTION_RETURN_(0);       
  }
}
#undef FUNCTION

inline int liveInput_getDataSequential_gen( void *obj, pPcmBuffer data ) 
  { liveInput_getDataSequential( (pLiveInput)obj, data ); }


/* Destructor: frees the class data object and returns NULL */
pLiveInput liveInput_destroy( pLiveInput obj )
#define FUNCTION "liveInput_destroy"
{_FUNCTION_ENTER_
/*     if (obj != NULL) {
       // close devicehandle (if open...)
       liveInput_stopRecording(obj);
       
       #ifdef USE_PORTAUDIO
       // maybe wait for buffers to be flushed here...?
       
       PaError err = Pa_Terminate();
       if( err != paNoError ) {
         FEATUM_ERROR(2,"PortAudio error: %s\n", Pa_GetErrorText( err ) );
       }
       pcmBufferFree(obj->bufWrapper,0);
       #endif
       // free internal buffer:
       pcmBufferFree(&(obj->buffer),1);
       printf("  liveInput: dropped %i blocks\n",obj->nSkipped);
       free(obj);
     }*/
     liveInput_destroyData(obj);
     if (obj != NULL) free(obj);
     _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION


/* Destructor: frees the class data object and returns NULL */
pLiveInput liveInput_destroyData( pLiveInput obj )
#define FUNCTION "liveInput_destroyData"
{_FUNCTION_ENTER_
     if (obj != NULL) {
       // close devicehandle (if open...)
       liveInput_stopRecording(obj);
       
       #ifdef USE_PORTAUDIO
       // maybe wait for buffers to be flushed here...?
       
       PaError err = Pa_Terminate();
       if( err != paNoError ) {
         FEATUM_ERROR(2,"PortAudio error: %s\n", Pa_GetErrorText( err ) );
       }
       obj->bufWrapper = pcmBufferFree(obj->bufWrapper,0);
       #endif
       // free internal buffer:
       pcmBufferFree(&(obj->buffer),1);
       printf("  liveInput: dropped %i blocks\n",obj->nSkipped);
//       free(obj);
     }
     _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION

/*********** private functions ********************/

// copy FROM ringbuffer into pcmBuffer (starting at position skip there) and update all indicies
int liveInput_rb_copy( pLiveInput obj, pPcmBuffer data, LONG_IDX lng )
#define FUNCTION "rb_copy"
{_FUNCTION_ENTER_

  LONG_IDX avail = obj->idxWr-obj->idxRd;
  LONG_IDX tocopy;
  LONG_IDX skip = data->nBlocks;
  if (avail < lng) {
    FEATUM_WARNING(10,"less blocks available in buffer than requested (%i < $i)",avail,lng);
    tocopy = avail;
  }
  else tocopy = lng;
  
  if (tocopy > 0) {
  #ifdef COMPILE_CHECKS
  if (skip >= data->nBlocksAlloc) _FUNCTION_RETURN_(0);
  if (lng > data->nBlocksAlloc) _FUNCTION_RETURN_(0);
  #endif
  
  obj->buffer.nBlocks = obj->buffer.nBlocksAlloc;
  FEATUM_DEBUG(9,"avail=%i, lng_requested=%i,  tocopy=%i, skip = %i\n",avail, lng, tocopy,skip);
  obj->idxWarpedRd = obj->idxRd % obj->bufferSizeBlocks;
  if (obj->bufferSizeBlocks - obj->idxWarpedRd >= tocopy) { // one step copy
    obj->idxRd += pcmBuffer_copy(&(obj->buffer), data, obj->idxWarpedRd, obj->idxWarpedRd+tocopy, skip);
    FEATUM_DEBUG(11,"1step data->nBlocks=%i",data->nBlocks);
  } else {  // two step copy (warping over ringbuffer end)
    LONG_IDX tmp = pcmBuffer_copy(&(obj->buffer), data, obj->idxWarpedRd, obj->bufferSizeBlocks, skip);
    obj->idxRd += tmp;
    FEATUM_DEBUG(11,"2step  A: data->nBlocks=%i",data->nBlocks);
    obj->idxRd += pcmBuffer_copy(&(obj->buffer), data, 0, tocopy - tmp, skip+tmp);
    FEATUM_DEBUG(11,"2step  B: data->nBlocks=%i",data->nBlocks);
  }
  obj->idxWarpedRd = obj->idxWr % obj->bufferSizeBlocks;
  
  }
  _FUNCTION_RETURN_(tocopy);
}
#undef FUNCTION


#endif  // LIVE_REC
