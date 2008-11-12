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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This file is a C++ wrapper for wave-input.c
It provides a C++ object with the same functionality as the C object!

This class handles the reading of PCM WAVE files (RIFF format).
Other file types, such as compressed audio (MP3 or OGG) are not yet supported!

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef __WAVE_INPUT_HPP
#define __WAVE_INPUT_HPP


#ifdef __WINDOWS
#define USE_FOPEN 1
#endif

extern "C" {
  #include "wave-input.h"
}
#include "audio-stream.hpp"

class cWaveInput: public cAudioStream {
  sWaveInput _data; 

  public:
    cWaveInput( const char *filename, LONG_IDX maxBuf ) : cAudioStream() {
      streamType = TYPE_WAVEINPUT;
      waveInput_create( &(_data), filename, maxBuf );
      // TODO: error handling!
      param = new cWaveParameters(_data.parameters);
      streamDirection = AUDIOSTREAM_READ;
    }
   
    // create legacy ANSI C audioStream object
    virtual void createAudioStream()
    {
      cAudioStream::createAudioStream((void*)&_data, TYPE_WAVEINPUT, param,   waveInput_getDataSequential_gen ); //int (*getframe_method)(void *, pPcmBuffer)) 
    }
    
    //long getSampleRate() { return waveInput_getSampleRate( &(_data) ); }
    
/*
    LONG_IDX secondsToSamples( FLOAT_TYPE se ) 
      { return waveInput_secondsToSamples( &(_data), se ); }
    
    FLOAT_TYPE samplesToSeconds( LONG_IDX sa ) 
      { return waveInput_samplesToSeconds( &(_data), sa ); }
*/

    /* get Data from an arbitrary position (start, in samples, rel. to file begin)
       if the data is not in the memory buffer, it will be read directly from disk
       no buffering will be performed
     */
    int getDataRandom( cPcmBuffer &data, LONG_IDX start ) 
      { int ret = waveInput_getDataRandom( &(_data), &(data._data), start ); 
        return ret ; // TODO: error handling!
      }  // TODO: pcmBuffer as c++ object!

    /* gets data from current position, data must be allocated by calling function
       data->nSamp must be initialized with the number of samples to read
       after reading in the data, nSamp will be set to the actual number of samples
       read
     */
    virtual int readFrame( cPcmBuffer &data )
      { int ret = waveInput_getDataSequential ( &_data, &(data._data) ); 
        return ret; // TODO: error handling!
      }
 
    virtual int writeFrame( cPcmBuffer &data ) { return 0; }
    
    // set file position in BLOCKS(!) 
    // return value: file position in bytes seeked to (inkl. header...), 0 on error
    int setPos( LONG_IDX pos ) 
      { return waveInput_setPos ( &_data, pos ); }

    virtual ~cWaveInput() { 
      waveInput_destroyData( &_data ); 
    }
};



#endif // __WAVE_INPUT_HPP





