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
 
#ifndef __LIVE_INPUT_HPP
#define __LIVE_INPUT_HPP

extern "C" {
  #include "live-input.h"
}
#include "audio-stream.hpp"

class cLiveInput: public cAudioStream {
  sLiveInput _data;
  
  public:
         cLiveInput( const char *devicename, int deviceId, LONG_IDX maxBuf, const cWaveParameters &_param, int status) : cAudioStream()
         {
           streamType = TYPE_LIVEINPUT; 
           pLiveInput ret 
              = liveInput_create( &_data, devicename, deviceId, maxBuf, _param.sampleRate(), _param.sampleType(), _param.nChan(), status); 
           // TODO: error handling!
           param = new cWaveParameters(_param.param);
           streamDirection = AUDIOSTREAM_READ;
         }

         cLiveInput( const char *devicename, int deviceId, LONG_IDX maxBuf, int sampleRate, int bits, int channels, int status)  : cAudioStream()
         {
//      waveInput_create( &(_data), filename, maxBuf );

           streamType = TYPE_LIVEINPUT; 
           pLiveInput ret 
              = liveInput_create( &_data, devicename, deviceId, maxBuf, sampleRate, pcmBitsToSampleType(bits, BYTEORDER_LE, 0), channels, status);
           // TODO: error handling!
           param = new cWaveParameters(_data.parameters);
           streamDirection = AUDIOSTREAM_READ;
         }

         // create legacy ANSI C audioStream object
         virtual void createAudioStream()
         {
           cAudioStream::createAudioStream((void*)&_data, TYPE_LIVEINPUT, param,   liveInput_getDataSequential_gen ); //int (*getframe_method)(void *, pPcmBuffer)) 
         }

         int listDevices() const { return liveInput_listDevices(); }

         // status: 0: stopped, 1:started
         int setStatus( int status ) {
           return liveInput_setStatus( &_data, status );
         }
/*
         LONG_IDX liveInput_secondsToSamples( pLiveInput obj, FLOAT_TYPE se );
         FLOAT_TYPE liveInput_samplesToSeconds( pLiveInput obj, LONG_IDX sa );
*/

         /* gets data from current position, data must be allocated by calling function
            data->nSamp must be initialized with the number of samples to read
            after reading in the data, nSamp will be set to the actual number of samples
            read
          */
         virtual int readFrame( cPcmBuffer &buf ) {
           return liveInput_getDataSequential( &_data, &(buf._data) );
         }
         
         virtual int writeFrame( cPcmBuffer &data ) { return 0; }

         /* Destructor: frees the class data object and returns NULL */
         ~cLiveInput() { liveInput_destroyData( &_data ); }

};


#endif // #ifndef __LIVE_INPUT_HPP
