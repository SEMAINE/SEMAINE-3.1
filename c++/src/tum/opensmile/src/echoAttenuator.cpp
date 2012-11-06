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

AEC  adaptive echo canceller

expects two input reader levels in THIS order: waveIn(recorded stream) ; waveOut(played stream)

*/


#include <echoAttenuator.hpp>
//#include <math.h>

#define MODULE "cEchoAttenuator"


SMILECOMPONENT_STATICS(cEchoAttenuator)

SMILECOMPONENT_REGCOMP(cEchoAttenuator)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CECHOATTENUATOR;
  sdescription = COMPONENT_DESCRIPTION_CECHOATTENUATOR;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  if (ct->setField("readerOutp", "data reader for wave output data (which is to be 'subtracted' from input)",
                  sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE) == -1) {
   rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("maxdelay","maximum delay in seconds",0.10);
    ct->setField("mindelay","minumum delay in seconds",0.00);
    //ct->setField("filterSize","size of filter window in seconds",0.02); 
    //ct->setField("blocksize","blocksize to process at once (>= filterSize) in seconds",0.1); 
    ct->setField("alpha","alpha fro exponential gliding average envelope computation",0.001); 
    ct->setField("modThresh","output amplitude activation threshold",0.001); 
    ct->setField("envWeight","output envelope weight",1.5); 
    ct->setField("fixedDelay","fixed delay for output driven input attenuation (in seconds)",0.04); 
  )

  SMILECOMPONENT_MAKEINFO(cEchoAttenuator);
}

SMILECOMPONENT_CREATE(cEchoAttenuator)

//-----

cEchoAttenuator::cEchoAttenuator(const char *_name) :
  cDataProcessor(_name),
  readerOutp(NULL),
  Cxy(NULL),
  Cyy(NULL),
  matnew(NULL),
  outEnv(0.0)
{
  char *tmp = myvprint("%s.readerOutp",getInstName());
  readerOutp = (cDataReader *)(cDataReader::create(tmp));
  if (reader == NULL) {
    COMP_ERR("Error creating dataReader '%s'",tmp);
  }
  if (tmp!=NULL) free(tmp);
}

void cEchoAttenuator::mySetEnvironment()
{
  cDataProcessor::mySetEnvironment();
  readerOutp->setComponentEnvironment(getCompMan(), -1, this);
}

void cEchoAttenuator::fetchConfig()
{
  cDataProcessor::fetchConfig();

  mindelay = (FLOAT_DMEM)getDouble("mindelay");
  SMILE_IDBG(2,"mindelay = %f",mindelay);
  if (mindelay<0.0) {
    SMILE_IERR(1,"mindelay must be >= 0! Setting mindelay=0.0 !");
    mindelay=0.0;
  }

  maxdelay = (FLOAT_DMEM)getDouble("maxdelay");
  SMILE_IDBG(2,"maxdelay = %f",maxdelay);
  if (maxdelay<0.0) {
    SMILE_IERR(1,"maxdelay must be >= 0! Setting maxdelay=0.0 !");
    maxdelay=0.0;
  }
  if (maxdelay < mindelay) {
    SMILE_IERR(1,"maxdelay must be >= mindelay! Setting maxdelay=mindelay (%f) !",mindelay);
    maxdelay=mindelay;
  }

  modThresh = getDouble("modThresh");
  envWeight = getDouble("envWeight");
  alpha = getDouble("alpha");
  fixedDelay = getDouble("fixedDelay");
}

int cEchoAttenuator::myConfigureInstance()
{
  int ret=1;
  ret *= readerOutp->configureInstance();
  if (ret)
    ret = cDataProcessor::myConfigureInstance();
  return ret;
}

int cEchoAttenuator::configureWriter(sDmLevelConfig &c)
{
  TT = c.T;
  
  if (TT != 0.0) {
    mindelayF = lastdelay = (long)floor(mindelay/TT);
    maxdelayF = (long)ceil(maxdelay/TT);
  } else {
    mindelayF = lastdelay = (long)floor(mindelay);
    maxdelayF = (long)ceil(maxdelay);
  }

  if (blocksizeW != blocksizeR) {
    blocksizeW = blocksizeR;
    blocksizeW_sec = blocksizeR_sec;
    c.blocksizeWriter = blocksizeR;
  }

  /*
  if (Cxy == NULL) Cxy = new double[maxdelayF+1];
  if (Cyy == NULL) Cyy = new double[2*filterSize];
*/
  reader->setupSequentialMatrixReading(blocksizeR,blocksizeR+maxdelayF,-maxdelayF);
  readerOutp->setupSequentialMatrixReading(blocksizeR,blocksizeR+maxdelayF,-maxdelayF);

  return 1;
}

int cEchoAttenuator::myFinaliseInstance()
{
  if (!readerOutp->finaliseInstance()) return 0;

  return cDataProcessor::myFinaliseInstance();
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cEchoAttenuator::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, filtering data",t);
  if (isEOI()) return 0;

  int ret = 1;
  
  if (!(writer->checkWrite(blocksizeW))) { smileYield(); return 0; }

  cMatrix *_in=NULL;
  cMatrix *_out=NULL;
  
  if (reader->isNextMatrixReadOk() && readerOutp->isNextMatrixReadOk()) {
    _in = reader->getNextMatrix();
    _out = readerOutp->getNextMatrix();
  } else { smileYield(); return 0; }
  
    // this should never happen!
  if ((_out==NULL)||(_in==NULL)) { SMILE_IERR(1,"inp and outp are out of sync... this should not happen.. it might be a bug in dataReader->isNextMatrixReadOk()!"); return 0; }  

  if (matnew == NULL) matnew = new cMatrix(_in->N, blocksizeW, _in->type);
  long x,y;

//  return 1;

  ///-----------------------

 // long N = filterSize;  // N should be = filterSize
                      // _pre should be = maxdelayF

  long size = maxdelayF + 1;

 
  long mydelay=0;
  if (TT!=0.0) mydelay = (long)floor(fixedDelay/TT);
  else mydelay = (long)floor(fixedDelay);
  
  if (mydelay > maxdelayF) mydelay=maxdelayF;

  // // filtered input data will be saved back to dataMemory:

  // simple attenuation of input data, based on output data envelope shifted by a fixed delay
  for (y=maxdelayF; y < maxdelayF+blocksizeW; y++) {
    outEnv = alpha * fabs(_out->dataF[(y-mydelay)*(_in->N)]) + (1.0-alpha)*outEnv;
    double f=1.0;
    if (outEnv > modThresh) {
      if (outEnv>1.0) outEnv = 1.0;
      f -= sqrt(outEnv)*envWeight;
      f = f*f*f;
      if (f<0.0) f=0.0;
    }
    for (x=0; x<_out->N; x++) {
      matnew->dataF[(y-maxdelayF)*(_out->N)+x] = _in->dataF[y*(_in->N)+x] * (FLOAT_DMEM)f;
    }
  }
  matnew->tmetaReplace(_in->tmeta); 

  writer->setNextMatrix(matnew);
  


  return 1;
}


cEchoAttenuator::~cEchoAttenuator()
{
  if (matnew != NULL) delete matnew;
  if (readerOutp != NULL) { delete readerOutp; }
  /*
  if (Cxy != NULL) delete[] Cxy;
  if (Cyy != NULL) delete[] Cyy;
  */
}

