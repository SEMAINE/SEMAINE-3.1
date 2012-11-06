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

computes spectral features such as flux, roll-off, centroid, etc.

*/


#ifndef __CSPECTRAL_HPP
#define __CSPECTRAL_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CSPECTRAL "This component computes spectral features such as flux, roll-off, centroid, and user defined band energies (rectangular summation of FFT magnitudes), etc."
#define COMPONENT_NAME_CSPECTRAL "cSpectral"

#undef class
class DLLEXPORT cSpectral : public cVectorProcessor {
  private:
    int squareInput, flux, centroid, nBands, nRollOff, entropy;
    int variance, skewness, kurtosis, slope;
    long nScale;
    int maxPos, minPos;
    long *bandsL, *bandsH; // frequencies
    double *bandsLi, *bandsHi; // indicies (fft bins), real values
    double *wghtLi, *wghtHi; // indicies (fft bins), real values
    double *rollOff;
    double fsSec;
    const double *frq;
    
    int isBandValid(long start, long end)
    {
      if ((start>=0)&&(end>0)) return 1;
      else return 0;
    }
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();

    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);


  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSpectral(const char *_name);

    virtual ~cSpectral();
};




#endif // __CSPECTRAL_HPP
