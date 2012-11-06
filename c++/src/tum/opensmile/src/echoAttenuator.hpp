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

AEC
*/


#ifndef __CECHOATTENUATOR_HPP
#define __CECHOATTENUATOR_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CECHOATTENUATOR "Experimental: pseudo echo cancellation by attenuating input based on output envelope"
#define COMPONENT_NAME_CECHOATTENUATOR "cEchoAttenuator"

#undef class
class DLLEXPORT cEchoAttenuator : public cDataProcessor {
  private:
    cDataReader * readerOutp;
    cMatrix * matnew;

    double *Cxy, *Cyy;
    double outEnv;
    double alpha, envWeight, modThresh, fixedDelay;

    double maxdelay, mindelay, TT;
    double filterSizeS, blocksizeS;
    long maxdelayF,mindelayF,lastdelay;  // delays in frames
    long filterSize,blocksize;  // size of echo FIR filter


  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();
    virtual void mySetEnvironment();

    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);

   // buffer must include all (# order) past samples
    //virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );
    
/*
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
*/
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cEchoAttenuator(const char *_name);

    virtual ~cEchoAttenuator();
};




#endif // __CECHOATTENUATOR_HPP
