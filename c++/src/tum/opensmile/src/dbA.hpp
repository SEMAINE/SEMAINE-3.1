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


/*  openSMILE component: dbA

applies dbX weighting to fft magnitudes

*/


#ifndef __CDBA_HPP
#define __CDBA_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>
#include <math.h>

#define COMPONENT_DESCRIPTION_CDBA "This component performs dbX (dbA,dbB,dbC,...) equal loudness weighting of FFT bin magnitudes. Currently only dbA weighting is implemented."
#define COMPONENT_NAME_CDBA "cDbA"

#define CURVE_DBA 0
#define CURVE_DBB 1
#define CURVE_DBC 2

extern void computeDBA(FLOAT_DMEM *x, long blocksize, FLOAT_DMEM F0);

#undef class
class DLLEXPORT cDbA : public cVectorProcessor {
  private:
    int curve;
    int usePower;
    FLOAT_DMEM **filterCoeffs;
    
    int computeFilters( long blocksize, double frameSizeSec, int idxc );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(const sDmLevelConfig *c);

    virtual int dataProcessorCustomFinalise();
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cDbA(const char *_name);

    virtual ~cDbA();
};




#endif // __CDBA_HPP
