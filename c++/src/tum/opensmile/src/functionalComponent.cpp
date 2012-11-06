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

*/


#include <functionalComponent.hpp>

#define MODULE "cFunctionalComponent"


SMILECOMPONENT_STATICS(cFunctionalComponent)

SMILECOMPONENT_REGCOMP(cFunctionalComponent)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALCOMPONENT;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALCOMPONENT;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  //  ConfigType *ct = new ConfigType(scname); if (ct == NULL) OUT_OF_MEMORY;
  SMILECOMPONENT_IFNOTREGAGAIN( {} )
  //   ct->setField(""...);
 
  SMILECOMPONENT_MAKEINFO_NODMEM_ABSTRACT(cFunctionalComponent);
}

SMILECOMPONENT_CREATE_ABSTRACT(cFunctionalComponent)

//-----

cFunctionalComponent::cFunctionalComponent(const char *_name, int _nTotal, const char*_names[]) :
  cSmileComponent(_name),
  nTotal(_nTotal),
  enab(NULL),
  functNames(_names),
  nEnab(0),
  timeNorm(TIMENORM_SEGMENT),
  timeNormIsSet(0),
  T(0.0)
{
  if (nTotal>0)
    enab = (int*)calloc(1,sizeof(int)*nTotal);
}

void cFunctionalComponent::parseTimeNormOption()
{
  if (isSet("norm")) { timeNormIsSet = 1; }

  const char*Norm = getStr("norm");
  if (Norm != NULL) {
    if (!strncmp(Norm,"tur",3)) timeNorm=TIMENORM_SEGMENT;
    else if (!strncmp(Norm,"seg",3)) timeNorm=TIMENORM_SEGMENT;
    else if (!strncmp(Norm,"sec",3)) timeNorm=TIMENORM_SECOND;
    else if (!strncmp(Norm,"fra",3)) timeNorm=TIMENORM_FRAME;
  }

  SMILE_IDBG(2,"timeNorm = %i (set= %i)\n",timeNorm,timeNormIsSet);
}

void cFunctionalComponent::fetchConfig()
{
  int i; 
  for (i=0; i<nTotal; i++) {
    if (enab[i]) nEnab++;
  }
}

const char* cFunctionalComponent::getValueName(long i)
{
  int j=0; int n=-1;
  if (functNames==NULL) return NULL;
  for (j=0; j<nTotal; j++) {
    if (enab[j]) n++;
    if (i==n) return functNames[j];
  }
  return NULL;
}

long cFunctionalComponent::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  SMILE_ERR(1,"dataType FLOAT_DMEM not yet supported in component '%s' of type '%s'",getTypeName(), getInstName());
  return 0;
}

long cFunctionalComponent::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{
  SMILE_ERR(1,"dataType INT_DMEM not yet supported in component '%s' of type '%s'",getTypeName(), getInstName());
  return 0;
}

cFunctionalComponent::~cFunctionalComponent()
{
  if (enab!=NULL) free(enab);
}

