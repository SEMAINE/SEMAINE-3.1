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

volume analyser. checks for clipped samples and other problems in the audio

*/


#ifndef __CVOLANALYSE_HPP
#define __CVOLANALYSE_HPP

#include <smileCommon.hpp>
#include <winToVecProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVOLANALYSE "volume analyser, several sanity checks"
#define COMPONENT_NAME_CVOLANALYSE "cVolanalyse"

typedef struct {
  long nPeaks;
  long nClips;
  long maxClipLen;
  long maxFakeClipLen;
  long nFakeClips;
  long clip, fclip;
  
  FLOAT_DMEM lastVal, lastlastVal;
  FLOAT_DMEM offset,  Noffset;
  
  FLOAT_DMEM maxDelta;
  FLOAT_DMEM firstAbsVal; int firstSet;
  FLOAT_DMEM maxAbsVal;
  
  int nFirst, nLast;
  FLOAT_DMEM meanFirst, meanLast;
  
  long lastNT;
  
  int resFlushed;
} sVolanalyseData;

#define N_RETVALS 12

#undef class
class DLLEXPORT cVolanalyse : public cWinToVecProcessor {
  private:
    sVolanalyseData *vaData;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();
    virtual int myFinaliseInstance();
    
    virtual int getMultiplier() { return N_RETVALS;}
    virtual int setupNamesForElement(int i, const char*name, long nEl);
    virtual int doProcess(int idxi, cMatrix *row, FLOAT_DMEM*y);
    virtual int doFlush(int i, FLOAT_DMEM*x);  
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVolanalyse(const char *_name);

    virtual ~cVolanalyse();
};




#endif // __CVOLANALYSE_HPP
