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

data reader from activeMq... TODO

*/


#include <activeMqSource.hpp>
#define MODULE "cActiveMqSource"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cActiveMqSource)

SMILECOMPONENT_REGCOMP(cActiveMqSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CACTIVEMQSOURCE;
  sdescription = COMPONENT_DESCRIPTION_CACTIVEMQSOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
   // ct->makeMandatory(ct->setField("topic","topic to read data vectors from",(const char*) NULL));
   ct->setField("nValues","number of (feature) values to read (if vector obtained from activeMq is smaller than this value, it will be padded with zeros, otherwise it will be stripped)",1);
   ct->setField("fStep","reference frameStep in seconds (TODO: read from an input level via a dataReader)",0.01);
   ct->setField("vadRecv","VAD component to send the vadInhibit message to",(const char*)NULL);
  )

  SMILECOMPONENT_MAKEINFO(cActiveMqSource);
}

SMILECOMPONENT_CREATE(cActiveMqSource)

//-----

cActiveMqSource::cActiveMqSource(const char *_name) :
  cDataSource(_name),
  warned(0),
  nValues(1),
  running(0)
{
  // ...
}

void cActiveMqSource::fetchConfig()
{
  cDataSource::fetchConfig();

  nValues = getInt("nValues");
  SMILE_IDBG(2,"nValues = %i",nValues);

  fStep = (FLOAT_DMEM)getDouble("fStep");
  SMILE_IDBG(2,"fStep = %f",fStep);

  vadRecv = getStr("vadRecv");
  SMILE_IDBG(2,"vadRecv = '%s'",vadRecv);
}

/*
int cActiveMqSource::myConfigureInstance()
{
    // call writer->setConfig here to set the dataMemory level configuration and override config file and defaults...
    
  int ret = 1;
  ret *= cDataSource::myConfigureInstance();
  // ...
  return ret;
}
*/

int cActiveMqSource::setupNewNames(long nEl)
{
  writer->addField("amqData",nValues);

  allocVec(nValues);

  namesAreSet=1;
  return 1;
}

int cActiveMqSource::myFinaliseInstance()
{
  int ret = cDataSource::myFinaliseInstance();
  return ret;

//  return cDataSource::myFinaliseInstance();
}

// This is called from TumFeatureExtractor.cpp
void cActiveMqSource::waveFileReceive(void *data, long long N)
{
  sWaveParameters para;
  if (smilePcm_parseWaveHeader(data, N, &para)) {

    float sr = (float)(para.sampleRate);
    float n  = (float)(para.nBlocks);
    float lenSec = ((float)1.0/sr * n);
    long lenFrames = (long)(lenSec / fStep);

    SMILE_IDBG(3,"received a wave file, length = %f s (%i frames)",lenSec,lenFrames);
    // now send a message with this information to the VAD component
    if (vadRecv != NULL) {
      cComponentMessage m;
      

    }

  }
}

int cActiveMqSource::myTick(long long t)
{
  //SMILE_DBG(4,"tick # %i, writing value vector",t);
  running = 1;

  // todo: fill with random values...
//  vec->dataF[0] = (FLOAT_DMEM)t+(FLOAT_DMEM)3.3;
//  writer->setNextFrame(vec);
  
  return 0;
}


cActiveMqSource::~cActiveMqSource()
{
  running = 0;
  // if (featureReceiver != NULL) delete featureReceiver;
}


#endif // HAVE_SEMAINEAPI
