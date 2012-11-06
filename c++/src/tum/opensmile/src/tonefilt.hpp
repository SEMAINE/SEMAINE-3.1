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

on-line semi-tone filter bank

*/


#ifndef __CTONEFILT_HPP
#define __CTONEFILT_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CTONEFILT "This component implements an on-line, sample by sample semi-tone filter bank which can be used as first step for the computation of CHROMA features as a replacement of cTonespec. The filter is based on correlating with a sine wave of the exact target frequency of a semi-tone for each note in the filter-bank."
#define COMPONENT_NAME_CTONEFILT "cTonefilt"

#undef class
class DLLEXPORT cTonefilt : public cDataProcessor {
  private:
    double outputPeriod; /* in seconds */
    long  /*outputBuffersize,*/ outputPeriodFrames; /* in frames */
    int nNotes;
    double firstNote;
    long N, Nf;
    double inputPeriod;
    double decayFN, decayF0;
    
    cVector *tmpVec;
    FLOAT_DMEM *tmpFrame;

    double **corrS, **corrC;
    double *decayF;
    double *freq;
    long * pos;

    void doFilter(int i, cMatrix *row, FLOAT_DMEM*x);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);
      
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cTonefilt(const char *_name);

    virtual ~cTonefilt();
};




#endif // __CTONEFILT_HPP
