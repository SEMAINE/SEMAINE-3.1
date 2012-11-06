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

example for dataProcessor descendant

*/


#ifndef __CPITCHJITTER_HPP
#define __CPITCHJITTER_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CPITCHJITTER "This component computes Voice Quality parameters Jitter (pitch period deviations) and Shimmer (pitch period amplitude deviations). It requires the raw PCM frames and the corresponding fundamental frequency (F0) as inputs."
#define COMPONENT_NAME_CPITCHJITTER "cPitchJitter"

#undef class
class DLLEXPORT cPitchJitter : public cDataProcessor {
  private:
    int onlyVoiced;
    int jitterLocal, jitterDDP, shimmerLocal;
    int jitterLocalEnv, jitterDDPEnv, shimmerLocalEnv;
    int periodLengths, periodStarts;
    long F0fieldIdx;
    long lastIdx, lastMis;

    double searchRangeRel;
    double minF0;
    const char * F0field;
    cDataReader *F0reader;
    cVector *out;
    long Nout;

    // jitter data memory:
    FLOAT_DMEM lastT0;
    FLOAT_DMEM lastDiff;
    FLOAT_DMEM lastJitterDDP, lastJitterLocal;
    FLOAT_DMEM lastJitterDDP_b, lastJitterLocal_b;

    // shimmer data memory:
    FLOAT_DMEM lastShimmerLocal;
    FLOAT_DMEM lastShimmerLocal_b;

    double crossCorr(FLOAT_DMEM *x, long Nx, FLOAT_DMEM *y, long Ny);
    FLOAT_DMEM amplitudeDiff(FLOAT_DMEM *x, long Nx, FLOAT_DMEM *y, long Ny, double *maxI0, double *maxI1, FLOAT_DMEM *_A0, FLOAT_DMEM *_A1);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    // virtual int dataProcessorCustomFinalise();
    virtual int setupNewNames(long nEl);
    // virtual int setupNamesForField();
    virtual int configureReader();
    virtual int configureWriter(sDmLevelConfig &c);


  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchJitter(const char *_name);

    virtual ~cPitchJitter();
};




#endif // __CPITCHJITTER_HPP
