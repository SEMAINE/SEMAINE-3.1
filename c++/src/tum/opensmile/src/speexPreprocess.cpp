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

Uses Speex preprocessor to :
  reduce noise,
  perform VAD
  ( perform AGC )
  ( reduce reverb )
  ( cancell echo ?? )

*/


#include <speexPreprocess.hpp>
#include <math.h>

#define MODULE "cSpeexPreprocess"

//#define HAVE_SPEEXLIB
#ifdef HAVE_SPEEXLIB


SMILECOMPONENT_STATICS(cSpeexPreprocess)

SMILECOMPONENT_REGCOMP(cSpeexPreprocess)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSPEEXPREPROCESS;
  sdescription = COMPONENT_DESCRIPTION_CSPEEXPREPROCESS;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

/* // TODO: ?? if reader has no dmLevel to read from... reader disables itsself..?
  if (ct->setField("readerOutp", "data reader for wave output data (which is to be 'subtracted' from input)",
                  sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE) == -1) {
   rA=1; // if subtype not yet found, request , re-register in the next iteration
  }
*/

  SMILECOMPONENT_IFNOTREGAGAIN(
//    ct->setField("maxdelay","maximum delay in seconds",0.10);
    //ct->setField("mindelay","minumum delay in seconds",0.00);
    //ct->setField("filterSize","size of filter window in seconds",0.02); 
    // TODO: frame input mode if blocksize == 0.0 !
    //ct->setField("blocksize","blocksize to process at once in seconds (if =0.0: frame input mode is assumed: to be implemented!)",0.1); 
    //ct->setField("alpha","alpha for exponential gliding average envelope computation",0.001); 
    //ct->setField("modThresh","output amplitude activation threshold",0.001); 
    //ct->setField("envWeight","output envelope weight",1.5); 
    //ct->setField("fixedDelay","fixed delay for output driven input attenuation (in seconds)",0.04); 
    ct->setField("enableDenoise","enable denoise filter in speex preprocessor",1);
    ct->setField("enableVAD","enable speex VAD (todo: provide output to turn detector!)",1);
    ct->setField("frameInput","1= process frames instead of blocks",0);
    ct->setField("blocksize_sec", NULL ,0.02); 
    //ct->setField("enableDereverb","enable speex dereverb filter",0);
    //ct->setField("enableAGC","enable speex automatic gain control filter",0);
    //ct->setField("enableEchoCancel","enable speex automatic gain control filter",0);
  )

  SMILECOMPONENT_MAKEINFO(cSpeexPreprocess);
}

SMILECOMPONENT_CREATE(cSpeexPreprocess)

//-----

cSpeexPreprocess::cSpeexPreprocess(const char *_name) :
  cDataProcessor(_name),
  //readerOutp(NULL),
  matnew(NULL),
  echoSt(NULL), ppState(NULL),
  ref_buf(NULL), echo_buf(NULL), e_buf(NULL),
  enableDenoise(0), enableAGC(0), enableVAD(0), enableDereverb(0), enableEchoCancel(0)
{
/*
  char *tmp = myvprint("%s.readerOutp",getInstName());
  readerOutp = (cDataReader *)(cDataReader::create(tmp));
  if (readerOutp == NULL) {
    COMP_ERR("Error creating dataReader '%s'",tmp);
  }
  if (tmp!=NULL) free(tmp);
  */
}

void cSpeexPreprocess::mySetEnvironment()
{
  cDataProcessor::mySetEnvironment();
  /*
  if (readerOutp != NULL) 
    readerOutp->setComponentEnvironment(getCompMan(), -1, this);
    */
}

void cSpeexPreprocess::fetchConfig()
{
  cDataProcessor::fetchConfig();

  enableDenoise = getInt("enableDenoise");
  enableVAD = getInt("enableVAD");
  frameInput = getDouble("frameInput");
  //filtersizeS = getDouble("filtersize");
}

int cSpeexPreprocess::myConfigureInstance()
{
  int ret=1;
  /*if (readerOutp != NULL) {
    ret *= readerOutp->configureInstance();
  }*/
  if (ret)
    return cDataProcessor::myConfigureInstance();
}

int cSpeexPreprocess::configureWriter(sDmLevelConfig &c)
{
  if (blocksizeW != blocksizeR) {
    blocksizeW = blocksizeR;
    blocksizeW_sec = blocksizeR_sec;
    c.blocksizeWriter = blocksizeR;
  }
}

int cSpeexPreprocess::myFinaliseInstance()
{
  int ret=1;
  /*if (readerOutp!=NULL) {
    int ret = readerOutp->finaliseInstance();
  }*/

  ret *= cDataProcessor::myFinaliseInstance();
  if (ret==0) return 0;

  int sampleRate = 0;  

  if (frameInput) { // frame input mode...
    if (reader->getLevelN() > 0) 
      TT = reader->getFrameSizeSec() / (double)(reader->getLevelN());
    else 
      TT = 1.0;
  } else {
    TT = reader->getLevelT();
  }

  sampleRate = (int)round(1.0/TT);

  if (!frameInput) { // NOT frame input mode
    reader->setupSequentialMatrixReading(blocksizeR,blocksizeR);
  }
  //  readerOutp->setupSequentialMatrixReading(blocksizeR,blocksizeR);


  int x;
  float f;
  ppState = speex_preprocess_state_init(blocksizeR, sampleRate);

  if (enableDenoise) {
    x=1;
    /*
    f=-20.0;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &f);
    */
  } else {
    x=0;
  }
  speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_DENOISE, &x);

  if (enableAGC) {
    x=8000; // TODO : config...
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_AGC_LEVEL, &x);
    x=1;
  } else {
    x=0;
  }
  speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_AGC, &x);

  if (enableDereverb) {
    x=1;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_DEREVERB, &x);
    f=0.1; // TODO: config...
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f=0.1; // TODO: config...
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
  } else {
    x=0;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_DEREVERB, &x);
    f=0.0;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &f);
    f=0.0;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &f);
  }

  if (enableVAD) {
    x=1;
    /*
    x=50;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_PROB_START, &x);
    x=50;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &x);
    */
  } else {
    x=0;
  }
  speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_VAD, &x);

  if (enableEchoCancel) {
  /*
    echoSt = speex_echo_state_init(blocksizeR, 4096);
    speex_echo_ctl(echoSt, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_ECHO_STATE, echoSt);
  */
    
    /*
    f=-50.0;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE, &f);
    f=-10.0;
    speex_preprocess_ctl(ppState, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &f);
    */

    ref_buf = (int16_t*)malloc(sizeof(int16_t)*blocksizeR);
    echo_buf = (int16_t*)malloc(sizeof(int16_t)*blocksizeR);
  }

  e_buf = (int16_t*)malloc(sizeof(int16_t)*blocksizeR);

  return 1;
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cSpeexPreprocess::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, filtering data",t);
  if (isEOI()) return 0;

  int ret = 1;

  //  printf("reader avail: %i\n",reader->getNAvail());
  //  printf("readerOutp avail: %i\n",readerOutp->getNAvail());

  if (!frameInput) { // NOT frame input mode...

    if (!(writer->checkWrite(blocksizeW))) { smileYield(); return 0; }

    cMatrix *_in=NULL;
    cMatrix *_out=NULL;

    /*
    if (readerOutp != NULL) {
      if (reader->isNextMatrixReadOk() && readerOutp->isNextMatrixReadOk()) {
        _in = reader->getNextMatrix();
        _out = readerOutp->getNextMatrix();
      } else { smileYield(); return 0; }
    } else {
    */
      if (reader->isNextMatrixReadOk()) {
        _in = reader->getNextMatrix();
      } else { smileYield(); return 0; }
    //}

    // this should never happen!
    //if ( ((_out==NULL)&&(readerOutp!=NULL)) || (_in==NULL) ) { 
    if (_in==NULL) {
      SMILE_IERR(1,"inp and outp are out of sync... this should not happen.. it might be a bug in dataReader->isNextMatrixReadOk()!"); 
      return 0; 
    }  

    if (matnew == NULL) matnew = new cMatrix(_in->N, blocksizeW, _in->type);
    long x,y;

    //  return 1;
    ///-----------------------

    //  long N = filterSize;  // N should be = filterSize
    // _pre should be = maxdelayF

    //long size = maxdelayF + 1;

    //long md = (long)round(fixedDelay/TT);


    if (enableEchoCancel) {
      for (x=0; x<blocksizeW; x++) {
        ref_buf[x] = (int16_t)round(_in->dataF[x*(_in->N)]*32767.0);
        echo_buf[x] = (int16_t)round(_out->dataF[x*(_out->N)]*32767.0);
        e_buf[x] = 0; //_out->dataF[x*(_out->N)];
      }
      speex_echo_cancellation(echoSt, ref_buf, echo_buf, e_buf);
    } else {
      for (x=0; x<blocksizeW; x++) {
        e_buf[x] = (int16_t)round(_in->dataF[x*(_in->N)]*32767.0);
      }
    }

    int vad = speex_preprocess_run(ppState, e_buf);

    if (enableVAD) printf("vad = %i ==> VOICE!\n",vad);
    // TODO: send VAD message, or write to output level...

    // FIXME:: speex's VAD is a kludge!!!  Get proper VAD!

    // filtered input data will be saved back to dataMemory:
    for (y=0; y < blocksizeW; y++) {
      for (x=0; x<_in->N; x++) {
        matnew->dataF[y*(_in->N)+x] = ((FLOAT_DMEM)(e_buf[y]))/32767.0; //_in->dataF[y*(_in->N)+x] * f;
      }
    }
    matnew->tmetaReplace(_in->tmeta); 

    writer->setNextMatrix(matnew);

    return 1;

  } else { // frame input mode:
    SMILE_IERR(1,"Frame input mode not supported yet!");
    return 0; // NOT SUPPORTED YET
  }
}


cSpeexPreprocess::~cSpeexPreprocess()
{
  if ((enableEchoCancel)&&(echoSt != NULL))
    speex_echo_state_destroy(echoSt);
  
  speex_preprocess_state_destroy(ppState);

  if (ref_buf != NULL) free(ref_buf);
  if (echo_buf != NULL) free(echo_buf);
  if (e_buf != NULL) free(e_buf);

  if (matnew != NULL) delete matnew;
  //if (readerOutp != NULL) { delete readerOutp; }
}

#endif // HAVE_SPEEXLIB
