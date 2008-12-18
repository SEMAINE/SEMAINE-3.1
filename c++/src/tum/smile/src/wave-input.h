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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This class handles the reading of PCM WAVE files (RIFF format).
Other file types, such as compressed audio (MP3 or OGG) are not yet supported!

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __WAVE_INPUT_H
#define __WAVE_INPUT_H

#ifdef __WINDOWS
#define USE_FOPEN 1
#endif

#include "pcm-data.h"
#include "wave-common.h"

/*
#define BYTEORDER_LE 0
#define BYTEORDER_BE 1

typedef struct {
  long sampleRate;
  long nChan;
  long blockSize;
  long nBPS;      // bytes per sample 
  long byteOrder;  // BYTEORDER_LE or BYTEORDER_BE
  LONG_IDX nBlocks;
} sWaveParameters;
typedef sWaveParameters* pWaveParameters;
*/

/* The class data structure */
typedef struct {
  char filename[PATH_STRING_SIZE+1];
  #ifdef USE_FOPEN
  FILE *fileHandle;
  #else
  int fileHandle;   
  #endif
  sWaveParameters parameters;
  
  // max. and actual size of wave buffer in bytes, set to -1 to load full wave file
  LONG_IDX maxBufferSizeBytes;
  LONG_IDX maxBufferSizeBlocks;
  LONG_IDX fileSizeBytes;
  LONG_IDX fileSizeBlocks;
  
//  LONG_IDX postBuffer;
//  LONG_IDX preBuffer;
    
  // indicies in samples relative to wave file begin of data currently
  // available in buffer
  LONG_IDX idxBufStart, idxBufEnd;  
  // corresponding ring buffer indicies
  LONG_IDX idxCurStart, idxCurEnd;

  // physical position after file header (bytes)
  LONG_IDX idxFileBeginBytes; 
  // current physical file read position in bytes
  LONG_IDX curPosBytes;   

  // virtual file read position in blocks
  LONG_IDX curPosBlocks;   
  // corresponding ring buffer index (in blocks)
  LONG_IDX curPosBuffer;
  
  sPcmBuffer buffer;
  
} sWaveInput;
typedef sWaveInput* pWaveInput;


/* Constructor: creates a class data object 
   maxBuf is the max. size of the internal wave buffer
   if the file is smaller than bufsize, only the amount of mem required to store
   the file will be allocated
   Set to -1 to always store the complete wave file in memory (DANGEROUS!)
    ->Rather use a high number, e.g. 158 760 000 (for 1h of 44.1khz)
*/
// usage: pClassName testObj = NULL;
//        testObj = testClass_create( testObj, ... );
pWaveInput waveInput_create( pWaveInput obj, const char *filename, LONG_IDX maxBuf );  
                               //LONG_IDX preBuffer, LONG_IDX postBuffer );

long waveInput_getSampleRate( pWaveInput obj );
LONG_IDX waveInput_secondsToSamples( pWaveInput obj, FLOAT_TYPE se );
FLOAT_TYPE waveInput_samplesToSeconds( pWaveInput obj, LONG_IDX sa );

/* get Data from an arbitrary position (start, in samples, rel. to file begin)
   if the data is not in the memory buffer, it will be read directly from disk
   no buffering will be performed
*/
int waveInput_getDataRandom( pWaveInput obj, pPcmBuffer data, LONG_IDX start );

/* gets data from current position, data must be allocated by calling function
   data->nSamp must be initialized with the number of samples to read
   after reading in the data, nSamp will be set to the actual number of samples
   read
*/
int waveInput_getDataSequential( pWaveInput obj, pPcmBuffer data );
int waveInput_getDataSequential_gen( void *obj, pPcmBuffer data );
//  { waveInput_getDataSequential( (pWaveInput)obj, data ); }

// set file position in BLOCKS(!) 
// return value: file position in bytes seeked to (inkl. header...), 0 on error
int waveInput_setPos( pWaveInput obj, LONG_IDX pos );

/* Destructor: frees the class data object and returns NULL */
pWaveInput waveInput_destroy( pWaveInput obj );

/* Data Destructor: frees all data except for the class data object and returns NULL */
// this is called from the c++ wrapper functions, where the class data object is freed, when the c++ object is destroyed
pWaveInput waveInput_destroyData( pWaveInput obj );

#endif // #ifndef __WAVE_INPUT_H
