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


/*
 * Some code in this file is taken from the Julius LVCSR engine, which is copyright by:
 *
 * Copyright (c) 1991-2007 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2007 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 *
 * See the file JULIUS_LICENSE for more details
 *
 */



/*  openSMILE component:

TUM keyword spotter (Julius version)

*/


#include <tumkwsjSink.hpp>

#define MODULE "cTumkwsjSink"

#ifdef HAVE_JULIUSLIB

SMILECOMPONENT_STATICS(cTumkwsjSink)

SMILECOMPONENT_REGCOMP(cTumkwsjSink)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CTUMKWSJSINK;
  sdescription = COMPONENT_DESCRIPTION_CTUMKWSJSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

    SMILECOMPONENT_IFNOTREGAGAIN_BEGIN
    ct->makeMandatory(ct->setField("configfile","ASR configfile to load","kws.cfg"));
  ct->setField("logfile","Julius logfile (default=(null) : no log file)",(const char*)NULL);
  ct->setField("lmfile","a priori keyword log-prob weights to load from file (keyword [tab] logprobweight : on each line)",(const char*)NULL);
  ct->setField("dynamicLm","use a user defined weighting of keyword a-priori probabilities",0);
  ct->setField("logProbFloor","language model log probability floor value",-999999.0);

  ct->setField("gprob","garbage log. probability",0.0);
  ct->setField("kprob","keyword log. probability",0.0);
  ct->setField("lmpenalty","prior for all words, except those specified in lmfile (only relevant when using dynamicLm=1)",0.0);
  ct->setField("wlenWeight","weight of word length (prob += wordlenght*wlenWeight)",0.2);
  ct->setField("debug","1 = show julius debug log output, 0 = don't show",0);
  ct->setField("numphon","number of phonemes",0);
  ct->setField("preSil","silence period at beginning of turn to keep (in seconds)",0.3);
  ct->setField("postSil","silence period at end of turn to keep (in seconds)",0.4);
  ct->setField("nopass2","1 = do not perform pass2 (you must also set the '-1pass' option in the julius config file!)",0);
  ct->setField("printResult","print output to console (1/0=yes/no)",1);

  ct->setField("resultRecp","component(s) to send 'asrKeywordOutput' messages to (use , to separate multiple recepients), leave blank (NULL) to not send any messages",(const char *) NULL);
  ct->setField("offlineMode","1 = off-line evaluation mode (this changes the exit behaviour of the decoder; in online mode the decoder will always return 1 (busy) when decoding; in off-line mode it will return 0 if it is processing if not in the EOI processing loop. This option will also set maxTurnQue=0 (infinite).",0);
  ct->setField("maxTurnQue","maximum number of speech turns to hold in que, set this to 0 (infinite) for offline evaluations, and 2 (or more) for online mode",2);
  SMILECOMPONENT_IFNOTREGAGAIN_END

    SMILECOMPONENT_MAKEINFO(cTumkwsjSink);
}

SMILECOMPONENT_CREATE(cTumkwsjSink)

//-----

cTumkwsjSink::cTumkwsjSink(const char *_name) :
cDataSink(_name),
configfile(NULL),
logfile(NULL),
printResult(0),
juliusIsSetup(0), juliusIsRunning(0),
numphon(0), glogprob((FLOAT_DMEM)0.0), klogprob((FLOAT_DMEM)0.0),
dataFlag(0),
turnEnd(0), turnStart(0), isTurn(0), endWait(-1),
lag(0), nPre(0), nPost(0),
curVidx(0), vIdxStart(0), vIdxEnd(0), wst(0), writelen(0), resultRecp(NULL), period((FLOAT_DMEM)0.0),
turnStartSmileTimeLast((FLOAT_DMEM)0.0), turnStartSmileTime((FLOAT_DMEM)0.0), turnStartSmileTimeCur((FLOAT_DMEM)0.0),
decoderThread(NULL), wlenWeight((FLOAT_DMEM)0.2), running(0),
lmWeights(NULL),dynamicLm(0),lmWinfo(NULL),numWords(0),lmpenalty(0.0)//, midx(0)
{
  smileMutexCreate(terminatedMtx);
  smileMutexCreate(dataFlgMtx);
  smileCondCreate(tickCond);
}

void cTumkwsjSink::fetchConfig()
{
  cDataSink::fetchConfig();

  configfile = getStr("configfile");
  SMILE_IDBG(2,"ASR configfile to load = '%s'",configfile);

  logfile = getStr("logfile");
  SMILE_IDBG(2,"Julius logfile = '%s'",logfile);

  glogprob = (FLOAT_DMEM)getDouble("gprob");
  klogprob = (FLOAT_DMEM)getDouble("kprob");
  numphon = getInt("numphon");

  resultRecp = getStr("resultRecp");
  SMILE_IDBG(2,"resultRecp = '%s'",resultRecp);
  if (resultRecp != NULL) sendResult = 1;

  printResult = getInt("printResult");
  SMILE_IDBG(2,"printResult = %i",printResult);

  wlenWeight = (FLOAT_DMEM)getDouble("wlenWeight");
  SMILE_IDBG(2,"wlenWeight = %f",wlenWeight);

  dynamicLm = getInt("dynamicLm");
  SMILE_IDBG(2,"dynamicLm = %i",dynamicLm);

  lmpenalty = getDouble("lmpenalty");
  SMILE_IDBG(2,"lmpenalty = %i",lmpenalty);

  nopass2 = getInt("nopass2");
  offlineMode = getInt("offlineMode");
  maxTurnQue = getInt("maxTurnQue");
  if (offlineMode) maxTurnQue = 0;
}

/*

static boolean opt_gprob(Jconf *jconf, char *arg[], int argnum)
{
  SMILE_WRN(1,"please do not use -gprob in julius config (it is ignored!), use 'gprob' in openSMILE config instead!");
  return TRUE;
}

static boolean opt_kprob(Jconf *jconf, char *arg[], int argnum)
{
  SMILE_WRN(1,"please do not use -kprob in julius config (it is ignored!), use 'kprob' in openSMILE config instead!");
  return TRUE;
}

static boolean opt_knum(Jconf *jconf, char *arg[], int argnum)
{
  SMILE_WRN(1,"please do not use -knum in julius config file (it is ignored!), use 'numkw' in openSMILE config instead!");
  return TRUE;
}
*/


/********************************** Julius callback loaders ***********************************/

LOGPROB kwj_userlm_uni_loader(WORD_INFO *winfo, WORD_ID w, LOGPROB ngram_prob)
{
  cTumkwsjSink * cl = (cTumkwsjSink *)(winfo->userptr);
  if (cl!=NULL) return cl->cbUserlmUni(winfo, w, ngram_prob);
  else return 0.0;
}

LOGPROB kwj_userlm_bi_loader(WORD_INFO *winfo, WORD_ID context, WORD_ID w, LOGPROB ngram_prob)
{
  cTumkwsjSink * cl = (cTumkwsjSink *)(winfo->userptr);
  if (cl!=NULL) return cl->cbUserlmBi(winfo, context, w, ngram_prob);
  else return 0.0;
}

LOGPROB kwj_userlm_lm_loader(WORD_INFO *winfo, WORD_ID *contexts, int clen, WORD_ID w, LOGPROB ngram_prob)
{
  cTumkwsjSink * cl = (cTumkwsjSink *)(winfo->userptr);
  if (cl!=NULL) return cl->cbUserlmLm(winfo, contexts, clen, w, ngram_prob);
  else return 0.0;
}


int kwj_external_fv_read_loader(void * cl, float *vec, int n) 
{
  // retval -2 : error
  // retval -3 : segment input
  // retval -1 : end of input
  if (cl!=NULL) return ((cTumkwsjSink*)cl)->getFv(vec, n);
  else return -1; 
}


static void kwj_result_pass2_loader(Recog *recog, void *dummy)
{
  cTumkwsjSink * cl = (cTumkwsjSink *)(recog->hook);
  if (cl!=NULL) cl->cbResultPass2(recog, dummy);
}



static void kwj_status_pass1_begin_loader(Recog *recog, void *dummy)
{
  cTumkwsjSink * cl = (cTumkwsjSink *)(recog->hook);
  if (cl!=NULL) cl->cbStatusPass1Begin(recog, dummy);
}

static void kwj_result_pass1_loader(Recog *recog, void *dummy)
{
  cTumkwsjSink * cl = (cTumkwsjSink *)(recog->hook);
  if (cl!=NULL) cl->cbResultPass1(recog, dummy);
}

void kwj_result_pass1_current_loader(Recog *recog, void *dummy)
{
  cTumkwsjSink * cl = (cTumkwsjSink *)(recog->hook);
  if (cl!=NULL) cl->cbResultPass1Current(recog, dummy);
}



/********************************** Julius output formatting functions ******************************/

void cTumkwsjSink::juAlignPass1Keywords(RecogProcess *r, HTK_Param *param)
{
//  int n;
  Sentence *s;
  SentenceAlign *now;//, *prev;


  s = &(r->result.pass1);
  /* do forced alignment if needed */
  if ((r->config->annotate.align_result_word_flag)&&(s->word_num>0)) {
    //now = result_align_new();
    //if (s->align == NULL) 
      now = result_align_new(); //else now = s->align;
    word_align(s->word, s->word_num, param, now, r);
    if (s->align == NULL) //result_align_free(s->align);
      s->align = now;
    else {
      result_align_free(s->align);
      s->align = now;
    }
//    else prev->next = now;
//    prev = now;
  }
} 


void cTumkwsjSink::juPutHypoPhoneme(WORD_ID *seq, int n, WORD_INFO *winfo)
{
  int i,j;
  WORD_ID w;
  static char buf[MAX_HMMNAME_LEN];

  if (seq != NULL) {
    for (i=0;i<n;i++) {
      if (i > 0) printf(" |");
      w = seq[i];
      for (j=0;j<winfo->wlen[w];j++) {
        center_name(winfo->wseq[w][j]->name, buf);
        printf(" %s", buf);
      }
    }
  }
  printf("\n");  
}


void cTumkwsjSink::fillKresult(Kresult *k, WORD_ID *seq, int n, WORD_INFO *winfo, LOGPROB *cmscore, int numOfFrames, SentenceAlign *alignment)
{
  int i;
  int kwcount = 0;

  if (seq != NULL) {
    for (i=0;i<n;i++) {
      if (strcmp("garbage",winfo->woutput[seq[i]])!=0 && strcmp("<s>",winfo->woutput[seq[i]])!=0 && strcmp("</s>",winfo->woutput[seq[i]])!=0) {
        //keywords:
        k->keyword[kwcount]=winfo->woutput[seq[i]];
        if (alignment == NULL) {
          k->kwStartTime[kwcount]= 0.0;
        } else {
          k->kwStartTime[kwcount]= (period * (double)(alignment->begin_frame[i])); // ?? n != ??
        }
        k->kwConf[kwcount]=cmscore[i];
        kwcount++;
        if (kwcount >= MAXNUMKW) {
          SMILE_IWRN(2,"keywords were discarded, more than MAXNUMKW=%i (tumkwsjSink.hpp!) keywords detected!",MAXNUMKW);  
          i=n; break; 
        }
      }
    }
  }
  k->numOfKw=kwcount;
  k->turnDuration=numOfFrames;
}



/************************************ Callback handlers ************************************************/

LOGPROB cTumkwsjSink::keywordLm(WORD_INFO *winfo, WORD_ID w)
{
  float prob = klogprob;
  float wordlength = 0.0;
  int vocasize = winfo->num;
  if(w>vocasize-numphon-1) {
    prob=glogprob;
  } else {
    wordlength = winfo->wlen[w];
    prob = prob+(log(wordlength)*wlenWeight);
    if(w==1) {
      prob=2;
    }
    if (dynamicLm) {
      prob += lmWeights[w];
//      if (prob < logProbFloor) prob = logProbFloor;
    }
  }
  return prob;
}

LOGPROB cTumkwsjSink::cbUserlmLm(WORD_INFO *winfo, WORD_ID *contexts, int clen, WORD_ID w, LOGPROB ngram_prob)
{
  return keywordLm(winfo, w);
}


LOGPROB cTumkwsjSink::cbUserlmUni(WORD_INFO *winfo, WORD_ID w, LOGPROB ngram_prob)
{
  /*
  float prob = klogprob;
  float wordlength = 0.0;
  int vocasize = winfo->num;
  if(w>vocasize-numphon-1) {
    prob=glogprob;
  }	
  wordlength = winfo->wlen[w];
  prob = prob+(log(wordlength)*wlenWeight);
  if(w==1) {
    prob=2;
  }
  return prob;
  */
  return keywordLm(winfo, w);
}

LOGPROB cTumkwsjSink::cbUserlmBi(WORD_INFO *winfo, WORD_ID context, WORD_ID w, LOGPROB ngram_prob)
{
  /*
  float prob = klogprob;
  float wordlength = 0.0;
  int vocasize = winfo->num;
  if(w>vocasize-numphon-1) {
    prob=glogprob;
  }	
  wordlength = winfo->wlen[w];
  prob = prob+(log(wordlength)*wlenWeight);
  if(w==1) {
    prob=2;
  }
  return prob;
  */
  return keywordLm(winfo, w);
}



void cTumkwsjSink::cbStatusPass1Begin(Recog *recog, void *dummy)
{
  if (!recog->jconf->decodeopt.realtime_flag) {
    VERMES("### Recognition: 1st pass (LR beam)\n");
  }
  wst = 0;
}



void cTumkwsjSink::cbResultPass1Current(Recog *recog, void *dummy)
{
  //int i, j, bgn;
  //int len, 
  int num;
  WORD_INFO *winfo;
  WORD_ID *seq;
  RecogProcess *r;
  Sentence *s;
  r=recog->process_list;
  if (! r->live) return;
  if (! r->have_interim) return;

  winfo = r->lm->winfo;
  seq = r->result.pass1.word;
  num = r->result.pass1.word_num;
  s = &(r->result.pass1);
  printf("score = %f , score_am = %f  score_lm = %f",r->result.pass1.score,r->result.pass1.score_am,r->result.pass1.score_lm);
  if ((num>0)&&(!isAbort())) {
    //juAlignPass1Keywords(r, r->am->mfcc->param);

    //create smile message:
    Kresult k;
    fillKresult(&k, seq, num, winfo, s->confidence, r->result.num_frame, /*s->align*/ NULL);
    if (resultRecp != NULL) {
      cComponentMessage msg("asrKeywordOutput");
      msg.intData[0] = 0;
      msg.custData = &k;
      msg.userTime1 = turnStartSmileTimeCur;
      msg.userTime2 = turnStartSmileTimeCur + ((double)(k.turnDuration))*period;
      sendComponentMessage( resultRecp, &msg );
      SMILE_IDBG(3,"sending 'asrKeywordOutput' message to '%s'",resultRecp);
    }
   //output content of k:
    int kc = 0;
    printf("----------current hypothesis:------------\n");
    printf("numOfKw:%i\n",k.numOfKw);
    printf("turnDuration:%i\n",k.turnDuration);
    printf("keywords: ");
    for (kc=0;kc<(k.numOfKw);kc++) {
      printf("%s ",k.keyword[kc]);
    }
    printf("\n");
    printf("kwConf: ");
    for (kc=0;kc<(k.numOfKw);kc++) {
      printf("%5.3f ",k.kwConf[kc]);
    }
    printf("\n");
    printf("kwStartTimes: ");
    for (kc=0;kc<(k.numOfKw);kc++) {
      printf("%.3f ",k.kwStartTime[kc]);
    }
    printf("\n");
    printf("-----------------------------------------\n");

    printf("\n\n");
    fflush(stdout);

  }
}




void cTumkwsjSink::cbResultPass1(Recog *recog, void *dummy)
{
  int i,j;
  static char buf[MAX_HMMNAME_LEN];
  WORD_INFO *winfo;
  WORD_ID *seq;
  int num;
  RecogProcess *r;
  Sentence *s;
  boolean multi;
//  int len;
  boolean have_progout = TRUE;

  if (nopass2) {
    lockMessageMemory();
    if (tsq.empty()) running = 0;
    unlockMessageMemory();
  }

  if (isAbort()) return;

  if (recog->process_list->next != NULL) multi = TRUE;
  else multi = FALSE;

  r=recog->process_list;
  if (! r->live) return;
  if (r->result.status < 0) return;	/* search already failed  */
  if (have_progout && r->config->successive.enabled) return; /* short pause segmentation */
  if (r->config->output.progout_flag) printf("\r");

  winfo = r->lm->winfo;
  seq = r->result.pass1.word;
  num = r->result.pass1.word_num;
  s = &(r->result.pass1);
  /* words */


  if (verbose_flag) {		/* output further info */
    /* phoneme sequence */
    printf("p1_phon:");
    for (i=0;i<num;i++) {
      for (j=0;j<winfo->wlen[seq[i]];j++) {
        center_name(winfo->wseq[seq[i]][j]->name, buf);
        printf(" %s", buf);
      }
      if (i < num-1) printf(" |");
    }
    printf("\n");
    if (debug2_flag) {
    /* logical HMMs */
      printf("pass1_best_HMMseq_logical:");
      for (i=0;i<num;i++) {
        for (j=0;j<winfo->wlen[seq[i]];j++) {
          printf(" %s", winfo->wseq[seq[i]][j]->name);
        }
        if (i < num-1) printf(" |");
      }
      printf("\n");
    }
  }

  juAlignPass1Keywords(r, r->am->mfcc->param);

  //create smile message:
  Kresult k;
  fillKresult(&k, seq, num, winfo, s->confidence, r->result.num_frame, s->align);
  if (resultRecp != NULL) {
    cComponentMessage msg("asrKeywordOutput");
    msg.intData[0] = 1;
    msg.custData = &k;
    msg.userTime1 = turnStartSmileTimeCur;
    msg.userTime2 = turnStartSmileTimeCur + ((double)(k.turnDuration))*period;
    sendComponentMessage( resultRecp, &msg );
    SMILE_IDBG(3,"sending 'asrKeywordOutput' message to '%s'",resultRecp);
  }
 //output content of k:
  int kc = 0;
  printf("-------result package (pass 1):----------\n");
  printf("numOfKw:%i\n",k.numOfKw);
  printf("turnDuration:%i\n",k.turnDuration);
  printf("keywords: ");
  for (kc=0;kc<(k.numOfKw);kc++) {
    printf("%s ",k.keyword[kc]);
  }
  printf("\n");
  printf("kwConf: ");
  for (kc=0;kc<(k.numOfKw);kc++) {
    printf("%5.3f ",k.kwConf[kc]);
  }
  printf("\n");
  printf("kwStartTimes: ");
  for (kc=0;kc<(k.numOfKw);kc++) {
    printf("%.3f ",k.kwStartTime[kc]);
  }
  printf("\n");
  printf("-----------------------------------------\n");

  printf("\n\n");
  fflush(stdout);


}





void cTumkwsjSink::cbResultPass2(Recog *recog, void *dummy)
{
//  int i, j;
//  int len;
  char ec[5] = {0x1b, '[', '1', 'm', 0};
  WORD_INFO *winfo;
  WORD_ID *seq;
  int seqnum;
  int n, num;
  Sentence *s;
  RecogProcess *r;
  boolean multi;
//  HMM_Logical *p;
//  SentenceAlign *align;

  if (recog->process_list->next != NULL) multi = TRUE;
  else multi = FALSE;

  lockMessageMemory();
  if (tsq.empty()) running = 0;
  unlockMessageMemory();

  if (isAbort()) return;

  r=recog->process_list;
  if (! r->live) return;
  if (multi) printf("[#%d %s]\n", r->config->id, r->config->name);

  if (r->result.status < 0) {
    switch(r->result.status) {
    case J_RESULT_STATUS_REJECT_POWER:
      printf("<input rejected by power>\n");
      break;
    case J_RESULT_STATUS_TERMINATE:
      printf("<input teminated by request>\n");
      break;
    case J_RESULT_STATUS_ONLY_SILENCE:
      printf("<input rejected by decoder (silence input result)>\n");
      break;
    case J_RESULT_STATUS_REJECT_GMM:
      printf("<input rejected by GMM>\n");
      break;
    case J_RESULT_STATUS_REJECT_SHORT:
      printf("<input rejected by short input>\n");
      break;
    case J_RESULT_STATUS_FAIL:
      printf("<search failed>\n");
      break;
    }
    return;
  }

  winfo = r->lm->winfo;
//  num = r->result.sentnum;
//assume just one sentence:
  num = 1;
//consider just sentence with index n=0
  n = 0;
  s = &(r->result.sent[n]);
  seq = s->word;
  seqnum = s->word_num;
  if (debug2_flag) {
    printf("\n%s",ec);		/* newline & bold on */
  }
  if (verbose_flag) {
    printf("p2_phon:");
    juPutHypoPhoneme(seq, seqnum, winfo);
  }
  if (debug2_flag) {
    ec[2] = '0';
    printf("%s\n",ec);		/* bold off & newline */
  }
  if (verbose_flag) {
    if (r->lmtype == LM_DFA) {
  /* output which grammar the hypothesis belongs to on multiple grammar */
  /* determine only by the last word */
      if (multigram_get_all_num(r->lm) > 1) {
	      printf("grammar%d: %d\n", n+1, s->gram_id);
      }
    }
  }
  //create smile message:
  Kresult k;
  fillKresult(&k, seq, seqnum, winfo, s->confidence, r->result.num_frame, s->align);
//  return;
  if (resultRecp != NULL) {
    cComponentMessage msg("asrKeywordOutput");
    msg.intData[0] = 2;
    msg.custData = &k;
    msg.userTime1 = turnStartSmileTimeCur;
    msg.userTime2 = turnStartSmileTimeCur + ((double)(k.turnDuration))*period;
    sendComponentMessage( resultRecp, &msg );
    SMILE_IDBG(3,"sending 'asrKeywordOutput' message to '%s'",resultRecp);
  }
  //output content of k:
  int kc = 0;
  printf("-------result package (pass 2):----------\n");
  printf("numOfKw:%i\n",k.numOfKw);
  printf("turnDuration:%i\n",k.turnDuration);
  printf("keywords: ");
  for (kc=0;kc<(k.numOfKw);kc++) {
    printf("%s ",k.keyword[kc]);
  }
  printf("\n");
  printf("kwConf: ");
  for (kc=0;kc<(k.numOfKw);kc++) {
    printf("%5.3f ",k.kwConf[kc]);
  }
  printf("\n");
  printf("kwStartTimes: ");
  for (kc=0;kc<(k.numOfKw);kc++) {
    printf("%.3f ",k.kwStartTime[kc]);
  }
  printf("\n");
  printf("-----------------------------------------\n");

  printf("\n\n\n");
  fflush(stdout);

}




void cTumkwsjSink::setupCallbacks(Recog *recog, void *data)
{
  //  callback_add(recog, CALLBACK_EVENT_PROCESS_ONLINE, status_process_online, data);
  //  callback_add(recog, CALLBACK_EVENT_PROCESS_OFFLINE, status_process_offline, data);
  //  callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recready, data);
  //  callback_add(recog, CALLBACK_EVENT_SPEECH_START, status_recstart, data);
  //  callback_add(recog, CALLBACK_EVENT_SPEECH_STOP, status_recend, data);
  //  callback_add(recog, CALLBACK_EVENT_RECOGNITION_BEGIN, status_recognition_begin, data);
  //  callback_add(recog, CALLBACK_EVENT_RECOGNITION_END, status_recognition_end, data);
  //  if (recog->jconf->decodeopt.segment) { /* short pause segmentation */
  //    callback_add(recog, CALLBACK_EVENT_SEGMENT_BEGIN, status_segment_begin, data);
  //    callback_add(recog, CALLBACK_EVENT_SEGMENT_END, status_segment_end, data);
  //  }

  callback_add(recog, CALLBACK_EVENT_PASS1_BEGIN, kwj_status_pass1_begin_loader, data); //sets wst=0

  //  {
  //    JCONF_SEARCH *s;
  //    boolean ok_p;
  //    ok_p = TRUE;
  //    for(s=recog->jconf->search_root;s;s=s->next) {
  //      if (s->output.progout_flag) ok_p = FALSE;
  //    }
  //    if (ok_p) {      
  //      have_progout = FALSE;
  //    } else {
  //      have_progout = TRUE;
  //    }
  //  }
  //  if (!recog->jconf->decodeopt.realtime_flag && verbose_flag && ! have_progout) {
  //    callback_add(recog, CALLBACK_EVENT_PASS1_FRAME, frame_indicator, data);
  //  }

  callback_add(recog, CALLBACK_RESULT_PASS1_INTERIM, kwj_result_pass1_current_loader, data);
  callback_add(recog, CALLBACK_RESULT_PASS1, kwj_result_pass1_loader, data);

  //#ifdef WORD_GRAPH
  //  callback_add(recog, CALLBACK_RESULT_PASS1_GRAPH, result_pass1_graph, data);
  //#endif
  //  callback_add(recog, CALLBACK_EVENT_PASS1_END, status_pass1_end, data);
  //  callback_add(recog, CALLBACK_STATUS_PARAM, status_param, data);
  //  callback_add(recog, CALLBACK_EVENT_PASS2_BEGIN, status_pass2_begin, data);
  //  callback_add(recog, CALLBACK_EVENT_PASS2_END, status_pass2_end, data);

  callback_add(recog, CALLBACK_RESULT, kwj_result_pass2_loader, data); // rejected, failed

  //  callback_add(recog, CALLBACK_RESULT_GMM, result_gmm, data);
  //  /* below will be called when "-lattice" is specified */
  //  callback_add(recog, CALLBACK_RESULT_GRAPH, result_graph, data);
  //  /* below will be called when "-confnet" is specified */
  //  callback_add(recog, CALLBACK_RESULT_CONFNET, result_confnet, data);
  //
  //  //callback_add_adin(CALLBACK_ADIN_CAPTURED, levelmeter, data);
}


/*
parse a line of the format:
  word <tab> logprob_weight
add the logprob to the lmWeights array at the correct position
*/
int cTumkwsjSink::parseLmWeightsLine(char *line, long lineNr, const char
*lmfile)
{
  int ret = 0;
  if (line != NULL) {

    char *l=line;
    long le = (long)strlen(line);
    while((le>0)&&((l[le-1]=='\n')||(l[le-1]=='\r'))) { l[le-1] = 0; le--; }
    char *cf = strchr(l,'\t');
    if (cf == NULL) cf = strchr(l,' ');
    if (cf != NULL) {
      *cf=0; cf++;
      le=(long)strlen(cf);
      while ((le>0)&&(cf[0]=='\t')||(cf[0]==' ')) {cf++; le--;}
      while ((le>0)&&(cf[le-1]=='\t')||(cf[le-1]==' ')) {cf[le-1]=0; le--;}
      double lmW=0.0; char *ep=NULL;
      if (le > 0) lmW = strtod(cf,&ep);


      le=(long)strlen(l);
      while ((le>0)&&(l[0]=='\t')||(l[0]==' ')) {l++; le--;}
      while ((le>0)&&(l[le-1]=='\t')||(l[le-1]==' ')) {l[le-1]=0; le--;}

      if (ep == cf) {
        SMILE_IERR(1,"parse error on line %i of lmweights file '%s', expected a number after the <tab> cf='%s'",lineNr,lmfile,cf);
        lmW = 0.0;
      }

      if (le>0) {
        int i;
        for (i=0; i<numWords; i++) {
        //printf("XXX %s -",lmWinfo->woutput[i]);
          if (strcasecmp(lmWinfo->woutput[i],l) == 0) {
            SMILE_IMSG(3,"adding lmweight %f for word '%s'",lmW,l);
            lmWeights[i] = (LOGPROB)lmW;
            ret = 1;
            break;
          }
        }
      }

    }

  }
  return ret;
}


int cTumkwsjSink::setupJulius()
{
  try {

    int argc=3;
    char* argv[3] = {"julius","-C",NULL};
    if (configfile != NULL)
      argv[2]=strdup(configfile);
    else
      argv[2]=strdup("kws.cfg");

    /* add application option dummies */
    /*
    j_add_option("-gprob", 1, 1, "garbage probability", opt_gprob);
    j_add_option("-kprob", 1, 1, "keyword probability", opt_kprob);
    j_add_option("-knum", 1, 1, "number of keywords", opt_knum);
    */  

    /* create a configuration variables container */
    jconf = j_jconf_new();
    //    j_config_load_file(jconf, strdup(configfile));
    if (j_config_load_args(jconf, argc, argv) == -1) {
      COMP_ERR("error parsing julius decoder options, this might be a bug. see tumkwsjSink.cpp!");
      j_jconf_free(jconf);
      free(argv[2]);
      return 0;
    }
    free(argv[2]);

    /* output system log to a file */
    if (getInt("debug") != 1) {
      jlog_set_output(NULL);
    }

    /* here you can set/modify any parameter in the jconf before setup */
    jconf->input.type = INPUT_VECTOR;
    jconf->input.speech_input = SP_EXTERN;
    jconf->decodeopt.realtime_flag = TRUE; // ??? 
    jconf->ext.period = (float)(reader->getLevelT());
    jconf->ext.veclen = reader->getLevelN();
    jconf->ext.userptr = (void *)this;
    jconf->ext.fv_read = kwj_external_fv_read_loader;

    /* Fixate jconf parameters: it checks whether the jconf parameters
    are suitable for recognition or not, and set some internal
    parameters according to the values for recognition.  Modifying
    a value in jconf after this function may be errorous.
    */
    if (j_jconf_finalize(jconf) == FALSE) {
      SMILE_IERR(1,"error finalising julius jconf in setupJulius()!");
      j_jconf_free(jconf);
      return 0;
    }

    /* create a recognition instance */
    recog = j_recog_new();
    /* use user-definable data hook to set a pointer to our class instance */
    recog->hook = (void *)this;
    /* assign configuration to the instance */
    recog->jconf = jconf;
    /* load all files according to the configurations */
    if (j_load_all(recog, jconf) == FALSE) {
      SMILE_IERR(1, "Error in loading model for Julius decoder");
      j_recog_free(recog);
      return 0;
    }

    SMILE_IMSG(2,"garbage prob.: %f",glogprob);
    SMILE_IMSG(2,"keyword prob.: %f",klogprob);
    SMILE_IMSG(2,"number of phonemes: %i",numphon);

    // register user LM, get vocab size, and init lmWeights with zero or load from file
    PROCESS_LM *lm; 
    for(lm=recog->lmlist;lm;lm=lm->next) {
      if (lm->lmtype == LM_PROB) {
        lm->winfo->userptr = (void*)this;  // PATCH: sent/vocabulary.h (WORD_INFO struct): ++   void * userptr;   // Pointer to userdata....
        j_regist_user_lm_func(lm, kwj_userlm_uni_loader, kwj_userlm_bi_loader, kwj_userlm_lm_loader);
        if ((numWords==0)&&((long)(lm->winfo->num)>0)) {
          lmWinfo = lm->winfo;
          numWords = (long)(lm->winfo->num);
        }
      }
    }

    // load lmWeights data:
    //printf("XXX HEREA");
    if (dynamicLm) {
      //printf("XXX HERE0");
      if (lmWinfo != NULL) {
        //printf("XXX HERE");
        lmWeights = (LOGPROB*)malloc(sizeof(LOGPROB)*numWords);
        if (lmWeights == NULL) { OUT_OF_MEMORY; }
        int i;
        for (i=0; i<numWords; i++) { lmWeights[i] = (LOGPROB)lmpenalty; }
        const char *lmfile = getStr("lmfile");
        if (lmfile != NULL) {
          FILE *lf=fopen(lmfile,"r");
          if (lf == NULL) { SMILE_IERR(1,"Error opening word weights file (lmfile) '%s'",lmfile); }
          else  {
            SMILE_IMSG(1,"lmfile: '%s'",lmfile);
          long lineNr = 0;
          char *line=NULL; size_t len=0;
          size_t r=-1;
          do {      
            r = getline(&line, &len, lf);
              //printf("XXX LINE '%s'",line);
            if ((r != (size_t)-1)&&(line!=NULL)) {
              lineNr++;
              parseLmWeightsLine(line,lineNr,lmfile);
            } 
          } while (r != (size_t)-1);
        }
        }

      } else { 
        SMILE_IERR(1,"no language model word info (vocabulary) found, check julius config!"); 
      }
    }
    //----

    /* checkout for recognition: build lexicon tree, allocate cache */
    if (j_final_fusion(recog) == FALSE) {
      fprintf(stderr, "ERROR: Error while setup work area for recognition\n");
      j_recog_free(recog);
      if (logfile) fclose(fp);
      return 0;
    }

    setupCallbacks(recog, NULL);

    /* output system information to log */
    j_recog_info(recog);

    terminated = FALSE;

  }
  catch (int) { }

  juliusIsSetup=1;

  return 1;
}

int cTumkwsjSink::myFinaliseInstance()
{
  int ret = cDataSink::myFinaliseInstance();
  if (ret==0) return 0;

//  int ap=0;

  // setup pre/post silence config:
  float _preSil = (float)getDouble("preSil");
  float _postSil = (float)getDouble("postSil");
  double _T = reader->getLevelT();
  if (_T!=0.0) preSil = (int)ceil(_preSil/_T);
  else preSil = (int)(_preSil);
  if (_T!=0.0) postSil = (int)ceil(_postSil/_T);
  else postSil = (int)(_postSil);

  period = _T;

  // setup julius, if not already set up
  if (!juliusIsSetup) ret *= setupJulius();

  return ret;
}

/*
void cTumkwsjSink::processResult(long long tick, long frameIdx, double time)
{
  if (printResult) {
    //p->Show();
  }
}
*/

int cTumkwsjSink::processComponentMessage( cComponentMessage *_msg )
{
  if (isMessageType(_msg,"turnEnd")) {
    if (( teq.size() > (unsigned int)maxTurnQue )&&(maxTurnQue > 0))  {
      teq.pop();
      tsq.pop();
    }
    TurnMsg tem;
    tem.time = _msg->userTime1;
    tem.vIdx = (long)(_msg->floatData[1]);
    tem.isForcedTurnEnd = _msg->intData[1];
//    tem.midx = midx++;
    teq.push(tem);
    
    /*turnEnd=1;
    nPost = (long)(_msg->floatData[0]);
    vIdxEnd = (long)(_msg->floatData[1]);
    turnStartSmileTimeLast = turnStartSmileTime;*/
    return 1;
  }
  if (isMessageType(_msg,"turnStart")) {
    TurnMsg tsm;
    tsm.time = _msg->userTime1;
    tsm.vIdx = (long)(_msg->floatData[1]);
    tsm.isForcedTurnEnd = 0;
//    tsm.midx = midx++;
    tsq.push(tsm);
    vIdxStart = tsm.vIdx;
    /*
    turnStart=1;
    nPre = (long)(_msg->floatData[0]);
    vIdxStart = (long)(_msg->floatData[1]);
    turnStartSmileTime = _msg->userTime1;*/
    return 1;
  }
  return 0;
}

int cTumkwsjSink::checkMessageQueue()
{
    int end = 0;

    lockMessageMemory();

    // TODO: check size of message queue in online mode:
    // if messages at the front of the queue are too old, throw them away

    // handle pre/post silence and turn detector interface
    // turn start message received?
    if (!tsq.empty() && !isTurn) {
      SMILE_IDBG(4, "turn start message retrieved from queue");
      // process old turnEnd message first...
/*      if (!teq.empty() && teq.front().vIdx < tsq.front().vIdx && 
           ( (teq.front().vIdx <= curVidx + postSil) ||
              (!teq.front().isForcedTurnEnd && teq.front().vIdx <= curVidx) )) {
printf("XXXX TEMrFO %i (cur=%i) [%i]\n",teq.front().vIdx,curVidx,teq.front().midx);
        teq.pop();
        end = 1;   // stop waiting
        SMILE_IDBG(3,"processed turn end message!");  
      }*/
      running = 1;
      turnStartSmileTimeCur = tsq.front().time;
      vIdxStart = tsq.front().vIdx;
      curVidx = tsq.front().vIdx - preSil;
      isTurn = 1; // begin decoding
//printf("XXXX TSMr %i (cur=%i) [%i] (%s)\n",vIdxStart,curVidx,tsq.front().midx,getInstName());
      tsq.pop();
      SMILE_IDBG(3,"processed turn start message!"); 
    }
    // turn end message received?
    if (!teq.empty()) { 
      SMILE_IDBG(4, "turn end message retrieved from queue!"); 
      // end of turn reached?
      // if turn end is "forced" (end of input), then don't wait for silence at turn end
      // - otherwise we will wait forever ...
//printf("XXXX TEMx %i (cur=%i) [%i]\n",teq.front().vIdx,curVidx,teq.front().isForcedTurnEnd);
      if (( (!teq.front().isForcedTurnEnd) && curVidx >= teq.front().vIdx + postSil) || 
          ( (teq.front().isForcedTurnEnd) && curVidx >= teq.front().vIdx ))
      { 
//printf("XXXX TEMr %i (cur=%i) [%i] (%s)\n",teq.front().vIdx,curVidx,teq.front().midx,getInstName());
        teq.pop();
        isTurn = 0; 
        end=1; 
        SMILE_IDBG(3,"turn end reached!");  
      }
      // if no frames have been written...
      /*if (curVidx == vIdxStart) { 
        SMILE_IWRN(2,"turn (vIdx %i - vIdx %i) has 0 frames (curVidx = %i)!",vIdxStart,vIdxEnd,curVidx);
        turnEnd=0; isTurn=0;
      }*/
    }
//printf("XXX isTurn=%i\n",isTurn);
    unlockMessageMemory();

    return end;
}


// this is called from julius decoder thread... 
int cTumkwsjSink::getFv(float *vec, int n)
{ 
  int ret=0;

  smileMutexLock(dataFlgMtx);

  if (terminated) { 
    smileMutexUnlock(dataFlgMtx);  
    recog->process_want_terminate = TRUE;
    return -1; 
  }  

  // we should wait for main thread to provide data, then set a flag that data was read..
  SMILE_IDBG(4,"yes... julius requests features from us (n=%i)!",n);

  // wait for turn start
  int end=0;
  // Here we have to use a do loop, because otherwise turn end messages
  // will never be detected.
  do {

    end = checkMessageQueue();
    if (end) ret = -3;

    if (!isTurn && !end) {
      smileCondWaitWMtx( tickCond, dataFlgMtx ); // wait one smile tick for things to change
    }

    if (terminated) { 
      smileMutexUnlock(dataFlgMtx);  
      recog->process_want_terminate = TRUE;
      return -1; 
    }  

  } while ((!isTurn)&&(!end));

  //printf("turn getFv\n");

  if (isTurn && !end) {
    int result=0;
    curVec = NULL;
    while (curVec == NULL)  {
      curVec = reader->getFrame(curVidx, -1, 0, &result);
      // TODO: check if curVidx is behind ringbuffer storage space!!
      if (result & DMRES_OORleft) {
        long tmp = reader->getMinR()+10;
        if (tmp > curVidx) {
          SMILE_IWRN(2,"ASR processing tried to get feature data behind beginning of ringbuffer, which is no more available! Skipping a few frames to catch up... (%i) : %i -> %i",tmp-curVidx, curVidx, tmp);
          curVidx = tmp;
        }
      }
      if (curVec == NULL) { 
        if (checkMessageQueue()) {
          ret = -3;
          break;
        }
        smileCondWaitWMtx( tickCond, dataFlgMtx );  

        if (terminated) { 
          smileMutexUnlock(dataFlgMtx);  
          recog->process_want_terminate = TRUE;
          return -1; 
        }  
      }
      else {
        curVidx++;
      }
    } // while (curVec == NULL) 

  }

  //printf("turn: %f n=%i\n",curVec->dataF[curVec->N-1],curVec->N);
  smileMutexUnlock(dataFlgMtx);

  int i;
  if (curVec != NULL) {
    for (i=0; i<MIN( curVec->N, n ); i++) {
      vec[i] = (float)(curVec->dataF[i]);
    }
  } else {
    for (i=0; i<n; i++) {
      vec[i] = 0.0;
    }
  }

  // TODO: input segmentation (via smile ComponentMessages...??)

  return ret;
}


SMILE_THREAD_RETVAL kwj_juliusThreadRunner(void *_obj)
{
  cTumkwsjSink * __obj = (cTumkwsjSink *)_obj;
  if (_obj != NULL) {
    __obj->juliusDecoderThread();
  }
  SMILE_THREAD_RET;
}

void cTumkwsjSink::juliusDecoderThread()
{
  bool term;
  smileMutexLock(terminatedMtx);
  term = terminated;
  smileMutexUnlock(terminatedMtx);

  /* enter recongnition loop */
  int ret = j_open_stream(recog, NULL);
  switch(ret) {
      case 0:			/* succeeded */
        break;
      case -1:      		/* error */
        /* go on to next input */
        //continue;
      case -2:			/* end of recognition process */
        if (jconf->input.speech_input == SP_RAWFILE) {
          //          fprintf(stderr, "%d files processed\n", file_counter);
        } else if (jconf->input.speech_input == SP_STDIN) {
          fprintf(stderr, "reached end of input on stdin\n");
        } else {
          fprintf(stderr, "failed to begin input stream\n");
        }
        return;
  }
  /*
  if (outfile_enabled) {
  outfile_set_fname(j_get_current_filename());
  }
  */

  // TODO: terminate mechanism!!

  /* start recognizing the stream */
  ret = j_recognize_stream(recog);
}

int cTumkwsjSink::startJuliusDecoder()
{
  juliusIsRunning = 1;
  return (int)smileThreadCreate( decoderThread, kwj_juliusThreadRunner, this );
}

int cTumkwsjSink::myTick(long long t)
{
  printf("tick-...\n");
  if (!juliusIsRunning) {
    if (!startJuliusDecoder()) return 0;
  }
  smileCondSignal( tickCond );
  reader->catchupCurR();

  // tick success?
  lockMessageMemory();
  int res = running;
  unlockMessageMemory();

  // TODO: smile request SLEEP... if asr is processing data, request a sleep if all other components retun 0 (i.e. if processing would end if we return 0...)

  if ((getLastNrun() == 1) && res) {
    // we are the only component running...
    // --> sleep some time in this thread!
    if (isEOI()) {
      smileSleep((int)(reader->getLevelT()*5000.0));
    } else {
      smileSleep((int)(reader->getLevelT()*1000.0));
    } 
    // in offline mode we have to return 0 to enter EOI processing loop
    // (this enables us to access CMN data produced by a cFullinputMean component
    if (offlineMode && !isEOI()) res = 0;
  }
/*
smileMutexLock(terminatedMtx);
  if (terminated == FALSE) res = 1;
  smileMutexUnlock(terminatedMtx);
*/

  return res;
}


cTumkwsjSink::~cTumkwsjSink()
{
  smileMutexLock(dataFlgMtx);
  terminated = TRUE;
  smileCondSignal( tickCond );
  smileMutexUnlock(dataFlgMtx);
  
  if (!(decoderThread == NULL)) smileThreadJoin(decoderThread);

  /* release all */
  j_recog_free(recog);

  //if (logfile) fclose(fp);

  smileMutexDestroy(terminatedMtx);
  smileMutexDestroy(dataFlgMtx);
  smileCondDestroy(tickCond);
}


#endif // HAVE_JULIUSLIB

