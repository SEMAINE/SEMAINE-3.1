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


/*  openSMILE component: cVecGlMean

compute gliding mean of input vectors and subtract it from the vectors

*/


#include <vecGlMean.hpp>

#define MODULE "cVecGlMean"


SMILECOMPONENT_STATICS(cVecGlMean)

SMILECOMPONENT_REGCOMP(cVecGlMean)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CVECGLMEAN;
  sdescription = COMPONENT_DESCRIPTION_CVECGLMEAN;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  
    /*
  if (ct->setField("auxReader", "data reader for fast fixed buffer mean computation",
                  sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE) == -1) {
   rA=1; // if subtype not yet found, request , re-register in the next iteration
  }
   */

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend",NULL,"zeromean");
    ct->setField("initFile","Filename of the file from which to load the initial mean values from",(const char*)NULL);
    ct->setField("updateMethod","Specifies how to incrementally update the mean values. One of the following:\n   'nul'  do not perform any updates, use only inital values\n   'exp'  exponential: m1 = alpha * m0  + (1-alpha) * x\n   'fix'  mean over buffer of fixed length\n   'avg'  moving average method with weighted fixed inital values\n   'iavg'  moving average method with weighted variable (updated per turn) inital values\n   NOTE: if 'resetOnTurn'==0 then 'avg' and 'iavg' are identical","iavg");
    ct->setField("alpha","The weighting factor alpha in gliding average computation",0.995);
    ct->setField("weight","The weighting factor for MAP mean update (julius lvcsr engine frontend method)",100);
    ct->setField("fixedBuffer","The size of the fixed length buffer [in seconds] (for fixed buffer mean, and for MAP method)",5.0);

    ct->setField("turnOnlyUpdate","1 = perform mean update only during turns (works for all three methods)",1);
    ct->setField("invertTurn","1 = invert turn state (i.e. changes a 'turnOnly' option into 'not turn' option)",0);
    ct->setField("resetOnTurn","1 = reset mean values at the beginning of each new turn",0);

    ct->setField("turnOnlyNormalise","1 = normalise only during turns (in between, data will pass through unmodified)",0);

    ct->setField("htkcompatible","A flag that indicates (if set to 1) wheter the last coefficient in 'initFile' is loaded into means[0] (use this when reading htk-compatible cmn init files, and NOT using htk-compatible mfccs)",0);

    ct->setField("turnStartMessage","You can use this option to define a custom message name for turn start messages, i.e. if you want to use voice activity start/end messages instead","turnStart");
    ct->setField("turnEndMessage","You can use this option to define a custom message name for turn end messages, i.e. if you want to use voice activity start/end messages instead","turnEnd");

    // OBSOLETE:
    //ct->setField("skipNfirst","number of frames at beginning of input to skip (i.e. not use for update of means)",10);
    //    ct->setField("htkcompatible","same as zeroThIsLast",1);
  )

  SMILECOMPONENT_MAKEINFO(cVecGlMean);
}

/*

mean update algos:

1. exponentially decaying mean
  - always
  - only turn
  - (reset on turn)
  - init data

2. mean over fixed past buffer
  - always
  - only turn
  - (reset on turn)
  - NO init data

3. julius MAP-update
  - (fixed init data, or zero)
  - always (= mean over full input.. w. weighted init data)
  - only turn (= mean over all turns w. weighted init data)
  - (reset on turn:) per turn, mean over turn, w. weighted init data
3b.  - init data update (only at beginning of turn!) (i.e. only if "reset on turn")
       "based on mean of previous sums" (also over turn boundaries.. fixed length list of prev sums)

4. init-data only

*/

SMILECOMPONENT_CREATE(cVecGlMean)

//-----

cVecGlMean::cVecGlMean(const char *_name) :
  cVectorProcessor(_name),
  initFile(NULL),
  alpha(0.0), weight(0.0),
  imeans(NULL), means(NULL), nMeans(0),
  buffer(NULL), bufferNframes(NULL),
  nAvgBuffer(0), wPtr(0), rPtr(0),
  updateMethod(0),
  turnOnlyNormalise(0),
  invertTurn(0),
  resetOnTurn(0),
  turnOnlyUpdate(0),
  zeroThIsLast(0),
  skipNfirst(10),
  fixedBuffer(0.0),
  fixedBufferFrames(0),
  isTurn(0), resetMeans(0), nFrames(0),
  turnStartMessage(NULL), turnEndMessage(NULL)
{
  
}

void cVecGlMean::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  initFile = getStr("initFile");
  SMILE_IDBG(2,"initFile = '%s'",initFile);
  
  alpha = (FLOAT_DMEM)getDouble("alpha");
  SMILE_IDBG(2,"alpha = %f",alpha);

  weight = (FLOAT_DMEM)getDouble("weight");
  SMILE_IDBG(2,"weight = %f",weight);

  const char * updateMethodStr = getStr("updateMethod");
  SMILE_IDBG(2,"updateMethod = '%s'",updateMethodStr);
  if (!strncmp(updateMethodStr,"exp",3)) {
    updateMethod = UPDMETHOD_EXP;
  } else if (!strncmp(updateMethodStr,"nul",3)) {
    updateMethod = UPDMETHOD_NUL;
  } else if (!strncmp(updateMethodStr,"fix",3)) {
    updateMethod = UPDMETHOD_FIX;
  } else if (!strncmp(updateMethodStr,"avg",3)) {
    updateMethod = UPDMETHOD_AVG;
  } else if (!strncmp(updateMethodStr,"iavg",4)) {
    updateMethod = UPDMETHOD_AVGI;
  } else {
    SMILE_IERR(1,"unknown update method '%s' (setting to 'exp' !)",updateMethodStr);
    updateMethod = UPDMETHOD_EXP;
  }

  zeroThIsLast = getInt("htkcompatible");
  SMILE_IDBG(2,"zeroThIsLast (htkcompatible) = %i",zeroThIsLast);

//  skipNfirst = getInt("skipNfirst");
//  SMILE_IDBG(2,"skipNfirst = %i",skipNfirst);
  
  turnOnlyUpdate = getInt("turnOnlyUpdate");
  SMILE_IDBG(2,"turnOnlyUpdate = %i",turnOnlyUpdate);

  turnOnlyNormalise = getInt("turnOnlyNormalise");
  SMILE_IDBG(2,"turnOnlyNormalise = %i",turnOnlyNormalise);

  resetOnTurn = getInt("resetOnTurn");
  SMILE_IDBG(2,"resetOnTurn = %i",resetOnTurn);

  invertTurn = getInt("invertTurn");
  SMILE_IDBG(2,"invertTurn = %i",invertTurn);

  fixedBuffer = getDouble("fixedBuffer");
  SMILE_IDBG(2,"fixedBuffer = %f",fixedBuffer);

  turnStartMessage = getStr("turnStartMessage");
  SMILE_IDBG(2,"turnStartMessage = '%s'",turnStartMessage);

  turnEndMessage = getStr("turnEndMessage");
  SMILE_IDBG(2,"turnEndMessage = '%s'",turnEndMessage);
}


int cVecGlMean::configureWriter(sDmLevelConfig &c)
{
  if (c.T != 0.0) {
    if (fixedBuffer > 0.0) {
      fixedBufferFrames = (long)ceil(fixedBuffer / c.T);
      /*
      //reader->setBlocksize(fixedBufferFrames+1);
      auxReader->setComponentEnvironment(getCompMan(), -1, this);
      auxReader->registerInstance();
      auxReader->configureInstance();
      auxReader->finaliseInstance();
      auxReader->setBlocksize(fixedBufferFrames+1);
      */
    }
  }

  return 1;
}


int cVecGlMean::myFinaliseInstance()
{
  int ret = cVectorProcessor::myFinaliseInstance();
  if (!ret) return 0;

    // open mean initialisation file and load values:
  FILE *f = NULL;
  if (initFile != NULL) {
    f = fopen(initFile,"r");
  }
  if (f!=NULL) {
    int i=0,n; float v;
    fscanf(f,"<MEAN> %i\n",&n);
    nMeans = n;
    imeans = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*n);
    //printf("nMeans to load: %i\n",n);
    if (zeroThIsLast) { i=1; }
    for (; i<n; i++) {
      if (fscanf(f," %f",&v) > 0) {
        imeans[i] = v;
        //printf("v(%i): %f\n",i,v);
      } else {
        imeans[i] = 0.0;
        SMILE_ERR(1,"parse error during loading of cepstral mean file: '%s'",initFile);
      }
    }
    if (zeroThIsLast) {
      if (fscanf(f," %f",&v) > 0) {
        imeans[0] = v;
        //printf("v(%i): %f\n",i,v);
      } else {
        imeans[0] = 0.0;
        SMILE_ERR(1,"parse error during load of cepstral mean file: %s\n",initFile);
      }
    }
    fclose(f);
    ret = 1;
  } else {
    if (initFile != NULL) {
      SMILE_ERR(1,"cannot open mean value initialisation file '%s'",initFile);
      ret = 0;
    } else {
      ret = 1;
      imeans=NULL;
    }
  }

  if ( (updateMethod == UPDMETHOD_FIX) || (updateMethod == UPDMETHOD_AVGI) ) {
    // allocate space for ring-buffer 
    if (fixedBufferFrames < 3) {
      SMILE_IERR(1,"fixedBuffer (in frames) is < 3, setting to minimum value of 3!!");
      fixedBufferFrames = 3;
    } 
  }

  return ret;
}

int cVecGlMean::processComponentMessage( cComponentMessage *_msg )
{
  const char * endM = NULL;
  if (turnEndMessage == NULL) endM = "turnEnd";
  else endM = turnEndMessage;

  if (isMessageType(_msg,endM)) {
    if (invertTurn) {
      isTurn = 1; resetMeans = 1;
    } else {
      isTurn = 0;
    }
    SMILE_IDBG(3,"received turnEnd (%s)",endM);
    return 1;
  }

  const char * startM = NULL;
  if (turnStartMessage == NULL) startM = "turnStart";
  else startM = turnStartMessage;

  if (isMessageType(_msg,startM)) {
    if (invertTurn) {
      isTurn = 0;
    } else {
      isTurn = 1;
      resetMeans = 1;
    }
    SMILE_IDBG(3,"received turnStart (%s)",startM);
    return 1;
  }
  return 0;
}


// a derived class should override this method, in order to implement the actual processing
/*
int cVecGlMean::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 1;
}
*/


// TODO: own buffer for fixedBuffer !

// a derived class should override this method, in order to implement the actual processing
int cVecGlMean::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int i,j;

  if (nMeans == 0) {
    nMeans = Nsrc;
  }
  
  int N = MIN(Nsrc,nMeans);
  N = MIN(N,Ndst);

  if (imeans == NULL) {
    imeans = (FLOAT_DMEM *)calloc(1,sizeof(FLOAT_DMEM)*N);
  }

  if ( (updateMethod == UPDMETHOD_FIX) || (updateMethod == UPDMETHOD_AVGI) ) {
    if (buffer == NULL) buffer = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*fixedBufferFrames*N);
    if (updateMethod == UPDMETHOD_AVGI) {
      if (bufferNframes == NULL) bufferNframes = (long*)malloc(sizeof(long)*fixedBufferFrames);
    }
  }

  int _isTurn;
  int _resetMeans;
  FLOAT_DMEM nVal;

  switch (updateMethod) {

    case UPDMETHOD_NUL :
      lockMessageMemory();
      _isTurn = isTurn;
      unlockMessageMemory();

      // subtract initial means only
      if ((_isTurn)||(!turnOnlyNormalise)) {
        for (i=0; i<N; i++) {
          dst[i] = src[i] - imeans[i];
        }
      } else {
        for (i=0; i<N; i++) {
          dst[i] = src[i];
        }
      }
      break;

    case UPDMETHOD_EXP :
      
      lockMessageMemory();
      _isTurn = isTurn;
      _resetMeans = 0;
      if (resetOnTurn) _resetMeans = resetMeans;
      unlockMessageMemory();

      if (means == NULL) {
        means = (FLOAT_DMEM *)calloc(1,sizeof(FLOAT_DMEM)*N);
        _resetMeans = 1; 
      }
      if (_resetMeans) {
        if (imeans != NULL) {
          for (i=0; i<N; i++) { means[i] = imeans[i]; }
        }
        _resetMeans = 0;
      }
      lockMessageMemory();
      resetMeans = _resetMeans;
      unlockMessageMemory();

      // update means:
      if ((_isTurn)||(!turnOnlyUpdate)) {
        for (i=0; i<N; i++) {
          means[i] = alpha * (means[i] - src[i]) + src[i];
        }
      }

      // subtract means
      if ((_isTurn)||(!turnOnlyNormalise)) {
        for (i=0; i<N; i++) {
          dst[i] = src[i] - means[i];
        }
      } else {
        for (i=0; i<N; i++) {
          dst[i] = src[i];
        }
      }

      break;
    case UPDMETHOD_FIX :

      lockMessageMemory();
      _isTurn = isTurn;
      _resetMeans = 0;
      if (resetOnTurn) _resetMeans = resetMeans;
      unlockMessageMemory();

      if (means == NULL) {
        means = (FLOAT_DMEM *)calloc(1,sizeof(FLOAT_DMEM)*N);
        _resetMeans = 1; 
      }
      if (_resetMeans) {
        if (imeans != NULL) {
          for (i=0; i<N; i++) { 
            buffer[i] = imeans[i]; // fill first frame in buffer
          }
          nAvgBuffer = 1;
          wPtr = 1; rPtr = 0;
        } else {
          nAvgBuffer = 0;
          wPtr = 0; rPtr = 0;
        }
        _resetMeans = 0;
      }
      lockMessageMemory();
      resetMeans = _resetMeans;
      unlockMessageMemory();

      // manage ring buffer (update means):
      if ((_isTurn)||(!turnOnlyUpdate)) {
        if (nAvgBuffer == fixedBufferFrames) {
          for (i=0; i<N; i++) {
            // add current input to sum in means[]:
            means[i] += src[i];
            // remove last input in buffer from sum in means[]:
            means[i] -= buffer[i + N*wPtr];
            // store current input in ringbuffer at position of last input:
            buffer[i + N*wPtr] = src[i];
          }
        } else {
          for (i=0; i<N; i++) {
            // add current input to sum in means[]:
            means[i] += src[i];
            // store current input in ringbuffer:
            buffer[i + N*wPtr] = src[i];
          }
          nAvgBuffer++;
        }

        wPtr++;
        if (wPtr == fixedBufferFrames) wPtr = 0;
      }

      // subtract means
      nVal = (FLOAT_DMEM)nAvgBuffer;
      if ( (nAvgBuffer > 0) && ((_isTurn)||(!turnOnlyNormalise)) ) {
        for (i=0; i<N; i++) {
          dst[i] = src[i] - means[i] / nVal;
        }
      } else {
        for (i=0; i<N; i++) {
          dst[i] = src[i];
        }
      }

      break;
    case UPDMETHOD_AVG :

      lockMessageMemory();
      _isTurn = isTurn;
      _resetMeans = 0;
      if (resetOnTurn) _resetMeans = resetMeans;
      unlockMessageMemory();

      if (means == NULL) {
        means = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*N);
        _resetMeans = 1; 
      }
      if (_resetMeans) {
        for (i=0; i<N; i++) { 
          means[i] = 0.0; 
        }
        nFrames = 0;
        _resetMeans = 0;
      }
      lockMessageMemory();
      resetMeans = _resetMeans;
      unlockMessageMemory();

      // update means
      if ((_isTurn)||(!turnOnlyUpdate)) {
        for (i=0; i<N; i++) {
          // add current input to sum in means[]:
          means[i] += src[i];
        }
        nFrames++;
      }

      // subtract means
      nVal = (FLOAT_DMEM)nFrames;
      if ( (nFrames > 0) && ((_isTurn)||(!turnOnlyNormalise)) ) {
        if (imeans != NULL) {
          for (i=0; i<N; i++) {
            dst[i] = src[i] - ( ( means[i] + weight * imeans[i] ) / ( nVal + weight ) );
          }
        } else {
          for (i=0; i<N; i++) {
            dst[i] = src[i] - ( ( means[i] ) / ( nVal ) );
          }
        }
      } else {
        for (i=0; i<N; i++) {
          dst[i] = src[i];
        }
      }

      break;
    case UPDMETHOD_AVGI:
      
      lockMessageMemory();
      _isTurn = isTurn;
      _resetMeans = 0;
      if (resetOnTurn) _resetMeans = resetMeans;
      unlockMessageMemory();

      if (means == NULL) {
        means = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*N);
        _resetMeans = 1; 
      }
      if (_resetMeans) {
        { // ok:? DO THIS AT BEGINNING OR END OF TURN????
          /* update initial means from previous data (including current sum)*/
          if ((nAvgBuffer > 0)&&(nFrames > 0)) {
            
            if (imeans == NULL) imeans = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*N);
            long long nFr = nFrames;
            for (j=0; j<nAvgBuffer; j++) {
              nFr += bufferNframes[j];
            }
            FLOAT_DMEM nFrf = (FLOAT_DMEM)nFr;
            for (i=0; i<N; i++) {
              imeans[i] = means[i];
              for (j=0; j<nAvgBuffer; j++) {
                imeans[i] += buffer[i + N*j];
              }
              imeans[i] /= nFrf;
              //printf("XXXXX update imeans[%i]  %f\n",i,imeans[i]);
            }
          }
        }
        for (i=0; i<N; i++) { 
          means[i] = 0.0; 
        }
        nFrames = 0;
        _resetMeans = 0;
      }

      lockMessageMemory();
      resetMeans = _resetMeans;
      unlockMessageMemory();

      // update means
      if ((_isTurn)||(!turnOnlyUpdate)) {
        for (i=0; i<N; i++) {
          // add current input to sum in means[]:
          means[i] += src[i];
        }
        nFrames++;
        if (nFrames > 0) {
          /* update buffer of sums... */
          for (i=0; i<N; i++) {
            buffer[i + N*wPtr] = means[i];
          }
          bufferNframes[wPtr] = nFrames;
          wPtr++;
          if (wPtr == fixedBufferFrames) wPtr = 0;
          if (nAvgBuffer < fixedBufferFrames) nAvgBuffer++;
        }
      }

      // subtract means
      nVal = (FLOAT_DMEM)nFrames;
      if ( (nFrames > 0) && ((_isTurn)||(!turnOnlyNormalise)) ) {
        if (imeans != NULL) {
          for (i=0; i<N; i++) {
            dst[i] = src[i] - ( ( means[i] + weight * imeans[i] ) / ( nVal + weight ) );
          }
        } else {
          for (i=0; i<N; i++) {
            dst[i] = src[i] - ( ( means[i] ) / ( nVal ) );
          }
        }
      } else {
        for (i=0; i<N; i++) {
          dst[i] = src[i];
        }
      }

      break;

    default:
      /* should never happen... */

      for (i=0; i<N; i++) {
        dst[i] = src[i];
      }
  }





/*

  if (fixedBufferFrames > 0) {
    // subtract means
    FLOAT_DMEM nAvgBufferD = 1.0;
    if (nAvgBuffer > 1) nAvgBufferD = (FLOAT_DMEM)nAvgBuffer;
    for (i=0; i<N; i++) {
      dst[i] = src[i] - means[i]/nAvgBufferD;
      //printf("means[%i]=%f\n",i,means[i]);
    }
  } else {
    // subtract means
    for (i=0; i<N; i++) {
      dst[i] = src[i] - means[i];
    }
  }

  // update means
  if (fixedBufferFrames > 0) {
    // fixed buffer update

    long idx = reader->getCurR() - fixedBufferFrames;
    // use auxReader to get last frame...
    if (idx < 0) {
      // only add to means, increase counter
      nAvgBuffer = reader->getCurR()+1;
      for (i=0; i<N; i++) {
        means[i] += src[i];
      }
      //printf("IDX <0 %i\n",idx);
    } else {
      nAvgBuffer = fixedBufferFrames;
      cVector *v = auxReader->getFrame(idx);
      if (v != NULL) {
        FLOAT_DMEM *l = v->dataF;
        for (i=0; i<N; i++) {
          means[i] += (src[i] - l[i]);
        }
      } else {
        printf("ERR\n");
      }
    }

  } else {

    // incremental update (exponential decay)
    if (skipNfirst) {
      skipNfirst--;
    } else {
      lockMessageMemory();
      int _isTurn = isTurn;
      unlockMessageMemory();
      if (invertTurn) _isTurn = !_isTurn;
      if ((_isTurn)||(alwaysUpdate)) {
        for (i=0; i<N; i++) {
          means[i] = alpha * (means[i] - src[i]) + src[i];
        }
      }
    }

  }
*/
  return Ndst;
}

cVecGlMean::~cVecGlMean()
{
  if (means!=NULL) free(means);
  if (imeans!=NULL) free(imeans);
  if (buffer!=NULL) free(buffer);
  if (bufferNframes!=NULL) free(bufferNframes);
}

