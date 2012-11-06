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


/*  openSMILE component: valbased selector

vector selector based on threshold of value <idx>

*/


#include <valbasedSelector.hpp>

#define MODULE "cValbasedSelector"

SMILECOMPONENT_STATICS(cValbasedSelector)

SMILECOMPONENT_REGCOMP(cValbasedSelector)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CVALBASEDSELECTOR;
  sdescription = COMPONENT_DESCRIPTION_CVALBASEDSELECTOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("threshold","Threshold for selection (see also 'invert' option)",1.0);
    ct->setField("idx", "The index of element to base the selection decision on. Currently only 1 element is supported, NO vector based thresholds etc. are possible.",0);
    ct->setField("invert", "1 = output the frame when element[idx] < threshold ; 0 = output the frame if element[idx] => threshold",0);
    ct->setField("allowEqual", "if this option is set to 1, output the frame also, when element[idx] == threshold",0);
    ct->setField("removeIdx", "1 = remove field element[idx} in output vector ; 0 = keep it",0);
  )
  
  SMILECOMPONENT_MAKEINFO(cValbasedSelector);
}


SMILECOMPONENT_CREATE(cValbasedSelector)

//-----

cValbasedSelector::cValbasedSelector(const char *_name) :
  cDataProcessor(_name),
  myVec(NULL), idx(0), invert(0), removeIdx(0), threshold(0.0)
{
}

void cValbasedSelector::fetchConfig()
{
  cDataProcessor::fetchConfig();

  // load all configuration parameters you will later require fast and easy access to here:

  threshold = (FLOAT_DMEM)getDouble("threshold");
  SMILE_DBG(2,"threshold = %f",threshold);

  idx = getInt("idx");
  invert = getInt("invert");
  allowEqual = getInt("allowEqual");
  removeIdx = getInt("removeIdx");

}

int cValbasedSelector::myFinaliseInstance()
{
  return cDataProcessor::myFinaliseInstance();
}


int cValbasedSelector::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, processing value vector",t);

  // get next frame from dataMemory
  cVector *vec = reader->getNextFrame();
  if (vec == NULL) {
    return 0;
  }

  // add offset
  int i = idx;
  if (i >= vec->N) i = vec->N-1;
  FLOAT_DMEM val = vec->dataF[i];
  
  int copy = 0;
  if ( ((invert)&&(val < threshold)) || ((!invert)&&(val > threshold)) || (allowEqual && (val==threshold)) ) {
    copy = 1;
  }
  
  if (copy) {
    // TODO: remove threshold value if removeIdx == 1
    if (removeIdx == 1) {

      if (myVec==NULL) myVec = new cVector(vec->N-1 , vec->type);
      int j,n=0;
      for(j=0; j<vec->N; j++) {
        if (j!=i) myVec->dataF[n] = vec->dataF[j];
        n++;
      }
      // save to dataMemory
      writer->setNextFrame(myVec);
      //delete myVec;

    } else {
      // save to dataMemory
      writer->setNextFrame(vec);
    }
  }

  return 1;
}


cValbasedSelector::~cValbasedSelector()
{
  // cleanup...
  if (myVec!=NULL) delete myVec;
}

