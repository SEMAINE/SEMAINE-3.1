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


#ifndef __CWAVESINK_HPP
#define __CWAVESINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>
#include <waveSource.hpp>  // include wave header and wave parameters structs

#define COMPONENT_DESCRIPTION_CWAVESINK "This component saves data to an uncompressed PCM WAVE file"
#define COMPONENT_NAME_CWAVESINK "cWaveSink"

#undef class
class DLLEXPORT cWaveSink : public cDataSink {
private:
  const char *filename;
  FILE * fHandle;
  //int lag;
  int frameRead;
  int buffersize;
  void *sampleBuffer; long sampleBufferLen;

  int nBitsPerSample;
  int nBytesPerSample;
  int sampleFormat;
  int nChannels;

  //double start, end, endrel;
  //long startSamples, endSamples, endrelSamples;

  long curWritePos;   // in samples??
  long nBlocks;
  //int eof;

  int writeWaveHeader();
  int writeData(cMatrix *m=NULL);

protected:
  SMILECOMPONENT_STATIC_DECL_PR

  virtual void fetchConfig();
  //virtual int myConfigureInstance();
  virtual int myFinaliseInstance();
  virtual int myTick(long long t);

  virtual int configureReader();

public:
  SMILECOMPONENT_STATIC_DECL

    cWaveSink(const char *_name);

  virtual ~cWaveSink();
};




#endif // __CWAVESINK_HPP
