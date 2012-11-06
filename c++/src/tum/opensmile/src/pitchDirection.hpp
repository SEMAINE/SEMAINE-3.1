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

example for dataProcessor descendant

*/


#ifndef __CPITCHDIRECTION_HPP
#define __CPITCHDIRECTION_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CPITCHDIRECTION "This component reads pitch data, detects pseudo syllables, and computes pitch direction estimates per syllable. Thereby the classes falling, flat, and rising are distinguished. \n    Required input fields: F0, F0env, and 'loudness' or 'RMSenergy'."
#define COMPONENT_NAME_CPITCHDIRECTION "cPitchDirection"

#undef class
class DLLEXPORT cPitchDirection : public cDataProcessor {
  private:
    cVector *myVec;
    long F0field, F0envField, LoudnessField, RMSField;
    double stbs, ltbs;
    long stbsFrames, ltbsFrames;
    FLOAT_DMEM * stbuf, * ltbuf;
    FLOAT_DMEM F0non0, lastF0non0, f0s;
    long stbufPtr, ltbufPtr; /* ring-buffer pointers */
    long bufInit; /* indicates wether buffer has been filled and valid values are to be expected */
    double ltSum, stSum;
    FLOAT_DMEM longF0Avg;
    long nFall,nRise,nFlat;

    int insyl;
    int f0cnt;
    FLOAT_DMEM lastE;
    FLOAT_DMEM startE, maxE, minE, endE;
    long sylen, maxPos, minPos, sylenLast;
    long sylCnt;
    double inpPeriod, timeCnt, lastSyl;

    FLOAT_DMEM startF0, lastF0, maxF0, minF0;
    long maxF0Pos, minF0Pos;

    const char *directionMsgRecp;
    int speakingRateBsize;

    int F0directionOutp, directionScoreOutp;
    int speakingRateOutp;
    int F0avgOutp, F0smoothOutp;

    /* speaking rate variables (buf0 is always the first half of buf1) */
    int nBuf0,nBuf1; /* number of frames collected in bufferA and bufferB */
    int nSyl0,nSyl1; /* number of syllable starts in bufferA and bufferB */
    double curSpkRate;

    int nEnabled;
    
    int isTurn, onlyTurn, invertTurn;
    int resetTurnData;
    const char * turnStartMessage, * turnEndMessage;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual void sendPitchDirectionResult(int result, double _smileTime, const char * _directionMsgRecp);

    virtual int processComponentMessage( cComponentMessage *_msg );

    // virtual int dataProcessorCustomFinalise();
     virtual int setupNewNames(long nEl);
    // virtual int setupNamesForField();
    virtual int configureWriter(sDmLevelConfig &c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchDirection(const char *_name);

    virtual ~cPitchDirection();
};




#endif // __CPITCHDIRECTION_HPP
