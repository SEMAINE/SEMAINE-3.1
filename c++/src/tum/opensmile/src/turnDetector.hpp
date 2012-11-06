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


#ifndef __CTURNDETECTOR_HPP
#define __CTURNDETECTOR_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CTURNDETECTOR "Speaker turn detector using data from cVadV1 component or cSemaineSpeakerID1 (adaptive VAD) to determine speaker turns and identify continuous segments of voice activity."
#define COMPONENT_NAME_CTURNDETECTOR "cTurnDetector"


#undef class
class DLLEXPORT cTurnDetector : public cDataProcessor {
  private:
    FLOAT_DMEM threshold;
    int nPost, nPre;
    int useRMS;
    int turnState, actState;
    long startP, startP0, endP0;
    double endSmileTime, startSmileTime, startSmileTimeS;
    double turnTime, turnStep, msgInterval;
    double maxTurnLengthS, graceS;
    long maxTurnLength, grace;

    int blockAll, blockStatus;
    int blockTurn, unblockTurnCntdn, unblockTimeout;

    long eoiMis;
    int forceEnd;
    int timeout; double lastDataTime;
    double timeoutSec;

    int debug;
    int cnt1, cnt2, cnt1s, cnt2s;

    long rmsIdx, autoRmsIdx, readVad;
    int autoThreshold;

    // variables for auto threshold statistics:
    int nmin, nmax;
    FLOAT_DMEM minmaxDecay;
    FLOAT_DMEM rmin, rmax, rmaxlast;
    FLOAT_DMEM tmpmin, tmpmax;
    FLOAT_DMEM dtMeanO, dtMeanT, dtMeanS;
    FLOAT_DMEM alphaM;
    FLOAT_DMEM nE, dtMeanAll;
    long nTurn, nSil;
    int tmpCnt;
    int calCnt;

    const char *recFramer, *recComp, *statusRecp;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual int isVoice(FLOAT_DMEM *src);
    virtual void updateThreshold(FLOAT_DMEM eRmsCurrent);

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
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
    
    cTurnDetector(const char *_name);

    virtual ~cTurnDetector();
};




#endif // __CTURNDETECTOR_HPP
