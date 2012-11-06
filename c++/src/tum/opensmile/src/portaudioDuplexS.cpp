/*F******************************************************************************
*
* openSMILE - open Speech and Music Interpretation by Large-space Extraction
*       the open-source Munich Audio Feature Extraction Toolkit
* Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
*
*
* Institute for Human-Machine Communication
* Technische Universitaet Muenchen (TUM)
* D-80333 Munich, Germany
*
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
*
******************************************************************************E*/


/*  openSMILE component:

portaudio dataPorcessor for full-duplex simultaneous recording and playback from the same audio device
(?? known to work on windows, linux, and mac)

*/


#include <portaudioDuplexS.hpp>
#define MODULE "cPortaudioDuplexS"

#ifdef HAVE_PORTAUDIO

SMILECOMPONENT_STATICS(cPortaudioDuplexS)

SMILECOMPONENT_REGCOMP(cPortaudioDuplexS)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CPORTAUDIODUPLEXS;
  sdescription = COMPONENT_DESCRIPTION_CPORTAUDIODUPLEXS;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("monoMixdownPB","mix down all channels to 1 mono channel (for playback)",0);
    ct->setField("monoMixdownREC","mix down all channels to 1 mono channel (for recording)",0);
    ct->setField("device","PortAudio device to use (device number)",-1);
    ct->setField("listDevices","(1/0=yes/no) list available portaudio devices during initialisation phase",0);
    ct->setField("sampleRate","recording AND playback sample rate (WARNING: no sample rate conversion of input level data will be performed prior to playback!)",44100);
    ct->setField("nBits","number of bits per sample and channel to use for playback (0=pass float values to portaudio)  [ NOT IMPLEMENTED YET ]",0);
    ct->setField("channels","number of channels to record",1);
    ct->setField("audioBuffersize","size of port audio playback&recording buffer in samples",1000);
    ct->setField("audioBuffersize_sec","size of port audio playback&recording buffer in seconds",0.05);
//    ct->setField("buffersize","(initial) (ring-)buffer size of wave writer in samples (default determined by 3*audioBuffersize)",0);
  )
  SMILECOMPONENT_MAKEINFO(cPortaudioDuplexS);
}

SMILECOMPONENT_CREATE(cPortaudioDuplexS)

//-----

cPortaudioDuplexS::cPortaudioDuplexS(const char *_name) :
cDataProcessor(_name),
paBuffer(NULL),
//mBuffersize(2000),
audioBuffersize(-1),
audioBuffersize_sec(-1.0),
//  curReadPos(0),
eof(0),  abort(0),
monoMixdownPB(0),
monoMixdownREC(0),
deviceId(0),
listDevices(0),
sampleRate(0),
channels(1),
dataFlagR(1),
dataFlagW(1),
nBits(16),
isPaInit(0),
callbackMatrix(NULL),
lastDataCount(0),
stream(NULL),
streamStatus(PA_STREAM_STOPPED)
{
  /*smileMutexCreate(callbackMtx);
  smileCondCreate(callbackCond);*/
}

void cPortaudioDuplexS::fetchConfig()
{
  cDataProcessor::fetchConfig();

  monoMixdownPB = getInt("monoMixdownPB");
  if (monoMixdownPB) { SMILE_IDBG(2,"monoMixdown for playback enabled!"); }
  monoMixdownREC = getInt("monoMixdownREC");
  if (monoMixdownREC) { SMILE_IDBG(2,"monoMixdown for recording enabled!"); }

 /*
 mBuffersize = getInt("buffersize");
  if (mBuffersize < 1) mBuffersize=1;
  SMILE_DBG(2,"buffersize = %i",mBuffersize);
*/
  if (isSet("audioBuffersize")) {
    audioBuffersize = getInt("audioBuffersize");
    if (audioBuffersize < 1) audioBuffersize=1;
    SMILE_DBG(2,"audioBuffersize = %i",audioBuffersize);
  }
  if (isSet("audioBuffersize_sec")) {
    audioBuffersize_sec = getInt("audioBuffersize_sec");
    if (audioBuffersize_sec < 1) audioBuffersize_sec = 1;
    SMILE_DBG(2,"audioBuffersize_sec = %i",audioBuffersize_sec);
  } 

  listDevices = getInt("listDevices");
  if (listDevices) { SMILE_IDBG(3,"listDevices enabled!"); }

  deviceId = getInt("device");
  SMILE_IDBG(2,"using portAudio device # %i",deviceId);

  sampleRate = getInt("sampleRate");
  if (sampleRate != 0) { SMILE_IDBG(2,"user-defined playback sample rate = %i",sampleRate); }

  channels = getInt("channels");
  if (channels < 1) channels=1;
  SMILE_DBG(2,"No. of recording channels = %i",channels);

  nBits = 33;
  nBPS = 4;
/*
nBits = getInt("nBits");
  switch(nBits) {
    case 8: nBPS=1; break;
    case 16: nBPS=2; break;
    case 24: nBPS=4; break;
    case 32: nBPS=4; break;
    case 33: nBPS=4; break;
    default:
      SMILE_IERR(1,"invalid number of bits requested: %i (allowed: 8, 16, 24, 32, 33(for 23-bit float))\n   Setting number of bits to default (16)",nBits);
      nBits=16;
  }
  SMILE_IDBG(2,"No. of bits per sample = %i",nBits);
*/

  //  // TODO: buffersize in seconds.. AND unlink buffersize = buffersize in datamemory!
}

int cPortaudioDuplexS::configureWriter(sDmLevelConfig &c)
{
  if (audioBuffersize > 0) {
    c.blocksizeWriter = audioBuffersize;
  } else if (audioBuffersize_sec > 0) {
    double TT = 1.0;
    if (c.T != 0.0) TT = c.T;
    c.blocksizeWriter = audioBuffersize = (long)ceil( audioBuffersize_sec / TT );
  } else {
    SMILE_IMSG(3,"using default audioBuffersize (1000 samples) since no option was given in config file");
    c.blocksizeWriter = audioBuffersize = 1000;
  }

  c.T = 1.0 / (double)(sampleRate);

  reader->setupSequentialMatrixReading(audioBuffersize, audioBuffersize);
  
  return 1;
}


int cPortaudioDuplexS::myConfigureInstance()
{
  int ret = 1;

  // initialise port audio
  if (!isPaInit) {
    PaError err = Pa_Initialize(); 
    if( err != paNoError ) COMP_ERR(0,"error initialising portaudio library (code %i)\n",err);
    isPaInit=1;
  }

#ifdef HAVE_PORTAUDIO_V19
  numDevices = Pa_GetDeviceCount();
#else
  numDevices = Pa_CountDevices();
#endif

  // list devices...
  if (listDevices) {
    if (listDevices != -1) {
      printDeviceList();
      listDevices=-1;
      SMILE_IMSG(1,"Device list was printed, cPortaudioDuplexS::myConfigureInstance will now return 0 in order to stop openSMILE initialisation!");
    }
    return 0;
  } else {
    if (numDevices <= 0) {
      SMILE_IERR(1,"NO PortAudio devices found! Cannot play live audio!");
    }
  }

  return cDataProcessor::myConfigureInstance();
}

int cPortaudioDuplexS::setupNewNames(long nEl)
{
  writer->addField("pcm",channels);
  namesAreSet = 1;
  return 1;
}

int cPortaudioDuplexS::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();

  if (ret) {
    callbackMatrix = new cMatrix( getChannels(), audioBuffersize );
    paBuffer = new float[audioBuffersize];

    channels = reader->getLevelN();
    if (channels > 2) {
      SMILE_IWRN(1,"channels was > 2 , it was limited to 2! This might cause problems...");
      channels=2;
    }
    if (channels < 1) {
      SMILE_IWRN(1,"channels was < 1 , it was automatically adjusted to 1! This might cause problems...");
      channels=1;
    }

    if (sampleRate == 0) 
      sampleRate = (int)(1.0/(reader->getLevelT()));

    SMILE_IDBG(2,"setting playback sampleRate: %i Hz",sampleRate);
    //    printf("XXXXXXXXXXXXXXx computed sampleRate as: %i Hz (per = %f)\n",sampleRate,reader->getLevelT()*1000);

  }

  return ret;
}


// channels = number of output channels (can be 1 or 2)
int matrixToPcmDataFloat_ds(void *outputBuffer, long __N, cMatrix *_mat, int channels, int mixdown=0) 
{
  int i,c,n;

  float *out = (float*)outputBuffer;

  for (i=0; i<MIN(_mat->nT,__N); i++) {
    if (mixdown) {
      out[i*channels] = 0.0;
      for (c=0; c<channels; c++) {
        for (n=0; n<_mat->N; n++) 
          out[i*channels+c] += (float)(_mat->dataF)[i*(_mat->N)+n];
      }
    } else {
      int minc = channels;
      if (_mat->N < minc) minc = _mat->N;
      for (c=0; c<channels; c++) {
        out[i*channels+c] = (float)(_mat->dataF)[i*(_mat->N)+c];
      }
    }
  }
  return 1;
}

// channels = number of output channels (can be 1 or 2)
int pcmDataFloatToMatrix_ds(const void *inputBuffer, long __N, cMatrix *_mat, int channels, int mixdown=0) 
{
  int i,c,n;

  const float *in = (const float*)inputBuffer;
  FLOAT_DMEM *out = _mat->dataF;

  for (i=0; i<MIN(_mat->nT,__N); i++) {
    if (mixdown) {
      out[i*_mat->N] = (FLOAT_DMEM)0.0;
      for (c=0; c<channels; c++) {
        for (n=0; n<_mat->N; n++) 
          out[i*_mat->N+n] += (FLOAT_DMEM)(in[i*channels+c]);
      }
    } else {
      int minc = channels;
      if (_mat->N < minc) minc = _mat->N;
      for (c=0; c<channels; c++) {
        if (c>minc) {
          out[i*_mat->N+c] = (FLOAT_DMEM)(0.0);
        } else {
          out[i*_mat->N+c] = (float)(in[i*channels+c]);
        }
      }
    }
  }
  return 1;
}



int cPortaudioDuplexS::myTick(long long t)
{
  if (isEOI()) return 0; //XXX ????
  int ret = 1;

  if (streamStatus == PA_STREAM_STOPPED) {
    if (!startDuplex()) return 0;
  }

  long len = Pa_GetStreamWriteAvailable(stream);
  if (len < 0) len = audioBuffersize;
  if (len > 0) {
    if (len > audioBuffersize) len = audioBuffersize;
    cMatrix * _mat = reader->getMatrixRel(-len,len);  
    if (_mat != NULL) {
      matrixToPcmDataFloat_ds(paBuffer, len, _mat, getChannels(), isMonoMixdownPB());
      Pa_WriteStream(stream, paBuffer, len); 
      ret = 1;
    } else {printf("N\n"); ret = 0; }
  } else { ret = 0; }

  if (writer->checkWrite(audioBuffersize)) {
    // make matrix.. 
    len = audioBuffersize;
    Pa_ReadStream(stream, paBuffer, len); 
    pcmDataFloatToMatrix_ds(paBuffer, len, callbackMatrix, getChannels(), isMonoMixdownREC());
    callbackMatrix->nT = len;
    // save to datamemory
    writer->setNextMatrix(callbackMatrix);
  }

  return ret;
}


cPortaudioDuplexS::~cPortaudioDuplexS()
{
  if (isPaInit) {
    //smileMutexLock(callbackMtx);
    stopDuplex();

    // TODO: set a flag to inform the callback recording has stoppped!!
    //   and, do not call Pa_terminate while the callback is running... i.e. introduce a callback mutex!
    isPaInit = 0;
    PaError err = Pa_Terminate();
    if( err != paNoError ) {
      SMILE_ERR(2,"PortAudio error (Pa_Terminate): %s\n", Pa_GetErrorText( err ) );
    }
    //smileMutexUnlock(callbackMtx);
  }

 // smileMutexDestroy(callbackMtx);
 // smileCondDestroy(callbackCond);
  if (callbackMatrix != NULL) delete callbackMatrix;
  if (paBuffer != NULL) delete paBuffer;
}

//--------------------------------------------------  portaudio specific


int cPortaudioDuplexS::startDuplex()
{
  PaStreamParameters  streamParameters;
  PaError             err = paNoError;

#ifdef HAVE_PORTAUDIO_V19
  if (Pa_IsStreamActive( stream ) == 1) {
#else
  if (Pa_StreamActive( stream ) == 1) {
#endif
    SMILE_WRN(2,"portAudio stream is already active (in startPlayback).");
    return 1;
  }

  if (deviceId < 0) {
    streamParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  } else {
    streamParameters.device = deviceId;
  }
  //

  SMILE_MSG(2,"playing on portAudio device with index %i",streamParameters.device);
  streamParameters.channelCount = channels;
  streamParameters.sampleFormat = paFloat32;

  /*	switch (nBits) {
  case 8: outputParameters.sampleFormat = paInt8; break;
  case 16: outputParameters.sampleFormat = paInt16; break;
  case 24: outputParameters.sampleFormat = paInt24; break;
  case 32: outputParameters.sampleFormat = paInt32; break;
  case 33: outputParameters.sampleFormat = paFloat32; break;
  default:
  COMP_ERR("invalid number of bits requested: %i (allowed: 8, 16, 24, 32, 33(for 23-bit float))\n",nBits);
  }*/

#ifdef HAVE_PORTAUDIO_V19
  const PaDeviceInfo * info = Pa_GetDeviceInfo( streamParameters.device );
  if (info != NULL) {
    streamParameters.suggestedLatency =
      info->defaultLowOutputLatency;
  } else {
    streamParameters.suggestedLatency = 0;
  }
  streamParameters.hostApiSpecificStreamInfo = NULL;
#endif

  /* Setup recording stream -------------------------------------------- */
  err = Pa_OpenStream(
    &(stream),
#ifdef HAVE_PORTAUDIO_V19
    &streamParameters,
    &streamParameters,
#else // V18 (old API)
    streamParameters.device, 
    streamParameters.channelCount, 
    streamParameters.sampleFormat, 
    NULL,
    streamParameters.device, 
    streamParameters.channelCount, 
    streamParameters.sampleFormat, 
    NULL,
#endif
    sampleRate,
    audioBuffersize*2,
/*
#ifndef HAVE_PORTAUDIO_V19
    1,
#endif
    */
    paClipOff,      /* we won't output out of range samples so don't bother clipping them */
    NULL,
    (void*) this );

  if( err != paNoError ) {
    COMP_ERR("error opening portaudio playback stream (code %i) \n  check samplerate(-> %i , maybe it is not supported?) \n  maybe incorrect device? (\"listDevices=1\" in config file displays a list of devices)\n",err,sampleRate);
  }

  err = Pa_StartStream( stream );
  if( err != paNoError ) COMP_ERR("cannot start portaudio stream (code %i)\n",err);
  streamStatus = PA_STREAM_STARTED;

  return 1;
}

int cPortaudioDuplexS::stopDuplex()
{
  PaError             err = paNoError;
  if (streamStatus == PA_STREAM_STOPPED) return 0;

  streamStatus =  PA_STREAM_STOPPED;

  abort = 1;

  err = Pa_CloseStream( stream );
  if( err != paNoError ) {
    SMILE_ERR(1,"cannot close portaudio stream (code %i)\n",err);
    return 0;
  }

  return 1;
}

int cPortaudioDuplexS::stopDuplexWait()
{
  PaError             err = paNoError;
  if (streamStatus == PA_STREAM_STOPPED) return 0;

  streamStatus =  PA_STREAM_STOPPED;

  abort = 1;

  err = Pa_StopStream( stream );
  if( err != paNoError ) {
    SMILE_ERR(1,"cannot close portaudio stream (code %i)\n",err);
    return 0;
  }

  return 1;
}

void cPortaudioDuplexS::printDeviceList(void)
{
  // query devices:
  SMILE_PRINT("== cPortAudioSink ==  There are %i audio devices:", numDevices );
  if( numDevices < 0 ) {
    SMILE_ERR(1, "Pa_CountDevices returned 0x%x\n", numDevices );
    return;
  }
  if( numDevices == 0 ) {
    SMILE_ERR(1, "No PortAudio audio devices were found! (Pa_CountDevices()=0)\n", numDevices );
    return;
  }

  const PaDeviceInfo *deviceInfo;

  int i;
  for( i=0; i<numDevices; i++ )
  {
    deviceInfo = Pa_GetDeviceInfo( i );
    SMILE_PRINT("  -> Device #%i: '%s'\n       #inputChan=%i #outputChan=%i\n",i,deviceInfo->name, deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
  }

#ifdef HAVE_PORTAUDIO_V19
  SMILE_PRINT("  The default device is #%i\n",Pa_GetDefaultInputDevice());
#else
  SMILE_PRINT("  The default device is #%i\n",Pa_GetDefaultInputDeviceID());
#endif

}

#endif // HAVE_PORTAUDIO
