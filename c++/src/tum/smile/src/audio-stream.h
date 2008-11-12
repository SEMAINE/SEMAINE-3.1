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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Audio stream wrapper class
  ==========================
  
This data provides a unique interface for reading audio streams
One Method is provided to read data:
    audioStream_getData()
    
Setting up the source is done in the constructor, either:
        by passing a method pointer to a standardised getframe method
        or by specifying type and and object of source
        
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef __AUDIO_STREAM_H
#define __AUDIO_STREAM_H

#include "pcm-data.h"

#define TYPE_WAVEINPUT 1    // PCM wave file reader
#define TYPE_TCPIP     2    // TCPIP pcm stream
#define TYPE_STREAM    3    // generic stream ?
#define TYPE_GENERIC   0    // function pointer interface
#define TYPE_INPUTFRAMER 9
#define TYPE_WAVEOUTPUT 11
#define TYPE_LIVEINPUT 10   // Live recorder (currently only /dev/dsp recorder for linux)

/* The class data structure */
typedef struct {
  int type;
  int nChan;
  int sampleType;
  int memOrga;
  int nBPS;
  long int sampleRate;
  int (*getframe_method)(void *, pPcmBuffer);
  void *inputObj;
} sAudioStream;
typedef sAudioStream* pAudioStream;

pAudioStream audioStream_create(void *input, int type);
pAudioStream audioStream_createGeneric(void *input, int type, 
                 int (*getframe_method)(void *, pPcmBuffer), int nChan, int sampleType, int memOrga, int sampleRate, int nBPS);

int audioStream_getData( pAudioStream obj, pPcmBuffer data );

LONG_IDX audioStream_secondsToSamples( pAudioStream obj, FLOAT_TYPE se );
FLOAT_TYPE audioStream_samplesToSeconds( pAudioStream obj, LONG_IDX sa );

pAudioStream audioStream_destroy(pAudioStream obj);

#endif // __AUDIO_STREAM_H
