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

This component computes pitch via the Harmonic Product Spectrum method.
As input it requires FFT magnitude data. 
Note that the type of input data is not checked, thus be careful when writing your configuration files!

*/


#ifndef __CPITCHBASE_HPP
#define __CPITCHBASE_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CPITCHBASE "Base class for all pitch classes, no functionality on its own!"
#define COMPONENT_NAME_CPITCHBASE "cPitchBase"


#undef class
class DLLEXPORT cPitchBase : public cVectorProcessor {
  private:
    const char *inputFieldPartial;
    int nCandidates;
    int scores, voicing;
    int F0C1, voicingC1;
    int F0raw;
    int voicingClip;

    double maxPitch, minPitch;

    FLOAT_DMEM *inData;
    FLOAT_DMEM *f0cand, *candVoice, *candScore;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    FLOAT_DMEM voicingCutoff;
    int octaveCorrection;

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    // to be overwritten by child class:
    virtual int pitchDetect(FLOAT_DMEM * inData, long _N, double _fsSec, double baseT, FLOAT_DMEM *f0cand, FLOAT_DMEM *candVoice, FLOAT_DMEM *candScore, long nCandidates);
    virtual int addCustomOutputs(FLOAT_DMEM *dstCur, long NdstLeft);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchBase(const char *_name);
    
    virtual ~cPitchBase();
};




#endif // __CPITCHBASE_HPP
