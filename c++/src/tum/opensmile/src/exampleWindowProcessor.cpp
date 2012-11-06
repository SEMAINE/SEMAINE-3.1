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

an example of a windowProcessor component

*/



#include <exampleWindowProcessor.hpp>
//#include <math.h>

#define MODULE "cExampleWindowProcessor"


SMILECOMPONENT_STATICS(cExampleWindowProcessor)

SMILECOMPONENT_REGCOMP(cExampleWindowProcessor)
{
  if (_confman == NULL) return NULL;
  int rA = 0;

  sconfman = _confman;
  scname = COMPONENT_NAME_CEXAMPLEWINDOWPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLEWINDOWPROCESSOR;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	// if you set description to NULL, the existing description will be used, thus the following call can
	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");
    // this is an example for adding an integer option:
	//ct->setField("inverse","1 = perform inverse FFT",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cExampleWindowProcessor);
}

SMILECOMPONENT_CREATE(cExampleWindowProcessor)

//-----

cExampleWindowProcessor::cExampleWindowProcessor(const char *_name) :
  cWindowProcessor(_name,1,0)
{
}


void cExampleWindowProcessor::fetchConfig()
{
  cWindowProcessor::fetchConfig();
  
  k = (FLOAT_DMEM)getDouble("k");
  SMILE_DBG(2,"k = %f",k);
  if ((k<0.0)||(k>1.0)) {
    SMILE_ERR(1,"k must be in the range [0;1]! Setting k=0.0 !");
    k=0.0;
  }
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cExampleWindowProcessor::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post )
{
  long n;

  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;
  for (n=0; n<_out->nT; n++) {
    *(y++) = *(x) - k * *(x-1);
    x++;
  }
  return 1;
}


cExampleWindowProcessor::~cExampleWindowProcessor()
{
}

