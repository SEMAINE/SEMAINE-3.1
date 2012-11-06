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


#include <functionalDCT.hpp>

#define MODULE "cFunctionalDCT"

#define N_FUNCTS  1

#define NAMES     "dct"

const char *dctNames[] = {NAMES};  

SMILECOMPONENT_STATICS(cFunctionalDCT)

SMILECOMPONENT_REGCOMP(cFunctionalDCT)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALDCT;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALDCT;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("firstCoeff","The first DCT coefficient to compute (coefficient 0 corresponds to the DC component)",1);
    ct->setField("lastCoeff","The last DCT coefficient to compute",6);
	  ct->setField("nCoeffs","An alternative option to lastCoeff (this option overwrites lastCoeff, if it is set): the number DCT coefficient to compute (lastCoeff = firstCoeff+nCoeffs-1).",6);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalDCT);
}

SMILECOMPONENT_CREATE(cFunctionalDCT)

//-----

cFunctionalDCT::cFunctionalDCT(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,dctNames),
  firstCoeff(1),
  lastCoeff(6),
  nCo(0),
  costable(NULL),
  tmpstr(NULL)
{
}

void cFunctionalDCT::fetchConfig()
{
  firstCoeff = getInt("firstCoeff");
  SMILE_IDBG(2,"firstCoeff = %i",firstCoeff);
  if (firstCoeff < 0) {
    SMILE_IWRN(2,"firstCoeff < 0 in config. forcing firstCoeff=0 !");
	  firstCoeff = 0;
  }
  
  if (isSet("nCoeffs")) {
    lastCoeff = firstCoeff + getInt("nCoeffs") - 1;
	  SMILE_IDBG(2,"using nCoeffs = %i to overwrite lastCoeff setting",getInt("nCoeffs"));
  } else {
    lastCoeff = getInt("lastCoeff");
  }
  SMILE_IDBG(2,"lastCoeff = %i",lastCoeff);
  enab[0] = 1;

  cFunctionalComponent::fetchConfig();
  nEnab += lastCoeff - firstCoeff;
}

void cFunctionalDCT::initCostable(long Nin, long Nout)
{
  if ((Nin>0)&&(Nout>0)) {
    costable = (FLOAT_DMEM * )malloc(sizeof(FLOAT_DMEM)*Nin*Nout);
    if (costable==NULL) OUT_OF_MEMORY;
    int i,m;
    nCo = lastCoeff - firstCoeff + 1;
    N=Nin;
    for (i=firstCoeff; i<=lastCoeff; i++) {
      for (m=0; m<Nin; m++) {
        costable[m + (i-firstCoeff)*Nin] = (FLOAT_DMEM)cos(M_PI*(double)i/(double)(N) * ((FLOAT_DMEM)(m) + 0.5) );
      }
    }
    factor = (FLOAT_DMEM)sqrt((double)2.0/(double)(N));
  }
}

const char* cFunctionalDCT::getValueName(long i)
{
  const char *n = cFunctionalComponent::getValueName(0);
  // append coefficient number
  if (tmpstr != NULL) free(tmpstr);
  tmpstr = myvprint("%s%i",n,i+firstCoeff);
  return tmpstr;
}

long cFunctionalDCT::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i,m;
  if ((Nin>0)&&(out!=NULL)) {
	if (costable == NULL) {
      initCostable(Nin,Nout);
	  if (costable == NULL) SMILE_IERR(1,"error initializing costable, probably Nin or Nout == 0 in cFunctionalDCT::process");
	}
	
	for (i=0; i < nCo; i++) {
	  out[i] = 0.0;
      for (m=0; m<Nin; m++) {
        out[i] +=  in[m] * costable[m+i*N];
      }
      out[i] *= factor; 
    }

    return nCo;
  }
  return 0;
}

/*
long cFunctionalDCT::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalDCT::~cFunctionalDCT()
{
  if (tmpstr != NULL) free(tmpstr);
}

