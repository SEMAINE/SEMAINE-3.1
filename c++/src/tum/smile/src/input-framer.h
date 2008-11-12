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
  Input framer class
  ==================
  
this class reads data from a wave file or stream object
and returns frames of variable size

multiple streams will be supported (i.e. stereo/mono, each channel processed 
seperately)

the main function is:
+ getFrame  
      which outputs frames from the buffer and reads in new data if the buffer
      is empty

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef __INPUT_FRAMER_H
#define __INPUT_FRAMER_H

#include "audio-stream.h"
#include "pcm-process.h"    // required for windowing functions
#include "feature-memory.h"

#define MAX_CLIENTS 10      // max number of "clients" , i.e. different parallel frameSize/frameStep configurations

typedef struct {
//  LONG_IDX   readIdx;
  LONG_IDX   frameLength;
  LONG_IDX   frameStep;
  int        windowFunc;
  FLOAT_TYPE preEmphasis;
  WORK_SAMPLE * preEmphasis_status;
  int        id;
} sInputFramerClient;
typedef sInputFramerClient* pInputFramerClient;

/* The class data structure */
typedef struct {
  pAudioStream input;
  int nChan, sampleType, memOrga, nBPS;
  long int sampleRate;
  
  pPcmRingBufferMR buffer;
  int maxFrameLength;

  int nClients;
  pInputFramerClient client[MAX_CLIENTS];
  int  clientLUT[MAX_CLIENTS+1];  // look-up table for id->arrayindex

  LONG_IDX masterWriteIdx;
  int mfHalf;   // which half of the master frame is currently being read from

} sInputFramer;
typedef sInputFramer* pInputFramer;


//int inputFramer_setupFeatureMemLevels( pInputFramer framer, pLLDs llds, pFeatureMemory mem, int *framerIDs, LONG_IDX *capacity )

// create the class with input pAudioStream <input> (not NULL!!)
pInputFramer inputFramer_create(pAudioStream input);
pInputFramer inputFramer_create2(pInputFramer obj, pAudioStream input);

// sets the audio format, the data read from input should be converted to before returning it via getFrame
// if nothing has been set, the input format will be used. 
// set a value to -1 to not change it and use the default already set in the inputFramer object
void inputFramer_setAudioParameters(pInputFramer obj, int nChan, int sampleType, int memOrga);

LONG_IDX inputFramer_getClient_frameStep(pInputFramer obj, int id);
LONG_IDX inputFramer_getClient_frameLength(pInputFramer obj, int id);

// add a framer client with parameters framesize (in seconds) and overlap (in percent)
// powOf2 specifies whether the nearest power of 2 framesize should be chosen
// instead of the strict values (overlap will then be adjusted to ensure the same framestep)
int inputFramer_addClientSecOvl(pInputFramer obj, float framesize, float overlap, int powOf2);

// add a framer client with parameters framesize (in seconds) and framestep (in seconds)
// powOf2 specifies whether the nearest power of 2 framesize should be chosen
// instead of the strict values (framestep will remain unaffected)
int inputFramer_addClientSecStep(pInputFramer obj, float framesize, float framestep, int powOf2);

// set windowing function for client <id>
int inputFramer_setWindowing(pInputFramer obj, int id, int type);

// set preEmphasis filter coefficient :  s(k) = s(k) - preEmphasis * s(k-1)
//  set to 0.0 to disable preEmphasis filter
int inputFramer_setPreEmphasis(pInputFramer obj, int id, FLOAT_TYPE preEmphasis);


// add a 'client'
int inputFramer_addClient(pInputFramer obj, int framesize, int frameoverlap);

// delete a 'client' (always use it when no longer interested in one framesize. it will ensure that getFrame doesn't block)
int inputFramer_deleteClient(pInputFramer obj, int id);

// after all clients have been added, initialize the buffer with this method:
// int inputFramer_initBuffer(pInputFramer obj);
// -> this is now done automatically when getFrame is called

// MAIN method, get frame for client ID
// NOTE: the memory must be freed by the application!? experimental: mem will be freed at EOF of input
// *frame must contain a vaild (not NULL) pointer to a pPcmBuffer pointer that must be initialized with NULL
int inputFramer_getFrame(pInputFramer obj, int id, pPcmBuffer *frame);

// free only the inputFramer class
pInputFramer inputFramer_destroy(pInputFramer obj);
pInputFramer inputFramer_destroyData(pInputFramer obj);
// also free input audio stream object
pInputFramer inputFramer_destroyWithInput(pInputFramer obj);

#endif // __INPUT_FRAMER_H
