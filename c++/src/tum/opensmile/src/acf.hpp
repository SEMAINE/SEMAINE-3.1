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

Autocorrelation Function (ACF)

*/


#ifndef __CACF_HPP
#define __CACF_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>
#include <fftXg.h>
#include <math.h>

#define COMPONENT_DESCRIPTION_CACF "This component computes the autocorrelation function (ACF) by sqauring a magnitude spectrum and applying an inverse Fast Fourier Transform. This component mus read from a level containing *only* FFT magnitudes in a single field. Use the 'cTransformFFT' and 'cFFTmagphase' components to compute the magnitude spectrum from PCM frames. Computation of the Cepstrum is also supported (this applies a log() function to the magnitude spectra)."
#define COMPONENT_NAME_CACF "cAcf"

#undef class
class DLLEXPORT cAcf : public cVectorProcessor {
  private:
    int usePower, cepstrum;
    FLOAT_DMEM **data, **w;
    int **ip;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
//    virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    virtual int dataProcessorCustomFinalise();

//    virtual void configureField(int idxi, long __N, long nOut);
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cAcf(const char *_name);

    virtual ~cAcf();
};




#endif // __CACF_HPP
