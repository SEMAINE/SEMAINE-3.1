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

This class handles the writing of PCM WAVE files (RIFF format) and RAW wave streams.

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

    /* constructor that allows specification of the isStream attribute, 
       which disables/enabled the writing of a pcm header to the output */
    inline cWaveOutput( const char *filename, long sampleRate, int sampleType, int channels, int _isStream ) : cAudioStream() {
      waveOutput_create( &(_data), filename, sampleRate, sampleType, channels );
      // TODO: error handling! if (... == NULL) then throw....
      param = new cWaveParameters(_data.parameters);
      streamDirection = AUDIOSTREAM_WRITE;
      streamType = TYPE_WAVEOUTPUT;    
      waveOutput_setStreaming( &(_data), _isStream );   
    }

    /* default constructor for pcm files */
    inline cWaveOutput( const char *filename, long sampleRate, int sampleType, int channels ) : cAudioStream() {
      waveOutput_create( &(_data), filename, sampleRate, sampleType, channels );
      // TODO: error handling! if (... == NULL) then throw....
      param = new cWaveParameters(_data.parameters);
      streamDirection = AUDIOSTREAM_WRITE;
      streamType = TYPE_WAVEOUTPUT;       
    }

    // create cd quality (44.1kHz 16bit, stereo) by default
    inline cWaveOutput( const char *filename ) {
      waveOutput_create( &(_data), filename, 44100, SAMPLETYPE_I16, 2 );
      // TODO: error handling!   if (... == NULL) then throw....
      param = new cWaveParameters(_data.parameters);
      streamDirection = AUDIOSTREAM_WRITE;
      streamType = TYPE_WAVEOUTPUT;             
    }

    /* isStream = 1 : set the output to streaming mode, i.e. do not write a pcm header
       isStream = 0 : write a wave file header after file open and before file close 
                      (seek back to beginning) */
    void setStreaming( int isStream ) {
      waveOutput_setStreaming( &(_data), isStream );
    }

    int writeDataRandom( cPcmBuffer &data, LONG_IDX start ) 
    { 
        int ret = waveOutput_writeDataRandom( &(_data), &(data._data), start ); 
        return ret ; // TODO: error handling!
    }

    /* 
      write the pcmBuffer in "data" to the file, increment file write pointer
      this functions performs necessary audio format conversions, if applicable
      (i.e. if file format parameters mismatch with pcmBuffer format)
     */
    virtual int writeFrame( cPcmBuffer &data )
      { int ret = waveOutput_writeDataSequential ( &_data, &(data._data) ); 
        return ret; // TODO: error handling!
      } 
 
    /* reading of frames is not supported, so it will always fail and return 0 */
    virtual int readFrame( cPcmBuffer &data ) { return 0; }
 
    // TODO:
    /* set file position in BLOCKS(!) 
       return value: file position in bytes seeked to (inkl. header...), 0 on error
     */
    int setPos( LONG_IDX pos ) 
       { } //return waveOutput_setPos ( &_data, pos ); }
  
    ~cWaveOutput() { waveOutput_destroyData( &_data ); }
};


#endif // #ifndef __WAVE_OUTPUT_HPP
