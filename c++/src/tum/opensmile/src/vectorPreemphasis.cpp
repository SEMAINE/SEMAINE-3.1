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

pre-emphasis per frame  (simplification, however, this is the way HTK does it... so for compatibility... here you go)
(use before window function is applied!)

*/


#include <vectorPreemphasis.hpp>

#define MODULE "cVectorPreemphasis"

SMILECOMPONENT_STATICS(cVectorPreemphasis)

SMILECOMPONENT_REGCOMP(cVectorPreemphasis)
{
  SMILECOMPONENT_REGCOMP_INIT
  
  scname = COMPONENT_NAME_CVECTORPREEMPHASIS;
  sdescription = COMPONENT_DESCRIPTION_CVECTORPREEMPHASIS;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("k","The pre-emphasis coefficient k in y[n] = x[n] - k*x[n-1]",0.97);
    ct->setField("f","The pre-emphasis frequency f in Hz : k = exp( -2*pi * f/samplingFreq. ) (this option will override k)",0,0,0);
    ct->setField("de","1 = perform de- instead of pre-emphasis",0);
  )
  SMILECOMPONENT_MAKEINFO(cVectorPreemphasis);
}

SMILECOMPONENT_CREATE(cVectorPreemphasis)

//-----

cVectorPreemphasis::cVectorPreemphasis(const char *_name) :
  cVectorProcessor(_name),
  k(0.0)
{

}

void cVectorPreemphasis::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  k=(FLOAT_DMEM)getDouble("k");

  if (isSet("f")) {
    f = getDouble("f");
  } else {
    f = -1.0;
  }

  if (f < 0.0) { 
    SMILE_IDBG(2,"k = %f",k);
    if ((k<0.0)||(k>1.0)) {
      SMILE_IERR(1,"k must be in the range [0;1]! Setting k=0.0 !");
      k=0.0;
    }
  } else {
    SMILE_IDBG(2,"using preemphasis frequency f=%f Hz instead of k",f);
  }

  de=getInt("de");
  SMILE_DBG(2,"de=%f",de);
}

int cVectorPreemphasis::dataProcessorCustomFinalise()
{
  int ret = cVectorProcessor::dataProcessorCustomFinalise();
  if (f >= 0.0) {
    double _T = getBasePeriod();
    k = (FLOAT_DMEM)exp( -2.0*M_PI * f * _T );
    SMILE_IDBG(2,"computed k from f (%f Hz) : k = %f  (samplingRate = %f Hz)",f,k,1.0/_T);
  }
  return ret;
}


// a derived class should override this method, in order to implement the actual processing
int cVectorPreemphasis::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 0;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorPreemphasis::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...

  *(dst++) = (1-k) * *(src++); // - k * *(src-1);
  long n;
  if (de) {
    for (n=1; n<Ndst; n++) {
      *(dst++) = *(src) + k * *(src-1);
      src++;
    }
  } else {
    for (n=1; n<Ndst; n++) {
      *(dst++) = *(src) - k * *(src-1);
      src++;
    }
  }
  return 1;
}

cVectorPreemphasis::~cVectorPreemphasis()
{
}

