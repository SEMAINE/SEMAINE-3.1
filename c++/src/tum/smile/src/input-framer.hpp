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

#ifndef __INPUT_FRAMER_HPP
#define __INPUT_FRAMER_HPP

extern "C" {
  #include "input-framer.h"       
}
#include "audio-stream.hpp"

//typedef sInputFramer* pInputFramer;

class cInputFramer: public cAudioStream {
  int defaultId;
 
  public:
    sInputFramer _data;
    cAudioStream &_input;
    
    cInputFramer( cAudioStream &input ) : cAudioStream(), _input(input) {
      streamType = TYPE_INPUTFRAMER;
      streamDirection = AUDIOSTREAM_READ;
      input.createAudioStream(  ); // input._pdata
//          virtual void createAudioStream(void *input, int type, const cWaveParameters *param, int (*getframe_method)(void *, pPcmBuffer)) 
      if (input._pdata != NULL) {
        inputFramer_create2( &_data, input._pdata ); 
      }
      _input = input;
      defaultId = 0;
    }
    

// sets the audio format, the data read from input should be converted to before returning it via getFrame
// if nothing has been set, the input format will be used. 
// set a value to -1 to not change it and use the default already set in the inputFramer object
    inline void setAudioParameters(cWaveParameters &param) {
      inputFramer_setAudioParameters(&_data, param.nChan(), param.sampleType(), param.memOrga());
    }
    inline void setAudioParameters(int nChan, int sampleType, int memOrga) {
      inputFramer_setAudioParameters(&_data, nChan, sampleType, memOrga);
    }

    inline LONG_IDX getClientFrameStep(int id) {
      return inputFramer_getClient_frameStep(&_data, id);
    }
    inline LONG_IDX getClientFrameLength(int id) {
      return inputFramer_getClient_frameLength(&_data, id);
    }

    // add a framer client with parameters framesize (in seconds) and overlap (in percent)
    // powOf2 specifies whether the nearest power of 2 framesize should be chosen
    // instead of the strict values (overlap will then be adjusted to ensure the same framestep)
    inline int addClientSecOvl(float framesize, float overlap, int powOf2) {
      return inputFramer_addClientSecOvl(&_data, framesize, overlap, powOf2);
    }
    inline int addClientSecOvl(float framesize, float overlap) {
      return inputFramer_addClientSecOvl(&_data, framesize, overlap, 0);
    }
    
    // add a framer client with parameters framesize (in seconds) and framestep (in seconds)
    // powOf2 specifies whether the nearest power of 2 framesize should be chosen
    // instead of the strict values (framestep will remain unaffected)
    inline int addClientSecStep(float framesize, float framestep, int powOf2) {
      return inputFramer_addClientSecStep(&_data, framesize, framestep, powOf2);
    }
    inline int addClientSecStep(float framesize, float framestep) {
      return inputFramer_addClientSecStep(&_data, framesize, framestep, 0);
    }
    
    // set windowing function for client <id>
    inline int setWindowing(int id, int type) {
      return inputFramer_setWindowing(&_data, id, type);
    }

    // set preEmphasis filter coefficient :  s(k) = s(k) - preEmphasis * s(k-1)
    //  set to 0.0 to disable preEmphasis filter
    inline int setPreEmphasis(int id, FLOAT_TYPE preEmphasis) {
      return inputFramer_setPreEmphasis(&_data, id, preEmphasis);
    }




    // add a 'client'
    inline int addClient(int framesize, int frameoverlap) {
      return inputFramer_addClient(&_data, framesize, frameoverlap);
    }

    // delete a 'client' (always use it when no longer interested in one framesize. it will ensure that getFrame doesn't block)
    inline int deleteClient(int id) {
      return inputFramer_deleteClient(&_data, id);
    }

    // after all clients have been added, initialize the buffer with this method:
    // int inputFramer_initBuffer(pInputFramer obj);
    // -> this is now done automatically when getFrame is called
    virtual int readFrame( cPcmBuffer &buf ) {
      sPcmBuffer *tmp = &(buf._data);            
      return inputFramer_getFrame(&_data, defaultId, &tmp);
    }
    virtual int writeFrame( cPcmBuffer &buf ) { return 0; }

    // MAIN method, get frame for client ID
    // NOTE: the memory must be freed by the application!? experimental: mem will be freed at EOF of input
    // *frame must contain a vaild (not NULL) pointer to a pPcmBuffer pointer that must be initialized with NULL
    inline int getFrame(int id, cPcmBuffer **frame) {
      if (*frame == NULL) {
        if (_data.client != NULL) {
          *frame = new cPcmBuffer(_data.client[id]->frameLength, _data.nChan, 
                        _data.sampleType , _data.memOrga, _data.sampleRate );
        }
      }
      sPcmBuffer *tmp = &((*frame)->_data);
      int ret = inputFramer_getFrame(&_data, id, &tmp);
      if (tmp->nBlocks == 0) {
        if (*frame != NULL) { delete *frame; *frame = NULL; }
      }
      return ret;
    }

    
    ~cInputFramer() {
      inputFramer_destroyData(&_data);  // should we destroy with input?
    }
               
};


// free only the inputFramer class
//pInputFramer inputFramer_destroy(pInputFramer obj);
// also free input audio stream object
//pInputFramer inputFramer_destroyWithInput(pInputFramer obj);

#endif // __INPUT_FRAMER_HPP
