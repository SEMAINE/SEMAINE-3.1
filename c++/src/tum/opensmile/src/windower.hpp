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

data windower.
takes frames from one level, applies window function, and saves to other level

*/


#ifndef __WINDOWER_HPP
#define __WINDOWER_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWINDOWER "This component applies applies window function to pcm frames."
#define COMPONENT_NAME_CWINDOWER "cWindower"

// WINF_XXXXXX constants are defined in smileUtil.hpp !
#undef class
class DLLEXPORT cWindower : public cVectorProcessor {
  private:
    double offset, gain;
    double sigma, alpha, alpha0, alpha1, alpha2, alpha3;
    long  frameSizeFrames;
    int   winFunc;    // winFunc as numeric constant (see #defines above)
    int   dtype;     // data type (DMEM_FLOAT, DMEM_INT)
    double *win;
    
    void precomputeWinFunc();
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int myFinaliseInstance();

    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWindower(const char *_name);

    virtual ~cWindower();
};




#endif // __WINDOWER_HPP
