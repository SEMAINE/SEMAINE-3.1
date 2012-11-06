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

BLSTM RNN processor

*/


#ifndef __CRNNVAD_HPP
#define __CRNNVAD_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>
#include <rnn.hpp>

#define COMPONENT_DESCRIPTION_CRNNVAD "BLSTM RNN processor."
#define COMPONENT_NAME_CRNNVAD "cRnnVad"

#undef class

class DLLEXPORT cVoiceModelC {
private:
  FLOAT_DMEM *mean;
  FLOAT_DMEM *var;
  FLOAT_DMEM *sum, *sum2;
  FLOAT_DMEM _N;
  long vecSize;
  long bs;
  long trainDur; // number of frames the model was trained on in total
  
  

public:
  FLOAT_DMEM alpha;
  FLOAT_DMEM mapWeight;

  cVoiceModelC(long _vecSize, long bufferSize, long Nbins=1000) : _N(0), vecSize(_vecSize), bs(bufferSize), trainDur(0), alpha(0.0), mapWeight(0.0)
  {
    long i;
    sum = new FLOAT_DMEM[vecSize];
    sum2 = new FLOAT_DMEM[vecSize];
    mean = new FLOAT_DMEM[vecSize];
    var = new FLOAT_DMEM[vecSize];
    for (i=0; i<vecSize; i++) {
      mean[i] = 0.0;
      var[i] = 0.0;
      sum[i] = 0.0;
      sum2[i] = 0.0;
    }
  }


  ~cVoiceModelC() 
  { 
    delete[] mean;
    delete[] var;
    delete[] sum;
    delete[] sum2;
  }

  long getTrainDur() { return trainDur; }

  // reset the model to its initial untrained state
  void reset() {
    int i;
    for (i=0; i<vecSize; i++) {
      sum[i] = 0.0;
      sum2[i] = 0.0;
    }
    _N = 0.0;
    trainDur = 0;
  }

  // add a frame to the voice model
  int addFrame(cVector *frame, long start=0);


  // (re)build the voice model from the current frame buffer
  void rebuildModel()
  {
    updateModel();
    //???
    reset();
  }

  // update an existing voice model from the current frame buffer (histogram of current data will be added to the model histogram)
  void updateModel()
  {
    long i;
    if (_N>0.0) {
      for (i=0; i<vecSize; i++) {
        mean[i] = sum[i] / _N;
        FLOAT_DMEM tmp = (sum2[i] / _N);
        var[i] = (sum2[i] / _N) - mean[i]*mean[i];
        if (mapWeight > 0.0) {
          sum[i] = mean[i] * mapWeight;
          sum2[i] = tmp * mapWeight;
          _N = mapWeight;
        }
      }
    }
  }

  void updateRange(cVector * frame, long ftIdx) { }

  int isFilled() { return 1; }
  int hasModel() { return (trainDur >= bs); }

  // return the distance of a vector to this models reference vector
  FLOAT_DMEM getFrameProb(cVector *frame, long start=0)
  {
    double lp = 0.0; // log prob.
    long i;
    FLOAT_DMEM *df = frame->dataF+start;
    //double tmp=log(1.0/(2.0*M_PI));
    for (i=0; i<vecSize; i++) {
      if (var[i] > 0.0) {
        double dist = ((double)df[i]-(double)mean[i]);
        lp += - log(var[i]) - dist*dist/(2.0*var[i]);
      }
    }
    return (FLOAT_DMEM)lp; 
  }

#ifdef DEBUG
  void printHgram()
  {
    long i;
    for (i=0; i<vecSize; i++) {
      printf(" mean[%i] = %f\n",i,mean[i]);
    }
  }
#endif
};


class DLLEXPORT cVoiceModelB {
private:
  FLOAT_DMEM *mean;
  FLOAT_DMEM *sum;
  FLOAT_DMEM _N;
  long vecSize;
  long bs;

public:

  cVoiceModelB(long _vecSize, long bufferSize, long Nbins=1000) : _N(0), vecSize(_vecSize), bs(bufferSize)
  {
    long i;
    sum = new FLOAT_DMEM[vecSize];
    mean = new FLOAT_DMEM[vecSize];
    for (i=0; i<vecSize; i++) {
      mean[i] = 0.0;
      sum[i] = 0.0;
    }
  }


  ~cVoiceModelB() 
  { 
    delete[] mean;
    delete[] sum;
  }

  // add a frame to the voice model
  int addFrame(cVector *frame, long start=0);


  // (re)build the voice model from the current frame buffer
  void rebuildModel()
  {
    long i;
    updateModel();
    for (i=0; i<vecSize; i++) {
      sum[i] = 0.0;
    }
  }

  // update an existing voice model from the current frame buffer (histogram of current data will be added to the model histogram)
  void updateModel()
  {
    long i;
    if (_N>0.0) {
      for (i=0; i<vecSize; i++) {
        mean[i] = sum[i] / _N;
      }
    }
  }

  void updateRange(cVector * frame, long ftIdx) { }

  int isFilled() { return 1; }
  int hasModel() { return (_N >= (FLOAT_DMEM)bs); }

  // return the distance of a vector to this models reference vector
  FLOAT_DMEM getFrameProb(cVector *frame, long start=0)
  {
    FLOAT_DMEM d=0.0;
    long i;
    FLOAT_DMEM *df = frame->dataF+start;
    for (i=0; i<vecSize; i++) {
        d += (df[i]-mean[i])*(df[i]-mean[i]);
    }
    return sqrt(d); 
  }

#ifdef DEBUG
  void printHgram()
  {
    long i;
    for (i=0; i<vecSize; i++) {
      printf(" mean[%i] = %f\n",i,mean[i]);
    }
  }
#endif
};

class DLLEXPORT cVoiceModelA {
private:
  cMatrix * buffer;
  long ptr;     // current write pointer
  long filled;  // number of valid samples in buffer
  int _isFilled; // 1 = if all samples are valid and buffer is being warped
  long nBins;
  sHistogram * model;
  int alienMinMax;
  int hasMinMax;
  long nMinMax; /* number of frames analysed */

public:
  FLOAT_DMEM * min;
  FLOAT_DMEM * max;


  cVoiceModelA(long vecSize, long bufferSize, long Nbins=1000) : ptr(0), filled(0), 
    _isFilled(0), nBins(Nbins), model(NULL), hasMinMax(0), alienMinMax(0), nMinMax(0)
  {
    buffer = new cMatrix(vecSize,bufferSize,DMEM_FLOAT);
    min = new FLOAT_DMEM[vecSize];
    max = new FLOAT_DMEM[vecSize];
  }

  void setRangeMasterModel(cVoiceModelA &vm) 
  {
    delete[] min; 
    delete[] max;
    alienMinMax = 1;
    min = vm.min;
    max = vm.max;
  }

  long getNminMax() { return nMinMax; }

  void allocModel(long vecSize, long Nbins) 
  {
    long i;
    if (model != NULL) freeModel();
    // NOTE: we rather use malloc for the model, since we use extern C functions in smileUtil for working with this memory...
    model = (sHistogram *)calloc(1,sizeof(sHistogram)*vecSize);
    for (i=0; i<vecSize; i++) {
      model[i].Nbins = Nbins;
    }
  }

  void freeModel()
  {
    long i;
    if (model == NULL) return;
    for (i=0; i<buffer->N; i++) {
      if (model[i].bins != NULL) free(model[i].bins);
    }
    free(model); model = NULL;
  }

  ~cVoiceModelA() 
  { 
    freeModel();
    delete buffer;
    if (!alienMinMax) {
      delete[] min;
      delete[] max;
    }
  }

  // add a frame to the voice model
  int addFrame(cVector *frame, long start=0);


  // (re)build the voice model from the current frame buffer
  void rebuildModel()
  {
    long i;
    // reset the model
    if (model != NULL) freeModel();
    allocModel(buffer->N,nBins);
    for (i=0; i<buffer->N; i++) {
      FLOAT_DMEM range=max[i]-min[i];
      model[i].min = min[i]-(FLOAT_DMEM)0.25*range;
      model[i].max = max[i]+(FLOAT_DMEM)0.25*range;
    }
    // build new model:
    smileStat_getPMFvecT(buffer->dataF, buffer->nT, buffer->N, model);
  }

  // update an existing voice model from the current frame buffer (histogram of current data will be added to the model histogram)
  void updateModel()
  {
    long i;
    // allocate model if it is not yet allocated
    if (model == NULL) {
      allocModel(buffer->N,nBins);
      for (i=0; i<buffer->N; i++) {
        FLOAT_DMEM range=max[i]-min[i];
        model[i].min = min[i]-(FLOAT_DMEM)0.25*range;
        model[i].max = max[i]+(FLOAT_DMEM)0.25*range;
      }
      hasMinMax=-1;
    }
    // build new model:
    smileStat_getPMFvecT(buffer->dataF, buffer->nT, buffer->N, model);
  }

  void updateRange(cVector * frame, long ftIdx)
  {
    if ((hasMinMax != -1)&&(!alienMinMax)) {
      long i;
      FLOAT_DMEM *df=frame->dataF+ftIdx;
      if (hasMinMax!=0) {
        for (i=0; i<buffer->N; i++) {
          if (min[i] > df[i]) min[i] = df[i];
          if (max[i] < df[i]) max[i] = df[i];
        }
      } else {
        for (i=0; i<buffer->N; i++) {
          min[i] = df[i];
          max[i] = df[i];
        }
        hasMinMax = 1;
      }
      nMinMax++;
    }
  }

  int isFilled() { return _isFilled; }
  int hasModel() { return (model != NULL); }

  // return the probability of a frame belonging to this model
  FLOAT_DMEM getFrameProb(cVector *frame, long start=0)
  {
    if (model != NULL) {
      return smileStat_probEstimVecLog(frame->dataF+start, model, buffer->N, PROBFLOOR_LOG);
    }
    return 1.0; // 1.0 indicates error: no model available!
  }

//#ifdef DEBUG
  void printHgram()
  {
    long i,j;
    if (model != NULL) {
      for (i=0; i<buffer->N; i++) {
        printf("---Feature %i-----\n",i);
        for (j=0; j<model[i].Nbins; j++) {
          printf("  bin %i: %f\n",j,model[i].bins[j]);
        }
      }
    }
  }

//#endif
};





class DLLEXPORT cRnnVad : public cDataProcessor {
  private:
    long holdTime;
    long voiceIdx, energyIdx, f0Idx, ftIdx;
    long cnt; long xcnt;
    long lastV;
    long lastLstmV, lastGmV;
    int isV;
    int lstmV, gmV;
    int voiceThreshSet;
    FLOAT_DMEM eavgExpAlpha;
    FLOAT_DMEM eavgExp;
    FLOAT_DMEM eavgVoice, eavgVoiceLast;
    FLOAT_DMEM eminVoice;
    FLOAT_DMEM eavgVoiceNr;
    FLOAT_DMEM voiceThresh;
    FLOAT_DMEM alpha, mapWeight;
    cVector *frameO;
    
    long vmUserBuffersize, vmRangeCal, vmUserNbins;
    cVoiceModelC *vmUser, *vmBg, *vmAgent;

    long trainDur;
    long agentBlockTime;
    long agentTurnCntdn;
    long agentTurnPastBlock;
    int alwaysRejectAgent;
    int doReset, agentTurn, userPresence;
    long userCnt, agentCntdn;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cRnnVad(const char *_name);

    virtual ~cRnnVad();
};




#endif // __CRNNVAD_HPP
