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

simple silence threshold based turn detector

*/


#include <turnDetector.hpp>

#define MODULE "cTurnDetector"

// default values (can be changed via config file...)
#define N_PRE  10
#define N_POST 20

SMILECOMPONENT_STATICS(cTurnDetector)

SMILECOMPONENT_REGCOMP(cTurnDetector)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CTURNDETECTOR;
  sdescription = COMPONENT_DESCRIPTION_CTURNDETECTOR;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("threshold","The silence threshold to use (the default value is for RMS energy, change it to -13.0 for log energy)",0.001);
    ct->setField("autoThreshold","1 = automatically adjust threshold (EXPERIMENTAL)",0);
    ct->setField("minmaxDecay","The decay constant used for min/max values in auto-thresholder (a larger value means a slower recovery from loud sounds)",0.9995);
  //TODO: these should be given in seconds and they should be converted to frames via period or via secToVidx..?
    ct->setField("nPre","number of frames > threshold until a turn start is detected",N_PRE);
    ct->setField("nPost","number of frames < threshold until a turn end is detected",N_POST);
  //---  
    ct->setField("useRMS","1 = the provided energy field in the input is rms energy instead of log energy",1);
    ct->setField("readVad","1 = use the result (vadBin) from the cVadV1 component instead of reading frame rms/log energy (the threshold is set to 0.55)",0);
    ct->setField("idx","The index of the RMS or LOG energy field to use (-1 to automatically find the field)",-1);
    ct->setField("messageRecp","The (cWinToVecProcessor type) component(s) to send 'frameTime' messages to (use , to separate multiple recepients), leave blank (NULL) to not send any messages. The messages will be sent at the turn end and (optionally) during the turn at fixed intervals configured by the 'msgInterval' parameter (if it is not 0).",(const char *) NULL);
    ct->setField("msgInterval","Interval at which to send 'frameTime' messages during an ongoing turn. Set to 0 to disable sending of intra turn messages.",0.0);
    ct->setField("eventRecp","The component(s) to send 'turnStart/turnEnd' messages to (use , to separate multiple recepients), leave blank (NULL) to not send any messages",(const char *) NULL);
    ct->setField("statusRecp","The component(s) to send 'turnSpeakingStatus' messages to (use , to separate multiple recepients), leave blank (NULL) to not send any messages",(const char *) NULL);
    ct->setField("maxTurnLength", "The maximum turn length in seconds (<= 0 : infinite). A turn end will be favoured by reducing nPost to 1 after this time",0.0);
    ct->setField("maxTurnLengthGrace", "The grace period to grant, after maxTurnLength is reached (in seconds). After a turn length of maxTurnLength + maxTurnLengthGrace an immediate turn end will be forced.",1.0);
    ct->setField("debug","log level to show some turn detector specific debug messages on",4);
    ct->setField("timeoutSec","turnEnd timeout in seconds (send turnEnd after timeoutSec seconds no input data)",2.0);
    ct->setField("eoiFramesMissing","set the number of frames that will be subtracted from the last turn end position (the forced turn end that will be sent when an EOI condition (end of input) is encountered). This is necessary, e.g. if you use delta or acceleration coefficients which introduce a lag of a few frames. Increase this value if SMILExtract hangs at the end of input when using the cTumkwsjSink component or a cFunctionals component, etc.",5);
    ct->setField("unblockTimeout","timeout in frames to wait after a turn block condition (started via s semaineCallback message)",60);
    ct->setField("blockStatus","apply event based speech detection block  for speakingStatus messages (i.e.  the sending of these messages is supressed)",0);
    ct->setField("blockAll","apply event based speech detection block for all types, i.e. the voice input is set to 0 by an incoming block message.",1);

  )
  SMILECOMPONENT_MAKEINFO(cTurnDetector);
}

SMILECOMPONENT_CREATE(cTurnDetector)

//-----

cTurnDetector::cTurnDetector(const char *_name) :
  cDataProcessor(_name),
  turnState(0), actState(0),
  useRMS(1),
  threshold((FLOAT_DMEM)0.0),
  nPost(N_POST),
  nPre(N_PRE),
  cnt1(0), cnt2(0), cnt1s(0), cnt2s(0), debug(4),
  startP(0),
  recFramer(NULL),
  recComp(NULL),
  statusRecp(NULL),
  rmsIdx(-1),
  autoRmsIdx(-1),
  autoThreshold(0),
  minmaxDecay((FLOAT_DMEM)0.9995),
  nmin(0), nmax(0),
  rmin(1.0), rmax(0.0), rmaxlast(0.0),
  tmpmin(0.0), tmpmax(0.0),
  dtMeanO(0.0), dtMeanAll(0.0), dtMeanT(0.0), dtMeanS(0.0),
  nE(0.0),
  nTurn(0), nSil(0),
  alphaM((FLOAT_DMEM)0.9999),
  calCnt(0),
  msgInterval(0.0),
  lastDataTime(0),
  timeout(0),
  timeoutSec(10.0),
  forceEnd(0),
  blockTurn(0), unblockTurnCntdn(0), blockAll(1), blockStatus(0)
{
}

void cTurnDetector::fetchConfig()
{
  cDataProcessor::fetchConfig();
  
  useRMS = getInt("useRMS");

  nPre = getInt("nPre");

  nPost = getInt("nPost");

  threshold = (FLOAT_DMEM)getDouble("threshold");
  if ((useRMS)&&(threshold<(FLOAT_DMEM)0.0)) { threshold = (FLOAT_DMEM)0.001; }

  autoThreshold = getInt("autoThreshold");

  rmsIdx = getInt("idx");
  debug = getInt("debug");

  eoiMis = getInt("eoiFramesMissing");

  readVad = getInt("readVad");
  SMILE_IDBG(2,"readVad = %i",readVad);

  if (readVad) {
    autoThreshold = 0;
    rmsIdx = -1;
    useRMS = 0;
    if (isSet("threshold"))
      threshold = (FLOAT_DMEM)getDouble("threshold");
    else
      threshold = (FLOAT_DMEM)0.55;
    //nPre = 10;
    //nPost = 20;
  }
  SMILE_IDBG(2,"nPre = %i",nPre);
  SMILE_IDBG(2,"nPost = %i",nPost);

  SMILE_IDBG(2,"useRMS = %i",useRMS);
  SMILE_IDBG(2,"idx = %i",rmsIdx);
  SMILE_IDBG(2,"autoThreshold = %i",autoThreshold);
  SMILE_IDBG(2,"threshold = %f",threshold);

  recFramer = getStr("messageRecp");
  SMILE_IDBG(2,"messageRecp = '%s'",recFramer);

  msgInterval = getDouble("msgInterval");
  SMILE_IDBG(2,"msgInterval = %f",msgInterval);

  recComp = getStr("eventRecp");
  SMILE_IDBG(2,"eventRecp = '%s'",recComp);

  statusRecp = getStr("statusRecp");
  SMILE_IDBG(2,"statusRecp = '%s'",statusRecp);

  timeoutSec = getDouble("timeoutSec");

  maxTurnLengthS = getDouble("maxTurnLength");
  graceS = getDouble("maxTurnLengthGrace");

  unblockTimeout = getInt("unblockTimeout");
  blockAll = getInt("blockAll");
  blockStatus = getInt("blockStatus");
}

int cTurnDetector::setupNewNames(long nEl)
{
  writer->addField( "isTurn" );
  namesAreSet = 1;

  // convert maxTurnLength from seconds to frames...
  double _T = (double)(reader->getLevelT());
  if (_T==0.0) _T = 1.0;
  if (maxTurnLengthS < 0.0) maxTurnLengthS = 0.0;
  maxTurnLength = (long)ceil(maxTurnLengthS / _T);
  if (graceS < 0.0) graceS = 0.0;
  grace = (long)ceil(graceS / _T);

  return 1;
}


void cTurnDetector::updateThreshold(FLOAT_DMEM eRmsCurrent)
{
  // compute various statistics on-line:

  // min "percentile" (robust min.)
  // "value below which Nmin values are.."
  if (eRmsCurrent < rmin) {
    nmin++;
    tmpmin += eRmsCurrent;
    if (nmin > 10) {
      rmin = tmpmin / (FLOAT_DMEM)nmin;
      //SMILE_IDBG(2,"adjusted rmin: %f",rmin);
      nmin = 0;
      tmpmin = 0;
    }
  }

  // auto decay..
  rmin *= (FLOAT_DMEM)1.0 + ((FLOAT_DMEM)1.0 - minmaxDecay);

  // max "percentile" (robust max.)
  if (eRmsCurrent > rmax) {
    nmax++;
    tmpmax += eRmsCurrent;
    if (nmax > 10) {
      rmaxlast = rmax;
      rmax = tmpmax / (FLOAT_DMEM)nmax;
      //SMILE_IDBG(2,"adjusted rmax: %f",rmax);
      nmax = 0;
      tmpmax = 0.0;
    }
  }

  // auto decay..
  rmax *= minmaxDecay;

  //}

  // mean overall (exponential decay)
  dtMeanO = minmaxDecay * (dtMeanO - eRmsCurrent) + eRmsCurrent;

  // mean overall, non decaying
  dtMeanAll = (dtMeanAll * nE + eRmsCurrent) / (nE+(FLOAT_DMEM)1.0);
  nE+=1.0;

  // mean of turns

  if (turnState) {
    nTurn++;
    dtMeanT = alphaM * (dtMeanT - eRmsCurrent) + eRmsCurrent;
  } else {
    nSil++;
  // mean of non-turns
    dtMeanS = alphaM * (dtMeanS - eRmsCurrent) + eRmsCurrent;
  }


  // update threshold based on collected statistics:
  FLOAT_DMEM newThresh;
/*  if (dtMeanT == 0.0) {
    //newThresh = ( rmax + (rmax - dtMeanS) ) * 1.1;
  } else {
    //newThresh = 0.5*(dtMeanT + ( rmax + (rmax - dtMeanS) )*1.1) ;
    newThresh = MAX( 0.5*(dtMeanT + rmaxlast ), rmaxlast * 1.41) ;
  }*/

  if (nTurn == 0) { newThresh = dtMeanAll * (FLOAT_DMEM)2.0; }
  else {
    newThresh = (FLOAT_DMEM)0.15 * ( rmax + rmin ) ;
    FLOAT_DMEM w = (FLOAT_DMEM)nTurn / ( (FLOAT_DMEM)nSil + (FLOAT_DMEM)nTurn);
    FLOAT_DMEM w2 = sqrt((FLOAT_DMEM)1.0-w)+(FLOAT_DMEM)1.0;
    if (dtMeanO < newThresh) { newThresh = w*newThresh + ((FLOAT_DMEM)1.0-w)*MAX(dtMeanO*w2, (FLOAT_DMEM)1.2*w2*dtMeanAll) ; }
  }
  

  threshold = (FLOAT_DMEM)0.8*threshold + (FLOAT_DMEM)0.2*newThresh;

#ifdef DEBUG
  tmpCnt++;
  if (tmpCnt>200) {
    SMILE_IDBG(2,"THRESH: %f  rmax %f  rmin %f  dtMeanO %f  dtMeanAll %f",threshold,rmax,rmin,dtMeanO,dtMeanAll);
    //SMILE_IDBG(2,"dtMeanO: %f",dtMeanO);
    //SMILE_IDBG(2,"dtMeanT: %f",dtMeanT);
    //SMILE_IDBG(2,"dtMeanS: %f",dtMeanS);
    tmpCnt = 0;
  }
#endif
}

int cTurnDetector::isVoice(FLOAT_DMEM *src)
{
  if (src[rmsIdx] > threshold) return 1;
  else return 0;
}

// a derived class should override this method, in order to implement the actual processing
int cTurnDetector::myTick(long long t)
{
  static int lastVIdx = 0;
  static double lastVTime = 0.0;
  // Send a end-of-turn message at the end of input!
  if ((isEOI() || timeout) && turnState) {
    if (recComp!=NULL) {
      SMILE_IDBG(3, "forced turnEnd message!");
      cComponentMessage cmsg("turnEnd");
      cmsg.floatData[0] = (double)nPost;
      cmsg.floatData[1] = (double)(lastVIdx - nPost - eoiMis);
      cmsg.floatData[2] = (double)(reader->getLevelT());
      cmsg.intData[0] = 0; // 0 = incomplete turn...
      cmsg.intData[1] = 2;  // set forced turn end flag to EOI (2)
      cmsg.userTime1 = lastVTime;
      sendComponentMessage( recComp, &cmsg );
      SMILE_IMSG(3,"sending turnEnd message to '%s' (due to TIMEOUT)",recComp);
    }
    if (recFramer!=NULL) {
      cComponentMessage cmsg("turnFrameTime");
      cmsg.intData[0] = 0; /* 0=incomplete turn */
      // send start/end in frames of input level
      cmsg.floatData[0] = (double)startP;
      cmsg.floatData[1] = (double)(lastVIdx - nPost - eoiMis);
      double _T = (double)(reader->getLevelT());
      if (_T!=0.0) {
        // also send start/end as actual data time in seconds
        cmsg.floatData[2] = ((double)startP) * _T;
        cmsg.floatData[3] = ((double)(lastVIdx - nPost - eoiMis)) * _T;
        // and send period of input level
        cmsg.floatData[4] = _T;
      }
      cmsg.intData[1] = 2; // intData[1] : 0=normal end / 1=forced turn end (max length) / 2=forced turn End (EOI)
      cmsg.userTime1 = startSmileTime;
      cmsg.userTime2 = endSmileTime;
      sendComponentMessage( recFramer, &cmsg );
      SMILE_IMSG(4,"sending turnFrameTime message (turn end) to '%s' (due to TIMEOUT)",recFramer);
    }

    turnState = 0;
    cnt1 = 0; cnt2 = 0; turnTime=0.0;
  }

  // get next frame from dataMemory
  cVector *vec = reader->getNextFrame();
  if (vec == NULL) {
    if ((getSmileTime()-lastDataTime > timeoutSec)&&(lastDataTime>0)) { timeout = 1; }
    return 0;
  }
  lastDataTime = getSmileTime();

  if (vec->tmeta) {
    lastVIdx = vec->tmeta->vIdx;
    lastVTime = vec->tmeta->smileTime;
  }
  
  cVector *vec0 = new cVector(1);  // TODO: move vec0 to class...
  
  FLOAT_DMEM *src = vec->dataF;
  FLOAT_DMEM *dst = vec0->dataF;

  if (rmsIdx < 0) {
    if (autoRmsIdx < 0) { // if autoRmsIdx has not been set yet
      if (readVad) {
        autoRmsIdx = vec->fmeta->fieldToElementIdx( vec->fmeta->findFieldByPartialName( "vadBin" ) );
        SMILE_IDBG(3,"automatically found vadBin field at index %i in input vector",autoRmsIdx);
      } else {
        if (useRMS) {
          autoRmsIdx = vec->fmeta->fieldToElementIdx( vec->fmeta->findFieldByPartialName( "RMS" ) );
          SMILE_IDBG(3,"automatically found RMSenergy field at index %i in input vector",autoRmsIdx);
        } else {
          autoRmsIdx = vec->fmeta->fieldToElementIdx( vec->fmeta->findFieldByPartialName( "LOG" ) );
          SMILE_IDBG(3,"automatically found LOGenergy field at index %i in input vector",autoRmsIdx);
        }
      }
      
    }
    rmsIdx = autoRmsIdx;
  }

  // just to be sure we don't exceed arrray bounds...
  if (rmsIdx >= vec->N) rmsIdx = vec->N-1;
  
  //printf("s : %f\n",src[rmsIdx]);
  if (autoThreshold) updateThreshold(src[rmsIdx]);

  //if (src[rmsIdx] > threshold) {
  if (!autoThreshold || calCnt > 200) {

  int vo = isVoice(src);
//  int forceEnd = 0;
  if (turnState && (maxTurnLength > 0) && !forceEnd) {
    if ((grace > 0)&&(vec->tmeta->vIdx - startP  > maxTurnLength)) {
      // during grace period, end turn even if only 1 frame is non-voiced (no nPost smoothing)
      if (vo==0) { 
        //cnt2 = nPost-eoiMis; // <<--offline!
        cnt2 = nPost+1; // <--online 
        forceEnd = 1; 
      }
    }
    if ( (vec->tmeta->vIdx - startP)  > (maxTurnLength+grace) ) {
      // grace period expired, hard turn end forced right here...
      //cnt2 = nPost-eoiMis; // <<--offline!
      cnt2 = nPost+1; // <--online 
      forceEnd = 1;
    }
  }

  int statusBlocked=0;
  if (blockAll) {
    lockMessageMemory();
    if ((blockTurn) || (unblockTurnCntdn > 0)) {
      /*
      if (turnState && vo) {
      cnt2 = nPost+1; /// <<-online only
      }
      */
      vo=0;
      if (unblockTurnCntdn > 0) {
        unblockTurnCntdn--;
        if ((unblockTurnCntdn == 0)&&(vo)) { cnt1 = nPre+20; }
        // careful: this (the +20) might break things if there are sounds when the character finished speaking...
      }
    }
    unlockMessageMemory();
  } else {
    if (blockStatus) {
      lockMessageMemory();
      if ((blockTurn) || (unblockTurnCntdn > 0)) {
        statusBlocked = 1;
        if (unblockTurnCntdn > 0) {
          unblockTurnCntdn--;
        }
      }
      unlockMessageMemory();
    }
  }


  if (forceEnd) {
    vo = 0; 
//    printf("XX forceEnd%i cnt2=%i nPost=%i\n",forceEnd,cnt2,nPost);
  }

  if (vo) {
    cnt2 = 0; cnt2s=0;

    if (!actState) {
      if (cnt1s<=0) { 
        if (vec->tmeta != NULL) startSmileTimeS = vec->tmeta->smileTime; 
      }
      cnt1s++;
      if (cnt1s > 1) {
        actState = 1; cnt1s=0; cnt2s=0;
        SMILE_IMSG(debug+1,"detected voice activity start at vIdx %i!",startP);
        if (statusRecp!=NULL) {
          cComponentMessage cmsg("turnSpeakingStatus");
          cmsg.intData[0] = 1;
          cmsg.floatData[1] = (double)(vec->tmeta->vIdx-1);
          cmsg.floatData[2] = (double)(reader->getLevelT());
          cmsg.userTime1 = startSmileTimeS;
          sendComponentMessage( statusRecp, &cmsg );
          SMILE_IDBG(4,"sending turnSpeakingStatus (1) message to '%s'",recComp);
        }
      }
    }

    if (!turnState) {
      if (cnt1<=0) { 
        if (vec->tmeta != NULL) startSmileTime = vec->tmeta->smileTime;/*offline: time !  FIX THIS PERMANENTLY...*/
      }
      cnt1++;
      if (cnt1 > nPre) {
//printf("XXX vo=%i turnState=%i actState=%i cnt2=%i\n",vo,turnState,actState,cnt2);
        startP = vec->tmeta->vIdx - cnt1; // cnt1 was nPre !
        turnState = 1; cnt1 = 0; cnt2 = 0; turnTime=0.0;
        //SMILE_IMSG(debug,"detected turn start at vIdx %i!",startP);
        SMILE_IMSG(3,"detected turn start at vIdx %i!",startP);
        if (recComp!=NULL) {
          cComponentMessage cmsg("turnStart");
          cmsg.floatData[0] = (double)nPre;
          cmsg.floatData[1] = (double)startP;
          cmsg.floatData[2] = (double)(reader->getLevelT());
          cmsg.userTime1 = startSmileTime;
          sendComponentMessage( recComp, &cmsg );
          SMILE_IMSG(debug,"sending turnStart message to '%s'",recComp);
        }
      }
    }
  } else {
    cnt1 = 0; cnt1s=0;
    if (cnt2<=0) { 
      if (vec->tmeta != NULL) endSmileTime = vec->tmeta->smileTime;/*offline: time !  FIX THIS PERMANENTLY...*/
    }
    if (actState) {
      cnt2s++;
      if (cnt2s > 5) {
        actState = 0; cnt2s=0; cnt1s=0;
        SMILE_IMSG(debug+1,"detected voice activity end at vIdx %i!",startP);
        if (statusRecp!=NULL) {
        cComponentMessage cmsg("turnSpeakingStatus");
        cmsg.intData[0] = 0;
        cmsg.floatData[1] = (double)(vec->tmeta->vIdx - 2);
        cmsg.floatData[2] = (double)(reader->getLevelT());
        cmsg.userTime1 = endSmileTime;
        sendComponentMessage( statusRecp, &cmsg );
        SMILE_IDBG(4,"sending turnSpeakingStatus (0) message to '%s'",recComp);
        }
      }
    }
    cnt2++;
    if (turnState) {
      if (cnt2 > nPost) {
        turnState = 0; cnt1 = 0; cnt2 = 0;
        SMILE_IMSG(debug,"detected turn end at vIdx %i !",(vec->tmeta->vIdx)-nPost);
        // WARNING: if we set cnt2 to nPost manually we run the risk of having negative turn lengths
        //    when we subtract nPost to get to the turn end:
        //  solution:  introduce another variable that is either nPost, or set to 0 if we set cnt2 during forcing a turn end

        long blag;
        if (forceEnd) blag = eoiMis;
        else blag = nPost;

        if (recFramer!=NULL) {
          cComponentMessage cmsg("turnFrameTime");
          cmsg.intData[0] = 1; /* indicates a turn end */
          // send start/end in frames of input level
          cmsg.floatData[0] = (double)startP;
          cmsg.floatData[1] = (double)(vec->tmeta->vIdx - blag);
          double _T = (double)(reader->getLevelT());
          if (_T!=0.0) {
            // also send start/end as actual data time in seconds
            cmsg.floatData[2] = ((double)startP) * _T;
            cmsg.floatData[3] = ((double)(vec->tmeta->vIdx - blag)) * _T;
            // and send period of input level
            cmsg.floatData[4] = _T;
          }
          cmsg.intData[1] = forceEnd; // intData[1] : 0=normal end / 1=forced turn end (max length)
          cmsg.userTime1 = startSmileTime;
          cmsg.userTime2 = endSmileTime;
          sendComponentMessage( recFramer, &cmsg );
          SMILE_IMSG(4,"sending turnFrameTime message (turn end) to '%s'",recFramer);
        }
        if (recComp!=NULL) {
          cComponentMessage cmsg("turnEnd");
          cmsg.floatData[0] = (double)nPost;
          cmsg.floatData[1] = (double)(vec->tmeta->vIdx - blag);
          cmsg.floatData[2] = (double)(reader->getLevelT());
          cmsg.intData[0] = 1;  // turn end
          cmsg.intData[1] = forceEnd; // intData[1] : 0=normal end / 1=forced turn end (max length)
          cmsg.userTime1 = endSmileTime;
          sendComponentMessage( recComp, &cmsg );
          SMILE_IDBG(3,"sending turnEnd message to '%s'",recComp);
        }
        forceEnd = 0;
      }
    }
  }

  } else { calCnt++; }

  /* if turnState and interval timeout, then send message */
  if ((turnState)&&(msgInterval > 0.0)) {
    if (turnTime > msgInterval) {
      turnTime = 0.0;
      if (recFramer!=NULL) {
          cComponentMessage cmsg("turnFrameTime");
          cmsg.intData[0] = 0; /* indicates an incomplete turn */
          // send start/end in frames of input level
          cmsg.floatData[0] = (double)startP;
          cmsg.floatData[1] = (double)(vec->tmeta->vIdx);
          double _T = (double)(reader->getLevelT());
          if (_T!=0.0) {
            // also send start/end as actual data time in seconds
            cmsg.floatData[2] = ((double)startP) * _T;
            cmsg.floatData[3] = ((double)(vec->tmeta->vIdx)) * _T;
            // and send period of input level
            cmsg.floatData[4] = _T;
          }
          cmsg.userTime1 = startSmileTime;
          if (vec->tmeta != NULL) cmsg.userTime2 = vec->tmeta->smileTime; 
          else cmsg.userTime2 = startSmileTime; 
          sendComponentMessage( recFramer, &cmsg );
          SMILE_IMSG(4,"sending turnFrameTime message (intra turn) to '%s'",recFramer);
        }
    }
    turnTime += (double)(reader->getLevelT());
  }

  dst[0] = (FLOAT_DMEM)turnState;

  // save to dataMemory
  writer->setNextFrame(vec0);
  delete vec0;

  return 1;
}

int cTurnDetector::processComponentMessage( cComponentMessage *_msg )
{
	if (isMessageType(_msg,"semaineCallback")) {
		// determine origin by message's user-defined name, which can be set in the config file
		SMILE_IMSG(3,"received 'semaineCallback' message '%s'",_msg->msgname);
    if (!strncmp(_msg->msgname,"start",5)) { blockTurn = 1; unblockTurnCntdn = 0; }
    else if (!strncmp(_msg->msgname,"end",3)) {
      blockTurn = 0; unblockTurnCntdn = unblockTimeout;
    }
  /*  else if (!strncmp(_msg->msgname,"present",7)) { if (userPresence != 1) { userPresence = 1; doReset=1; } }
    else if (!strncmp(_msg->msgname,"absent",6)) { if (userPresence != 0) { userPresence = 0; doReset=1; } }*/
		return 1;  // message was processed
	}

	return 0; // if message was not processed
}


cTurnDetector::~cTurnDetector()
{
}

