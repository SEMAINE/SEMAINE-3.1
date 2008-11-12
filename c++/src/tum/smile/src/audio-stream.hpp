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

#ifndef __AUDIO_STREAM_HPP
#define __AUDIO_STREAM_HPP

extern "C" {
  #include "audio-stream.h"
}
#include "wave-common.hpp"
#include "pcm-data.hpp"

#define AUDIOSTREAM_READ 1;
#define AUDIOSTREAM_WRITE 2;

class cAudioStream {
  
  protected:
    cWaveParameters *param;
    void *inputObj;
    int streamDirection;
    int streamType;
    
  public:
    pAudioStream _pdata;
    
    cAudioStream() {param = NULL; _pdata = NULL; };

    virtual void createAudioStream() { }

    virtual void createAudioStream(void *input) 
    {
      _pdata = audioStream_create( input, streamType );
 
    }

    // this is for compatibility with ANSI C implementation
    virtual void createAudioStream(void *input, int type, const cWaveParameters *_param, int (*getframe_method)(void *, pPcmBuffer)) 
    {
      if (param == NULL) return;
      _pdata = audioStream_createGeneric( input, type, 
                 getframe_method, _param->nChan(), _param->sampleType(), _param->memOrga(), _param->sampleRate(), _param->nBPS());
    }
    
    virtual int readFrame( cPcmBuffer &data ) = 0;
    virtual int writeFrame( cPcmBuffer &data ) = 0;

    long getSampleRate() { if (param) return param->sampleRate(); }
    LONG_IDX secondsToSamples( FLOAT_TYPE se )  
      { if (param) return ( (LONG_IDX) ( (FLOAT_TYPE)param->sampleRate() * se ) ); };
    FLOAT_TYPE samplesToSeconds( LONG_IDX sa )
      { if (param) return ( (FLOAT_TYPE)sa / (FLOAT_TYPE)param->sampleRate() ); };
    
    virtual ~cAudioStream() { 
      if (_pdata != NULL) audioStream_destroy(_pdata); 
      if (param != NULL) delete param;
    };
    
};


/*
class cAudioStream {
  sAudioStream _data;
  
  public:

    cAudioStream(cAudioSource input, int type) {
      audioStream_create2(input, int type);
    }
    
pAudioStream audioStream_createGeneric(void *input, int type, 
                 int (*getframe_method)(void *, pPcmBuffer), int nChan, int sampleType, int memOrga, int sampleRate, int nBPS);

int audioStream_getData( pAudioStream obj, pPcmBuffer data );

LONG_IDX audioStream_secondsToSamples( pAudioStream obj, FLOAT_TYPE se );
FLOAT_TYPE audioStream_samplesToSeconds( pAudioStream obj, LONG_IDX sa );

pAudioStream audioStream_destroy(pAudioStream obj);
         
}
*/

#endif // __AUDIO_STREAM_HPP
