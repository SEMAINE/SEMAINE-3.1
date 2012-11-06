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

functionals container


passes unsorted row data array AND (if required) sorted row data array to functional processors

*/


#ifndef __CFUNCTIONALSVECTOVEC_HPP
#define __CFUNCTIONALSVECTOVEC_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>
#include <functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALSVECTOVEC "compute functionals from framewise input (vectors) instead of contours (over time..)"
#define COMPONENT_NAME_CFUNCTIONALSVECTOVEC "cFunctionalsVecToVec"

#undef class
class DLLEXPORT cFunctionalsVecToVec : public cVectorProcessor {
  private:
    int nFunctTp, nFunctTpAlloc;  // number of cFunctionalXXXX types found
    char **functTp;  // type names (without cFunctional prefix)
    int *functTpI;   // index of cFunctionalXXXX type in componentManager
    int *functI;   // index of cFunctionalXXXX type in componentManager
    int *functN;   // number of output values of each functional object
    cFunctionalComponent **functObj;
    int requireSorted;
    
  protected:
    int nFunctionalsEnabled;
    int nFunctValues;  // size of output vector

    SMILECOMPONENT_STATIC_DECL_PR
    static int rAcounter;

//    virtual void fetchConfig();
    virtual int myConfigureInstance();

    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

//    virtual int getMultiplier();
    //virtual int configureWriter(const sDmLevelConfig *c);
    virtual int setupNamesForField(int idxi, const char*name, long nEl);

//    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
//    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
//    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalsVecToVec(const char *_name);

    virtual ~cFunctionalsVecToVec();
};




#endif // __CFUNCTIONALSVECTOVEC_HPP
