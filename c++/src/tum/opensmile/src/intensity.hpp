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

compute simplified intensity according to :
 Andreas Kieﬂling - Extraktion und Klassifikation prosodischer Merkmale in der automatischen Sprachverarbeitung
 Pg. 156-157

*/


#ifndef __CINTENSITY_HPP
#define __CINTENSITY_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CINTENSITY "This component computes simplified frame intensity (narrow band approximation). IMPORTANT: It expects UNwindowed raw PCM frames as input!! A Hamming window is internally applied and the resulting signal is squared bevore applying loudness compression, etc."
#define COMPONENT_NAME_CINTENSITY "cIntensity"

#undef class
class DLLEXPORT cIntensity : public cVectorProcessor {
  private:
    double I0;
    double *hamWin;
    long nWin;
    double winSum;

    int intensity, loudness;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(const sDmLevelConfig *c);

    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cIntensity(const char *_name);

    virtual ~cIntensity();
};




#endif // __CINTENSITY_HPP
