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


#ifndef __CPITCHSHS_HPP
#define __CPITCHSHS_HPP

#include <smileCommon.hpp>
#include <pitchBase.hpp>

#define COMPONENT_DESCRIPTION_CPITCHSHS "This component computes the fundamental frequency via the Sub-Harmonic-Sampling (SHS) method (this is related to the Harmonic Product Spectrum method)."
#define COMPONENT_NAME_CPITCHSHS "cPitchShs"


#undef class
class DLLEXPORT cPitchShs : public cPitchBase {
  private:
    int nHarmonics;
    FLOAT_DMEM Fmint, Fstept;
    FLOAT_DMEM nOctaves, nPointsPerOctave;
    FLOAT_DMEM *SS;
    FLOAT_DMEM *Fmap;
    FLOAT_DMEM compressionFactor;
    double base;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int setupNewNames(long nEl);
    
    // to be overwritten by child class:
    virtual int pitchDetect(FLOAT_DMEM * inData, long _N, double _fsSec, double baseT, FLOAT_DMEM *f0cand, FLOAT_DMEM *candVoice, FLOAT_DMEM *candScore, long nCandidates);
    virtual int addCustomOutputs(FLOAT_DMEM *dstCur, long NdstLeft);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchShs(const char *_name);
    
    virtual ~cPitchShs();
};




#endif // __CPITCHSHS_HPP
