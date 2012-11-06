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


#include <tumkwsaSink.hpp>

#define MODULE "cTumkwsaSink"


#ifdef HAVE_ATKLIB

#include <HThreads.h>

static const char * version="SALasr:  [MW/FE 29/05/09]";


SMILECOMPONENT_STATICS(cTumkwsaSink)

SMILECOMPONENT_REGCOMP(cTumkwsaSink)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CTUMKWSASINK;
  sdescription = COMPONENT_DESCRIPTION_CTUMKWSASINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

  SMILECOMPONENT_IFNOTREGAGAIN_BEGIN
    ct->setField("configfile","ASR configfile to load","asr.cfg");
    ct->setField("printResult","print classification/regression result to console (1/0=yes/no)",1);
    
  SMILECOMPONENT_IFNOTREGAGAIN_END

  SMILECOMPONENT_MAKEINFO(cTumkwsaSink);
}

SMILECOMPONENT_CREATE(cTumkwsaSink)

//-----

SMILE_THREAD_RETVAL ATKresultThreadRunner(void *_obj);
#define nLAG 10


cTumkwsaSink::cTumkwsaSink(const char *_name) :
  cDataSink(_name),
  printResult(0),
  feChan(NULL),
  ansChan(NULL),
  rman(NULL),
  arec(NULL),
  hset(NULL),
  dict(NULL),
  gram(NULL),
  //ngram(NULL),
  //amon(NULL),
  //wasSpeaking(0),
  queue(NULL),
  curTime(0),
  prevSpeakingIndex(0),
  thisSpeakingIndex(0),
  countdown(0),
  atkIsSetup(0)
{
  smileMutexCreate(terminatedMtx);
}

void cTumkwsaSink::fetchConfig()
{
  cDataSink::fetchConfig();
  
  configfile = getStr("configfile");
  SMILE_DBG(3,"ASR configfile to load = '%s'",configfile);

  printResult = getInt("printResult");
  SMILE_DBG(3,"printResult = %i",printResult);

  queue = new ObsDataQueue(nLAG);
}

int cTumkwsaSink::setupATK()
{
  try {

  // ------- load HTK config......
  int argc=3;
  const char* argv[3] = {"ASR","-C",NULL};
  if (configfile != NULL)
    argv[2]=configfile;
  else
    argv[2]="asr.cfg";

  if (InitHTK(argc,(char**)argv,version,TRUE)<SUCCESS){
    COMP_ERR("failed to initialise ASR subsystem!");
  }

  ConfParam *cParm[MAXGLOBS];       /* config parameters */
  int numParm,i;
  char ngramFN[100],buf[100];
  ngramFN[0] = '\0';

  // Read configuration parms for ANGRAM to see if NGram used
  numParm = GetConfig((char *)"ANGRAM", TRUE, cParm, MAXGLOBS);
  if (numParm>0){
    if (GetConfStr(cParm,numParm,(char *)"NGRAMFILE",buf)) strcpy(ngramFN,buf);
  }

  // Create Buffers
  feChan = new ABuffer("feChan");
  ansChan = new ABuffer("ansChan");

  // create a resource manager
  rman = new ARMan();

  arec = new ARec("ARec",feChan,ansChan,rman,0);

  // create global resources
  hset = new AHmms("HmmSet"); // load info in config
  dict = new ADict("ADict");
  gram = new AGram("AGram");
  rman->StoreHMMs(hset);
  rman->StoreDict(dict);
  rman->StoreGram(gram);

  ResourceGroup *main = rman->NewGroup("main");
  main->AddHMMs(hset);
  main->AddDict(dict);
  main->AddGram(gram);

  if (strlen(ngramFN)>0){
    ANGram * ngram  = new ANGram("ANGram");
    rman->StoreNGram(ngram);
    main->AddNGram(ngram);
  }

  arec->Start();
  arec->SendMessage("usegrp(main)");
  arec->SendMessage("start()");

  terminated = FALSE;

  }
  catch (ATK_Error e){ ReportErrors((char*)"ATK",e.i); return 0;}
  catch (HTK_Error e){ ReportErrors((char*)"HTK",e.i); return 0;}

  HThread t = HCreateThread("resultThread", 0, HPRIO_NORM, ATKresultThreadRunner, this);
  ATKresThread = t->thread;
  //smileThreadCreate( ATKresThread, ATKresultThreadRunner, this );

  atkIsSetup=1;
  
  return 1;
}

/*
int cTumkwsaSink::myFinaliseInstance()
{
  int ap=0;

  int ret=1;
  ret *= cDataSink::myFinaliseInstance();
  if (ret==0) return 0;

//  ret *= setupATK();
  
  return ret;
}
*/

// get features and put into packet
int cTumkwsaSink::makeFeaturePacket(FLOAT_DMEM *frame, int Nsrc, int isTurn, APacket **p)
{
	int i;

    if (Nsrc != 39) {
      SMILE_WRN(1,"wrong dimension of ASR input feature vector! expected 39 (13 mfcc + delta + delta) , found %i",Nsrc);
      return 0;
    }

	BufferInfo info;
	info.srcPK = MFCC + HASZEROC + HASDELTA + HASACCS + HASZEROM ;
//	info.srcFF = 0; //WAVEFILE;
	info.srcSampRate = 625;
	info.frSize = Nsrc*4;
	info.frRate = 100000;
	info.tgtPK = MFCC + HASZEROC + HASDELTA + HASACCS + HASZEROM ;
	info.tgtSampRate = 100000;
	info.tgtVecSize = Nsrc;
	info.spDetSil = 0;
	info.chPeak = 0;
	info.spDetSp = 0;
	info.spDetSNR = 0;
	info.spDetThresh = 0;
	info.curVol = 0;
	info.matTranFN = NULL;

	AObsData * o = new AObsData(&info, 1);

	prevSpeakingIndex = thisSpeakingIndex;
	thisSpeakingIndex = isTurn;
	if (prevSpeakingIndex==1 && thisSpeakingIndex==0) {
//		printf("turn end detected, now waiting 20 frames to compute best word hypothesis\n");
//		countdown=20;
		countdown=0;
 	}
	if (countdown > 0) {
		countdown=countdown-1;
		if ((countdown == 0)&&(isTurn!=1))
//		  	printf(" stopping decoder. recognition result will appear soon.\n");
			outputtrigger=1;
 	}
	if ((countdown > 0) || (isTurn==1)) {
		o->data.vq[0] = 1;
	} else {
		o->data.vq[0] = 0;
	}
//	if (prevSpeakingIndex==0.0 && thisSpeakingIndex==1.0) {
//		printf("turn start detected.\n");
//	}

//if ((features[speakingIndex]==0.0)&&(wasSpeaking==1)) { o->data.vq[0] = 1; wasSpeaking=0; }
//if ((features[speakingIndex]==1.0)) { wasSpeaking=1; }
//	o->data.fv[1][0] = (float)nFeaturesSelected;

	for (i=0; i<Nsrc; i++) {
      o->data.fv[1][i+1] = frame[i];
	}

//	short speakStatus = o->data.vq[0];
//	if (queue->PushObs(o) == 1) { // only generate packet if FIFO is full

//	  AObsData *tmp = queue->GetObs();
//	  tmp->data.vq[0] = speakStatus;  // use delayed packet, but current speaking status
	  *p = new APacket(o); //tmp

	  // TODO: USE SMILE TIMES HERE:::
	  (*p)->SetStartTime(curTime);
	  curTime += 100000; //info.tgtSampRate; // HARDC_SAMPR (=10) * 10000; //info.tgtSampRate;  //
	  (*p)->SetEndTime(curTime);
	  //
	  return 1;
//    }
	return 0;
}

void cTumkwsaSink::processResult(long long tick, long frameIdx, double time, APacket *p)
{
  if (printResult && (p!=NULL)) {
    p->Show();
  }
}

SMILE_THREAD_RETVAL ATKresultThreadRunner(void *_obj)
{
  cTumkwsaSink * __obj = (cTumkwsaSink *)_obj;
  if (_obj != NULL) {
    __obj->ATKresultThread();
  }
  SMILE_THREAD_RET;
}

void cTumkwsaSink::ATKresultThread()
{
  Boolean term;
  smileMutexLock(terminatedMtx);
  term = terminated;
  smileMutexUnlock(terminatedMtx);

  while (term==FALSE) {
    APacket p = ansChan->GetPacket();
    if (p.GetKind() == StringPacket){
      AStringData * sd = (AStringData *)p.GetData();
      if (sd->data.find("TERMINATED") != string::npos) {
        smileMutexLock(terminatedMtx);
        terminated = TRUE;
        smileMutexUnlock(terminatedMtx);
      }
    }

    // TODO: string output:  (call printResult function...)
    processResult(0, 0, 0.0, &p);

    smileMutexLock(terminatedMtx);
    term = terminated;
    smileMutexUnlock(terminatedMtx);
  }
}

int cTumkwsaSink::myTick(long long t)
{
  if (!atkIsSetup) {
    if (!setupATK()) return 0;
  }
  
  SMILE_DBG(4,"tick # %i, passing frame to ASR engine",t);
  cVector *vec= reader->getNextFrame();  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  //else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;

  APacket * p;
  if (makeFeaturePacket(vec->dataF,vec->N-1, (int)(vec->dataF[vec->N-1]), &p)) {
    feChan->PutPacket(*p);
    delete p;
  }

  // tick success?
  int res = 0;
  smileMutexLock(terminatedMtx);
  if (terminated == FALSE) res = 1;
  smileMutexUnlock(terminatedMtx);

  return res;
}


cTumkwsaSink::~cTumkwsaSink()
{
//TODO: properly terminate ATK threads...

  if (arec != NULL) {
    arec->SendMessage("terminate()");
    arec->Join();
  }
  smileMutexLock(terminatedMtx);
  terminated = TRUE;
  smileMutexUnlock(terminatedMtx);
  smileThreadJoin(ATKresThread);

  if(arec!=NULL) delete arec;

  if(rman!=NULL) delete rman;
  if(hset!=NULL) delete hset;
  if(dict!=NULL) delete dict;
  if(gram!=NULL) delete gram;
  if(feChan!=NULL) delete feChan;
  if(ansChan!=NULL) delete ansChan;

  if (queue != NULL) delete queue;
  
  smileMutexDestroy(terminatedMtx);
}


/**********************************************************************/

ObsDataQueue::ObsDataQueue(int _nEl) :
  nEl(_nEl),
  ptr(0),
  n(0)
{
  int i;
  buf = (AObsData**)malloc(sizeof(AObsData*)*nEl);
  for (i=0; i<nEl; i++) buf[i] = NULL;
}

// return value:
// 0: space left in fifo
// 1: fifo full after adding x
// 2: fifo full, x cannot be added
int ObsDataQueue::PushObs(AObsData *x)
{
  if (n==nEl) return 2;
  if (buf[ptr] != NULL) delete buf[ptr];
  buf[ptr] = x;
  ptr = (ptr+1)%nEl;
  n++;
  if (n==nEl) return 1;
  else return 0;
}

// returns NULL if no more data is in FIFO
AObsData * ObsDataQueue::GetObs()
{
  AObsData *tmp=NULL;
  if (n>0) {
    tmp=buf[ptr];
    buf[ptr] = NULL;
    n--;
  }
  return tmp;
}

ObsDataQueue::~ObsDataQueue()
{
  int i;
  for (i=0; i<nEl; i++) if (buf[i] != NULL) delete buf[i];
  free(buf);
}

#endif //HAVE_ATKLIB

