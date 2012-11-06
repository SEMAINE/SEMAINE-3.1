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

TUM keyword spotter (ATK version)

*/


#ifndef __CTUMKWSASINK_HPP
#define __CTUMKWSASINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>

#ifdef HAVE_ATKLIB

#include <ARec.h>


#define COMPONENT_DESCRIPTION_CTUMKWSASINK "keyword spotter using ATK decoder"
#define COMPONENT_NAME_CTUMKWSASINK "cTumkwsaSink"


#undef class
class ObsDataQueue {
public:
  ObsDataQueue(int _nEl);
  ~ObsDataQueue();
  int PushObs(AObsData *x); // return false, if FIFO is full and packet cannot be added
  AObsData * GetObs();  // returns NULL if no more data is in FIFO
private:
  AObsData ** buf;
  int nEl,n;
  int ptr;
};

class DLLEXPORT cTumkwsaSink : public cDataSink {
  private:
    const char * configfile;
    smileMutex terminatedMtx;
    smileThread ATKresThread;
    
    //APacket *p;
    ABuffer *feChan;
    ABuffer *ansChan;
    ARMan *rman;
    ARec *arec;

    AHmms *hset; // load info in config
    ADict *dict;
    AGram *gram;

    ResourceGroup *main;
    
    Boolean terminated;
    
    string sentence;
    bool newsentence;
    float sentstart;
    float sentend;
    float sentconf;
    int wordcounter;

    int speakingIndex,seqIdx;
    int nFeaturesSelected;
    int *featureIndex;
    long long curTime;
    int wasSpeaking;
    int atkIsSetup;
    
    int prevSpeakingIndex;
    int thisSpeakingIndex;
    int countdown;
    bool outputtrigger;

    ObsDataQueue *queue;
    
    int makeFeaturePacket(FLOAT_DMEM *frame, int Nsrc, int isTurn, APacket **p);
    int setupATK();
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    int printResult;

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual void processResult(long long tick, long frameIdx, double time, APacket *p);

  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL

    void ATKresultThread();
    cTumkwsaSink(const char *_name);

    virtual ~cTumkwsaSink();
};


#endif //HAVE_ATKLIB



#endif // __CTUMKWSASINK_HPP
