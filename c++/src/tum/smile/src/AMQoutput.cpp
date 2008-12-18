/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
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
 *******************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This class saves features from featureMemory in CSV format

input can be joined from multiple levels in the featureMemory
However, the levels must have the same frameStep!

Merging data from asynchronus level might be supported in the future
 (then frames from levels with greater frameStep will be duplicated, etc.)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




#include "AMQoutput.hpp"

#include <cstdlib>
#include <sstream>
#include <decaf/lang/System.h>

cAmqOutput::cAmqOutput( cFeatureMemory &mem, FeatureSender *_featureSender, MetaMessenger * _meta ) : 
	cGenOutput(mem),
	featureSender(_featureSender),
 	meta(_meta)
{
  //featureSender = _featureSender; 
  //MetaMessenger = _meta;
}

cAmqOutput::~cAmqOutput()
{
}

int cAmqOutput::sendLastFrame(int speakingStatus) {
  LONG_IDX oldIdx = _data.vIdxSaved;
  setIdxToLastFrame();
  int ret = sendCurrentFrame(speakingStatus);
  _data.vIdxSaved = oldIdx; 
  return ret;
}

int cAmqOutput::sendLastFrame() {
  int ret = sendLastFrame(0);
  return ret;
}


int cAmqOutput::sendCurrentFrame(int speakingStatus) {

    pOutputVector vec = getFrame();

    if (vec == NULL) return 0;
    if (vec->filled == 0) {
      FEATUM_WARNING(4,"vector not filled, will not send empty data!");
      _FUNCTION_RETURN_(0);
    }

    int n,i;
    n = vec->n;
    if (!featureSender->areFeatureNamesSet()) {
      std::vector<std::string> tumFeatureNames(n+1);
      for (i=0; i<n; i++) {
        std::string name(_data.outputDef.names[i]); 
        tumFeatureNames[i] = name;
      }
      tumFeatureNames[i]="speaking";
//      tumFeatureNames[i+1]="seqNr";
      featureSender->setFeatureNames(tumFeatureNames);
    }

    std::vector<float> features(n+1);
    for (i=0; i<n; i++) 
      features[i] = (float)(vec->data[i]);
    features[i] = speakingStatus;
//    features[i+1] = seqNr;
//    seqNr++;

    if (meta != NULL) {
      featureSender->sendFeatureVector(features, meta->getTime());
    } else {
      featureSender->sendFeatureVector(features, 0);
    }
    _FUNCTION_RETURN_(1);
}


