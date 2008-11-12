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

This class handles the reading of PCM WAVE files (RIFF format).
Other file types, such as compressed audio (MP3 or OGG) are not yet supported!

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __WAVE_OUTPUT_HPP
#define __WAVE_OUTPUT_HPP

extern "C" {
  #include "wave-output.h"
}
#include "audio-stream.hpp"

class cWaveOutput: public cAudioStream {
  sWaveOutput _data; 

  public:
    inline cWaveOutput( const char *filename, long sampleRate, int sampleType, int channels ) : cAudioStream() {
      waveOutput_create( &(_data), filename, sampleRate, sampleType, channels );
      // TODO: error handling!
      param = new cWaveParameters(_data.parameters);
      streamDirection = AUDIOSTREAM_WRITE;
      streamType = TYPE_WAVEOUTPUT;       
    }

    // create cd quality (44.1kHz 16bit, stereo) by default
    inline cWaveOutput( const char *filename ) {
      waveOutput_create( &(_data), filename, 44100, SAMPLETYPE_I16, 2 );
      // TODO: error handling!
      param = new cWaveParameters(_data.parameters);
      streamDirection = AUDIOSTREAM_WRITE;
      streamType = TYPE_WAVEOUTPUT;             
    }
     
    //    long getSampleRate() { return waveInput_getSampleRate( &(_data) ); }
  /*  
    LONG_IDX secondsToSamples( FLOAT_TYPE se ) 
      { return waveOutut_secondsToSamples( &(_data), se ); }
    
    FLOAT_TYPE samplesToSeconds( LONG_IDX sa ) 
      { return waveOutput_samplesToSeconds( &(_data), sa ); }
*/
    int writeDataRandom( cPcmBuffer &data, LONG_IDX start ) 
      { int ret = waveOutput_writeDataRandom( &(_data), &(data._data), start ); 
        return ret ; // TODO: error handling!
      }  // TODO: pcmBuffer as c++ object!

    /* gets data from current position, data must be allocated by calling function
       data->nSamp must be initialized with the number of samples to read
       after reading in the data, nSamp will be set to the actual number of samples
       read
     */
    virtual int writeFrame( cPcmBuffer &data )
      { int ret = waveOutput_writeDataSequential ( &_data, &(data._data) ); 
        return ret; // TODO: error handling!
      } // TODO: pcmBuffer as c++ object!
 
    virtual int readFrame( cPcmBuffer &data ) { return 0; }
 
     // set file position in BLOCKS(!) 
     // return value: file position in bytes seeked to (inkl. header...), 0 on error
     int setPos( LONG_IDX pos ) 
       { } //return waveOutput_setPos ( &_data, pos ); }

     ~cWaveOutput() { waveOutput_destroyData( &_data ); }
};


#endif // #ifndef __WAVE_OUTPUT_HPP
