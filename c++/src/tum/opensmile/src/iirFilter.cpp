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


#include <iirFilter.hpp>
#include <math.h>

#define MODULE "cIirFilter"


SMILECOMPONENT_STATICS(cIirFilter)

SMILECOMPONENT_REGCOMP(cIirFilter)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CIIRFILTER;
  sdescription = COMPONENT_DESCRIPTION_CIIRFILTER;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend", NULL, "sma");
    ConfigType * iirFilter = new ConfigType("iirFilter");
    iirFilter->setField("Acoeff", "The filter's A coefficients (array, a0..a(arraysize) ).", 0.0, ARRAY_TYPE);
    iirFilter->setField("Bcoeff", "The filter's B coefficients (array, b1..b(arraysize) ).", 0.0, ARRAY_TYPE);
    iirFilter->setField("B0", "The coefficient 'b0'. This is usually 1, however you may for some reason might want to change it, and this option lets you...", 1.0);
    ct->setField("filter","Array containing the configuration of the filters", iirFilter, ARRAY_TYPE);

    //ct->setField("smaWin","The size of the moving average window. A larger window means more smoothing.",3);
    ct->setField("blocksize", NULL ,1); // <-- increase blocksize for better performance at the cost of higher latency
  )
  SMILECOMPONENT_MAKEINFO(cIirFilter);
}

SMILECOMPONENT_CREATE(cIirFilter)

//-----

cIirFilter::cIirFilter(const char *_name) :
  cWindowProcessor(_name),
  outHistory(NULL),
  filterOrder(NULL),
  nA(NULL), nB(NULL),
  nFilters(0),
  A(NULL), B(NULL)
{
}


void cIirFilter::fetchConfig()
{
  cWindowProcessor::fetchConfig();
  

//  setWindow(smaWin/2,smaWin/2);
}

/*
int cIirFilter::setupNamesForField(int i, const char*name, long nEl)
{
  char *tmp = myvprint("%s_de",name);
  writer->addField( name, nEl );
  return nEl;
}
*/

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cIirFilter::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post, int rowGlobal )
{
  long n;
  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;

  for (n=0; n<_out->nT; n++) {
//    y[n] = x[n];
    // TODO....

  }

  return 1;
}


cIirFilter::~cIirFilter()
{
}

