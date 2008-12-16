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

#ifndef __LIVE_INPUT_HPP
#define __LIVE_INPUT_HPP
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Live Input Module (Recording from soundcard) 
============================================

Supported: 
		   PortAudio library   (working)
		   Simple Linux OSS    (still with bugs...)

The choice of input type is configured at compile time with #define 
(or --with-XXX when using autoconf)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 

extern "C" {
  #include "live-input.h"
}
#include "audio-stream.hpp"

class cLiveInput: public cAudioStream {
  sLiveInput _data;
  
  public:
  		 /* create live input object by specifying recording parameters as cWaveParameters class 
		   devicename: for linux OSS (e.g. /dev/dsp [default])
           deviceId  : for portaudio
		   */
         cLiveInput( const char *devicename, int deviceId, LONG_IDX maxBuf, const cWaveParameters &_param, int status) : cAudioStream()
         {
           streamType = TYPE_LIVEINPUT; 
           pLiveInput ret 
              = liveInput_create( &_data, devicename, deviceId, maxBuf, _param.sampleRate(), _param.sampleType(), _param.nChan(), status); 
           // TODO: error handling!
           param = new cWaveParameters(_param.param);
           streamDirection = AUDIOSTREAM_READ;
         }

  		 /* create live input object by specifying recording parameters explicitely
		   devicename: for linux OSS (e.g. /dev/dsp [default])
           deviceId  : for portaudio
          */
         cLiveInput( const char *devicename, int deviceId, LONG_IDX maxBuf, int sampleRate, int bits, int channels, int status)  : cAudioStream()
         {
           streamType = TYPE_LIVEINPUT; 
           pLiveInput ret 
              = liveInput_create( &_data, devicename, deviceId, maxBuf, sampleRate, pcmBitsToSampleType(bits, BYTEORDER_LE, 0), channels, status);
           // TODO: error handling!
           param = new cWaveParameters(_data.parameters);
           streamDirection = AUDIOSTREAM_READ;
         }

         // create ANSI C audioStream object
         virtual void createAudioStream()
         {
           cAudioStream::createAudioStream((void*)&_data, TYPE_LIVEINPUT, param,   liveInput_getDataSequential_gen ); //int (*getframe_method)(void *, pPcmBuffer)) 
         }

		 /*
		   List all available PortAudio input devices (print them via printf() )
		  */
         int listDevices() const { return liveInput_listDevices(); }

         /* set recording status
		     status: 0 = stopped, 1 = started
		  */   
         int setStatus( int status ) {
           return liveInput_setStatus( &_data, status );
         }

         /* 
		    gets data from current position, data must be allocated by calling function
            data->nSamp must be initialized with the number of samples to read
            after reading in the data, nSamp will be set to the actual number of samples
            read
          */
         virtual int readFrame( cPcmBuffer &buf ) {
           return liveInput_getDataSequential( &_data, &(buf._data) );
         }
         
         /* 
            writing is not supported... this is an input only class
            (in the future, however, audio output may be added for
             full-duplex devices via this method)
          */
         virtual int writeFrame( cPcmBuffer &data ) { return 0; }

         /* Destructor: frees the class data object and returns NULL */
         ~cLiveInput() { liveInput_destroyData( &_data ); }

};


#endif // #ifndef __LIVE_INPUT_HPP
