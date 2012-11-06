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

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#include <exampleSink.hpp>

#define MODULE "cExampleSink"


SMILECOMPONENT_STATICS(cExampleSink)

SMILECOMPONENT_REGCOMP(cExampleSink)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CEXAMPLESINK;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLESINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","The name of a text file to dump values to (this file will be overwritten, if it exists)",(const char *)NULL);
    ct->setField("lag","Output data <lag> frames behind",0,0,0);
  )

  SMILECOMPONENT_MAKEINFO(cExampleSink);
}

SMILECOMPONENT_CREATE(cExampleSink)

//-----

cExampleSink::cExampleSink(const char *_name) :
  cDataSink(_name),
  fHandle(NULL)
{
  // ...
}

void cExampleSink::fetchConfig()
{
  cDataSink::fetchConfig();
  
  filename = getStr("filename");
  SMILE_DBG(2,"filename = '%s'",filename);
  lag = getInt("lag");
  SMILE_DBG(2,"lag = %i",lag);
}


/*
int cExampleSink::myConfigureInstance()
{
  return  cDataSink::myConfigureInstance();
  
}
*/


int cExampleSink::myFinaliseInstance()
{
  // FIRST call cDataSink myFinaliseInstance, this will finalise our dataWriter...
  int ret = cDataSink::myFinaliseInstance();

  /* if that was SUCCESSFUL (i.e. ret == 1), then do some stuff like
     loading models or opening output files: */

  if ((ret)&&(filename != NULL)) {
    fHandle = fopen(filename,"w");
    if (fHandle == NULL) {
      SMILE_IERR(1,"failed to open file '%s' for writing!",filename);
      COMP_ERR("aborting");
	    //return 0;
    }
  }

  return ret;
}


int cExampleSink::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, reading value vector:",t);
  cVector *vec= reader->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  //else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;
  
  // now print the vector:
  int i;
  for (i=0; i<vec->N; i++) {
    //SMILE_PRINT("  (a=%i vi=%i, tm=%fs) %s.%s = %f",reader->getCurR(),vi,tm,reader->getLevelName(),vec->name(i),vec->dataF[i]);
    printf("  %s.%s = %f\n",reader->getLevelName(),vec->name(i),vec->dataF[i]);
  }
  
  if (fHandle != NULL) {
    for (i=0; i<vec->N; i++) {
      fprintf(fHandle, "%s = %f\n",vec->name(i),vec->dataF[i]);
    }
  }

  // tick success
  return 1;
}


cExampleSink::~cExampleSink()
{
  if (fHandle != NULL) {
    fclose(fHandle);
  }
}

