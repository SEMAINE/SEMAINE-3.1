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

portaudio dataSource for live recording from sound device
known to work on windows, linux, and mac

*/


#include <portaudioSource.hpp>
#define MODULE "cPortaudioSource"

#ifdef HAVE_PORTAUDIO

SMILECOMPONENT_STATICS(cPortaudioSource)

SMILECOMPONENT_REGCOMP(cPortaudioSource)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CPORTAUDIOSOURCE;
  sdescription = COMPONENT_DESCRIPTION_CPORTAUDIOSOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("monoMixdown","Mix down all recorded channels to 1 mono channel",0);
    ct->setField("device","PortAudio device to use (device number, see the option 'listDevices' to get information on device numbers)",-1);
    ct->setField("period",(const char*)NULL,0,0,0);
    ct->setField("listDevices","If set to 1, openSMILE will list available portaudio devices during initialisation phase and exit immediately after that (you might get an error message on windows, which you can ignore).",0);
    ct->setField("sampleRate","The sampling rate to use for audio recording (if supported by device!)",16000);
    ct->setField("channels","The number of channels to record (check your device's capabilities!",1);
    ct->setField("selectChannel","Select only the specified channel from 'channels' that are recorded. Set to -1 to grab all channels.",-1);
    ct->setField("nBits","The number of bits per sample and channel",16);
    ct->setField("nBPS","The number of bytes per sample and channel (0=determine automatically from nBits)",0,0,0);
    ct->setField("audioBuffersize","The size of the portaudio recording buffer in samples (overwrites audioBuffersize_sec, if set)",0,0,0);
    ct->setField("audioBuffersize_sec","size of the portaudio recording buffer in seconds. This value has influence on the system latency. Setting it too high might introduce a high latency. A too low value might lead to dropped samples and reduced performance.",0.05);
    ct->setField("byteSwap","1 = swap bytes, big endian <-> little endian (usually not required)",0,0,0);
    //ct->setField("buffersize","(initial) (ring-)buffer size of wave reader in samples",0);
  )
  SMILECOMPONENT_MAKEINFO(cPortaudioSource);
}

SMILECOMPONENT_CREATE(cPortaudioSource)

//-----

cPortaudioSource::cPortaudioSource(const char *_name) :
  cDataSource(_name),
  //mBuffersize(2000),
  audioBuffersize(-1),
  audioBuffersize_sec(-1.0),
  curReadPos(0),
  eof(0),  abort(0),
  monoMixdown(0),
  deviceId(0),
  listDevices(0),
  sampleRate(0),
  channels(1),
  dataFlag(1),
  nBits(16),
  isPaInit(0),
  callbackMatrix(NULL),
  lastDataCount(0),
  stream(NULL),
  streamStatus(PA_STREAM_STOPPED),
  byteSwap(0),
  warned(0)
{
  smileMutexCreate(dataFlagMtx);
  smileMutexCreate(callbackMtx);
  smileCondCreate(callbackCond);
}

void cPortaudioSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  monoMixdown = getInt("monoMixdown");
  if (monoMixdown) { SMILE_IDBG(2,"monoMixdown enabled!"); }

  /*
  mBuffersize = getInt("buffersize");
  if (mBuffersize < 1) mBuffersize=1;
  SMILE_DBG(2,"buffersize = %i",mBuffersize);
*/
  
  if (isSet("audioBuffersize")) {
    audioBuffersize = getInt("audioBuffersize");
//    if (audioBuffersize < 1024) audioBuffersize=1024;
    SMILE_DBG(2,"audioBuffersize = %i",audioBuffersize);
    //printf("audioBuffersize = %i",audioBuffersize);
  }
  if (isSet("audioBuffersize_sec")) {
    audioBuffersize_sec = getDouble("audioBuffersize_sec");
//    if (audioBuffersize_sec < 0.0) audioBuffersize_sec = 0.05;
    SMILE_DBG(2,"audioBuffersize_sec = %i",audioBuffersize_sec);
    //printf("audioBuffersize_sec = %f",audioBuffersize_sec);
  } 

  listDevices = getInt("listDevices");
  if (listDevices) { SMILE_IDBG(3,"listDevices enabled!"); }

  deviceId = getInt("device");
  SMILE_DBG(2,"using portAudio device # %i",deviceId);

  sampleRate = getInt("sampleRate");
  SMILE_DBG(2,"recording sample rate = %i",sampleRate);

  channels = getInt("channels");
  if (channels < 1) channels=1;
  SMILE_DBG(2,"No. of recording channels = %i",channels);

  selectChannel = getInt("selectChannel");
  if (selectChannel >= 0) { SMILE_DBG(2,"only selected %i. channel.",selectChannel); }
  
  nBits = getInt("nBits");
  nBPS = getInt("nBPS");
  if (nBPS==0) {
  switch(nBits) {
    case 8: nBPS=1; break;
    case 16: nBPS=2; break;
    case 24: nBPS=4; break;
    case 32: nBPS=4; break;
    case 33: nBPS=4; break;
    case 0:  nBPS=4; nBits=32; break;
    default:
      SMILE_ERR(1,"invalid number of bits requested: %i (allowed: 8, 16, 24, 32, 33(for 23-bit float))\n   Setting number of bits to default (16)",nBits);
      nBits=16;
  }
  }
  SMILE_DBG(2,"No. of bits per sample = %i",nBits);

  byteSwap = getInt("byteSwap");
//  // TODO: AND unlink buffersize = buffersize in datamemory!
}

int cPortaudioSource::configureWriter(sDmLevelConfig &c)
{
  c.T = 1.0 / (double)(sampleRate);

  if (audioBuffersize > 0) {
    c.blocksizeWriter = audioBuffersize;
    blocksizeW = audioBuffersize;
  } else if (audioBuffersize_sec > 0.0) {
    double TT = 1.0;
    if (c.T != 0.0) TT = c.T;
    c.blocksizeWriter = blocksizeW = audioBuffersize = (long)ceil( audioBuffersize_sec / TT );
    blocksizeW_sec = audioBuffersize_sec;
  } else {
    SMILE_IMSG(3,"using default audioBuffersize (1024 samples) since no option was given in config file");
    c.blocksizeWriter = audioBuffersize = 1024;
  }
//  printf("blocksizeWriter: %i ab: %i %f\n",c.blocksizeWriter,audioBuffersize,audioBuffersize_sec);
//  COMP_ERR("stop");

  return 1;
}

int cPortaudioSource::myConfigureInstance()
{
  int ret = 1;

  // initialise port audio, if not already initialised!
  if (!isPaInit) {
    PaError err = Pa_Initialize(); isPaInit=1;
    if( err != paNoError ) COMP_ERR(0,"error initialising portaudio library (code %i)\n",err);
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
      SMILE_MSG(1,"Device list was printed, cPortaudioSource::myConfigureInstance will now return 0 in order to stop openSMILE initialisation!");
    }
    return 0;
  } else {
    if (numDevices <= 0) {
      SMILE_ERR(1,"NO PortAudio devices found! Cannot record live audio!");
    }
  }

  return cDataSource::myConfigureInstance();
}

int cPortaudioSource::setupNewNames(long nEl)
{
  if ((monoMixdown)||(selectChannel>=0)) {
    writer->addField("pcm",1);
  } else {
    writer->addField("pcm",channels);
  }
  namesAreSet = 1;
  return 1;
}

int cPortaudioSource::myFinaliseInstance()
{
  int ret = cDataSource::myFinaliseInstance();
  if (ret) {
    if ((monoMixdown)||(selectChannel>=0)) {
      callbackMatrix = new cMatrix( 1, audioBuffersize );
    } else {
      callbackMatrix = new cMatrix( getChannels(), audioBuffersize );
    }
    //ret*=startRecording(); 
    // we start recording during first tick... this will ensure that all componentes are finalised when the first callback is called!
  }
  return ret;
}


int cPortaudioSource::myTick(long long t)
{
  if (isEOI()) return 0; //XXX ????
  
  if (streamStatus == PA_STREAM_STOPPED) {
    if (!startRecording()) return 0;
  }

  // this will always try to fill the buffer as much as possible... for multiple threads this is absolutely ok,
  //   for single threads the buffer might overflow... thus for single threads we might also want to check for (writer->getNAvail() < audioBuffersize )
//writer->getN
  if ((writer->checkWrite(audioBuffersize)) && (writer->getNAvail() < audioBuffersize )) {
    //printf("wait\n");
    smileCondTimedWait(callbackCond,1000);
  } else {
    //printf("no wait! %i  %i  %i bs=%i\n",t,writer->getNAvail(), (writer->checkWrite(audioBuffersize)), audioBuffersize);
    if (writer->getNFree() < audioBuffersize*2) {
      if (!warned) {
        SMILE_WRN(3,"audio buffer FULL, processing seems to be too slow. audio data possibly lost.");
        warned=1;
      }
    } else {
      warned = 0;
    }
  }

  return 1;
  
/*
  // here we check wether we will wait for data, i.e. yield time to other threads or not...
  if (writer->checkWrite(audioBuffersize) && (writer->getNAvail() < audioBuffersize )) {
//                                           printf("pa wait...\n");
    smileCondTimedWait(callbackCond,1000);
                                           //printf("pa wait END...\n");
    return 1;
  } else {
    if (writer->getNAvail() >= audioBuffersize ) { smileYield(); return 1; }
    else return 0;
  }
  
  if (writer->getNAvail() <= 0) {
    SMILE_ERR(1,"no input data from portaudio recording device for 1000 ticks after dataMemory level was empty! aborting!");
    return 0;
  }

                                           printf("pa re 1...\n");
  return 0;
  */
}


cPortaudioSource::~cPortaudioSource()
{
  if (isPaInit) {
    smileMutexLock(callbackMtx);
    stopRecording();

//  usleep(10000);
// TODO: set a flag to inform the callback recording has stoppped!!
//   and, do not call Pa_terminate while the callback is running... i.e. introduce a callback mutex!
    isPaInit = 0;
    PaError err = Pa_Terminate();
    if( err != paNoError ) {
      SMILE_ERR(2,"PortAudio error (Pa_Terminate): %s\n", Pa_GetErrorText( err ) );
    }
    smileMutexUnlock(callbackMtx);
  }
  smileMutexDestroy(dataFlagMtx);
  smileMutexDestroy(callbackMtx);
  smileCondDestroy(callbackCond);
  if (callbackMatrix != NULL) delete callbackMatrix;
}

//--------------------------------------------------  portaudio specific


int pcmDataToMatrix(const void *buf, cMatrix *m, int nBPS, int nBits, int nChan, int monoMixdown, int selectChannel)
{
    int i,c;
    const int8_t *b8=(int8_t*)buf;
    const uint8_t *ub8=(uint8_t*)buf;
    const int16_t *b16 = (int16_t*)buf;
    const int32_t *b32 = (int32_t*)buf;
    const float *b32f = (float*)buf;

    if (selectChannel >= 0) { // select a single channel by index from a multi channel recording

      switch(nBPS) {
        case 1: // 8-bit int
          for (i=0; i<m->nT; i++) {
            FLOAT_DMEM tmp;
            tmp = (FLOAT_DMEM)b8[i*nChan+selectChannel];
            m->setF(0,i,tmp/(FLOAT_DMEM)127.0);
          }
          break;
        case 2: // 16-bit int
          for (i=0; i<m->nT; i++) {
            FLOAT_DMEM tmp;
            tmp = (FLOAT_DMEM)b16[i*nChan+selectChannel];
            m->setF(0,i,tmp/(FLOAT_DMEM)32767.0);
          }
          break;
        case 3: // 24-bit int
          COMP_ERR("pcmDataToMatrix: 24-bit wave file with 3 bytes per sample encoding not yet supported!");
        /*
        int24_t *b24 = (int24_t*)buf;
        for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
          m->setF(c,i,((FLOAT_DMEM)b24[i*nChan+c])/(32767.0*256.0));
        } break;
        */
          break;
        case 4: // 32-bit int or 24-bit packed int
          if (nBits == 24) {
            for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp;
              tmp = (FLOAT_DMEM)(b32[i*nChan+selectChannel]&0xFFFFFF);
              m->setF(0,i,tmp/(FLOAT_DMEM)(32767.0*256.0));
            }
            break;
          } else if (nBits == 32) {
            for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp;
              tmp = (FLOAT_DMEM)(b32[i*nChan+selectChannel]);
              m->setF(0,i,tmp/(FLOAT_DMEM)(32767.0*32767.0*2.0));
            }
            break;
//TODO: case nBits=33  : 32-bit float samples from portaudio....

      /*      for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
              m->setF(c,i,((FLOAT_DMEM)b32[i*nChan+c])/(32767.0*32767.0*2.0));
            } break;*/
        }
        default:
          SMILE_ERR(1,"pcmDataToMatrix: cannot convert unknown sample format to float! (nBPS=%i, nBits=%i)",nBPS,nBits);
          return 0;
      }

    } else if (monoMixdown) {

      switch(nBPS) {
        case 1: // 8-bit int
          for (i=0; i<m->nT; i++) {
            FLOAT_DMEM tmp = 0.0;
            for (c=0; c<nChan; c++) {
              tmp += (FLOAT_DMEM)b8[i*nChan+c];
            }
            m->setF(0,i,(tmp/(FLOAT_DMEM)nChan)/(FLOAT_DMEM)127.0);
          }
          break;
        case 2: // 16-bit int
          for (i=0; i<m->nT; i++) {
            FLOAT_DMEM tmp = 0.0;
            for (c=0; c<nChan; c++) {
              tmp += (FLOAT_DMEM)b16[i*nChan+c];
            }
            m->setF(0,i,(tmp/(FLOAT_DMEM)nChan)/(FLOAT_DMEM)32767.0);
          }
          break;
        case 3: // 24-bit int
          COMP_ERR("pcmDataToMatrix: 24-bit wave file with 3 bytes per sample encoding not yet supported!");
        /*
        int24_t *b24 = (int24_t*)buf;
        for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
          m->setF(c,i,((FLOAT_DMEM)b24[i*nChan+c])/(32767.0*256.0));
        } break;
        */
          break;
        case 4: // 32-bit int or 24-bit packed int
          if (nBits == 24) {
            for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp = 0.0;
              for (c=0; c<nChan; c++) {
                tmp += (FLOAT_DMEM)(b32[i*nChan+c]&0xFFFFFF);
              }
              m->setF(0,i,(tmp/(FLOAT_DMEM)nChan)/(FLOAT_DMEM)(32767.0*256.0));
            }
            break;
          } else if (nBits == 32) {
            for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp = 0.0;
              for (c=0; c<nChan; c++) {
                tmp += (FLOAT_DMEM)(b32[i*nChan+c]);
              }
              m->setF(0,i,(tmp/(FLOAT_DMEM)nChan)/(FLOAT_DMEM)(32767.0*32767.0*2.0));
            }
            break;
//TODO: case nBits=33  : 32-bit float samples from portaudio....

      /*      for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
              m->setF(c,i,((FLOAT_DMEM)b32[i*nChan+c])/(32767.0*32767.0*2.0));
            } break;*/
        }
        default:
          SMILE_ERR(1,"pcmDataToMatrix: cannot convert unknown sample format to float! (nBPS=%i, nBits=%i)",nBPS,nBits);
          return 0;
      }

    } else { // no mixdown, multi-channel matrix output

      switch(nBPS) {
        case 1: // 8-bit int
          for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
            m->setF(c,i,((FLOAT_DMEM)b8[i*nChan+c])/(FLOAT_DMEM)127.0);
          } break;
        case 2: // 16-bit int
          for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
            m->setF(c,i,((FLOAT_DMEM)b16[i*nChan+c])/(FLOAT_DMEM)32767.0);
          } break;
        case 3: // 24-bit int
        //  COMP_ERR("pcmDataToMatrix: 24-bit wave file with 3 bytes per sample encoding not yet supported!");
        
        //b8 = (uint8_t*)buf;
        for (i=0; i<m->nT; i++) 
          for (c=0; c<nChan; c++) {
            FLOAT_DMEM tmp = (FLOAT_DMEM)ub8[i*3*nChan+c] * (FLOAT_DMEM)(256.0*256.0) + (FLOAT_DMEM)ub8[i*3*nChan+c+1]*(FLOAT_DMEM)256.0 + (FLOAT_DMEM)ub8[i*3*nChan+c+2]; 
            m->setF(c,i,(tmp/(FLOAT_DMEM)(32767.0*256.0)));
          } break;
        
        //  break;
        case 4: // 32-bit int or 24-bit packed int
          if (nBits == 24) {
            for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
              //m->setF(c,i,((FLOAT_DMEM)(b32[i*nChan+c]&0xFFFFFF))/(32767.0*32767.0*2.0));
              m->setF(c,i,((FLOAT_DMEM)((b32[i*nChan+c]&0xFFFFFFFF)>>8))/(FLOAT_DMEM)(32767.0*256.0));
            } break;
          } else if (nBits == 32) {
            for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
              // byte swap:
              uint8_t xx[4];
              int32_t *xxx = (int32_t *)&xx;
              *xxx = b32[i*nChan+c];
/*
              uint8_t tmp2 = xx[0];
              xx[0] = xx[3];
              xx[3] = tmp2;
              tmp2 = xx[1];
              xx[1] = xx[2];
              xx[2] = tmp2;*/
              m->setF(c,i,((FLOAT_DMEM)(*xxx))/(FLOAT_DMEM)(32767.0*32767.0*2.0));
//              m->setF(c,i,((FLOAT_DMEM)b32[i*nChan+c])/(32767.0*32767.0*4.0));

            } break;
          } else if (nBits == 33) {
            for (i=0; i<m->nT; i++) for (c=0; c<nChan; c++) {
              m->setF(c,i,((FLOAT_DMEM)b32f[i*nChan+c]));
            } break;
          }
//TODO: case nBits=33  : 32-bit float samples from portaudio....


        default:
          SMILE_ERR(1,"pcmDataToMatrix: cannot convert unknown sample format to float! (nBPS=%i, nBits=%i)",nBPS,nBits);
          return 0;
      }
    }
    
    return 1;
}

/* This routine will be called by the PortAudio engine when audio is available.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int paSource_recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
#ifdef HAVE_PORTAUDIO_V19
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
#else // V18 (old API)
                           PaTimestamp outTime,
#endif
                           void *userData )
{
  cPortaudioSource * obj = (cPortaudioSource *)userData;

  if (obj == NULL) { return paAbort; }

  if (!smileMutexLock(obj->callbackMtx)) return paAbort;
  
  if (obj->isAbort()) {
    smileMutexUnlock(obj->callbackMtx);
    return paAbort;
  }
  cDataWriter * writer = obj->getWriter();

  //printf("PA_callback data arrived %i!!\n",framesPerBuffer); fflush(stdout);

  if (writer->checkWrite(framesPerBuffer)) {

    // make matrix.. // TODO... move this "new" to the init code...!
    cMatrix * mat = obj->callbackMatrix; //new cMatrix( obj->getChannels(), framesPerBuffer );
    if (mat==NULL) {
      smileMutexUnlock(obj->callbackMtx);
      return paAbort;
    }
  
    pcmDataToMatrix(inputBuffer, mat, obj->getNBPS(), obj->getNBits(), obj->getChannels(), obj->isMonoMixdown(), obj->getSelectedChannel());

    obj->setNewDataFlag();
    smileCondSignal(obj->callbackCond);

    // save to datamemory
    writer->setNextMatrix(mat);
    // TODO... move this "delete" to the destructor...!
//    delete mat;
  } else {
    //printf("ERR: audio data was lost! increase data memory buffersize of cPortaudioSource components!\n");
  }

  smileMutexUnlock(obj->callbackMtx);
  return paContinue;
}


int cPortaudioSource::startRecording()
{
    PaStreamParameters  inputParameters;
    PaError             err = paNoError;
//paFrames=audio
    // ????? //if (paFrames == 0) return 0;
#ifdef HAVE_PORTAUDIO_V19
    if (Pa_IsStreamActive( stream ) == 1) {
#else
    if (Pa_StreamActive( stream ) == 1) {
#endif
      SMILE_WRN(2,"portAudio stream is already active (in startRecording).");
      return 1;
    }

    if (deviceId < 0) {
      inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    } else {
      inputParameters.device = deviceId;
    }
//

    SMILE_MSG(2,"recording from portAudio device with index %i",inputParameters.device);
    inputParameters.channelCount = channels;
    switch (nBits) {
      case 8: inputParameters.sampleFormat = paInt8; break;
      case 16: inputParameters.sampleFormat = paInt16; break;
      case 24: inputParameters.sampleFormat = paInt24; break;
      case 32: inputParameters.sampleFormat = paInt32; break;
      case 33: inputParameters.sampleFormat = paFloat32; break;
      default:
        COMP_ERR("invalid number of bits requested: %i (allowed: 8, 16, 24, 32, 33(for 23-bit float))\n",nBits);
    }

#ifdef HAVE_PORTAUDIO_V19
    const PaDeviceInfo * info = Pa_GetDeviceInfo( inputParameters.device );
    if (info != NULL) {
      inputParameters.suggestedLatency =
        info->defaultLowInputLatency;
    } else {
      inputParameters.suggestedLatency = 0;
    }
    inputParameters.hostApiSpecificStreamInfo = NULL;
#endif

//    obj->bufWrapper  = pcmBufferMakeWrapper( obj->bufWrapper, 0, 1, SAMPLETYPE_I16, 0, 0, NULL);
    /* Setup recording stream -------------------------------------------- */
    err = Pa_OpenStream(
              &(stream),
#ifdef HAVE_PORTAUDIO_V19
              &inputParameters,
              NULL,                  /* &outputParameters, */
#else // V18 (old API)
              inputParameters.device,
              inputParameters.channelCount,
              inputParameters.sampleFormat,
              NULL,
              paNoDevice, 0, 0, NULL,
#endif
              sampleRate,
              audioBuffersize,
#ifndef HAVE_PORTAUDIO_V19
              1,
#endif
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paSource_recordCallback,
              (void*) this );

    if( err != paNoError ) {
        COMP_ERR("error opening portaudio recording stream (code %i) \n  check samplerate(-> %i , maybe it is not supported?) \n  maybe incorrect device? (\"listDevices=1\" in config file displays a list of devices)\n",err,sampleRate);
    }

	smileMutexLock(callbackMtx);
    err = Pa_StartStream( stream );
    if( err != paNoError ) COMP_ERR("cannot start portaudio stream (code %i)\n",err);

//    obj->absTimeStart = liveInput_getTime();
    streamStatus = PA_STREAM_STARTED;
    smileMutexUnlock(callbackMtx);

    return 1;
}

int cPortaudioSource::stopRecording()
{
  PaError             err = paNoError;
  if (streamStatus == PA_STREAM_STOPPED) return 0;

  streamStatus =  PA_STREAM_STOPPED;
  // TODO: timestamp!
//  absTimeStop = liveInput_getTime();

  err = Pa_CloseStream( stream );
  if( err != paNoError ) {
    SMILE_ERR(1,"cannot close portaudio stream (code %i)\n",err);
    return 0;
  }

  return 1;
}

int cPortaudioSource::stopRecordingWait()
{
  PaError             err = paNoError;
  if (streamStatus == PA_STREAM_STOPPED) return 0;

  streamStatus =  PA_STREAM_STOPPED;
  // TODO: timestamp!
//  absTimeStop = liveInput_getTime();

  err = Pa_StopStream( stream );
  if( err != paNoError ) {
    SMILE_ERR(1,"cannot close portaudio stream (code %i)\n",err);
    return 0;
  }

  return 1;
}

void cPortaudioSource::printDeviceList(void)
{

//  PaError err = Pa_Initialize();
//  if( err != paNoError ) {
//    COMP_ERR("error initialising portaudio library (code %i)\n",err);
//    return 0;
//  }

  // query devices:
  SMILE_PRINT("== cPortAudioSource ==  There are %i audio devices:", numDevices );
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

//  err = Pa_Terminate();
//  if( err != paNoError ) {
//    SMILE_ERR(2,"PortAudio error (Pa_Terminate): %s\n", Pa_GetErrorText( err ) );
//  }

  return;
}

#endif // HAVE_PORTAUDIO
