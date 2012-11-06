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

OBSOLETE windowframer

*/


#ifndef __WINDOWFRAMER_HPP
#define __WINDOWFRAMER_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWINDOWFRAMER "creates frames from stream input and applies window function to frames"
#define COMPONENT_NAME_CWINDOWFRAMER "cWindowFramer"

// WINF_XXXXXX constants are defined in smileCommon.hpp !
#undef class
class DLLEXPORT cWindowFramer : public cDataProcessor {
  private:
    double frameSize, frameStep;
    double offset, gain;
    double sigma, alpha, alpha0, alpha1, alpha2, alpha3;
    long  frameSizeFrames, frameStepFrames;
    int   fsfGiven;   // flag that indicates whether frameSizeFrame, etc. was specified directly (to override frameSize in seconds)
    int   fstfGiven;   // flag that indicates whether frameStepFrame, etc. was specified directly (to override frameStep in seconds)
    int   winFunc;    // winFunc as numeric constant (see #defines above)
    int   dtype;     // data type (DMEM_FLOAT, DMEM_INT)
    double *win;
    
    void precomputeWinFunc();
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(const sDmLevelConfig *c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWindowFramer(const char *_name);

    virtual ~cWindowFramer();
};




#endif // __WINDOWFRAMER_HPP
