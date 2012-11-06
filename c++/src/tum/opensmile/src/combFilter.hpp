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


/*  openSMILE component: iirFilter

general iir filter bank

*/




#ifndef __CCOMBFILTER_HPP
#define __CCOMBFILTER_HPP

#include <smileCommon.hpp>
#include <windowProcessor.hpp>

#define COMPONENT_DESCRIPTION_CCOMBFILTER "This component implements a general comb-filter bank."
#define COMPONENT_NAME_CCOMBFILTER "cCombFilter"

#undef class

const int COMB_IIR = 1;
const int COMB_FIR = 0;

class combFilter {
  FLOAT_DMEM *outputBuffer;
  FLOAT_DMEM gain, gainInv;
  long delay; long bd;
  int type; // COMB_IIR, or COMB_FIR

  // the input array must be larger than the output array by at least the comb filter delay. 
  // The *in pointer points to the first element of "new" data. The past data will be accessed using negtive array indicies, so you must ensure that there is valid memory at these locations
  // *out must be of size N
  void processFIR(FLOAT_DMEM *in, FLOAT_DMEM *out, long N, long stepIn=1, long stepOut=1)
  {
    long n; long _n=0, _nO=0; 
    long d2i = delay*stepIn;
    for (n=0; n<N; n++) {
      out[_nO] = gainInv * in[_n] + gain * in[_n - d2i];
      _n += stepIn;
      _nO += stepOut;
    }
  }

  // *in and *out arrays must be of size N
  void processIIR(FLOAT_DMEM *in, FLOAT_DMEM *out, long N, long stepIn=1, long stepOut=1)
  {
    long n; long _n=0, _nO=0; 
    long d2o = delay*stepOut;
    // use output history
    for (n=0; n<MIN(N,delay); n++) {
      out[_nO] = gainInv * in[_n] + gain * outputBuffer[n*bd];
      _n += stepIn;
      _nO += stepOut;
    }
    // use current output if N>delay
    for (n=delay; n<N; n++) {
      out[_nO] = gainInv * in[_n] + gain * out[_nO - d2o];
      _n += stepIn;
      _nO += stepOut;
    }
    // update output buffer
    if (N>delay) {
      // copy end of output buffer
      for (n=0; n<delay; n++) {
        outputBuffer[n*bd] = out[(N-delay+n)*stepOut];
      }
    } else {
      // shift output buffer and copy new samples to end
      for (n=0; n<delay-N; n++) {
        outputBuffer[n*bd]  = outputBuffer[(n+delay-N)*bd];
      }
    }
  }

public:
  // bufferDim is the dimensionality of the output buffer, 
  // this is the number of *different* input streams this comb filter object will process
  combFilter(FLOAT_DMEM _gain, long _delay, int _type, long bufferDim=1) : 
    gain(_gain), delay(_delay), type(_type), outputBuffer(NULL) 
  {
    if (delay < 1) delay = 1;
    if (gain > 1.0) gain = 1.0;
    else if (gain < 0.0) gain = 0.0;
    gainInv = (FLOAT_DMEM)1.0 - gain;
    if (type == COMB_IIR) {
      outputBuffer = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*delay*bufferDim);
      bd=bufferDim;
    }
  }

  ~combFilter() {
    if (outputBuffer != NULL) free(outputBuffer);
  }

  void process(FLOAT_DMEM *in, FLOAT_DMEM *out, long N, long stepIn=1, long stepOut=1)
  {
    if (type == COMB_IIR) processIIR(in, out, N);
    else processFIR(in, out, N);
  }
};


class DLLEXPORT cCombFilter : public cWindowProcessor {
private:
  int ftype;
  int nFilters;
  combFilter **filters;

protected:
  SMILECOMPONENT_STATIC_DECL_PR

  virtual void fetchConfig();

  virtual int configureWriter(const sDmLevelConfig *c);
  virtual int setupNamesForField(int i, const char*name, long nEl);

  // buffer must include all (# order) past samples
  virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post, int rowGlobal );


public:
  SMILECOMPONENT_STATIC_DECL

    cCombFilter(const char *_name);

  virtual ~cCombFilter();
};


#endif // __CCOMBFILTER_HPP
