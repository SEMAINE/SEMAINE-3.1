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

Audio fingerprinter (e.g. for music recordings, etc.)
saves compact feature data (melspec) of 
  beginning, end and middle (to determine shifts)
  100 averages over 100 parts of song (song divided into 100 parts)
  overall average spectrum (fft)
  rms energy (average)

*/


#include <fingerprint.hpp>
//#include <math.h>

#define MODULE "cFingerprint"


SMILECOMPONENT_STATICS(cFingerprint)

SMILECOMPONENT_REGCOMP(cFingerprint)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFINGERPRINT;
  sdescription = COMPONENT_DESCRIPTION_CFINGERPRINT;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

	  SMILECOMPONENT_IFNOTREGAGAIN( {}
    ct->setField("filename","filename to save audio fingerprint data to","fingerprint.dat");
  )

  SMILECOMPONENT_MAKEINFO(cFingerprint);
}

SMILECOMPONENT_CREATE(cFingerprint)

//-----

cFingerprint::cFingerprint(const char *_name) :
  cVectorProcessor(_name),
  fpData(NULL),
  filename(NULL)
{
}


void cFingerprint::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  filename=getStr("filename");
  SMILE_DBG(2,"filename = '%s'",filename);
}

int cFingerprint::myFinaliseInstance()
{
  int ret = cVectorProcessor::myFinaliseInstance();
  if (ret) {
    fpData = (sFingerprinterData*)calloc(1,sizeof(sFingerprinterData)*getNf());

    // check for filename... "delete" if found!
    if (filename != NULL) {
      FILE *f = fopen(filename,"wb");
      if (f!=NULL) fclose(f);
    }
  }
  return ret;
}

int cFingerprint::setupNamesForField(int i, const char*name, long nEl)
{
  addNameAppendField(name,"nFrames");
  addNameAppendField(name,"nMel");
  addNameAppendField(name,"nAvgFrames");
  addNameAppendField(name,"nFft");
  addNameAppendField(name,"melBeg",N_MEL*WINLEN);
  addNameAppendField(name,"melMid",N_MEL*WINLEN);
  addNameAppendField(name,"melEnd",N_MEL*WINLEN);
  addNameAppendField(name,"melAvg",N_MEL*100);
  addNameAppendField(name,"fftAvg",N_FFT);
  addNameAppendField(name,"energyAvg",1);
  return 5+3*N_MEL*WINLEN+N_FFT+N_MEL*100;
}

// *src should be an N_MEL (=26 ?) dimensional vector containing melspec => this must be ensured in the config file
int cFingerprint::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi)
{
  long i;

  sFingerprinterData *v = fpData+idxi;
  
  long mm = N_MEL;
  if (Nsrc < N_MEL) mm=Nsrc;
  
  if (v->cur < WINLEN) { // fill melBeg
    float * out = v->melBeg + N_MEL*v->cur;
    long i;
    for (i=0; i<mm; i++) {
      out[i] = (float)(src[i]);
      //printf("i=%i\n",i);
    }
  }
  
  if ((v->cur>=MID)&&(v->cur < MID+WINLEN)) { // fill melMid
    float * out = v->melMid + N_MEL*(v->cur-MID);
    long i;
    for (i=0; i<mm; i++) {
      out[i] = (float)(src[i]);
    }
  } 
  
  
  // overall fft average, first N_FFT bins (fftmag appended to N_MEL elements in *src):
  float * out = v->fftAvg;
  long mm2 = Nsrc;
  if (N_MEL+N_FFT < mm2) mm2 = N_MEL+N_FFT;
  for (i=N_MEL; i<mm2; i++) {
      out[i-N_MEL] = (float)(src[i]);
  }
  

  float energy=0.0;
  for (i=0; i<N_MEL; i++) {
      energy += (float)(src[i]);
  }
  energy /= (float)N_MEL;
  
  v->energyAvg += energy;
  
  // meldump
  if (v->nMeldump <= v->cur) {
    long old = v->nMeldump;
    v->nMeldump += MELDUMP_BLOCK;
    v->meldump = (float *)crealloc(v->meldump, sizeof(float)*N_MEL*v->nMeldump, sizeof(float)*N_MEL*old); 
  }
  out = v->meldump + N_MEL*v->cur;
  for (i=0; i<mm; i++) {
    out[i] = (float)(src[i]);
  }
  
  v->cur++;
  v->nFrames = (float)(v->cur);
  
  if (isEOI()) return 0; // fail on end of input?? does this make sense??
  
  return -1; // no output, however success
}

int cFingerprint::flushVectorFloat(FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi)
{
  sFingerprinterData *v = fpData+idxi;
  
  if (v->resFlushed) return 0;
  if (isEOI()) {
    v->resFlushed=1;
    v->idxi=idxi;
    long n,i;    
    
    // compute proper mean values from the sums
    if (v->nFrames > 0.0) {
      v->energyAvg /= v->nFrames;
      for (n=0; n<N_FFT; n++) {
        v->fftAvg[n] /= v->nFrames;
      }  
    }
    
    // melEnd
    for (n=v->cur-WINLEN-1; n<v->cur; n++) {
      float * out = v->melEnd + N_MEL*(n - (v->cur-WINLEN-1));
      if (n >= 0) {
        float * in = v->meldump + N_MEL*n;
        for (i=0; i<N_MEL; i++) {
          out[i] = in[i];
        }
      } else { // ???
        for (i=0; i<N_MEL; i++) {
          out[i] = 0; 
        }
      }
    } 
    
    // average over 100 song parts..
    float oneP = (float)v->nFrames / (float)100.0;
    int p = 1;
    long nP = 0;
    float *in=v->meldump;
    for (n=0; n<v->cur; n++) {
      float * out = v->melEnd + N_MEL*(p-1);
      if (n>(int)round((float)p*oneP)) {
        // div by nP:
        if (nP>0) {
          for (i=0; i<N_MEL; i++) {
            out[i] /= (float)nP;
          }
        }
        nP=0;
        p++; if (p>100) p=100;
        out = v->melEnd + N_MEL*(p-1);
      }
      
      for (i=0; i<N_MEL; i++) {
        out[i] += in[i];
      }
      nP++;
      in += N_MEL;
    }
    
    // save result to file, dump sFingerprinterData ...
    FILE * f;
    if (filename != NULL) {
      // check if file exists, if yes, append
      f = fopen(filename,"rb");
      if (f!=NULL) {
        fclose(f);
        f=fopen(filename,"ab");
      } else {
        f=fopen(filename,"wb");
      }
      
      if (!fwrite(v, sizeof(sFingerprinterData), 1, f)) {
        SMILE_IERR(1,"error saving fingerprint data to '%s'",filename);
      }
      fclose(f);
    }

    // generate result frame in *y ...
    ///... TODO    
    
    return 1;
  }

  return 0;
  // return the number of actually set components in y!!
  // return 0 on failue
  // return -1 if you don't want to set a new output frame...
}


cFingerprint::~cFingerprint()
{
  int i;
  if (fpData != NULL) {
    for (i=0; i<getNf(); i++) {
      if (fpData[i].meldump != NULL) free(fpData[i].meldump);
    }
    free(fpData);
  }
}

