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

functionals: percentiles and quartiles, and inter-percentile/quartile ranges

*/


#ifndef __CFUNCTIONALPERCENTILES_HPP
#define __CFUNCTIONALPERCENTILES_HPP

#include <smileCommon.hpp>
#include <dataMemory.hpp>
#include <functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALPERCENTILES "  percentile values and inter-percentile ranges (including quartiles, etc.). This component sorts the input array and then chooses the value at the index closest to p*buffer_len for the p-th percentile (p=0..1)."
#define COMPONENT_NAME_CFUNCTIONALPERCENTILES "cFunctionalPercentiles"

#undef class
class DLLEXPORT cFunctionalPercentiles : public cFunctionalComponent {
  private:
    int nPctl, nPctlRange;
    double *pctl;
    int *pctlr1, *pctlr2;
    char *tmpstr;
    int quickAlgo, interp;
    long varFctIdx;

    long getPctlIdx(double p, long N);
    FLOAT_DMEM getInterpPctl(double p, FLOAT_DMEM *sorted, long N);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalPercentiles(const char *_name);
    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout);
    //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);

    //virtual long getNoutputValues() { return nEnab; }
    virtual const char* getValueName(long i);
    virtual int getRequireSorted() { if (quickAlgo) return 0; else return 1; }

    virtual ~cFunctionalPercentiles();
};




#endif // __CFUNCTIONALPERCENTILES_HPP
