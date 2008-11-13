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
 
 /*  IMPLEMENTATION of the class <waveInput>
     See the corresponding header file (.h) for documentation!  */
 #define MODULE "wave-input"
 
 // this file contains the function definitions and the private class data

//!!!! TODO: check correct handling of obj->curPosBlocks !!! 
// so far it is only changed in rb_copy, which is not really the most approriate position
// probably it should be changed after n bytes have been READ from the file
// since curPosBlocks is the file block pointer and should have nothing to do with the ringbuffer


#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#ifndef __WINDOWS
#include <string.h>  // for memcpy function
#endif 

#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "wave-input.h"
#include "pcm-data.h"

#undef FUNCTION

#ifdef __cplusplus 
extern "C" {
#endif

// private functions:
//int copy_buffer(pPcmBuffer src, pPcmBuffer dst, int start, int end, int skip);
#ifdef USE_FOPEN
int waveFile_readHeader( FILE * file, pWaveParameters p );
#else
int waveFile_readHeader( int file, pWaveParameters p );
#endif

pPcmBuffer waveInput_readRawDataFromFile( pWaveInput obj, pPcmBuffer dataread, LONG_IDX start, LONG_IDX length );
int rb_copy( pWaveInput obj, pPcmBuffer data, LONG_IDX lng );

// TODO: check for preBuffer + postBuffer << maxBuf!!
pWaveInput waveInput_create( pWaveInput obj, const char *filename, LONG_IDX maxBuf)
                             //LONG_IDX preBuffer, LONG_IDX postBuffer )
#define FUNCTION "waveInput_create"
{_FUNCTION_ENTER_

     LONG_IDX bufsize;
     int alloced = 0;
     //int sampleType; 
     
     if (filename == NULL) {
       FEATUM_ERR_FATAL(0,"filename == NULL!");
       _FUNCTION_RETURN_(NULL);             
     }
     
     if (obj == NULL) {
       obj = (pWaveInput)calloc(1,sizeof(sWaveInput)); 
       if (obj != NULL) alloced = 1;
     } else {
       memzero(obj,sizeof(sWaveInput));
     }
     
     if (obj == NULL) { 
       FEATUM_ERR_FATAL(0,"error allocating object for pWaveInput");
       _FUNCTION_RETURN_(NULL);      
     }
     
     strncpy((char *)&(obj->filename),filename,sizeof(obj->filename));

     // open wave file:
     #ifdef USE_FOPEN
     obj->fileHandle = fopen(filename,"rb");
     if (obj->fileHandle == NULL) {
     #else
     obj->fileHandle = open(filename,O_RDONLY);
     if (obj->fileHandle < 0) {
     #endif
       FEATUM_ERR_FATAL(0,"cannot open input file %s",filename);
       free(obj);
       _FUNCTION_RETURN_( NULL );   
     }

     // read header
     if (!waveFile_readHeader( obj->fileHandle, &(obj->parameters) )) {
       if (alloced) free(obj);
       #ifdef USE_FOPEN
       if (obj->fileHandle != NULL) fclose(obj->fileHandle);
       #else
       if (obj->fileHandle > 0) close(obj->fileHandle);
       #endif
       _FUNCTION_RETURN_( NULL );
     }
        
     obj->fileSizeBlocks = obj->parameters.nBlocks;
     obj->fileSizeBytes = obj->parameters.nBlocks * obj->parameters.blockSize;

     // todo: check for maxBuf 0
     if (maxBuf < -1) maxBuf = -1;
     obj->maxBufferSizeBytes = maxBuf;

     FEATUM_DEBUG(10,"max buf (bytes) set by calling application: %i",maxBuf);
     // allocate wave buffer
     if ((obj->parameters.nBlocks * obj->parameters.blockSize
            < maxBuf) || (maxBuf == -1)) { // if total input is smaller than maxBuf,
                                           // then only allocate size of input for buffer
       obj->maxBufferSizeBytes = obj->parameters.nBlocks * obj->parameters.blockSize;
     }
     obj->maxBufferSizeBlocks = obj->maxBufferSizeBytes / (obj->parameters.blockSize);
     FEATUM_DEBUG(10,"max buf (bytes) after reading wave header: %i",obj->maxBufferSizeBytes);
          
/*     switch(obj->parameters.nBPS) {
       case 2: sampleType = SAMPLETYPE_I16; break;
       case 1: sampleType = SAMPLETYPE_I8; break;
       case 3: sampleType = SAMPLETYPE_I24; break;
       case 4: sampleType = SAMPLETYPE_FP; break;  //??
       default:
          FEATUM_ERR_FATAL(0,"bytes per sample value of %i not supported!",obj->parameters.nBPS);               
          if (alloced) free(obj);
          #ifdef USE_FOPEN
          if (obj->fileHandle != NULL) fclose(obj->fileHandle);
          #else
          if (obj->fileHandle > 0) close(obj->fileHandle);
          #endif
          _FUNCTION_RETURN_( NULL );
     }
     obj->parameters.sampleType = sampleType;
     */
 
     if (obj->maxBufferSizeBytes != 0) {
       pcmBufferAllocate( &(obj->buffer), obj->maxBufferSizeBlocks, obj->parameters.nChan, 
                                obj->parameters.sampleType, MEMORGA_INTERLV, obj->parameters.sampleRate);
     } else { // if maxBuf is set to zero: disable buffering, but set audio parameters
       obj->buffer.nBlocksAlloc = 0;       
       obj->buffer.nBytesAlloc = 0;
       obj->buffer.data = NULL;
       obj->buffer.sampleType = obj->parameters.sampleType;
       obj->buffer.memOrga = MEMORGA_INTERLV;
       obj->buffer.sampleRate = obj->parameters.sampleRate;
       obj->buffer.nChan = obj->parameters.nChan;
       obj->buffer.nBPS = obj->parameters.nBPS;
     }
     FEATUM_DEBUG(7,"sampleType = %i",obj->parameters.sampleType);
     
     #ifdef USE_FOPEN
     obj->curPosBytes = obj->idxFileBeginBytes = ftell(obj->fileHandle);
     #else
     obj->curPosBytes = obj->idxFileBeginBytes = lseek(obj->fileHandle, 0, SEEK_CUR);
     #endif
     obj->curPosBlocks = 0;
     
     // load audio data to fill buffer
     //waveInput_getDataRandom( pWaveInput obj, pPcmBuffer data, LONG_IDX start )
     obj->idxBufStart = 0; obj->idxCurStart = 0;
     FEATUM_DEBUG(10,"number of blocks TO read into wave ring-buffer: %i",obj->buffer.nBlocksAlloc);
     if (obj->buffer.nBlocksAlloc >0) {
     if (!waveInput_readRawDataFromFile( obj, &(obj->buffer), obj->idxBufStart, obj->buffer.nBlocksAlloc )) {
       FEATUM_ERROR_FATAL(0,"Error reading data from wave file %s",filename);
       if (alloced) free(obj);
       #ifdef USE_FOPEN
       if (obj->fileHandle != NULL) fclose(obj->fileHandle);
       #else
       if (obj->fileHandle > 0) close(obj->fileHandle);
       #endif
       _FUNCTION_RETURN_( NULL );
     }
     }
     obj->idxCurEnd = obj->idxBufEnd = obj->buffer.nBlocks;
     FEATUM_DEBUG(10,"number of blocks read into wave ring-buffer: %i",obj->buffer.nBlocks);
     
//     if ( obj->maxBufferSizeBytes/2 < obj->preBuffer + obj->postBuffer) {
//       FEATUM_WARNING(2,"wave-input: preBuffer+postBuffer are larger than maxBufferSize/2 ! clipping both values..." );
//       obj->preBuffer = obj->maxBufferSizeBlocks/4;
//       obj->postBuffer = obj->maxBufferSizeBlocks/4;      
//     } else {
//       obj->preBuffer = preBuffer / obj->parameters.blockSize;
//       obj->postBuffer = postBuffer / obj->parameters.blockSize;        
//     }

     obj->curPosBuffer = 0;
     
     _FUNCTION_RETURN_( obj );
}
#undef FUNCTION

long waveInput_getSampleRate( pWaveInput obj )
#define FUNCTION "waveInput_getSampleRate"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    _FUNCTION_RETURN_( obj->parameters.sampleRate );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

LONG_IDX waveInput_secondsToSamples( pWaveInput obj, FLOAT_TYPE se )
#define FUNCTION "waveInput_secondsToSamples"
{_FUNCTION_ENTER_

  if (obj != NULL) {
    _FUNCTION_RETURN_( (LONG_IDX) ( (FLOAT_TYPE)obj->parameters.sampleRate * se ) );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

FLOAT_TYPE waveInput_samplesToSeconds( pWaveInput obj, LONG_IDX sa )
#define FUNCTION "waveInput_samplesToSeconds"
{_FUNCTION_ENTER_

  if (obj != NULL) {
     _FUNCTION_RETURN_ ( (FLOAT_TYPE)sa / (FLOAT_TYPE)obj->parameters.sampleRate );
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


/* get Data from an arbitrary position (start, in samples, rel. to file begin)
   if the data is not in the memory buffer, it will be read directly from disk
   no buffering will be performed
*/
int waveInput_getDataRandom( pWaveInput obj, pPcmBuffer data, LONG_IDX start )
#define FUNCTION "waveInput_getDataRandom"
{_FUNCTION_ENTER_

  pPcmBuffer dataread = NULL;
  
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (data == NULL) _FUNCTION_RETURN_(0);
    #endif
    
    // read from file
    dataread = waveInput_readRawDataFromFile(obj, NULL, start, data->nBlocksAlloc);
    if (dataread == NULL) _FUNCTION_RETURN_(0);
    int nread = dataread->nBlocks;
        
    // copy and (possibly) convert data
    pcmBuffer_copy(dataread, data, 0, dataread->nBlocks, 0);
    pcmBufferFree(dataread, 0);
    
    // reset previous current position?
    //lseek(obj->fileHandle, obj->curPosBytes, SEEK_SET);
    // TODO: error handling
    _FUNCTION_RETURN_(nread);
  } else {
    _FUNCTION_RETURN_(0);
  }
  
}
#undef FUNCTION


int waveInput_setPos( pWaveInput obj, LONG_IDX pos )
#define FUNCTION "waveInput_readRawDataFromFile"
{_FUNCTION_ENTER_

  if (obj != NULL) {

    if (pos < 0) pos = 0;
    obj->curPosBlocks = pos;
  
    long a;
    #ifdef USE_FOPEN
    a = fseek(obj->fileHandle, obj->curPosBlocks * (obj->parameters.blockSize) + obj->idxFileBeginBytes, SEEK_SET);
    if (a != -1) a = obj->curPosBlocks * (obj->parameters.blockSize) + obj->idxFileBeginBytes;
    #else
    a = lseek(obj->fileHandle, obj->curPosBlocks * (obj->parameters.blockSize) + obj->idxFileBeginBytes, SEEK_SET);
    #endif

    _FUNCTION_RETURN_(a);
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


// allocates a pcmBuffer Object with same properties as wave file and returns pointer to it
// start and length in blocks
pPcmBuffer waveInput_readRawDataFromFile( pWaveInput obj, pPcmBuffer dataread, LONG_IDX start, LONG_IDX length )
#define FUNCTION "waveInput_readRawDataFromFile"
{_FUNCTION_ENTER_

  LONG_IDX nRead = -1;
  char *buf; char *buf0;
  LONG_IDX i,j,n;
  
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (start < 0) start = 0;
    if (length <= 0) _FUNCTION_RETURN_(NULL);
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
      dataread->sampleType = obj->parameters.sampleType;
    } 
    
    
    if (dataread == NULL) {
      FEATUM_ERR_FATAL(0,"Error allocating PCM buffer");
      _FUNCTION_RETURN_(NULL);
    }

    FEATUM_DEBUG(9,"Seeking to file position %i",start * (obj->parameters.blockSize) + obj->idxFileBeginBytes);
    long int a;
    //TODO: maybe use llseek ???
    #ifdef USE_FOPEN
    a = fseek(obj->fileHandle, start * (obj->parameters.blockSize) + obj->idxFileBeginBytes, SEEK_SET);
    if (a != -1) a = start * (obj->parameters.blockSize) + obj->idxFileBeginBytes;
    #else
    a = lseek(obj->fileHandle, start * (obj->parameters.blockSize) + obj->idxFileBeginBytes, SEEK_SET);
    #endif
    if (a  == -1) {
      perror("lseek");
      FEATUM_ERROR(0,"Error seeking to file position %ll",start * (obj->parameters.blockSize) + obj->idxFileBeginBytes);
      pcmBufferFree(dataread,0);
      _FUNCTION_RETURN_(NULL);
    };
    FEATUM_DEBUG(10,"Seekd to file position %i",a);
    
    if (dataread->nBlocksAlloc < length) length = dataread->nBlocksAlloc;
    
    if (length > 0) {
      FEATUM_DEBUG(11,"TO_read(bytes) = %i",dataread->blockSize*length);
      #ifdef USE_FOPEN
      nRead = fread( dataread->data, 1, dataread->blockSize*length, obj->fileHandle);
      #else
      nRead = read(obj->fileHandle, dataread->data, dataread->blockSize*length);
      #endif
      FEATUM_DEBUG(11,"nRead = %i",nRead);
    }
    if (nRead >= 0) {
      dataread->nBlocks = abs(nRead / (dataread->blockSize));
    } else { dataread->nBlocks = 0; }
         
    _FUNCTION_RETURN_(dataread); 
  }
  _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION




int waveInput_reloadBuffer( pWaveInput obj ) 
#define FUNCTION "waveInput_reloadBuffer"
{_FUNCTION_ENTER_

  // read all that has to be read
//  FEATUM_DEBUG(11,"curposblocks: %i -- pb: %i\n",obj->curPosBlocks,obj->preBuffer);
  FEATUM_DEBUG(11,"curposblocks: %i\n",obj->curPosBlocks);
//  LONG_IDX toread = obj->maxBufferSizeBlocks - obj->preBuffer - (obj->idxBufEnd - obj->curPosBlocks); // - obj->postBuffer;
  LONG_IDX toread = obj->maxBufferSizeBlocks  - (obj->idxBufEnd - obj->curPosBlocks); // - obj->postBuffer;

  FEATUM_DEBUG(11,"toread: %i\n",toread);
  pPcmBuffer tmpBuf = NULL;
  if (toread > 0) {
    tmpBuf = waveInput_readRawDataFromFile( obj, NULL, obj->curPosBlocks, toread );
      
  #ifdef COMPILE_CHECKS
  if (tmpBuf == NULL) {
    FEATUM_ERR_FATAL(0,"Error reading data from file or allocating memory!\n");
    _FUNCTION_RETURN_(0);           
  }
  if (tmpBuf->nBlocks == 0) {
    FEATUM_ERR_FATAL(5,"Zero blocks read from wave-file!!\n");
    _FUNCTION_RETURN_(0);           
  }
  #endif
  
  LONG_IDX copied = 0;
  LONG_IDX readblocks = tmpBuf->nBlocks;
  FEATUM_DEBUG(11,"readblocks = %i (to copy)",readblocks);  
  FEATUM_DEBUG(12,"obj->maxBufferSizeBlocks = %i , obj->buffer.nBlocksAlloc = %i", obj->maxBufferSizeBlocks, obj->buffer.nBlocksAlloc);      

  FEATUM_DEBUG(12,"obj->idxCurEnd = %i",obj->idxCurEnd);
  // copy first part into buffer
  // from obj->idxCurEnd to maxBufferSizeBlocks
  if (obj->idxCurEnd < obj->maxBufferSizeBlocks) {
    if (obj->maxBufferSizeBlocks - obj->idxCurEnd < tmpBuf->nBlocks) { // one step copy
      copied = pcmBuffer_copy(tmpBuf, &(obj->buffer), 0, readblocks , obj->idxCurEnd);
      FEATUM_DEBUG(11,"1step copied = %i",copied);
    } else { 
      copied = pcmBuffer_copy(tmpBuf, &(obj->buffer), 0, obj->maxBufferSizeBlocks - obj->idxCurEnd , obj->idxCurEnd);
      FEATUM_DEBUG(11,"2step copied S1 = %i",copied);
    }
    obj->idxCurEnd += copied;
  }

  FEATUM_DEBUG(12,"(after step1 copy) obj->idxCurEnd = %i",obj->idxCurEnd);
  // copy second part into buffer
  // from 0 to ((obj->idxCurStart??)) obj->idxCurEnd-1
  if ((obj->idxCurEnd-1 >= 0)&&(readblocks-copied > 0)) {
    copied = pcmBuffer_copy(tmpBuf, &(obj->buffer), copied, readblocks, 0 );
    FEATUM_DEBUG(11,"2step copied S2 = %i",copied);
    //obj->idxCurEnd = copied;
  }
//  obj->idxCurStart = obj->idxCurEnd;
  pcmBufferFree(tmpBuf,0);
      
  // stupid correction
  obj->buffer.nBlocks = obj->buffer.nBlocksAlloc;
            
  obj->idxBufStart = obj->idxBufEnd; // - obj->preBuffer;
  obj->idxBufEnd += readblocks;    
  obj->idxCurEnd =   obj->idxBufEnd % obj->maxBufferSizeBlocks;
  obj->idxCurStart =   obj->idxBufStart % obj->maxBufferSizeBlocks;
  FEATUM_DEBUG(12,"(at end) obj->idxCurEnd = %i",obj->idxCurEnd);
  }

  _FUNCTION_RETURN_(1);
}
#undef FUNCTION



/* gets data from current position, data must be allocated by calling function
   data->nBlocksAlloc blocks will be read, if possible
   after reading in the data, data->nBlocks will be set to the actual number of blocks
   read
*/
int waveInput_getDataSequential( pWaveInput obj, pPcmBuffer data ) 
#define FUNCTION "waveInput_getDataSequential"
{_FUNCTION_ENTER_

  int i;
  
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (data == NULL) _FUNCTION_RETURN_(0);
    #endif
    // TODO: check EOF!
    // TODO: handle disabled buffering (maxBuf == 0) 
    
    FEATUM_DEBUG(11,"idxBufEnd: %i  idxBufStart: %i  curPosBlocks: %i  curPosBuffer: %i\n",obj->idxBufEnd,obj->idxBufStart,obj->curPosBlocks,obj->curPosBuffer);
    FEATUM_DEBUG(11,"idxCurEnd: %i  idxCurStart: %i  maxBufSize: %i\n",obj->idxCurEnd,obj->idxCurStart,obj->maxBufferSizeBlocks);    
    FEATUM_DEBUG(9,"data->nBlocksAlloc=%i, data->nBlocks=%i",data->nBlocksAlloc, data->nBlocks);
    
    if (obj->maxBufferSizeBlocks == 0) { // read directly from file if buffer is disabled
      int ret = waveInput_getDataRandom( obj, data, obj->curPosBlocks );
      obj->curPosBlocks += ret;
      _FUNCTION_RETURN_(ret);
//??      waveInput_readRawDataFromFile( obj, data, obj->curPosBlocks, LONG_IDX length )
      // TODO: check if obj->curPosBlocks has to be increased after read!
    }

    LONG_IDX tocopy = data->nBlocksAlloc-data->nBlocks;
    LONG_IDX copied = 0;
    
    if (tocopy > 0) {
      do {
        // copy part of data that is available in the buffer, try to copy as much as possible
        copied += rb_copy(obj,data,tocopy-copied);
        if (data->nBlocks < tocopy) {  // reload
          FEATUM_DEBUG(10,"reload during copy (missing=%i)",tocopy-data->nBlocks);
          if (waveInput_reloadBuffer(obj) == 0) _FUNCTION_RETURN_(0);
        }
        FEATUM_DEBUG(11,"tocopy=%i, data->nBlocks=%i",tocopy,data->nBlocks);
      } while(data->nBlocks < tocopy);
    }
    
    _FUNCTION_RETURN_(tocopy);
      
  } else {
    _FUNCTION_RETURN_(0);       
  }
}
#undef FUNCTION


inline int waveInput_getDataSequential_gen( void *obj, pPcmBuffer data ) 
  { waveInput_getDataSequential( (pWaveInput)obj, data ); }


/* Destructor: frees the class data object and returns NULL */
pWaveInput waveInput_destroy( pWaveInput obj )
#define FUNCTION "waveInput_destroy"
{_FUNCTION_ENTER_

     if (obj != NULL) {
       // close filehandle (if open...)
       #ifdef USE_FOPEN
       if (obj->fileHandle != NULL) fclose(obj->fileHandle);
       #else
       if (obj->fileHandle >= 0) close(obj->fileHandle);
       #endif
       
       // free internal buffer:
       pcmBufferFree(&(obj->buffer),1);
     
       free(obj);
     }
         
     _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION

/* Destructor: frees all data except for the class data object and returns NULL */
pWaveInput waveInput_destroyData( pWaveInput obj )
#define FUNCTION "waveInput_destroyData"
{_FUNCTION_ENTER_

     if (obj != NULL) {
       // close filehandle (if open...)
       #ifdef USE_FOPEN
       if (obj->fileHandle != NULL) { fclose(obj->fileHandle); obj->fileHandle = NULL; }
       #else
       if (obj->fileHandle >= 0) { close(obj->fileHandle); obj->fileHandle = 0; }
       #endif
       
       // free internal buffer:
       pcmBufferFree(&(obj->buffer),1);
     }
         
     _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION


/*********** private functions ********************/

#include <unistd.h>
#include <inttypes.h>

/* WAVE Header struct, valid only for PCM Files */
typedef struct {
  uint32_t	Riff;    /* Must be little endian 0x46464952 (RIFF) */
  uint32_t	FileSize;
  uint32_t	Format;  /* Must be little endian 0x45564157 (WAVE) */
  
  uint32_t	Subchunk1ID;  /* Must be little endian 0x20746D66 (fmt ) */
  uint32_t	Subchunk1Size;
  uint16_t	AudioFormat;
  uint16_t	NumChannels;
  uint32_t	SampleRate;
  uint32_t	ByteRate;
  uint16_t	BlockAlign;
  uint16_t	BitsPerSample;
  
  uint32_t	Subchunk2ID;  /* Must be little endian 0x61746164 (data) */
  uint32_t  Subchunk2Size;
} sRiffPcmWaveHeader;

typedef struct {
  uint32_t SubchunkID;
  uint32_t SubchunkSize;
} sRiffChunkHeader;

#ifdef USE_FOPEN
int waveFile_readHeader( FILE * file, pWaveParameters p )
#else
int waveFile_readHeader( int file, pWaveParameters p )
#endif
#define FUNCTION "waveFile_readHeader"
{_FUNCTION_ENTER_

  int nRead;
  sRiffPcmWaveHeader head;
  sRiffChunkHeader chunkhead;
  int safetytimeout = 20;  // max <safetytimeout> chunks of 99kb size before 'data' chunk
  
  #ifdef USE_FOPEN
  if ((p != NULL)&&(file != NULL)) {
    nRead = fread(&head, 1, sizeof(head), file);
  #else
  if ((p != NULL)&&(file >= 0)) {
    nRead = read(file, &head, sizeof(head));
  #endif
    if (nRead != sizeof(head)) return 0;
    /* Check for valid header , TODO: support other endianness */
	if ((head.Riff != 0x46464952) ||
		(head.Format != 0x45564157) ||
		(head.Subchunk1ID != 0x20746D66) ||
//		(head.Subchunk2ID != 0x61746164) ||
		(head.AudioFormat != 1) ||
		(head.Subchunk1Size != 16)) {
                            printf("%x\n",head.Riff);
                            printf("%x\n",head.Format);
                            printf("%x\n",head.Subchunk1ID);
                            printf("%x\n",head.Subchunk2ID);
                            printf("%x\n",head.AudioFormat);
                            printf("%x\n",head.Subchunk1Size);

                            FEATUM_ERR_FATAL(0,"bogus wave/riff header or file in wrong format!");
                            return 0;
        }
    while ((head.Subchunk2ID != 0x61746164)&&(safetytimeout>0)) { // 'data'
      // keep searching for 'data' chunk:
      if (head.Subchunk2Size < 99999) {
        char * tmp = malloc(head.Subchunk2Size);
        #ifdef USE_FOPEN
        nRead = fread(tmp, 1, head.Subchunk2Size, file);
        #else
        nRead = read(file, tmp, head.Subchunk2Size);
        #endif
        if (nRead != head.Subchunk2Size) return 0;
        free(tmp);
      } else { 
        FEATUM_ERR_FATAL(0,"WAVE input: Subchunk2Size > 99999\n");
        return 0; 
      }
      #ifdef USE_FOPEN
      nRead = fread(&chunkhead, 1, sizeof(chunkhead), file);
      #else
      nRead = read(file, &chunkhead, sizeof(chunkhead));
      #endif
      head.Subchunk2ID = chunkhead.SubchunkID;
      head.Subchunk2Size = chunkhead.SubchunkSize;
      safetytimeout--;
    }
    if (safetytimeout <= 0) {
      FEATUM_ERR_FATAL(0,"No data subchunk found in wave-file (read first %i chunks)!!\n",safetytimeout);
      return 0;                       
    }
    p->sampleRate = head.SampleRate;
    p->nChan = head.NumChannels;
    p->nBPS = head.BlockAlign/head.NumChannels;
//    p->bits = head.BitsPerSample;
    FEATUM_DEBUG(7,"bits per sample = %i",head.BitsPerSample);
    p->sampleType = pcmBitsToSampleType( head.BitsPerSample, BYTEORDER_LE, 0 );
/*
    if (head.NumChannels * head.BitsPerSample / 8 != head.BlockAlign) {
      FEATUM_ERR_FATAL(0,"Error reading wave file header: head.BlockAlign != head.NumChannels * head.BitsPerSample / 8");                     
      return 0;
    }
    */
    p->nBlocks = head.Subchunk2Size / head.BlockAlign;
    p->blockSize = head.BlockAlign;
    
    // TODO: ??
    p->byteOrder = BYTEORDER_LE;
    p->memOrga = MEMORGA_INTERLV;
    
    _FUNCTION_RETURN_(1); // success
  }
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION

// copy FROM ringbuffer into pcmBuffer (starting at position skip there) and update all indicies
int rb_copy( pWaveInput obj, pPcmBuffer data, LONG_IDX lng )
#define FUNCTION "rb_copy"
{_FUNCTION_ENTER_

  LONG_IDX avail = obj->idxBufEnd - obj->curPosBlocks;
  LONG_IDX tocopy;
  LONG_IDX skip = data->nBlocks;
  if (avail < lng) tocopy = avail;
  else tocopy = lng;
  
  #ifdef COMPILE_CHECKS
  if (skip >= data->nBlocksAlloc) _FUNCTION_RETURN_(0);
  if (lng > data->nBlocksAlloc) _FUNCTION_RETURN_(0);
  #endif
  
  obj->buffer.nBlocks = obj->buffer.nBlocksAlloc;
  FEATUM_DEBUG(9,"avail = %i, lng =%i,  tocopy = %i, skip = %i\n",avail, lng, tocopy,skip);
  if (obj->maxBufferSizeBlocks - obj->curPosBuffer >= tocopy) { // one step copy
    pcmBuffer_copy(&(obj->buffer), data, obj->curPosBuffer, obj->curPosBuffer+tocopy, skip);
    FEATUM_DEBUG(11,"1step data->nBlocks=%i",data->nBlocks);
  } else {  // two step copy (warping over ringbuffer end)
    LONG_IDX tmp = pcmBuffer_copy(&(obj->buffer), data, obj->curPosBuffer, obj->maxBufferSizeBlocks, skip);
    FEATUM_DEBUG(11,"2step  A: data->nBlocks=%i",data->nBlocks);
    pcmBuffer_copy(&(obj->buffer), data, 0, tocopy - tmp, skip+tmp);
    FEATUM_DEBUG(11,"2step  B: data->nBlocks=%i",data->nBlocks);
  }
  obj->curPosBlocks += tocopy;
  obj->curPosBuffer = obj->curPosBlocks % obj->maxBufferSizeBlocks;
  _FUNCTION_RETURN_(tocopy);
}
#undef FUNCTION



#ifdef __cplusplus 
}
#endif

