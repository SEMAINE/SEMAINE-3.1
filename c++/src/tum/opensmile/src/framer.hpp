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

dataFramer

*/


#ifndef __CFRAMER_HPP
#define __CFRAMER_HPP

#include <smileCommon.hpp>
#include <winToVecProcessor.hpp>

#define COMPONENT_DESCRIPTION_CFRAMER "This component creates frames from single dimensional input stream. It is possible to specify the frame step and frame size independently, thus allowing for overlapping frames or non continuous frames."
#define COMPONENT_NAME_CFRAMER "cFramer"

#undef class
class DLLEXPORT cFramer : public cWinToVecProcessor {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    //virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    virtual int getMultiplier();
    //virtual int configureWriter(const sDmLevelConfig *c);
    //virtual int setupNamesForField(int idxi, const char*name, long nEl);
    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFramer(const char *_name);

    virtual ~cFramer();
};




#endif // __CFRAMER_HPP
