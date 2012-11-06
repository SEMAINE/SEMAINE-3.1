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

example dataSource
writes data to data memory...

*/


#include <exampleSource.hpp>
#define MODULE "cExampleSource"

SMILECOMPONENT_STATICS(cExampleSource)

SMILECOMPONENT_REGCOMP(cExampleSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CEXAMPLESOURCE;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLESOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nValues","The number of random values to generate",1);
    ct->setField("randSeed","The random seed",1.0);
  )

  SMILECOMPONENT_MAKEINFO(cExampleSource);
}

SMILECOMPONENT_CREATE(cExampleSource)

//-----

cExampleSource::cExampleSource(const char *_name) :
  cDataSource(_name)
{
  // ...
}

void cExampleSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  nValues = getInt("nValues");
  SMILE_DBG(2,"nValues = %i",nValues);
  randSeed = getDouble("randSeed");
  SMILE_DBG(2,"nValues = %f",randSeed);
}

/*
int cExampleSource::myConfigureInstance()
{
  int ret = cDataSource::myConfigureInstance();
  // ...

  return ret;
}
*/

int cExampleSource::configureWriter(sDmLevelConfig &c)
{
  // configure your writer by setting values in &c

  return 1;
}

// NOTE: nEl is always 0 for dataSources....
int cExampleSource::setupNewNames(long nEl)
{
  // configure dateMemory level, names, etc.
  writer->addField("randVal",nValues);
  writer->addField("const");
  // ...

  allocVec(nValues+1);
  return 1;
}

/*
int cExampleSource::myFinaliseInstance()
{
  return cDataSource::myFinaliseInstance();
}
*/

int cExampleSource::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, writing value vector",t);

  // todo: fill with random values...
  vec->dataF[0] = (FLOAT_DMEM)t+(FLOAT_DMEM)3.3;
  writer->setNextFrame(vec);
  
  return 1;
}


cExampleSource::~cExampleSource()
{
  // ..
}
