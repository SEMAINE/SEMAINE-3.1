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

waveSource : reads PCM WAVE files (RIFF format)

Support for a negative start index was added by Benedikt Gollan (TUM).

*/


#ifndef __WAVE_SOURCE_HPP
#define __WAVE_SOURCE_HPP

#include <smileCommon.hpp>
#include <dataSource.hpp>

#define COMPONENT_DESCRIPTION_CWAVESOURCE "This component reads an uncompressed RIFF (PCM-WAVE) file and saves it as a stream to the data memory. For most feature extraction tasks you will now require a cFramer component."
#define COMPONENT_NAME_CWAVESOURCE "cWaveSource"

/*
#define BYTEORDER_LE    0
#define BYTEORDER_BE    1
#define MEMORGA_INTERLV 0
#define MEMORGA_SEPCH   1

typedef struct {
  long sampleRate;
  int sampleType;
  int nChan;
  int blockSize;
  int nBPS;       // actual bytes per sample
  int nBits;       // bits per sample (precision)
  int byteOrder;  // BYTEORDER_LE or BYTEORDER_BE
  int memOrga;    // MEMORGA_INTERLV  or MEMORGA_SEPCH
  long nBlocks;  // nBlocks in buffer
} sWaveParameters;
*/

#undef class
class DLLEXPORT cWaveSource : public cDataSource {
  private:
    const char *filename;
    //long buffersize;
    FILE *filehandle;
    sWaveParameters pcmParam;

    int negativestart;
    long negstartoffset;
    double start, end, endrel;
    long startSamples, endSamples, endrelSamples;
    
    int monoMixdown;    // if set to 1, multi-channel files will be mixed down to 1 channel
    long pcmDataBegin;  // in bytes
    long curReadPos;   // in samples
    int eof;

    int readWaveHeader();
    int readData(cMatrix *m=NULL);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWaveSource(const char *_name);

    virtual ~cWaveSource();
};




#endif // __EXAMPLE_SOURCE_HPP
