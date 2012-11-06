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

Audio fingerprinter (e.g. for music recordings, etc.)
saves compact feature data (melspec) of 
  beginning, end and middle (to determine shifts)
  100 averages over 100 parts of song (song divided into 100 parts)
  overall average spectrum (fft)
  rms energy (average)
  
*/


#ifndef __CFINGERPRINT_HPP
#define __CFINGERPRINT_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CFINGERPRINT "Audio fingerprinter (experimental, not supported, over simple approach)"
#define COMPONENT_NAME_CFINGERPRINT "cFingerprint"

#define N_MEL 26
#define N_FFT 512
#define WINLEN 100
#define MID   5000   // 50 * 100 frames @ 10ms frameperiod
#define MELDUMP_BLOCK  40000

typedef struct {
  int resFlushed;
  int idxi;
  long cur;
  int nMeldump;  // number of allocated mel frames
  float *meldump; // dirty hack to save mel data
  // output data vector starts below... dump this to a file using data-dump sink
  float nFrames;  // number of frames in input
  float nMel;   // = N_MEL
  float winLen;  // = WINLEN
  float nAvgFrames; //  always 100...
  float nFft;      // = N_FFT
  float melBeg[N_MEL*WINLEN];  // 100 frames (every 3rd 10ms frame over 3s at beginning)
  float melMid[N_MEL*WINLEN];  // 100 frames (every 3rd 10ms frame over 3s in the middle) ??
  float melEnd[N_MEL*WINLEN];  // 100 frames (every 3rd 10ms frame over last 3s)
  float melAvg[N_MEL*100];  // 100 average frames to resemble whole song
  float fftAvg[N_FFT];
  float energyAvg;  // average energy computed from melpsec sum
} sFingerprinterData;

#define N_RETVALS 12

#undef class
class DLLEXPORT cFingerprint : public cVectorProcessor {
  private:
    const char *filename;
    sFingerprinterData *fpData;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();
    virtual int myFinaliseInstance();
    
    //virtual int getMultiplier() { return N_RETVALS;}
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int flushVectorFloat(FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFingerprint(const char *_name);

    virtual ~cFingerprint();
};




#endif // __CFINGERPRINT_HPP
