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

LPC, compute LPC coefficients from wave data (PCM) frames

*/


#ifndef __CFORMANTLPC_HPP
#define __CFORMANTLPC_HPP


#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#ifdef HAVE_GSL_ZSOLVE

#define COMPONENT_DESCRIPTION_CFORMANTLPC "This component computes formant frequencies and bandwidths by solving for the roots of the LPC polynomial. The formant trajectories can and should be smoothed by the cFormantSmoother component."
#define COMPONENT_NAME_CFORMANTLPC "cFormantLpc"

#define LPC_METHOD_ACF   0
//#define LPC_METHOD_ACF2   1
#define LPC_METHOD_BURG   5


#undef class
class DLLEXPORT cFormantLpc : public cVectorProcessor {
  private:
    int nFormants;
    int saveFormants;
    int saveIntensity;
    int saveBandwidths;
    int saveNumberOfValidFormants;
    int useLpSpec;
    int nSmooth;
    int medianFilter, octaveCorrection;

    int nLpc;
    long lpcCoeffIdx;
    long lpcGainIdx;
    long lpSpecIdx, lpSpecN;

    double minF, maxF;
    
    double T;
    double *lpc, *roots;
    double *formant, *bandwidth;

    // int calcFormantLpcBurg (FLOAT_DMEM x[], long n, FLOAT_DMEM a[], int m, FLOAT_DMEM *xms);

    // calcFormantLpc returns the lpc gain (error?)
    //FLOAT_DMEM calcFormantLpcAcf(FLOAT_DMEM * acf, int _p, FLOAT_DMEM *lpc, FLOAT_DMEM *refl=NULL);
    FLOAT_DMEM calcFormantLpc(const FLOAT_DMEM *x, long Nsrc, FLOAT_DMEM * lpc, long nCoeff, FLOAT_DMEM *refl);

    //void autoCorrF(const FLOAT_DMEM *x, FLOAT_DMEM *acf, int n, int lag);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int setupNewNames(long nEl);
    virtual void findInputFields();
    //virtual int myFinaliseInstance();

    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFormantLpc(const char *_name);

    virtual ~cFormantLpc();
};

#endif // HAVE_GSL_ZSOLVE

#endif // __CFORMANTLPC_HPP
