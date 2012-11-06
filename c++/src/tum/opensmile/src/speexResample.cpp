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

Resampling of pcm audio data using functions from speexDsp library

*/


#include <speexResample.hpp>
#include <math.h>

#define MODULE "cSpeexResample"

//#define HAVE_SPEEXLIB
#ifdef HAVE_SPEEXLIB


SMILECOMPONENT_STATICS(cSpeexResample)

SMILECOMPONENT_REGCOMP(cSpeexResample)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSPEEXRESAMPLE;
  sdescription = COMPONENT_DESCRIPTION_CSPEEXRESAMPLE;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN(
    //ct->setField("blocksize","blocksize to process at once in seconds (if =0.0: frame input mode is assumed: to be implemented!)",0.1); 
    ct->setField("blocksize_sec", NULL ,0.02); 
    ct->setField("frameInput","1 = read single frames (vectors) instead of blocks, 0 = read blocks",0);
    ct->setField("targetRate","sampling rate in Hz of output audio",16000);
    ct->setField("forceSourceRate","source sampling rate is usually determined by the input level, however you may use this parameter to override that setting. if you assign a value > 0 to this parameter it specifies the input sampling rate in Hz. ",0);
    ct->setField("quality","resampling quality ( low -> high: 1 - 10 )",7);
  )

  SMILECOMPONENT_MAKEINFO(cSpeexResample);
}

SMILECOMPONENT_CREATE(cSpeexResample)

//-----

cSpeexResample::cSpeexResample(const char *_name) :
  cDataProcessor(_name),
  resampSt(NULL),
  targetRate(0),
  forceSourceRate(0),
  frameInput(0),
  //blocksize(1000),
  //blocksizeS(0.0),
  matnew(NULL), in(NULL), out(NULL)
{
}

void cSpeexResample::mySetEnvironment()
{
  cDataProcessor::mySetEnvironment();
}

void cSpeexResample::fetchConfig()
{
  cDataProcessor::fetchConfig();

  targetRate = getInt("targetRate");

  forceSourceRate = getInt("forceSourceRate");

  //blocksizeS = getDouble("blocksize");

  quality = getInt("quality");
}

/*
int cSpeexResample::myConfigureInstance()
{
  return cDataProcessor::myConfigureInstance();
}
*/

int cSpeexResample::configureWriter(sDmLevelConfig &c)
{
  if (!frameInput) { // NOT frame input mode...
    TT = reader->getLevelT();

    if ((forceSourceRate <= 0.0)&&(TT!=0.0)) {
      forceSourceRate = (long)round(1.0/TT);
    } else {
      if (TT == 0.0) SMILE_IERR(1,"error computing sample rate of input level with period = 0!");
    }

    blocksizeW = c.blocksizeWriter = (long)ceil((float)blocksizeR*((float)targetRate/(float)forceSourceRate))+ 8;

  } else {
    c.blocksizeWriter = blocksizeR; // this will be changed later...
    // in frame input mode we need to know the value of N (number of elements in input level)
    // thus, we must do this computation in finalise...
  }

  c.T = (1.0) / (double)(targetRate);

  return 1;
}


int cSpeexResample::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();
  if (ret==0) return 0;

  if (frameInput) { // NOT frame input mode...
    // in frame input mode we do the blocksizeW and sourceRate computation here, because we need to know the value of N (number of elements in input level)
    if (reader->getLevelN() > 0) {
      TT = reader->getFrameSizeSec() / (double)(reader->getLevelN());
    } else {
      TT = 1.0;
    }
     
    if ((forceSourceRate <= 0.0)&&(TT!=0.0)) {
      forceSourceRate = (long)round(1.0/TT);
    } else {
      if (TT == 0.0) SMILE_IERR(1,"error computing sample rate of input level with period = 0!");
    }

    blocksizeW = (long)ceil((float)blocksizeR*((float)targetRate/(float)forceSourceRate))+ 8;
    writer->setBlocksize(blocksizeW);
    // TODO::: do we need an ouput blocksize for frameMode ?? shouldn't the output be a frame ..?

  } else {
   reader->setupSequentialMatrixReading(blocksizeR,blocksizeR);
  }

  if (!frameInput) { // NOT frame input mode
    nChannels = reader->getLevelN();   // number of elements is number of cahnnel: assumption, only ONE input!
  } else {
    nChannels = reader->getLevelNf();  // number fields = nChannels in frame input mode
  }

  int err=0;
  resampSt = speex_resampler_init(nChannels, forceSourceRate, targetRate, quality, &err);
  // set input /output stride ???


  return 1;
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cSpeexResample::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, resampling data",t);
  if (isEOI()) return 0;

  int ret = 1;

  //  printf("reader avail: %i\n",reader->getNAvail());
  //  printf("readerOutp avail: %i\n",readerOutp->getNAvail());

  if (!frameInput) { // NOT frame input mode...

    if (!(writer->checkWrite(blocksizeW))) { smileYield(); return 0; }

    cMatrix *_in=NULL;
    cMatrix *_out=NULL;
    long i;

    _in = reader->getNextMatrix();
    if (_in == NULL) { return 0; }

    if (matnew == NULL) matnew = new cMatrix(_in->N, blocksizeW, _in->type);
    else matnew->nT = blocksizeW;
    long x,y;

#if FLOAT_DMEM_NUM == FLOAT_DMEM_FLOAT
    in = _in->dataF;
    out = matnew->dataF;
#else
    if (in == NULL) in = malloc(sizeof(float)*_in->N*_in->nT);
    if (out == NULL) out = malloc(sizeof(float)*matnew->N*matnew->nT);

    // conversion is necessary
    for (i=0; i< (_in->N * _in->nT); i++) { in[i] = _in->dataF[i]; }
#endif

    spx_uint32_t inLen, outLen;
    inLen = (spx_uint32_t)(_in->nT);
    outLen = (spx_uint32_t)(matnew->nT); 

    int res = speex_resampler_process_interleaved_float(resampSt, in, &inLen, out, &outLen);

    // scale by a factor of 0.95 to avoid clipping due to resample filters
#if FLOAT_DMEM_NUM == FLOAT_DMEM_FLOAT
   for (i=0; i< (matnew->N * matnew->nT); i++) { matnew->dataF[i] *= 0.95; }
#else
    for (i=0; i< (matnew->N * matnew->nT); i++) { matnew->dataF[i] = out[i]*0.95; }
#endif

    matnew->nT = (long)outLen;
    
    matnew->tmetaClone(_in->tmeta, _in->nT); 
    matnew->tmetaResample(forceSourceRate, targetRate , 0) ; // TODO:::: scale tmeta...

    writer->setNextMatrix(matnew);

    return 1;

  } else { // frame input mode:
    SMILE_IERR(1,"frame input mode not yet implemented!");
    return 0; // NOT SUPPORTED YET
  }
}


cSpeexResample::~cSpeexResample()
{
  if (matnew != NULL) { delete matnew; }
#if FLOAT_DMEM_NUM != FLOAT_DMEM_FLOAT
  if (in != NULL) free(in);
  if (out != NULL) free(out);
#endif
  if (resampSt != NULL) { speex_resampler_destroy(resampSt); }
}

#endif // HAVE_SPEEXLIB
