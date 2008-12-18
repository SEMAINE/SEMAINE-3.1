/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
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

 /*  IMPLEMENTATION of the class <waveOutput>
     See the corresponding header file (.h) for documentation!  */
#define MODULE "wave-output"
#include "featum_common.h"

 // this file contains the function definitions and the private class data

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WINDOWS
#include <string.h>  // for memcpy function
#endif 

#include "featum_util.h"       //( optional )
#include "wave-output.h"
#include "pcm-data.h"

#undef FUNCTION

// private functions:
//int copy_buffer(pPcmBuffer src, pPcmBuffer dst, int start, int end, int skip);
//int waveFile_readHeader( int file, pWaveParameters p );
//pPcmBuffer waveInput_readRawDataFromFile( pWaveInput obj, pPcmBuffer dataread, LONG_IDX start, LONG_IDX length );
//int rb_copy( pWaveInput obj, pPcmBuffer data, LONG_IDX lng );

// enable streaming functionality (i.e. do not attempt to seek on stream to write wave header)
void waveOutput_setStreaming( pWaveOutput obj, int isStream )
{
  if (obj != NULL) {
    obj->isStream = isStream;
  }
}


pWaveOutput waveOutput_create( pWaveOutput obj, const char *filename, long sampleRate, int sampleType, int channels )
#define FUNCTION "waveOutput_create"
{_FUNCTION_ENTER_
     LONG_IDX bufsize;
     int alloced; 

     if (filename == NULL) {
       FEATUM_ERR_FATAL(0,"filename == NULL, no filename given!");
       _FUNCTION_RETURN_(NULL);
     }
     
     if (obj == NULL) {
       obj = (pWaveOutput)calloc(1,sizeof(sWaveOutput));
     } else {
       memzero(obj,sizeof(sWaveOutput));
     }
        
     if (obj == NULL) { 
       FEATUM_ERR_FATAL(0,"error allocating object memory for pWaveOutput");
       _FUNCTION_RETURN_( NULL );   
     }
     alloced = 1;
     
     strncpy((char *)&(obj->filename),filename,sizeof(obj->filename));
     obj->parameters.sampleRate = sampleRate;
     obj->parameters.nChan = channels;
     obj->parameters.byteOrder = BYTEORDER_LE;    // TODO ... (host byte order?)
     obj->parameters.nBlocks = 0;
     obj->parameters.sampleType = sampleType; // pcmBitsToSampleType(bits, BYTEORDER_LE);
     obj->parameters.nBPS = pcmSampleTypeToNBPS(obj->parameters.sampleType);
     obj->parameters.blockSize = channels * obj->parameters.nBPS;

     FEATUM_DEBUG(4,"nChan=%i nBPS=%i sampleRate=%i  (bits=%i)\n",obj->parameters.nChan, obj->parameters.nBPS, obj->parameters.sampleRate, pcmSampleTypeToBits(sampleType));
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
     // open wave file:
     #ifdef USE_FOPEN
     obj->fileHandle = fopen(filename,"wb");
     if (obj->fileHandle == NULL) {
     #else
     obj->fileHandle = open(filename,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
     if (obj->fileHandle < 0) {
     #endif
       FEATUM_ERR_FATAL(0,"cannot open file %s for writing",filename);
       free(obj);
       _FUNCTION_RETURN_( NULL );   
     }
     
     // build initial header
     waveOutput_buildHeader( obj );

     // write dummy header:
     if (!waveOutput_writeHeader( obj )) {
       FEATUM_ERR_FATAL(0,"cannot write wave header to file %s!",filename);
       #ifdef USE_FOPEN
       if (obj->fileHandle != NULL) fclose(obj->fileHandle);
       #else
       if (obj->fileHandle > 0) close(obj->fileHandle);
       #endif
       _FUNCTION_RETURN_( NULL );   
     }
     
     _FUNCTION_RETURN_( obj );
}
#undef FUNCTION

LONG_IDX waveOutput_secondsToSamples( pWaveOutput obj, double se )
#define FUNCTION "waveOutput_secondsToSamples"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    _FUNCTION_RETURN_( (LONG_IDX) ( (double)obj->parameters.sampleRate * se ));
  }
  _FUNCTION_RETURN_(0);  
}
#undef FUNCTION

double waveOutput_samplesToSeconds( pWaveOutput obj, LONG_IDX sa )
#define FUNCTION "waveOutput_samplesToSeconds"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    _FUNCTION_RETURN_( ( (double)sa / (double)obj->parameters.sampleRate ) );
  }
  _FUNCTION_RETURN_(0);  
}
#undef FUNCTION

int waveOutput_setStartTime( pWaveOutput obj, uint32_t sec, uint32_t uSec)
#define FUNCTION "waveOutput_setStartTime"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    obj->startSec = sec;
    obj->startUSec = uSec;
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);  
}
#undef FUNCTION

int waveOutput_setStopTime( pWaveOutput obj, uint32_t sec, uint32_t uSec)
#define FUNCTION "waveOutput_setStopTime"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    obj->stopSec = sec;
    obj->stopUSec = uSec;
    _FUNCTION_RETURN_(1);
  }
  _FUNCTION_RETURN_(0);  
}
#undef FUNCTION


int waveOutput_writeDataRandom( pWaveOutput obj, pPcmBuffer data, LONG_IDX start )
#define FUNCTION "waveOutput_writeDataRandom"
{_FUNCTION_ENTER_
  FEATUM_ERROR_FATAL(0,"NOT YET IMPLEMENTED");
  _FUNCTION_RETURN_(0);  
}
#undef FUNCTION








/* gets data from current position, data must be allocated by calling function
   data->nBlocksAlloc blocks will be read, if possible
   after reading in the data, data->nBlocks will be set to the actual number of blocks
   read
*/
int waveOutput_writeDataSequential( pWaveOutput obj, pPcmBuffer data ) 
#define FUNCTION "waveOutput_writeDataSequential"
{_FUNCTION_ENTER_
  int i;
  
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (data == NULL) _FUNCTION_RETURN_(0);
    #endif
    FEATUM_DEBUG(8,"about to write to file... (sampleType = %i, buffer sampleType: %i)",obj->parameters.sampleType, data->sampleType);

    // TODO: check if conversion is necessary!!!
    
    // convert data to output format if necessary:
    pPcmBuffer out = pcmBufferAllocate(NULL, data->nBlocks, obj->parameters.nChan, 
                                       obj->parameters.sampleType, MEMORGA_INTERLV, 
                                       obj->parameters.sampleRate);
    if (out == NULL) {
      FEATUM_ERROR_FATAL(1,"pcm buffer could not be allocated!\n");
      _FUNCTION_RETURN_(0);        
    }
    pcmBuffer_copy(data,out,0,-1,0);
    LONG_IDX ret = 0;
    
    #ifdef USE_FOPEN
    FEATUM_DEBUG(8,"fopen/fwrite code...");
    if (obj->fileHandle != NULL) {
      // write data to file
      if (out->data != NULL) {
        FEATUM_DEBUG(8,"calling fwrite... nBlocks: %i", out->nBlocks);
        ret = fwrite((void*)(out->data),out->blockSize,out->nBlocks,obj->fileHandle);
        obj->parameters.nBlocks += ret;
      }
      if ( ret != data->nBlocks) {
        FEATUM_ERROR(2,"fewer blocks written than there should be.. %i < %i",ret,data->nBlocks);
        _FUNCTION_RETURN_(0);
      }
    } else { FEATUM_ERROR_FATAL(1,"file not open"); _FUNCTION_RETURN_(0); }
    #else
    if (obj->fileHandle >= 0) {
      // write data to file
      ret = write(obj->fileHandle, out->data, data->blockSize * data->nBlocks);
      obj->parameters.nBlocks += ret / data->blockSize;
      if (ret != data->blockSize * data->nBlocks) {
        FEATUM_ERROR(2,"fewer bytes written than there should be.. %i < %i",ret,data->blockSize * data->nBlocks);
        _FUNCTION_RETURN_(0);
      }
      ret /= data->blockSize;
    } else { FEATUM_ERROR_FATAL(1,"file not open"); _FUNCTION_RETURN_(0); }
    #endif
    
    pcmBufferFree(out,0);
    _FUNCTION_RETURN_(ret);
  } 
  _FUNCTION_RETURN_(0);
}
#undef FUNCTION


int waveOutput_buildHeader( pWaveOutput obj )
#define FUNCTION "waveOutput_buildHeader"
{_FUNCTION_ENTER_
  if (obj == NULL) _FUNCTION_RETURN_(0);

  FEATUM_DEBUG(5,"nChan=%i nBPS=%i sampleRate=%i bits=%i\n",obj->parameters.nChan, obj->parameters.nBPS, obj->parameters.sampleRate, pcmSampleTypeToBits( obj->parameters.sampleType ));

  obj->header.Riff = 0x46464952;  // RIFF
  obj->header.FileSize = sizeof(obj->header) + obj->parameters.nBlocks * obj->parameters.blockSize; 
  obj->header.Format = 0x45564157; // WAVE
  obj->header.SubchunkFmtID = 0x20746D66; // fmt
  obj->header.Subchunk1Size = 16;
  obj->header.AudioFormat = 1;  // pcm
  obj->header.NumChannels = obj->parameters.nChan;
  obj->header.SampleRate = obj->parameters.sampleRate;
  obj->header.ByteRate = obj->parameters.sampleRate * obj->parameters.blockSize;
  obj->header.BlockAlign = obj->parameters.blockSize;
  obj->header.BitsPerSample = pcmSampleTypeToBits( obj->parameters.sampleType );
  obj->header.SubchunkTimeID = 0x656D6974;  // time
  obj->header.SubchunkTimeSize = 16;
  obj->header.startSec = obj->startSec;
  obj->header.startUSec = obj->startUSec;
  obj->header.stopSec = obj->stopSec;
  obj->header.stopUSec = obj->stopUSec;
  obj->header.SubchunkDataID = 0x61746164; // data
  obj->header.SubchunkDataSize = obj->parameters.nBlocks * obj->parameters.blockSize;
  FEATUM_DEBUG(9,"sizeof HEADER: %i",sizeof(obj->header));
  FEATUM_DEBUG(9,"datasize: %i",obj->parameters.nBlocks * obj->parameters.blockSize);
  FEATUM_DEBUG(9,"blocksize: %i", obj->parameters.blockSize);
  _FUNCTION_RETURN_(1);       
}
#undef FUNCTION


int waveOutput_writeHeader( pWaveOutput obj )
#define FUNCTION "waveOutput_writeHeader"
{_FUNCTION_ENTER_
  if (obj == NULL) _FUNCTION_RETURN_(0);
  if (obj->isStream)  _FUNCTION_RETURN_(1);  // do not write wave header for streams
 
  #ifdef USE_FOPEN
  if (obj->fileHandle != NULL) {
    // write header
    long curPos = ftell(obj->fileHandle);
    fseek(obj->fileHandle, 0, SEEK_SET);
    if (fwrite(&(obj->header),sizeof(obj->header),1,obj->fileHandle) != 1) _FUNCTION_RETURN_(0);
    if (curPos != 0) // do not seek if header was written for the first time!
      fseek(obj->fileHandle, curPos, SEEK_SET);
  }
  #else
  if (obj->fileHandle >= 0) {
    // TODO: maybe use llseek?
    long curPos = lseek(obj->fileHandle, 0, SEEK_CUR);
    lseek(obj->fileHandle, 0, SEEK_SET);
    if (write(obj->fileHandle, &(obj->header), sizeof(obj->header)) != sizeof(obj->header)) _FUNCTION_RETURN_(0);
    if (curPos != 0) // do not seek if header was written for the first time!
      lseek(obj->fileHandle, curPos, SEEK_SET);
  }
  #endif
  _FUNCTION_RETURN_(1);       
}
#undef FUNCTION

/* Destructor: frees the class data object and returns NULL */
pWaveOutput waveOutput_destroy( pWaveOutput obj )
#define FUNCTION "waveOutput_destroy"
{_FUNCTION_ENTER_
     
     if (obj != NULL) {
       // update header (file lengths and close time)
       waveOutput_buildHeader(obj);
            
       // write header
       waveOutput_writeHeader(obj);
       
       // close filehandle (if open...)
       #ifdef USE_FOPEN
       if (obj->fileHandle != NULL) {
         fclose(obj->fileHandle);
       }
       #else
       if (obj->fileHandle >= 0) {
         close(obj->fileHandle);
       }
       #endif
       
       free(obj);
     }
         
     _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION


/* Destructor: frees the data in the class data object and returns NULL */
pWaveOutput waveOutput_destroyData( pWaveOutput obj )
#define FUNCTION "waveOutput_destroyData"
{_FUNCTION_ENTER_
     
     if (obj != NULL) {
       // update header (file lengths and close time)
       waveOutput_buildHeader(obj);
            
       // write header
       waveOutput_writeHeader(obj);
       
       // close filehandle (if open...)
       #ifdef USE_FOPEN
       if (obj->fileHandle != NULL) {
         fclose(obj->fileHandle);
         obj->fileHandle = NULL;
       }
       #else
       if (obj->fileHandle >= 0) {
         close(obj->fileHandle);
         obj->fileHandle = 0;
       }
       #endif
     }
         
     _FUNCTION_RETURN_(NULL);
}
#undef FUNCTION
