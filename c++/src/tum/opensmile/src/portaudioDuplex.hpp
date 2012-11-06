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

portAudio dataSink for live audio playback
(?? known to work on windows, linux, and mac)

*/


#ifndef __CPORTAUDIODUPLEX_HPP
#define __CPORTAUDIODUPLEX_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#ifdef HAVE_PORTAUDIO

#include <portaudio.h>

#define COMPONENT_DESCRIPTION_CPORTAUDIODUPLEX "dataProcessor for full-duplex playback and recording of live audio using PortAudio library"
#define COMPONENT_NAME_CPORTAUDIODUPLEX "cPortaudioDuplex"


#define PA_STREAM_STOPPED 0
#define PA_STREAM_STARTED 1

#undef class
class DLLEXPORT cPortaudioDuplex : public cDataProcessor {
  private:
    PaStream *stream;
    long paFrames;
    int deviceId;
    int streamStatus;
    int listDevices;
    int numDevices;
    int lastDataCount;
    int isPaInit;
    
    long audioBuffersize;
    double audioBuffersize_sec;
    //long mBuffersize;    

    int monoMixdownPB, monoMixdownREC;    // if set to 1, multi-channel files will be mixed down to 1 channel

    int eof, abort; // NOTE : when setting abort, first lock the callbackMtx!!!
    int channels, sampleRate, nBits, nBPS;
    
    //int setupDevice();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);


    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    // variables for the callback method:
    //smileMutex dataFlagMtx;
    smileMutex callbackMtx;
    smileCond  callbackCond;
    int dataFlagR, dataFlagW;
    cMatrix *callbackMatrix;
    
    cPortaudioDuplex(const char *_name);

    void printDeviceList();

    int startDuplex();
    int stopDuplex();
    int stopDuplexWait();
    
    cDataReader * getReader() { return reader; }
    cDataWriter * getWriter() { return writer; }

    /*void setReadDataFlag() {
      smileMutexLock(dataFlagMtx);
      dataFlag = 1;
      //lastDataCount=0;
      smileMutexUnlock(dataFlagMtx);
    }*/

    int getNBPS() { return nBPS; }
    int getNBits() { return nBits; }
    int getChannels() { return channels; }
    int getSampleRate() { return sampleRate; }
    
    int isAbort() { return abort; }
    int isMonoMixdownPB() { return monoMixdownPB; }
    int isMonoMixdownREC() { return monoMixdownREC; }
    
    virtual ~cPortaudioDuplex();
};


#endif // HAVE_PORTAUDIO


#endif // __CPORTAUDIODUPLEX_HPP
