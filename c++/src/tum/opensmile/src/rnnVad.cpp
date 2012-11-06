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

example of a vector processor

*/



#include <rnnVad.hpp>

#define MODULE "cRnnVad"


SMILECOMPONENT_STATICS(cRnnVad)

SMILECOMPONENT_REGCOMP(cRnnVad)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CRNNVAD;
  sdescription = COMPONENT_DESCRIPTION_CRNNVAD;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
    //ct->setField("nameAppend",NULL,"processed");
    ct->setField("voiceIdx","The index of the field which contains the 'voice' class output activation",1);
    ct->setField("voiceThresh","The threshold to apply to the 'voice' output activation. Voice is above threshold, silence is below. If you do *not* specify this value in the config file, rnnVad will detect 'voice' only if the voice field has a higher value than all other fields (excluding energy, F0, and everything above 'ftIdx' (if != -1)). ",0.5,0,0);
    ct->setField("energyIdx","The index of the field which contains the energy/loudness/intensity/etc. value (set to -1 to disable)",3);
    ct->setField("f0Idx","Index of F0 input field (set to -1 to disable)",4);
    ct->setField("ftIdx","Start index of (MFCC type) features (at end of input vector!) which will be used for voice model building (set to -1 to disable)",5);
    ct->setField("vmUserBuffersize_sec","Amount of voice data (in seconds) to collect for building user voice model.",5.0);
    ct->setField("vmRangeCal_sec","Amount of input data (in seconds) to collect for for estimating feature value range. (After this range has been estimated, a model can be built, if enough data for the model has been collected)",10.0);
    ct->setField("vmUserNbins","Number of histogram bins to use for user voice model.",150);
    ct->setField("alpha","alpha for on-line mvn updates (exponential).",0.0001);
    ct->setField("mapWeight","weight for incremental updates... (frame weights)",200);
    ct->setField("holdTime","The minimum voice activity hold time in frames (for longer chunks this time is increased up to 2x, if you are *not* using the 'voiceThresh' mode)",40);

    ct->setField("agentBlockTime","initial user speech time during which to block agent in frames (this must be high enough in order to have robust enough models)",1000);
    ct->setField("agentTurnPastBlock","time the VAD will be blocked after receiving an agent speech end message (in frames, usually 100fps).",60);
  	ct->setField("alwaysRejectAgent", "1 = never detect a speaker turn while the agent is speaking",0);
    ct->setField("eavgExpAlpha","alpha for computing expontial smoothed short time mean of energy",0.001);

    // TODO: resolve these indicies by field name!
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cRnnVad);
}

SMILECOMPONENT_CREATE(cRnnVad)


//-----

// add a frame to the voice model
// Histogram GM
int cVoiceModelA::addFrame(cVector *frame, long start) 
{
  long i;
  if (frame == NULL) return 0;
  if (frame->N-start != buffer->N) {
    SMILE_WRN(2,"cVoiceModelA: addFrame: frameSize (%i) - ftIdx (%i) must match buffer frame size (%i)!",frame->N,start, buffer->N);
    return 0;
  }
  
  //fprintf(stderr,"XXX ptr %i bufferN %i nuffernT %i\n",ptr,buffer->N,buffer->nT);
  FLOAT_DMEM *df = buffer->dataF + ptr*buffer->N;
  for (i=0; i<buffer->N; i++) {
    df[i] = frame->dataF[i+start];
  }

  ptr++; 
  if (filled < buffer->nT) filled++;
  if (ptr >= buffer->nT) {
    ptr = 0;
    _isFilled = 1;
    return 2;
  }
  return 1;
}


// NN model
int cVoiceModelB::addFrame(cVector *frame, long start) 
{
  long i;
  if (frame == NULL) return 0;
  if (frame->N-start != vecSize) {
    SMILE_WRN(2,"cVoiceModelB: addFrame: frameSize (%i) - ftIdx (%i) must match buffer frame size (%i)!",frame->N,start, vecSize);
    return 0;
  }
  
  //fprintf(stderr,"XXX ptr %i bufferN %i nuffernT %i\n",ptr,buffer->N,buffer->nT);
  FLOAT_DMEM *df = frame->dataF+start;
  for (i=0; i<vecSize; i++) {
    sum[i] += df[i];
  }
  _N += 1.0;

  if ((long)_N == bs) return 2;
  else return 1;
}

// 1 mixture Gaussian model
int cVoiceModelC::addFrame(cVector *frame, long start) 
{
  long i;
  if (frame == NULL) return 0;
  if (frame->N-start != vecSize) {
    SMILE_WRN(2,"cVoiceModelA: addFrame: frameSize (%i) - ftIdx (%i) must match buffer frame size (%i)!",frame->N,start, vecSize);
    return 0;
  }
  
  //fprintf(stderr,"XXX ptr %i bufferN %i nuffernT %i\n",ptr,buffer->N,buffer->nT);
  FLOAT_DMEM *df = frame->dataF+start;
  for (i=0; i<vecSize; i++) {
    
    // online update
    if (alpha > 0.0) {
      //printf("ONLINE UPDATE!\n");
      mean[i] = ((FLOAT_DMEM)1.0-alpha) * mean[i] + alpha * df[i];
      var[i] = ((FLOAT_DMEM)1.0-alpha) * var[i] + alpha * (df[i]*df[i]-mean[i]*mean[i]) ;
    }
    
    sum[i] += df[i];
    sum2[i] += df[i]*df[i];
    
  }
  _N += 1.0;
  trainDur++;

  if (alpha > 0.0) return 1;

  if ((trainDur % bs) == 0) { 
    return 2;
  }
  else 
    return 1;
}

//-----

cRnnVad::cRnnVad(const char *_name) :
  cDataProcessor(_name), frameO(NULL), lastV(0), cnt(0), isV(0), xcnt(0), lstmV(0), gmV(0), lastLstmV(0), lastGmV(0),
  eavgVoice(0.0), eavgVoiceNr(1.0), eavgVoiceLast(0.0),
  vmUser(NULL), vmBg(NULL), vmAgent(NULL), eminVoice(0.0), voiceThreshSet(0), voiceThresh(0.0),
  doReset(0), agentTurn(0), userPresence(0),
  trainDur(0), mapWeight(0.0), alpha(0.0), eavgExpAlpha(0.0), eavgExp(0.0),
  userCnt(0), agentCntdn(0), agentTurnCntdn(0)
{

}

void cRnnVad::fetchConfig()
{
  cDataProcessor::fetchConfig();
 
  voiceIdx = getInt("voiceIdx");
  SMILE_IDBG(2,"voiceIdx = %i",voiceIdx);
  energyIdx = getInt("energyIdx");
  SMILE_IDBG(2,"energyIdx = %i",energyIdx);

  ftIdx = getInt("ftIdx");
  SMILE_IDBG(2,"ftIdx = %i",ftIdx);

  f0Idx = getInt("f0Idx");
  SMILE_IDBG(2,"f0Idx = %i",f0Idx);

  vmUserNbins = getInt("vmUserNbins");
  holdTime = getInt("holdTime");

  if (isSet("voiceThresh")) {
    voiceThresh = (FLOAT_DMEM)getDouble("voiceThresh");
    voiceThreshSet = 1;
  }

  agentBlockTime = getInt("agentBlockTime");
  agentTurnPastBlock = getInt("agentTurnPastBlock");
  alwaysRejectAgent = getInt("alwaysRejectAgent");

  if (isSet("alpha")) {
    alpha = (FLOAT_DMEM)getDouble("alpha");
  }
  if (isSet("mapWeight")) {
    mapWeight = (FLOAT_DMEM)getDouble("mapWeight");
  }
  
  eavgExpAlpha = (FLOAT_DMEM)getDouble("eavgExpAlpha");

}



int cRnnVad::setupNewNames(long nEl)
{
  writer->addField("voiceAct");
	namesAreSet=1;
  return 1;
}



int cRnnVad::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();

  if (ret) {
    frameO = new cVector(1);
    double vmUserBuffersize_sec = getDouble("vmUserBuffersize_sec");
    vmUserBuffersize = (long)ceil(vmUserBuffersize_sec / reader->getLevelT());
    double vmRangeCal_sec = getDouble("vmRangeCal_sec");
    vmRangeCal = (long)ceil(vmRangeCal_sec / reader->getLevelT());
  }

  return ret;
}


int cRnnVad::processComponentMessage( cComponentMessage *_msg )
{
	if (isMessageType(_msg,"semaineCallback")) {
		// determine origin by message's user-defined name, which can be set in the config file
		SMILE_IDBG(3,"received 'semaineCallback' message '%s'",_msg->msgname);
    if (!strncmp(_msg->msgname,"start",5)) { agentTurn = 1; agentTurnCntdn = 0; }
    else if (!strncmp(_msg->msgname,"end",3)) {
      agentTurn = 0; agentTurnCntdn = agentTurnPastBlock;
    }
    else if (!strncmp(_msg->msgname,"present",7)) { if (userPresence != 1) { userPresence = 1; doReset=1; } }
    else if (!strncmp(_msg->msgname,"absent",6)) { if (userPresence != 0) { userPresence = 0; doReset=1; } }
		return 1;  // message was processed
	}

	return 0; // if message was not processed
}


// a derived class should override this method, in order to implement the actual processing
int cRnnVad::myTick(long long t)
{
  cVector * frame = reader->getNextFrame();
  if (frame == NULL) return 0;

  /* 
    create voice model objects 
  */
  if (vmUser == NULL) {
    if (ftIdx >= 0) {
      vmUser = new cVoiceModelC(frame->N-ftIdx, vmUserBuffersize, vmUserNbins);
      vmUser->alpha = alpha;
      vmUser->mapWeight = mapWeight;
    }
  }
  if (vmBg == NULL) {
    if (ftIdx >= 0) {
      vmBg = new cVoiceModelC(frame->N-ftIdx, vmUserBuffersize, vmUserNbins);
      //vmBg->setRangeMasterModel(*vmUser);
      vmBg->alpha = alpha/2;
      vmBg->mapWeight = mapWeight;
    }
  }
  if (vmAgent == NULL) {
    if (ftIdx >= 0) {
      vmAgent = new cVoiceModelC(frame->N-ftIdx, vmUserBuffersize, vmUserNbins);
      //vmBg->setRangeMasterModel(*vmUser);
      vmAgent->alpha = alpha/2;
      vmAgent->mapWeight = mapWeight;
    }
  }

  /* compute min/max (range) of input features for voice model */
  if (ftIdx >= 0) {
    vmUser->updateRange(frame,ftIdx);
  }
  

  cnt++;

  FLOAT_DMEM E=0.0;
  FLOAT_DMEM vact = frame->dataF[voiceIdx];
  FLOAT_DMEM oact = 0.0;
  long i;


  /* get voice probability, either
    a) from CTC or classification (softmax) output activations
    b) a single regression output by applying a threshold
   */
  if (voiceThreshSet) {
    oact = voiceThresh;
  } else {
    long myN = frame->N;
    if (ftIdx > 0) myN = ftIdx;
    for (i=0; i<myN; i++) {
      if ((i != energyIdx)&&(i!=f0Idx)&&(i != voiceIdx)) oact += frame->dataF[i];
    }
  }
  
  /* get frame energy */
  if (energyIdx >= 0) {
    E = frame->dataF[energyIdx];
    eavgExp = eavgExp * ((FLOAT_DMEM)1.0-eavgExpAlpha) + E*eavgExpAlpha;
  }

  /*
    get the "agent's talking state" ..
  */
  int _agentTurn = 0;
  int noV = 0;
  lockMessageMemory();
  if (doReset == 1) {
    vmUser->reset();
//    vmBg->reset();
    doReset=0;
  }
  _agentTurn = agentTurn;
  if ((agentTurn)||(agentTurnCntdn>0)) {
    if (alwaysRejectAgent) { noV = 1; }
    else {
      long mintime = MIN(vmUser->getTrainDur(),vmBg->getTrainDur());
      mintime = MIN(mintime, vmAgent->getTrainDur());
      if (mintime < agentBlockTime) { noV=1; }
    }
  }
  if (agentTurnCntdn > 0) agentTurnCntdn--;
  unlockMessageMemory();

  /*
    classify voice activity based on learnt user voice model
    EXPERIMENTAL
    TODO: use this result for voice prediction (but NOT for learning!)
  */
  if (ftIdx >= 0) {
    FLOAT_DMEM p = 1.0;
    FLOAT_DMEM pbg = 1.0;
    FLOAT_DMEM pag = 1.1;
    if (vmUser->hasModel()) {
      p = vmUser->getFrameProb(frame,ftIdx);
    }
    if (vmBg->hasModel()) {
      pbg = vmBg->getFrameProb(frame,ftIdx);
    }
    if (vmAgent->hasModel()) {
      pag = vmAgent->getFrameProb(frame,ftIdx);
      //printf("AM\n");
    }

    char c = 'B';
    if ((p==1.0)||(pbg==1.0)) c='x';
    else if (pbg < p) c='U';
    if ((pag != (FLOAT_DMEM)1.1)&&(pag > pbg)&&(pag > p)) c='A';
    //if (c!='x') printf("XXres: lpU: %f - lpB: %f  - lpA : %f  (%c)\n",p,pbg,pag,c);
    //printf("eavg: %f   E. %f \n",eavgExp,E);
    if (c=='U') { 
      //if (cntU > 3) {
      if (E > eavgExpAlpha) {
        printf(" U "); if (userCnt < 20) userCnt++; 
         gmV = 1; //lastGmV = cnt; 
      }
      //}
    } // todo: start threshold
    else //if (c!='x') 
    { 
      if (userCnt > 0) userCnt--;
      if (c=='A') {
        printf(" A "); 
        agentCntdn = 31;
        // TODO add block time if A detected..
      }
      gmV = 0;
    }

  }
  if (agentCntdn>0) {
    if (userCnt < 10) {
      noV = 1;
    }
    agentCntdn--;
  }

  
  /*
    detect voice activity start
  */
//  if ((vact > oact)||((eavgVoiceLast != 0.0)&&(E > eavgVoiceLast*1.1))) {
  if ((vact > oact)) {
//    if ((energyIdx==0)||(E>(eavgVoiceLast+eminVoice)*0.5)) {
    //printf("XX : E %f  eavg %f  emin: %f\n",E,eavgVoice/eavgVoiceNr,eminVoice);

    xcnt++;
    if (!lstmV) { // voice start...
      SMILE_IMSG(3,"lstm voice start vIdx=%i",frame->tmeta->vIdx);
      if (eavgVoice > 0.0) {
        eavgVoice = eavgVoice * (FLOAT_DMEM)50.0 / eavgVoiceNr;
        eavgVoiceNr = (FLOAT_DMEM)50.0;
        eminVoice = (FLOAT_DMEM)0.5 * eavgVoice / (FLOAT_DMEM)50.0;
      } else {
        eavgVoiceNr = (FLOAT_DMEM)0.0;
        eminVoice = (FLOAT_DMEM)-1.0;
      }
    }
    //printf("  v.. "); fflush(stdout);
    lastLstmV = cnt; lstmV = 1; 
    // TODO: compute "speech rate"
    //}
  }

  


  /* update the agent voice model */
  if (_agentTurn) {
    if (ftIdx >= 0) {
      if (E > eavgExpAlpha) {
        if (vmAgent->addFrame(frame,ftIdx) == 2) {
          vmAgent->updateModel();
        }
      }
    }
  }

  /* 
     compute energy average of highly voiced regions 
     and update the voice model with the data in these regions
  */
  if ((lstmV)&&(cnt < lastLstmV+15)&&(!_agentTurn)&&(!noV)&&(agentTurnCntdn<=0)) {
    eavgVoice += E; eavgVoiceNr+=1.0;
    if (eminVoice == -1.0) eminVoice = E;
    else { if (eminVoice > E) eminVoice = E; }

    if (ftIdx >= 0) {
      //if ((!noV)&&((energyIdx <= 0)||(E > (eavgVoice/eavgVoiceNr+eminVoice)*0.5))) {
      if (((energyIdx <= 0)||(E > (eavgVoice/eavgVoiceNr+eminVoice)*0.5))) {
        if (vmUser->addFrame(frame,ftIdx) == 2) {
          //if (vmUser->getNminMax() > vmRangeCal) 
            vmUser->updateModel();
        }
      }
    }
  }

  /*
    Update the background model when no voice is detected and the agent is not speaking
  */
  if ((!lstmV)&&(!gmV)&&(!_agentTurn)&&(!noV)&&(agentTurnCntdn<=0)&&(agentCntdn<=10)) {
    if ((ftIdx >= 0)&&(!_agentTurn)) {
      if (vmBg->addFrame(frame,ftIdx) == 2) {
        //if (vmUser->getNminMax() > vmRangeCal) // NOTE: vmUser here is *correct* ..!
        vmBg->updateModel();
      }
    }
  }


  /*
    compute dynamic turn end threshold/timeout
  */
  int thresh;

  if (voiceThreshSet) {
    thresh = (int)((float)holdTime);
  } else {
    thresh = (int)((float)holdTime * 0.9);
    if (xcnt > 1) thresh=(int)((float)holdTime * 1.2);
    if (xcnt > 3) thresh=(int)((float)holdTime * 1.8);
    if (xcnt > 5) thresh=(int)((float)holdTime * 2.1);
  }

  /*
    check for end of lstm voice activity
  */
  if (cnt > lastLstmV+thresh) { 
    if (lstmV) { // voice end...
      if (eavgVoiceNr > 0.0) eavgVoiceLast = eavgVoice / eavgVoiceNr;
      SMILE_IMSG(3,"\n lstm voice end (Eavg: %f) vIdx=%i",eavgVoiceLast,frame->tmeta->vIdx);
    }
    lstmV = 0; xcnt = 0; 
  }

  /*
    check for end of GM voice activity
  */
  /*
  if (cnt > lastGmV+thresh) { 
    if (gmV) { // voice end...
      //if (eavgVoiceNr > 0.0) eavgVoiceLast = eavgVoice / eavgVoiceNr;
      SMILE_IMSG(3,"\n voice end (Eavg: %f) vIdx=%i",eavgVoiceLast,frame->tmeta->vIdx);
    }
    gmV = 0; xcnt = 0; 
  }
*/

 


  /*
    generate output frame with voicing info
  */

  frameO->dataF[0] = (FLOAT_DMEM)(lstmV|gmV)&&(!noV);
  writer->setNextFrame(frameO);

  return 1;
}

cRnnVad::~cRnnVad()
{
  if (frameO != NULL) delete frameO;
  if (vmUser != NULL) {
#ifdef DEBUG
     vmUser->printHgram();
#endif

    delete vmUser;
  }
  if (vmBg != NULL) delete vmBg;
  if (vmAgent != NULL) delete vmAgent;
}

