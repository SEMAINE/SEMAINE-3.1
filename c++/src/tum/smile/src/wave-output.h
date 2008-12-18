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
 
#ifndef __WAVE_OUTPUT_H
#define __WAVE_OUTPUT_H

#include "pcm-data.h"
#include "wave-common.h"

#ifdef __WINDOWS
#define USE_FOPEN 1
#endif

#include <unistd.h>
#include <inttypes.h>

/* WAVE Header struct, valid only for PCM Files */
typedef struct {
  uint32_t	Riff;    /* Must be little endian 0x46464952 (RIFF) */
  uint32_t	FileSize;
  uint32_t	Format;  /* Must be little endian 0x45564157 (WAVE) */
  
  uint32_t	SubchunkFmtID;  /* Must be little endian 0x20746D66 (fmt ) */
  uint32_t	Subchunk1Size;
  uint16_t	AudioFormat;
  uint16_t	NumChannels;
  uint32_t	SampleRate;
  uint32_t	ByteRate;
  uint16_t	BlockAlign;
  uint16_t	BitsPerSample;
  
  uint32_t  SubchunkTimeID;  /* Must be little endian 0x656D6974 (time) */
  uint32_t  SubchunkTimeSize; /* Must be 16 */
  uint32_t  startSec;
  uint32_t  startUSec;
  uint32_t  stopSec;
  uint32_t  stopUSec;

  uint32_t	SubchunkDataID;  /* Must be little endian 0x61746164 (data) */
  uint32_t  SubchunkDataSize;
} sRiffWaveHeader_Out;

/* The class data structure */
typedef struct {
  char filename[PATH_STRING_SIZE+1];
  #ifdef USE_FOPEN
  FILE *fileHandle;
  #else
  int fileHandle;   
  #endif
  sWaveParameters parameters;
  sRiffWaveHeader_Out header;
  
  uint32_t startSec, startUSec;
  uint32_t stopSec, stopUSec;
  
  int isStream;
} sWaveOutput;
typedef sWaveOutput* pWaveOutput;


/* Constructor: creates a class data object 
   maxBuf is the max. size of the internal wave buffer
   if the file is smaller than bufsize, only the amount of mem required to store
   the file will be allocated
   Set to -1 to always store the complete wave file in memory (DANGEROUS!)
    ->Rather use a high number, e.g. 158 760 000 (for 1h of 44.1khz)
*/
// usage: pClassName testObj = NULL;
//        testObj = testClass_create( testObj, ... );
pWaveOutput waveOutput_create( pWaveOutput obj, const char *filename, long sampleRate, int sampleType, int channels );

void waveOutput_setStreaming( pWaveOutput obj, int isStream );

LONG_IDX waveOutput_secondsToSamples( pWaveOutput obj, double se );
double waveOutput_samplesToSeconds( pWaveOutput obj, LONG_IDX sa );

/* get Data from an arbitrary position (start, in samples, rel. to file begin)
   if the data is not in the memory buffer, it will be read directly from disk
   no buffering will be performed
*/
int waveOutput_writeDataRandom( pWaveOutput obj, pPcmBuffer data, LONG_IDX start );

/* gets data from current position, data must be allocated by calling function
   data->nSamp must be initialized with the number of samples to read
   after reading in the data, nSamp will be set to the actual number of samples
   read
*/
int waveOutput_writeDataSequential( pWaveOutput obj, pPcmBuffer data );

/* Destructor: frees the class data object and returns NULL */
pWaveOutput waveOutput_destroy( pWaveOutput obj );
pWaveOutput waveOutput_destroyData( pWaveOutput obj );

#endif // #ifndef __WAVE_OUTPUT_H
