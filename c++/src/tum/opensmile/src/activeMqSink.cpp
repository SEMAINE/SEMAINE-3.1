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

ActiveMQ feature sender

*/


#include <activeMqSink.hpp>

#define MODULE "cActiveMqSink"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cActiveMqSink)

SMILECOMPONENT_REGCOMP(cActiveMqSink)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CACTIVEMQSINK;
  sdescription = COMPONENT_DESCRIPTION_CACTIVEMQSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
    SMILECOMPONENT_IFNOTREGAGAIN( {}
//    ct->makeMandatory(ct->setField("topic","ActiveMQ topic to send to",(const char *)NULL)); // semaine.data.analysis.features.voice
//    ct->setField("lag","output data <lag> frames behind",0);
  )

  SMILECOMPONENT_MAKEINFO(cActiveMqSink);
}

SMILECOMPONENT_CREATE(cActiveMqSink)

//-----

cActiveMqSink::cActiveMqSink(const char *_name) :
  cDataSink(_name),
  featureSender(NULL),
  meta(NULL),
  warned(0)
{
}

void cActiveMqSink::fetchConfig()
{
  cDataSink::fetchConfig();
}

/*
int cActiveMqSink::myConfigureInstance()
{
  int ret=1;

  ret *= cDataSink::myConfigureInstance();
  
  return ret;
}
*/

int cActiveMqSink::myTick(long long t)
{
  if (featureSender == NULL) {
    if (warned==0) {
      SMILE_IERR(1,"the semaine featureSender has not been set (it is NULL!), please check the log output of the semaine components, and check the semaineCfg section in the config file! (this warning will be shown only once, the system will not work!)");
      warned = 1;
    }
    return 0;
  }

  SMILE_DBG(4,"tick # %i, reading value vector:",t);
  cVector *vec= reader->getNextFrame();  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  //else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;

  int n,i;
  n = vec->N;
  if (!featureSender->areFeatureNamesSet()) {
    std::vector<std::string> tumFeatureNames(n);
    for (i=0; i<n; i++) {
      std::string name(vec->fmeta->getName(i)); 
      tumFeatureNames[i] = name;
    }
    //    tumFeatureNames[i]="speaking";
    //      tumFeatureNames[i+1]="seqNr";
    featureSender->setFeatureNames(tumFeatureNames);
  }

  std::vector<float> features(n);
  for (i=0; i<n; i++) {
    features[i] = (float)(vec->dataF[i]);
  }
  //    features[i] = speakingStatus;
  //    features[i+1] = seqNr;
  //    seqNr++;

//printf("about to send\n");
// NOTE: (FIXME) sometimes the activeMq sender hangs here:
//               restarting activeMq helps to solve this problem
//               nevertheless it should be investigated...
  if (meta != NULL) {
    featureSender->sendFeatureVector(features, meta->getTime(), false);
  } else {
    featureSender->sendFeatureVector(features, 0, false);
  }
//printf("after sending\n");

  // tick success
  return 1;
}


cActiveMqSink::~cActiveMqSink()
{
}

#endif // HAVE_SEMAINEAPI
