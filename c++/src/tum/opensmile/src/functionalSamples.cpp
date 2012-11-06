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

functionals: samples values

*/


#include <functionalSamples.hpp>

#define MODULE "cFunctionalSamples"


#define FUNCT_SAMPLES     0

#define N_FUNCTS  1

#define NAMES     "samples"

#define DEFAULT_NR_SAMPLES 5

const char *samplesNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalSamples)

SMILECOMPONENT_REGCOMP(cFunctionalSamples)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALSAMPLES;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALSAMPLES;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("samplepos","Array of positions of samples to copy to the output. The size of this array determines the number of sample frames that will be passed to the output. The given positions must be in the range from 0 to 1, indicating the relative position whithin the input segment, where 0 is the beginning and 1 the end of the segment.",0.0,ARRAY_TYPE);
  )
  
  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalSamples);
}

SMILECOMPONENT_CREATE(cFunctionalSamples)

//-----

cFunctionalSamples::cFunctionalSamples(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,samplesNames),
  samplepos(0)
{
}

void cFunctionalSamples::fetchConfig()
{
  nSamples = getArraySize("samplepos");
  SMILE_IDBG(2,"nSamples from config = %d", nSamples);
  if (nSamples > 0) {
    samplepos = (double*) malloc(sizeof(double) * nSamples);
    for (int i = 0; i < nSamples; ++i) {
      samplepos[i] = getDouble_f(myvprint("samplepos[%i]", i));
      if (samplepos[i] < 0.0) {
        SMILE_WRN(2,"(inst '%s') samplepos[%i] is out of range [0..1] : %f (clipping to 0.0)",getInstName(),i,samplepos[i]);
        samplepos[i] = 0.0;
      }
      if (samplepos[i] > 1.0) {
        SMILE_WRN(2,"(inst '%s') samplepos[%i] is out of range [0..1] : %f (clipping to 1.0)",getInstName(),i,samplepos[i]);
        samplepos[i] = 1.0;
      }
    }
  }
  else {
    nSamples = DEFAULT_NR_SAMPLES;
    samplepos = (double*) malloc(sizeof(double) * nSamples);
    for (int i = 0; i < nSamples; ++i) {
      samplepos[i] = (double)i / (DEFAULT_NR_SAMPLES-1.0);
      SMILE_IDBG(2,"samplepos[%d] = %.2f", i, samplepos[i]);
    }
  }
  enab[0] = 1;
  cFunctionalComponent::fetchConfig();
  nEnab = nSamples;
}

const char* cFunctionalSamples::getValueName(long i)
{
  const char *n = cFunctionalComponent::getValueName(0);
    // append [i]
  tmpstr = myvprint("%s%.3f",n,samplepos[i]);
  return tmpstr;
}

long cFunctionalSamples::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted,  FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  if ((Nin>0)&&(out!=NULL)) {
    FLOAT_DMEM Nind = (FLOAT_DMEM)Nin;
    for (int spi = 0; spi < nSamples; ++spi) {
      SMILE_IDBG(5,"Nind = %.2f, samplepos[%d] = %.2f",Nind,spi,samplepos[spi]);
      int si = (int)((Nind - 1.0) * samplepos[spi]);
      SMILE_IDBG(5,"si(%d) = %d",spi,si);
      out[spi] = in[si];
    }
    return nSamples;
  }
  else {
    return 0;
  }
}

/*
long cFunctionalSamples::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalSamples::~cFunctionalSamples()
{
  free(samplepos);
  /*if (samplepos != 0) {
    delete[] samplepos;
    samplepos = 0;
  }*/
}

