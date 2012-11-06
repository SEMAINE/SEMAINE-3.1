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

example dataSink
writes data to data memory...

*/


#include <plug_exampleSink.hpp>

#define MODULE "cExampleSinkPlugin"


SMILECOMPONENT_STATICS(cExampleSinkPlugin)

SMILECOMPONENT_REGCOMP(cExampleSinkPlugin)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CEXAMPLESINK;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLESINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","file to dump values to","dump.txt");
    ct->setField("lag","output data <lag> frames behind",0);
  )
  SMILECOMPONENT_MAKEINFO(cExampleSinkPlugin);
}

SMILECOMPONENT_CREATE(cExampleSinkPlugin)

//-----

cExampleSinkPlugin::cExampleSinkPlugin(const char *_name) :
  cDataSink(_name)
{
  // ...
printf("I'm here...\n");
}

void cExampleSinkPlugin::fetchConfig()
{
  cDataSink::fetchConfig();
printf("I'm here in fetchConfig...\n");
  
  filename = getStr("filename");
printf("filename=%s\n",filename);
printf("log=%i\n",(long)&smileLog);
smileLog.message(FMT("test"), 2, "cYEAH");

  SMILE_DBG(2,"filename = '%s'",filename);
  lag = getInt("lag");
  SMILE_DBG(2,"lag = %i",lag);
}

/*
int cExampleSink::myConfigureInstance()
{
  int ret=1;
  ret *= cDataSink::myConfigureInstance();
  // ....
  //return ret;
}
*/

/*
int cExampleSink::myFinaliseInstance()
{
  int ret=1;
  ret *= cDataSink::myFinaliseInstance();
  // ....
  //return ret;
}
*/

int cExampleSinkPlugin::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, reading value vector:",t);
  cVector *vec= reader->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;
  
  // now print the vector:
  SMILE_IMSG(2,"FUCKING AWSOME PLUGIN OUTPUT:");
  int i;
  for (i=0; i<vec->N; i++) {
    printf("  (a=%i vi=%i, tm=%fs) %s.%s = %f\n",reader->getCurR(),vi,tm,reader->getLevelName(),vec->name(i),vec->dataF[i]);
  }

// SMILE_PRINT("%i",var1,)


  // tick success
  return 1;
}


cExampleSinkPlugin::~cExampleSinkPlugin()
{
  // ...
}

