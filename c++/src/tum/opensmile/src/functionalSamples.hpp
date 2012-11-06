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

functionals: rise/fall times, up/down-level times

*/

#ifndef __CFUNCTIONALSAMPLES_HPP
#define __CFUNCTIONALSAMPLES_HPP

#include <smileCommon.hpp>
#include <dataMemory.hpp>
#include <functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALSAMPLES "sampled values at equidistant frames"
#define COMPONENT_NAME_CFUNCTIONALSAMPLES "cFunctionalSamples"

#undef class
class DLLEXPORT cFunctionalSamples : public cFunctionalComponent {
  private:
    double* samplepos;
    int nSamples;
    char *tmpstr;
    /*int nUltime, nDltime;
    double *ultime, *dltime;
    int norm;
    int varFctIdx;*/
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalSamples(const char *_name);
    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout);
    //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);

    //virtual long getNoutputValues() { return nEnab; }
    virtual const char* getValueName(long i);
    virtual int getRequireSorted() { return 0; }

    virtual ~cFunctionalSamples();
};




#endif // __CFUNCTIONALSAMPLES_HPP
