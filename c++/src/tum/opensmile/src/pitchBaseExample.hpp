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


#ifndef __CPITCHBASEEXAMPLE_HPP
#define __CPITCHBASEEXAMPLE_HPP

#include <smileCommon.hpp>
#include <pitchBase.hpp>

#define COMPONENT_DESCRIPTION_CPITCHBASEEXAMPLE "Base class for all pitch classes, no functionality on its own!"
#define COMPONENT_NAME_CPITCHBASEEXAMPLE "cPitchBaseExample"


#undef class
class DLLEXPORT cPitchBaseExample : public cPitchBase {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
/*	  virtual int setupNewNames(long nEl); */
    
    // to be overwritten by child class:
    virtual int pitchDetect(FLOAT_DMEM * _inData, long _N, double _fsSec, double _baseT, FLOAT_DMEM *_f0cand, FLOAT_DMEM *_candVoice, FLOAT_DMEM *_candScore, long _nCandidates);
    virtual int addCustomOutputs(FLOAT_DMEM *dstCur, long NdstLeft);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchBaseExample(const char *_name);
    
    virtual ~cPitchBaseExample();
};




#endif // __CPITCHBASEEXAMPLE_HPP
