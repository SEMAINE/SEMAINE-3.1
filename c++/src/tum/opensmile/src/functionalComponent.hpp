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

a single statistical functional or the like....
(template class)

*/


#ifndef __CFUNCTIONAL_HPP
#define __CFUNCTIONAL_HPP

#include <smileCommon.hpp>
#include <dataMemory.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALCOMPONENT "This is an abstract functional class, it is the base for all functional extractor classes."
#define COMPONENT_NAME_CFUNCTIONALCOMPONENT "cFunctionalComponent"

#define TIMENORM_UNDEFINED   -1
#define TIMENORM_SEGMENT   0
#define TIMENORM_SECOND    1
#define TIMENORM_SECONDS   1
#define TIMENORM_FRAME     2
#define TIMENORM_FRAMES    2
#define TIMENORM_SAMPLE    3  // currently not implemented
#define TIMENORM_SAMPLES   3  // currently not implemented
#define _TIMENORM_MAX      3

// obsolete: please use TIMENORM_XXX constants in new code..!
#define NORM_TURN     0
#define NORM_SECOND   1
#define NORM_FRAME    2
#define NORM_SAMPLES  3


#undef class
class DLLEXPORT cFunctionalComponent : public cSmileComponent {
  private:
    double T;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    int nEnab, nTotal;
    int *enab;
    int timeNorm, timeNormIsSet;
    const char **functNames;
    
    virtual void fetchConfig();
    // get size of one input element/frame in seconds
    double getInputPeriod() {return T;}  
    // parse a custom 'norm' (timeNorm) option if set (call this from fetchConfig)
    void parseTimeNormOption();

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalComponent(const char *_name, int _nTotal=0, const char *_names[]=NULL);
    void setInputPeriod(double _T) { T=_T; }

    void setTimeNorm(int _norm) { 
      if (!timeNormIsSet) {
        if ((_norm >= 0)&&(_norm <= _TIMENORM_MAX)&&(_norm!=TIMENORM_UNDEFINED)) {
          timeNorm = _norm;
        }
      }
    }

    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout);
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
    {
      return process(in,inSorted,out,Nin,Nout);
    }
    
    virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);
    virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM min, INT_DMEM max, INT_DMEM mean, INT_DMEM *out, long Nin, long Nout)
    {
      return process(in,inSorted,out,Nin,Nout);
    }

    virtual long getNoutputValues() { return nEnab; }
    virtual const char* getValueName(long i);
    virtual int getRequireSorted() { return 0; }

    virtual ~cFunctionalComponent();
};




#endif // __CFUNCTIONAL_HPP
