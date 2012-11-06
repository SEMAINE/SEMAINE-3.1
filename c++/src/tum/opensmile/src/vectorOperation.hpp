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

do elementary operations on vectors 
(i.e. basically everything that does not require history or context,
 everything that can be performed on single vectors w/o external data (except for constant parameters, etc.))

*/


#ifndef __CVECTOROPERATION_HPP
#define __CVECTOROPERATION_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVECTOROPERATION "This component performs elementary operations on vectors (i.e. basically everything that does not require history or context, everything that can be performed on single vectors w/o external data (except for constant parameters, etc.))"
#define COMPONENT_NAME_CVECTOROPERATION "cVectorOperation"

#define VOP_NORMALISE 0
#define VOP_ADD       1
#define VOP_MUL       2
#define VOP_LOG       3
#define VOP_NORMALISE_L1 4
#define VOP_SQRT      5
#define VOP_LOGA      6
#define VOP_POW       7
#define VOP_EXP       8
#define VOP_E         9
#define VOP_ABS      10
#define VOP_AGN      11

#undef class
class DLLEXPORT cVectorOperation : public cVectorProcessor {
  private:
    int powOnlyPos;
    double gnGenerator();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    int operation;
    FLOAT_DMEM param1,param2,logfloor;

    virtual void fetchConfig();
  
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVectorOperation(const char *_name);

    virtual ~cVectorOperation();
};




#endif // __CVECTOROPERATION_HPP
