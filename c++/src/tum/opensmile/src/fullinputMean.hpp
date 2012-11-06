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

computes mean of full input ?

*/


#ifndef __CFULLINPUTMEAN_HPP
#define __CFULLINPUTMEAN_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CFULLINPUTMEAN "This component performs mean normalizing on a data series. A 2-pass analysis of the data is performed, which makes this component unusable for on-line analysis. In the first pass, no output is produced and the mean value (over time) is computed for each input element. In the second pass the mean vector is subtracted from all input frames, and the result is written to the output dataMemory level. Attention: Due to the 2-pass processing the input level must be large enough to hold the whole data sequence."
#define COMPONENT_NAME_CFULLINPUTMEAN "cFullinputMean"

#undef class
class DLLEXPORT cFullinputMean : public cDataProcessor {
  private:
    int eNormMode;
    int flag; //, htkcompatible; long idx0;
    cVector *means;
    long nMeans;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    //virtual int configureWriter(sDmLevelConfig &c);

    //virtual void configureField(int idxi, long __N, long nOut);
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    //virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFullinputMean(const char *_name);

    virtual ~cFullinputMean();
};




#endif // __CFULLINPUTMEAN_HPP
