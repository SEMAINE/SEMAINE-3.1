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

voice activity detection based on LSF and Pitch features + smoothing

*/


#ifndef __CSEMAINESPEAKERID1_HPP
#define __CSEMAINESPEAKERID1_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CSEMAINESPEAKERID1 "This component implements voice activity detection based on a self-adapting nearest neighbour classifier."
#define COMPONENT_NAME_CSEMAINESPEAKERID1 "cSemaineSpeakerID1"

/* what we have to read from the input level:

FEATURES
vadBinary

*/


#undef class
class DLLEXPORT cSemaineSpeakerID1 : public cDataProcessor {
private:
  long nFt;
  FLOAT_DMEM *meanS, *meanN, **meanA;
  FLOAT_DMEM *varS, *varN, **varA;
  const char *initSpeechModel, *initNoiseModel, *initAgentModel;
  const char *saveSpeechModel, *saveNoiseModel, *saveAgentModel;
  FLOAT_DMEM agentModelWeight, noiseModelWeight, speechModelWeight;

  long agentBlockTime;
  long maxTrainTime;
  long runTime;
  int doReset, userPresence;

  long long nS, nN, *nA;  // number of examples in model
  int numAgents;
  int agentTurn, agentTurnCnt;
  int alwaysRejectAgent, ruleVadOnly;
  long nTurns;  // number of turns in model
  FLOAT_DMEM vadFuz0;
  FLOAT_DMEM a1,a0;
  int kla, debug;
  long trainDur;

  void findInputMapping();
  int saveModel(const char *filename, FLOAT_DMEM *coeff, int N, FLOAT_DMEM weight=1.0);
  int loadModel(const char *filename, FLOAT_DMEM *coeff, int N, FLOAT_DMEM weight=1.0);
  int saveModelArr(const char *filename, FLOAT_DMEM **coeff, int N, int Nmodels=1, FLOAT_DMEM weight=1.0);
  int loadModelArr(const char *filename, FLOAT_DMEM **coeff, int N, int Nmodels=1, FLOAT_DMEM weight=1.0);

  int initialiseVadModels();

protected:
  SMILECOMPONENT_STATIC_DECL_PR

    //virtual int isVoice(FLOAT_DMEM *src);
    //virtual void updateThreshold(FLOAT_DMEM eRmsCurrent);

    virtual void fetchConfig();
  //virtual int myConfigureInstance();
  virtual int myFinaliseInstance();
  virtual int myTick(long long t);

  virtual int processComponentMessage( cComponentMessage *_msg );
  //virtual int configureWriter(sDmLevelConfig &c);
  virtual int setupNewNames(long nEl);
  //virtual void configureField(int idxi, long __N, long nOut);
  //virtual int setupNamesForField(int i, const char*name, long nEl);
  //    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
  //    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

public:
  SMILECOMPONENT_STATIC_DECL

    cSemaineSpeakerID1(const char *_name);

  virtual ~cSemaineSpeakerID1();
};




#endif // __CSEMAINESPEAKERID1_HPP
