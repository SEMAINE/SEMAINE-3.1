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

experimental resampling by ideal fourier interpolation and band limiting
this component takes a complex (!) dft spectrum (generated from real values) as input

*/


#ifndef __CSPECRESAMPLE_HPP
#define __CSPECRESAMPLE_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CSPECRESAMPLE "This component implements a spectral domain resampling component. Input frames are complex valued spectral domain data, which will be shifted and scaled by this component, and a modified DFT is performed to synthesize samples at the new rate."
#define COMPONENT_NAME_CSPECRESAMPLE "cSpecResample"

#undef class
class DLLEXPORT cSpecResample : public cVectorProcessor {
  private:
    int antiAlias;
    long kMax;
    double sr;
    double fsSec;
    double targetFs;
    double resampleRatio;
    long _Nin, _Nout;
    FLOAT_DMEM *inData;
    const char * inputFieldPartial;

    sDftWork * dftWork;
    //FLOAT_DMEM *costable, *sintable;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    virtual int configureWriter(sDmLevelConfig &c);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSpecResample(const char *_name);
    virtual ~cSpecResample();
};




#endif // __CSPECRESAMPLE_HPP
