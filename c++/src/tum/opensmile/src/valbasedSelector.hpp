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

vector selector based on threshold of value <idx>

*/


#ifndef __CVALBASEDSELECTOR_HPP
#define __CVALBASEDSELECTOR_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVALBASEDSELECTOR "This component copies only those frames from the input to the output that match a certain threshold criterion, i.e. where a specified value N exceeds a certain threshold."
#define COMPONENT_NAME_CVALBASEDSELECTOR "cValbasedSelector"

#undef class
class DLLEXPORT cValbasedSelector : public cDataProcessor {
  private:
    long idx;
    int removeIdx, invert, allowEqual;
    FLOAT_DMEM threshold;
    cVector *myVec;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    // virtual int dataProcessorCustomFinalise();
    // virtual int setupNewNames(long nEl);
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int configureWriter(sDmLevelConfig &c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cValbasedSelector(const char *_name);

    virtual ~cValbasedSelector();
};




#endif // __CVALBASEDSELECTOR_HPP
