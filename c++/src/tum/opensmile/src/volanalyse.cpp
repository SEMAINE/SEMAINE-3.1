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


/*  openSMILE component: preemphasis

volume analyser, sanity checks

*/


#include <volanalyse.hpp>
#include <math.h>

#define MODULE "cVolanalyse"


SMILECOMPONENT_STATICS(cVolanalyse)

SMILECOMPONENT_REGCOMP(cVolanalyse)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CVOLANALYSE;
  sdescription = COMPONENT_DESCRIPTION_CVOLANALYSE;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWinToVecProcessor")

	  SMILECOMPONENT_IFNOTREGAGAIN( {}
    //ct->setField("k","pre-emphasis coefficient y[n] = x[n] - k*x[n-1]",0.97);
  )

  SMILECOMPONENT_MAKEINFO(cVolanalyse);
}

SMILECOMPONENT_CREATE(cVolanalyse)

//-----

cVolanalyse::cVolanalyse(const char *_name) :
  cWinToVecProcessor(_name),
  vaData(NULL)
{
}


void cVolanalyse::fetchConfig()
{
  cWinToVecProcessor::fetchConfig();
}

int cVolanalyse::myFinaliseInstance()
{
  int ret = cWinToVecProcessor::myFinaliseInstance();

  if (ret) {
    vaData = (sVolanalyseData*)calloc(1,sizeof(sVolanalyseData)*getNf());
  }

  return ret;
}

int cVolanalyse::setupNamesForElement(int i, const char*name, long nEl)
{
  addNameAppendField(name,"nClips");
  addNameAppendField(name,"maxClipLen");
  addNameAppendField(name,"nFakeClips");
  addNameAppendField(name,"maxFakeClipLen");
  addNameAppendField(name,"offset");
  addNameAppendField(name,"maxDelta");
  addNameAppendField(name,"firstAbsVal");
  addNameAppendField(name,"lastAbsVal");
  addNameAppendField(name,"maxAbsVal");
  addNameAppendField(name,"meanFirst");
  addNameAppendField(name,"meanLast");
  addNameAppendField(name,"clipratio");
  return getMultiplier();
}


// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cVolanalyse::doProcess(int idxi, cMatrix *row, FLOAT_DMEM*y)
{
  long n;

  if (row->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",row->type);
  
  FLOAT_DMEM *x=row->dataF;
  sVolanalyseData *v = vaData+idxi;
  
  if (v->lastNT == 0) v->lastNT = row->nT;
  FLOAT_DMEM fmean = 0.0, lmean=0.0; FLOAT_DMEM nfmean = 0.0, nlmean=0.0;
  #define NM 100
  
  for (n=0; n<row->nT; n++) {
    if (fabs(x[n]) > 1.0) { 
      if (v->clip==1) { v->nClips++; }
      v->clip++; 
    } else {
      if (v->clip>1) {
        if (v->clip > v->maxClipLen) v->maxClipLen=v->clip;
      }
      v->clip=0;
    }
    
    if ((x[n] == v->lastVal)&&(x[n] != 0.0)) {
      if (v->fclip==1) { v->nFakeClips++; }
      v->fclip++;
    } else {
      if (v->fclip>1) {
        if (v->fclip > v->maxFakeClipLen) v->maxFakeClipLen = v->fclip;
      }
      v->fclip=0;
    }
    
    if (fabs(x[n] - v->lastVal) > v->maxDelta) v->maxDelta = fabs(x[n] - v->lastVal);
    if (fabs(x[n] - v->lastVal) > v->maxDelta) v->maxDelta = fabs(x[n] - v->lastVal);
    
    if (fabs(x[n]) > v->maxAbsVal) v->maxAbsVal = fabs(x[n]);
    
    if ((n<NM)&&(!v->firstSet)) {
      fmean += fabs(x[n]);
      nfmean += 1.0;
    }
    if (n> row->nT-NM) {
      lmean += fabs(x[n]);
      nlmean += 1.0;
    }
    
    if ((fabs(v->lastlastVal) < fabs(v->lastVal)) && (fabs(v->lastVal) > x[n])) {
      v->nPeaks++;
    }
    v->lastlastVal = v->lastVal;
    v->lastVal=x[n];
    v->offset += x[n];
    v->Noffset += 1.0;
  }
  if (v->lastNT == row->nT) {
    v->meanLast = lmean/nlmean;
  }
  if (!v->firstSet) { 
    v->firstSet=1; 
    v->firstAbsVal = fabs(x[1]); 
    v->meanFirst = fmean/nfmean;
  }
  
  if (isEOI()) return 0;
  
  return -1;
}

int cVolanalyse::doFlush(int idxi, FLOAT_DMEM*y)
{
  sVolanalyseData *v = vaData+idxi;
  
  if (v->resFlushed) return 0;
  if (isEOI()) {
    // generate result frame...
    //FLOAT_DMEM *y=_out->dataF;
    //printf(".. idxi = %i\n",idxi);
    //printf("nClips: %i\n",v->nClips);
    y[0] = (FLOAT_DMEM)(v->nClips);
    //printf("maxClipLen: %i\n",v->maxClipLen);
    y[1] = (FLOAT_DMEM)(v->maxClipLen);
    //printf("nFakeClips: %i\n",v->nFakeClips);
    y[2] = (FLOAT_DMEM)(v->nFakeClips);
    //printf("maxFakeClipLen: %i\n",v->maxFakeClipLen);
    y[3] = (FLOAT_DMEM)(v->maxFakeClipLen);
    //printf("offset: %f\n",v->offset/v->Noffset);
    FLOAT_DMEM offs = v->offset/v->Noffset;
    y[4] = offs;
    y[5] = v->maxDelta;
    y[6] = v->firstAbsVal;
    y[7] = fabs(v->lastVal - offs);
    y[8] = v->maxAbsVal;
    y[9] = v->meanFirst;
    y[10] = ( v->meanLast - fabs(offs) );
    y[11] = (FLOAT_DMEM)(v->nClips) / (FLOAT_DMEM)(v->nPeaks);
    
    v->resFlushed=1;
    
    return getMultiplier();
  }

  //return getMultiplier();
  return 0;
  // return the number of actually set components in y!!
  // return 0 on failue
  // return -1 if you don't want to set a new output frame...
}


cVolanalyse::~cVolanalyse()
{
  if (vaData != NULL) free(vaData);
}

