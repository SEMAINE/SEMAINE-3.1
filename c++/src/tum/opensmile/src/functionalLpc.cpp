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

functional: dct of input data

*/


#include <functionalLpc.hpp>

#define MODULE "cFunctionalLpc"

#define N_FUNCTS  2

#define NAMES     "lpgain","lpc"
#define IDX_VAR_FUNCTS 1  // start of lpc array of variable length

const char *lpcNames[] = {NAMES};  

SMILECOMPONENT_STATICS(cFunctionalLpc)

SMILECOMPONENT_REGCOMP(cFunctionalLpc)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALLPC;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALLPC;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("lpGain","Output of LP gain (1=yes / 0=no).",0);
    ct->setField("lpc","Output of LP coefficients (1=yes / 0=no).",1);
    ct->setField("firstCoeff","The first LP coefficient to compute. The first coefficient is coeff 0 (default)",0);
    ct->setField("order","The order of the linear prediction (i.e. the max number of coefficients, starting at coefficient number 0).",5);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalLpc);
}

SMILECOMPONENT_CREATE(cFunctionalLpc)

//-----

cFunctionalLpc::cFunctionalLpc(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,lpcNames),
  firstCoeff(1),
  order(0), lpc(NULL), acf(NULL), tmpstr(NULL)
{
}

void cFunctionalLpc::fetchConfig()
{
  firstCoeff = getInt("firstCoeff");
  SMILE_IDBG(2,"firstCoeff = %i",firstCoeff);
  if (firstCoeff < 0) {
    SMILE_IWRN(2,"firstCoeff < 0 in config. forcing firstCoeff=0 !");
	  firstCoeff = 0;
  }
  order = getInt("order");
  if (order <= firstCoeff) {
    SMILE_IERR(2,"error in config: order (%i) cannot be smaller or equal than firstCoeff (%i) (firstCoeff=0 is the first coefficient)!",order,firstCoeff);
    order = firstCoeff;
  }

  if (getInt("lpGain")) {  enab[0] = 1;  }

  cFunctionalComponent::fetchConfig();
  nEnab += order - firstCoeff;

  if (acf == NULL) acf = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*(order+1));
  if (lpc == NULL) lpc = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*(order+1));
}

const char* cFunctionalLpc::getValueName(long i)
{
  if (i >= IDX_VAR_FUNCTS) {
    const char *n = cFunctionalComponent::getValueName(1);
    // append coefficient number
    if (tmpstr != NULL) free(tmpstr);
    tmpstr = myvprint("%s%i",n,i+firstCoeff);
    return tmpstr;
  } else {
    return cFunctionalComponent::getValueName(1);
  }
}

long cFunctionalLpc::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  long i;
  if ((Nin>0)&&(out!=NULL)) {
    //Durbin on input values...

    FLOAT_DMEM gain = 0.0;
    FLOAT_DMEM *refl = NULL; /* TODO: add support for reflection coefficients */

    smileDsp_autoCorr(in, Nin, acf, order+1);
    smileDsp_calcLpcAcf(acf, lpc, order, &gain, refl);

    for (i=firstCoeff; i<order; i++) {
      if (i-firstCoeff >= Nout) break;
      *(out++) = lpc[i];
    }
    return order-firstCoeff;
  }
  return 0;
}

/*
long cFunctionalLpc::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalLpc::~cFunctionalLpc()
{
  if (tmpstr != NULL) free(tmpstr);
  if (acf != NULL) free(acf);
  if (lpc != NULL) free(lpc);
}

