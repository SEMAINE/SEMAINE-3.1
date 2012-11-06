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

TUM keyword spotter (Julius version)

*/


#ifndef __CTUMKWSJSINK_HPP
#define __CTUMKWSJSINK_HPP

#pragma warning( disable : 4251 )

#include <smileCommon.hpp>
#include <dataSink.hpp>


//#define HAVE_JULIUSLIB
#ifdef HAVE_JULIUSLIB

// apr seems to define its own true, so we undefine it here... (we just have to make sure that we include this header file after any other apr/semaine/etc. headers
#ifdef TRUE
#undef TRUE
#endif 

extern "C" {
#undef LOG_DEBUG
#include <julius/juliuslib.h>
// julius defines min, max, abs, which collides with GNU stdlib and XQilla abs()
// there might be more of these silly lowercase(!) defines... Isn't it common practice to have all defines in uppercase where possible??
#undef abs
#undef min
#undef max
}


#define COMPONENT_DESCRIPTION_CTUMKWSJSINK "keyword spotter using Julius decoder"
#define COMPONENT_NAME_CTUMKWSJSINK "cTumkwsjSink"

#include <kwsjKresult.h>

#undef class


// A message received from the turn detector.
/*
struct TurnMsg {
    TurnMsg() : time(0), vIdx(0), isForcedTurnEnd(0) {}
    double time;     // smile time or whatever
    long vIdx;        // vector index
    int isForcedTurnEnd;  // forced turn end = at end of input
    long midx;
};

// A queue of turn detector messages.
typedef std::queue<TurnMsg> TurnMsgQueue;
*/

class DLLEXPORT cTumkwsjSink : public cDataSink {
  private:
    TurnMsgQueue tsq;   // turn start queue
    TurnMsgQueue teq;   // turn end queue
    int maxTurnQue;
    int running;
    int nopass2;
    int offlineMode;

    const char * configfile;
    smileMutex terminatedMtx;
    smileThread decoderThread;
    smileMutex dataFlgMtx;
    smileCond  tickCond;
    int juliusIsSetup, juliusIsRunning;
    double period;

    int sendResult;
    const char * resultRecp;

    int dynamicLm;
    double lmpenalty;
    LOGPROB * lmWeights;
    long numWords;
    WORD_INFO *lmWinfo;

    float glogprob;
    float klogprob;
    float wlenWeight;
    int numphon;

    // number of silence frames to keep at beginning and end of turn
    int preSil, postSil;
    long lag, endWait;
    int nPre, nPost;
    long curVidx, vIdxStart, vIdxEnd;

	  const char * logfile;
    FILE *fp;
    Recog *recog;
    Jconf *jconf;

    bool terminated;
    int dataFlag;
    int turnEnd; int turnStart; int isTurn;
    const cVector *curVec;

    double turnStartSmileTime, turnStartSmileTimeLast, turnStartSmileTimeCur;

    /*
    char * sentence;
    int speakingIndex,seqIdx;
    int nFeaturesSelected;
    int *featureIndex;
    long long curTime;
    int wasSpeaking;
  */  
    
/*
int prevSpeakingIndex;
    int thisSpeakingIndex;
    int countdown;
    bool outputtrigger;
*/

    /* required by julius output functions ? */
    int writelen;
    int wst;

    int setupJulius();
    int startJuliusDecoder();

    void setupCallbacks(Recog *recog, void *data);

    void fillKresult(Kresult *k, WORD_ID *seq, int n, WORD_INFO *winfo, LOGPROB *cmscore, int numOfFrames, SentenceAlign *alignment);
    void fillKresult1(Kresult *k, WORD_ID *seq, int n, WORD_INFO *winfo, LOGPROB *cmscore, int numOfFrames);
    void juPutHypoPhoneme(WORD_ID *seq, int n, WORD_INFO *winfo);
    void juAlignPass1Keywords(RecogProcess *r, HTK_Param *param);

    int checkMessageQueue();
    
    LOGPROB keywordLm(WORD_INFO *winfo, WORD_ID w);
    int parseLmWeightsLine(char *line, long lineNr, const char *lmfile);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    int printResult;

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    //virtual void processResult(long long tick, long frameIdx, double time);
    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL

   
    void ATKresultThread();
    cTumkwsjSink(const char *_name);

    void juliusDecoderThread();

      
      /* callbacks for julius : */

    int getFv(float *vec, int n);

    LOGPROB cbUserlmUni(WORD_INFO *winfo, WORD_ID w, LOGPROB ngram_prob);
    LOGPROB cbUserlmBi(WORD_INFO *winfo, WORD_ID context, WORD_ID w, LOGPROB ngram_prob);
    LOGPROB cbUserlmLm(WORD_INFO *winfo, WORD_ID *contexts, int clen, WORD_ID w, LOGPROB ngram_prob);

    void cbResultPass1(Recog *recog, void *dummy);
    void cbResultPass1Current(Recog *recog, void *dummy);
    void cbStatusPass1Begin(Recog *recog, void *dummy);

    void cbResultPass2(Recog *recog, void *dummy);

    virtual ~cTumkwsjSink();
};



#endif //HAVE_JULIUSLIB



#endif // __CTUMKWSJSINK_HPP
