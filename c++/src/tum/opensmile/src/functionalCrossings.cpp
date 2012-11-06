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

functionals: zero-crossings, mean-crossings, arithmetic mean

*/


#include <functionalCrossings.hpp>

#define MODULE "cFunctionalCrossings"


#define FUNCT_ZCR     0
#define FUNCT_MCR     1
#define FUNCT_AMEAN   2

#define N_FUNCTS  3

#define NAMES     "zcr","mcr","amean"

const char *crossingsNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalCrossings)

SMILECOMPONENT_REGCOMP(cFunctionalCrossings)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALCROSSINGS;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALCROSSINGS;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("zcr","1/0=enable/disable output of zero crossing rate",1);
    ct->setField("mcr","1/0=enable/disable output of mean crossing rate (the rate at which the signal crosses its arithmetic mean value (same as zcr for mean normalised signals)",1);
    ct->setField("amean","1/0=enable/disable output of arithmetic mean",0);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalCrossings);
}

SMILECOMPONENT_CREATE(cFunctionalCrossings)

//-----

cFunctionalCrossings::cFunctionalCrossings(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,crossingsNames)
{
}

void cFunctionalCrossings::fetchConfig()
{
  if (getInt("zcr")) enab[FUNCT_ZCR] = 1;
  if (getInt("mcr")) enab[FUNCT_MCR] = 1;
  if (getInt("amean")) enab[FUNCT_AMEAN] = 1;

  cFunctionalComponent::fetchConfig();
}

long cFunctionalCrossings::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i;
  if ((Nin>0)&&(out!=NULL)) {
    double amean;
    long zcr=0, mcr=0;

    if (enab[FUNCT_MCR]||enab[FUNCT_AMEAN]) {
      amean = (double)*in;
      for (i=1; i<Nin; i++) {
        amean += in[i];
      }
      amean /= (double)Nin;
    }
    
    for (i=1; i<Nin-1; i++) {
      in++;
      if (  ( (*(in-1) * *(in+1) <= 0.0) && (*(in)==0.0) ) || (*(in-1) * *(in) < 0.0)  ) zcr++;
      if (enab[FUNCT_MCR])
        if (  ( ((*(in-1)-amean) * (*(in+1)-amean) <= 0.0) && ((*(in)-amean)==0.0) ) || ((*(in-1)-amean) * (*(in)-amean) < 0.0)  ) mcr++;
    }
    
    int n=0;
    if (enab[FUNCT_ZCR]) out[n++]=(FLOAT_DMEM) ( (double)zcr / (double)Nin );
    if (enab[FUNCT_MCR]) out[n++]=(FLOAT_DMEM) ( (double)mcr / (double)Nin );
    if (enab[FUNCT_AMEAN]) out[n++]=(FLOAT_DMEM)amean;
    return n;
  }
  return 0;
}

/*
long cFunctionalCrossings::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalCrossings::~cFunctionalCrossings()
{
}

