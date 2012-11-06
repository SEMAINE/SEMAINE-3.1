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

functionals: statistical moments

*/



#include <functionalMoments.hpp>

#define MODULE "cFunctionalMoments"


#define FUNCT_VAR        0
#define FUNCT_STDDEV     1
#define FUNCT_SKEWNESS   2
#define FUNCT_KURTOSIS   3
#define FUNCT_AMEAN      4

#define N_FUNCTS  5

#define NAMES     "variance","stddev","skewness","kurtosis","amean"

const char *momentsNames[] = {NAMES};  // change variable name to your class...

SMILECOMPONENT_STATICS(cFunctionalMoments)

SMILECOMPONENT_REGCOMP(cFunctionalMoments)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALMOMENTS;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALMOMENTS;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("variance","1/0=enable/disable output of variance",1);
    ct->setField("stddev","1/0=enable/disable output of standard deviation",1);
    ct->setField("skewness","1/0=enable/disable output of skewness",1);
    ct->setField("kurtosis","1/0=enable/disable output of kurtosis",1);
    ct->setField("amean","1/0=enable/disable output of arithmetic mean",0);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalMoments);
}

SMILECOMPONENT_CREATE(cFunctionalMoments)

//-----

cFunctionalMoments::cFunctionalMoments(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,momentsNames)
{
}

void cFunctionalMoments::fetchConfig()
{
  if (getInt("variance")) enab[FUNCT_VAR] = 1;
  if (getInt("stddev")) enab[FUNCT_STDDEV] = 1;
  if (getInt("skewness")) enab[FUNCT_SKEWNESS] = 1;
  if (getInt("kurtosis")) enab[FUNCT_KURTOSIS] = 1;
  if (getInt("amean")) enab[FUNCT_AMEAN] = 1;

  cFunctionalComponent::fetchConfig();
}

long cFunctionalMoments::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i;
  if ((Nin>0)&&(out!=NULL)) {
    double m2=0.0, m3=0.0, m4=0.0;

    double Nind = (double)Nin;
    double tmp, tmp2;
    FLOAT_DMEM *in0=in;
    
    for (i=0; i<Nin; i++) {
      tmp = ((double)*(in++) - mean); // ?? * p(x) ??
      tmp2 = tmp*tmp;
      m2 += tmp2;
      tmp2 *= tmp;
      m3 += tmp2;
      m4 += tmp2*tmp;
    }
    m2 /= Nind;  // variance

    int n=0;
    if (enab[FUNCT_VAR]) out[n++]=(FLOAT_DMEM)m2;
    double sqm2=sqrt(m2);
    if (enab[FUNCT_STDDEV]) {
      if (m2 > 0.0) out[n++]=(FLOAT_DMEM)sqm2;
      else out[n++] = 0.0;
    }
    if (enab[FUNCT_SKEWNESS]) {
      if (m2 > 0.0) out[n++]=(FLOAT_DMEM)( m3/(Nind*m2*sqm2) );
      else out[n++] = 0.0;
    }
    if (enab[FUNCT_KURTOSIS]) {
      if (m2 > 0.0) out[n++]=(FLOAT_DMEM)( m4/(Nind*m2*m2) );
      else out[n++] = 0.0;
    }
    if (enab[FUNCT_AMEAN]) out[n++]=(FLOAT_DMEM)mean;
    return n;
  }
  return 0;
}

/*
long cFunctionalMoments::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalMoments::~cFunctionalMoments()
{
}

