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


/*  openSMILE component: weighted differential

computes a weighted differential as used and described in :

Björn Schuller, Florian Eyben, Gerhard Rigoll: 
"Fast and Robust Meter and Tempo Recognition for the Automatic Discrimination of Ballroom Dance Styles", 
Proc. ICASSP 2007, IEEE, Vol. I, pp. 217-220, ISBN 1-4244-0728-1, Honolulu, Hawaii, USA, 15.-20.04.2007.

*/



#ifndef __CWEIGHTEDDIFF_HPP
#define __CWEIGHTEDDIFF_HPP

#include <smileCommon.hpp>
#include <windowProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWEIGHTEDDIFF "This component computes a weighted and smoothed differential by considering the change of the current value wrt. a short term average window. "
#define COMPONENT_NAME_CWEIGHTEDDIFF "cWeightedDiff"

#undef class
class DLLEXPORT cWeightedDiff : public cWindowProcessor {
  private:
    int leftwin;
    int rightwin;
    int doRightWeight; // 1 = use right mean for weighting  / 0 = use mean of right mean/left mean as base for difference

    double *lSum, *rSum;
    double *lastL;
    double *nL, *nR;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();

    virtual int configureWriter(sDmLevelConfig &c);
    //virtual int setupNamesForField(int i, const char*name, long nEl);

    // buffer must include all (# order) past samples
    virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post, int rowGlobal );
    
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWeightedDiff(const char *_name);

    virtual ~cWeightedDiff();
};


#endif // __CWEIGHTEDDIFF_HPP
