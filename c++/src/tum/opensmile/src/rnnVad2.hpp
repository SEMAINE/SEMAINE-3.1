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

BLSTM RNN processor

*/


#ifndef __CRNNVAD2_HPP
#define __CRNNVAD2_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>
#include <rnn.hpp>

#define COMPONENT_DESCRIPTION_CRNNVAD2 "BLSTM RNN processor."
#define COMPONENT_NAME_CRNNVAD2 "cRnnVad2"

#undef class


/* ring buffer average computation over holdTime + decayTime segments */
/* hold of max over holdTime, decay over decayTime */
class cEavgHold 
{
private:
  /* moving average */
  FLOAT_DMEM *Ebuf;
  long EbufSize, nAvg;
  long EbufPtr, EbufPtrOld;
  double EavgSumCur;
  FLOAT_DMEM EmaxRiseAlpha; /* alpha for exponential rise smoothing of max energy: 1= rise immediately, 0=do not follow input */

  /* envelope */
  FLOAT_DMEM Emax;
  long EmaxAge;
  FLOAT_DMEM EmaxDecayStep;

  int holdTime;
  int decayTime;

public:
  /* holdTime and decayTime in frames/samples */
  cEavgHold(int _holdTime, int _decayTime, int _decayFunct=0, FLOAT_DMEM _EmaxRiseAlpha=0.2) :
      holdTime(_holdTime),
      decayTime(_decayTime),
      EmaxRiseAlpha(_EmaxRiseAlpha)
  {
    if (decayTime <= 0) decayTime = 1;
    if (holdTime <= 0) holdTime = 1;

    EbufSize = holdTime + decayTime;
    EbufPtr = 0;
    EbufPtrOld = 0;
    Ebuf = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*EbufSize);
    EavgSumCur = 0.0; nAvg = 0;
    Emax = 0.0;
    EmaxAge = 0;
    EmaxDecayStep = 0.0;
  }

  /* add next input to buffer */
  void nextE(FLOAT_DMEM E)
  {
    /* average: */
    EavgSumCur -= Ebuf[EbufPtrOld];
    Ebuf[EbufPtr] = E;
    EbufPtrOld = EbufPtr++;
    EavgSumCur += E;
    if (EbufPtr >= EbufSize) EbufPtr %= EbufSize;
    if (nAvg < EbufSize) nAvg++;
    /* envelope: */
    if (E > Emax) {
      Emax = Emax*((FLOAT_DMEM)1.0-EmaxRiseAlpha) + E*EmaxRiseAlpha;
      EmaxDecayStep = Emax/(FLOAT_DMEM)decayTime;
      EmaxAge = 0;
    } else {
      EmaxAge++;
      if (EmaxAge > holdTime && EmaxAge < holdTime+decayTime && Emax > EmaxDecayStep) {
        Emax -= EmaxDecayStep;
      }
    }
  }

  /* get the current short-term average */
  FLOAT_DMEM getAvg() 
  {
    if (nAvg > 100 || nAvg >= EbufSize) {
      return (FLOAT_DMEM) ( EavgSumCur / (double)nAvg ) ;
    } else { return 0.0; }
  }

  /* get the current envelope (max. hold) */
  FLOAT_DMEM getEnv() 
  {
    if (nAvg > 100  || nAvg >= EbufSize) return Emax;
    else return 0.0;
  }

  ~cEavgHold()
  {
    if (Ebuf != NULL) free(Ebuf);
  }
};


class DLLEXPORT cRnnVad2 : public cDataProcessor {
  private:
    cEavgHold * eUser;
    cEavgHold * eCurrent;
    cEavgHold * eAgent;
    cEavgHold * eBg;
    
    long voiceIdx, agentIdx, energyIdx, f0Idx;

    long cnt; 
    int isV;
    int vadDebug, allowEoverride;
    cVector *frameO;
    
    FLOAT_DMEM voiceThresh, agentThresh;
    
    //long agentBlockTime;
    long agentTurnCntdn;
    long agentTurnPastBlock;
    int alwaysRejectAgent, smartRejectAgent;
    int doReset, agentTurn, userPresence;
    long userCnt, agentCntdn;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cRnnVad2(const char *_name);

    virtual ~cRnnVad2();
};




#endif // __CRNNVAD2_HPP
