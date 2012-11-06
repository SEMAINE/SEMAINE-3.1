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

waveSink:
writes data to an uncompressed PCM WAVE file

*/


#ifndef __CWAVESINKCUT_HPP
#define __CWAVESINKCUT_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>
#include <waveSource.hpp>  // include wave header and wave parameters structs

#define COMPONENT_DESCRIPTION_CWAVESINKCUT "This component writes data to uncompressed PCM WAVE files. Only chunks, based on timings received via smile messages are written to files. The files may have consecutive numbers appended to the file name."
#define COMPONENT_NAME_CWAVESINKCUT "cWaveSinkCut"

#undef class
class DLLEXPORT cWaveSinkCut : public cDataSink {
  private:
    const char *fileExtension;
    const char *filebase;
    const char *fileNameFormatString;
    int multiOut;
    long curFileNr;

    long preSil, postSil;
    int turnStart, turnEnd, nPre, nPost, vIdxStart, vIdxEnd, curVidx;
    long curStart, curEnd;
    int isTurn, endWait;

    FILE * fHandle;
    void *sampleBuffer; long sampleBufferLen;

  	int nBitsPerSample;
	  int nBytesPerSample;
	  int sampleFormat;
    int nChannels;
    long fieldSize;
	
    long nOvl;

  	long curWritePos;   // in samples??
	  long nBlocks;

    int writeWaveHeader();
    int writeDataFrame(cVector *m=NULL);
    char * getCurFileName();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWaveSinkCut(const char *_name);

    virtual ~cWaveSinkCut();
};




#endif // __CWAVESINKCUT_HPP
