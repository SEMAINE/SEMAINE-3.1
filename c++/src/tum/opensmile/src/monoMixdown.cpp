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

simple mixer, which adds multiple channels (elements) to a single channel (element)

*/


#include <monoMixdown.hpp>

#define MODULE "cMonoMixdown"

SMILECOMPONENT_STATICS(cMonoMixdown)

SMILECOMPONENT_REGCOMP(cMonoMixdown)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CMONOMIXDOWN;
  sdescription = COMPONENT_DESCRIPTION_CMONOMIXDOWN;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN( 
    ct->setField("normalise","1/0 = yes/no : divide by the number of channels when adding samples from multiple channels.",1);
    ct->setField("bufsize","number of samples to process at once. Choose a number >> 1 for optimal performance. Too large buffer sizes may influence the latency!",1024);
  )
  
  SMILECOMPONENT_MAKEINFO(cMonoMixdown);
}


SMILECOMPONENT_CREATE(cMonoMixdown)

//-----

cMonoMixdown::cMonoMixdown(const char *_name) :
  cDataProcessor(_name), matout(NULL)
{
}

void cMonoMixdown::fetchConfig()
{
  cDataProcessor::fetchConfig();

  normalise = getInt("normalise");
  SMILE_IDBG(2,"normalise = %i",normalise);
  bufsize = getInt("bufsize");
  SMILE_IDBG(2,"bufsize = %i",bufsize);

}

int cMonoMixdown::configureWriter(sDmLevelConfig &c) 
{
  reader->setupSequentialMatrixReading(bufsize,bufsize);
  return 1; /* success */
}


int cMonoMixdown::setupNamesForField(int i, const char *name, long nEl)
{
  // add a field with a single element for each input array field
  writer->addField(name,1);
  return 1;
}


int cMonoMixdown::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();
  return ret;
}


int cMonoMixdown::myTick(long long t)
{
  /* actually process data... */

  // get next matrix from dataMemory
  cMatrix *mat = reader->getNextMatrix();
  if (mat == NULL) return 0;

  if (matout == NULL) matout = new cMatrix(mat->fmeta->N, mat->nT, mat->type);

  // sum up channels
  long i,j,c;
  for (i=0; i<mat->nT; i++) {
    for (j=0; j<matout->N; j++) {
      matout->dataF[i*matout->N+j] = 0.0; long st = mat->fmeta->field[j].Nstart;
      for (c=0; c<mat->fmeta->field[j].N; c++) 
        matout->dataF[i*matout->N+j] += mat->dataF[i*mat->N+st+c];
      if ((mat->fmeta->field[j].N > 0)&&(normalise))
        matout->dataF[i*matout->N+j] /= (FLOAT_DMEM)(mat->fmeta->field[j].N);
    }
  }

  // if you create a new vector here and pass it to setNextFrame(),
  // then be sure to assign a valid tmeta info for correct timing info:
  // e.g.:
  matout->tmetaReplace(mat->tmeta);


  // save to dataMemory
  writer->setNextMatrix(matout);

  return 1;
}


cMonoMixdown::~cMonoMixdown()
{
  // cleanup...
  if (matout != NULL) delete matout;
}

