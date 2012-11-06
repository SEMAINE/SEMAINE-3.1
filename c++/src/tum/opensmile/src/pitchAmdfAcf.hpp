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


#ifndef __CPITCHAMDFACF_HPP
#define __CPITCHAMDFACF_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CPITCHAMDFACF "UNSUPPORTED and INCOMPLETE component. This component computes pitch via the Harmonic Product Spectrum method. As input it requires FFT magnitude data. Note that the type of input data is not checked, thus be careful when writing your configuration files!"
#define COMPONENT_NAME_CPITCHAMDFACF "cPitchAmdfAcf"

#undef class
class DLLEXPORT cPitchAmdfAcf : public cVectorProcessor {
  private:
    int HNR;
	  int F0raw;
    int hpsVecMean, hpsVec;
    int nHarmonics;

    long nMag, magStart, N;  // N is the number of elements in the hps spectrum  (nMag/nHarmonics)
	  int onsFlag;
    double maxPitch, minPitch;
  	double voicingCutoff;
  	//FLOAT_DMEM lastPitch, lastlastPitch, glMeanPitch, pitchEnv;
	  float fsSec;

    FLOAT_DMEM *work;
    char *workB;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchAmdfAcf(const char *_name);
    
    long hpsPitchPeak(const FLOAT_DMEM *_hps, long N, long start, long end);

    virtual ~cPitchAmdfAcf();
};




#endif // __CPITCHAMDFACF_HPP
