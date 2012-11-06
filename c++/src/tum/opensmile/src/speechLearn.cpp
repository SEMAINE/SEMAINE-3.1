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

KNN learner for speech/non-speech features
last input field should be binary VAD

*/


#include <speechLearn.hpp>

#define MODULE "cSpeechLearn"


SMILECOMPONENT_STATICS(cSpeechLearn)

SMILECOMPONENT_REGCOMP(cSpeechLearn)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSPEECHLEARN;
  sdescription = COMPONENT_DESCRIPTION_CSPEECHLEARN;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("trainDur","duration of initial training phase in frames",500);
    ct->setField("debug","show classification result as debug output",0);
    ct->setField("a0","decay factor of fuzzy vad",0.05);
    ct->setField("a1","attack factor of fuzzy vad (>>a0)",0.2);
    ct->setField("initSpeechModel","file to load initial speech model coefficients from","model.speech");
    ct->setField("initNoiseModel","file to load initial noise model coefficients from","model.noise");
    ct->setField("initStopModel","file to load initial learn inhibit model coefficients from (if frame matches this model, it will be treated as noise, but the speech/noise model will not be updated)","model.stop");
    ct->setField("stopModelWeight","weight of initial stop model, ~ number of frames the model represents, set to -1 to disable on-line model update, set to 0 to disable initial model",2000);
    ct->setField("noiseModelWeight","weight of initial noise model, ~ number of frames the model represents, set to -1 to disable on-line model update, set to 0 to disable initial model",500);
    ct->setField("speechModelWeight","weight of initial speech model, ~ number of frames the model represents, set to -1 to disable on-line model update, set to 0 to disable initial model",1000);
    ct->setField("saveSpeechModel","file to save speech model coefficients to","model.speech");
    ct->setField("saveNoiseModel","file to save noise model coefficients to","model.noise");
    ct->setField("saveStopModel","file to save learn inhibit model coefficients to","model.stop");
  )
  SMILECOMPONENT_MAKEINFO(cSpeechLearn);
}

SMILECOMPONENT_CREATE(cSpeechLearn)

//-----

cSpeechLearn::cSpeechLearn(const char *_name) :
  cDataProcessor(_name),
  meanS(NULL), meanN(NULL), meanI(NULL),
  varS(NULL), varN(NULL), varI(NULL), debug(0), trainDur(0),
  nFt(0), vadFuz0(0.0), a0((FLOAT_DMEM)0.05), a1((FLOAT_DMEM)0.2), kla(0),
  nTurns(0), nS(0), nN(0), nI(0),
  initSpeechModel(NULL), initNoiseModel(NULL), initStopModel(NULL),
  saveSpeechModel(NULL), saveNoiseModel(NULL), saveStopModel(NULL),
  stopModelWeight((FLOAT_DMEM)0.0), noiseModelWeight((FLOAT_DMEM)0.0), speechModelWeight((FLOAT_DMEM)0.0)
{
   
}

void cSpeechLearn::fetchConfig()
{
  cDataProcessor::fetchConfig();
  
  trainDur = getInt("trainDur");

  debug = getInt("debug");

  a0 = (FLOAT_DMEM)getDouble("a0");
  a1 = (FLOAT_DMEM)getDouble("a1");

  initSpeechModel=getStr("initSpeechModel");
  initNoiseModel=getStr("initNoiseModel");
  initStopModel=getStr("initStopModel");
  stopModelWeight=(FLOAT_DMEM)getDouble("stopModelWeight");
  noiseModelWeight=(FLOAT_DMEM)getDouble("noiseModelWeight");
  speechModelWeight=(FLOAT_DMEM)getDouble("speechModelWeight");
  saveSpeechModel=strdup(getStr("saveSpeechModel"));
  //printf("saveSpeechModel %s\n",saveSpeechModel);
  saveNoiseModel=strdup(getStr("saveNoiseModel"));
  saveStopModel=strdup(getStr("saveStopModel"));

}

int cSpeechLearn::setupNewNames(long nEl)
{
  findInputMapping();

  writer->addField( "vadBin" );
  writer->addField( "vadFuz" );
  writer->addField( "vadSmo" );
  namesAreSet = 1;

  return 1;
}

void cSpeechLearn::findInputMapping()
{
  const FrameMetaInfo * fmeta = reader->getFrameMetaInfo();

  long vad1 = fmeta->findFieldByPartialName( "vadBin" );

  if (vad1 < 0) { vad1 = 0; } // default : fallbak to 0th field

  nFt = fmeta->fieldToElementIdx( vad1 );
  
  meanS = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  meanN = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  meanI = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  varS = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  varN = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  varI = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
}

int cSpeechLearn::loadModel(const char *filename, FLOAT_DMEM *coeff, int N, FLOAT_DMEM weight)
{
  int i;
  for (i=0; i<N; i++) coeff[i] = 0.0;

  if ((filename != NULL)&&(N>0)&&(coeff!=NULL)) {
    FILE *f = fopen(filename,"rb");
    if (f==NULL) {
      SMILE_IDBG(3,"model file '%s' not found or could not be opened",filename);
      return 0;
    }
    float Nf=0.0; float dummy=0.0;
    fread( &Nf, sizeof(float), 1, f);
    fwrite( &dummy, sizeof(float), 1, f); // nVec is ignored here (is always 1 currently)
    if ((int)Nf != N) {
      SMILE_IWRN(2,"number of coefficients in model file '%s' (%i) is unequal the requested number (%i)!",filename,(int)Nf,N);
    }
    N = MIN( (int)Nf, N );
    float *tmp = (float*)malloc(sizeof(float)*N);
    fread( tmp, sizeof(float)*N, 1, f);
    fclose(f);
    int i;
    if (weight < 1.0) weight=1.0;
    for (i=0; i<N; i++) { 
      coeff[i] = (FLOAT_DMEM)tmp[i] * weight; 
    }
    free(tmp);
    return N;
  }
  return 0;
}

int cSpeechLearn::saveModel(const char *filename, FLOAT_DMEM *coeff, int N, FLOAT_DMEM weight)
{
  if ((filename != NULL)&&(N>0)&&(coeff!=NULL)) {
printf("open %s\n",filename);
    FILE *f = fopen(filename,"wb");
    if (f==NULL) {
      SMILE_IDBG(3,"model file '%s' could not be opened for writing",filename);
      return 0;
    }
    float Nf=(float)N;
    float *tmp = (float*)malloc(sizeof(float)*N);
    int i;
    if (weight < 1.0) weight = 1.0;
    for (i=0; i<N; i++) { 
      tmp[i] = (float)( coeff[i] / weight ); 
    }
    fwrite( &Nf, sizeof(float), 1, f);
    Nf = 1.0; // nVec = 1, always
    fwrite( &Nf, sizeof(float), 1, f);
    fwrite( tmp, sizeof(float)*N, 1, f);
    fclose(f);
    free(tmp);
    return 1;
  }
  return 0;
}

int cSpeechLearn::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();
  if (ret) {
    // load initial model coefficients
    if (speechModelWeight != 0.0) {
      if (loadModel(initSpeechModel, meanS, nFt, speechModelWeight)) {
        if (speechModelWeight == -1.0) nS = 1;
        else nS = (int)round(speechModelWeight);
      }
    }
    if (noiseModelWeight != 0.0) {
      if (loadModel(initNoiseModel, meanN, nFt, noiseModelWeight)) {
        if (noiseModelWeight == -1.0) nN = 1;
        else nN = (int)round(noiseModelWeight);
      }
    }
    if (stopModelWeight != 0.0) {
      if (loadModel(initStopModel, meanI, nFt, stopModelWeight)) {
        if (stopModelWeight == -1.0) nS = 1;
        else nS = (int)round(stopModelWeight);
      }
    }
  }
  return ret;
}


// a derived class should override this method, in order to implement the actual processing
int cSpeechLearn::myTick(long long t)
{
  // get next frame from dataMemory
  //printf("tick");
  cVector *vec = reader->getNextFrame();
  if (vec == NULL) return 0;
  //printf(" OK \n");
  
  int updateI = 0;
  int cl = 0;

  cVector *vec0 = new cVector(3);  // TODO: move vec0 to class...
  
  FLOAT_DMEM *src = vec->dataF;
  FLOAT_DMEM *dst = vec0->dataF;

  int vadBin = 0;
  FLOAT_DMEM vadFuz = 0.0;
  
  if ((nFt>=0)&&(nFt<vec->N)) vadBin = (int)(src[nFt]);
  if ((nFt>=0)&&(nFt<vec->N)) vadFuz = src[nFt+1]; // pass on vadFuz (?) to turn detector

  // normalise all incoming vectors
  FLOAT_DMEM sum =0.0;  long i;
  for (i=0; i<MIN(nFt,vec->N); i++) {
    sum+=src[i]*src[i];
  }
  sum = sqrt(sum/(FLOAT_DMEM)nFt);
  if (sum > 0.0) {
  for (i=0; i<MIN(nFt,vec->N); i++) {
    src[i]/=sum;
  }
  }

  // match current frame to both models:

  // A: euclidean distance
  FLOAT_DMEM dS=0.0, dN=0.0, dI=0.0;
  if ((nN > 0) && (nS > 0)) {
    for (i=0; i<MIN(nFt,vec->N); i++) {
      FLOAT_DMEM tmp = (src[i] - meanS[i]/(FLOAT_DMEM)nS);
      dS += tmp*tmp;
      tmp = (src[i] - meanN[i]/(FLOAT_DMEM)nN);
      dN += tmp*tmp;
    }
  }
  if ((nI > 0)) {
    for (i=0; i<MIN(nFt,vec->N); i++) {
      FLOAT_DMEM tmp = (src[i] - meanI[i]/(FLOAT_DMEM)nI);
      dI += tmp*tmp;
    }
  }

  FLOAT_DMEM conf = 0.0;

  // start using model after we have enough training material
  if ((nS > trainDur)&&(nN>trainDur)) {

    if (kla==0) SMILE_IMSG(2,"started using learnt speaker verification model");

    kla=1;

    FLOAT_DMEM x=(FLOAT_DMEM)nS/(FLOAT_DMEM)(trainDur*2);
    if (nS > trainDur*2) x=1.0;

    // TODO: compute decision confidence  and use this for update decision...

    if ((dS >= dN)&&((dS >= dI)||(nI==0))) {
      //printf("noise  (vadBin=%i, dS=%f dN=%f, fuz=%f)\n",vadBin,dS,dN,vadFuz0);
        // noise or agent
      if ((nI>0)&&(dI < dN)) {
        // agent
        updateI = 1;
        cl=2;
        conf = (FLOAT_DMEM)1.0-(dI/dS)*(dI/dS);
      } else {
        //noise
        updateI = 0;
        cl=0;
        conf = (FLOAT_DMEM)1.0-(dN/dS)*(dN/dS);
      }
//      conf = 1.0-(dN/dS)*(dN/dS);
      vadFuz=(FLOAT_DMEM)0.0 + ((FLOAT_DMEM)1.0-x)*(FLOAT_DMEM)vadBin;
      vadBin=0;

    } else {
      //printf("SPEECH (vadBIn=%i, dS=%f dN=%f, fuz=%f)\n",vadBin,dS,dN,vadFuz0);
      //if ((dS < dN)&&((dS<dI)||(nI==0))) {
          // user speech
        cl=1;
        conf = (FLOAT_DMEM)1.0-(dS/dN)*(dS/dN);
        vadFuz=x * (FLOAT_DMEM)1.0 + ((FLOAT_DMEM)1.0-x)*(FLOAT_DMEM)vadBin;
        if (conf < 0.6) vadFuz *= conf; // ??
        vadBin=1;
        updateI=0;
      //} else {
        /*conf = 1.0-(dS/dN)*(dS/dN);
        vadFuz=x * 1.0 + (1.0-x)*(FLOAT_DMEM)vadBin;
        if (conf < 0.6) vadFuz *= conf; // ??
        vadBin=1;*/
      //}
    }


    if (vadFuz > vadFuz0) {
      vadFuz0 = vadFuz*a1 + ((FLOAT_DMEM)1.0-a1)*vadFuz0;
    } else {
      vadFuz0 = vadFuz*a0 + ((FLOAT_DMEM)1.0-a0)*vadFuz0;
    }



    //printf("cool vadFuz = %f\n",vadFuz0);
  } else {
    //  printf(" (vadBin=%i, dS=%f dN=%f)\n",vadBin,dS,dN);
    if ((nFt>=0)&&(nFt<vec->N)) vadFuz0 = src[nFt+1]; // pass on vadFuz (?) to turn detector
  }

  if (debug) {
    if (cl==0) { printf("noise bin=%i, fuz=%f  conf %f\n",vadBin,vadFuz0,conf); }
    else if (cl==2) { printf("Agent bin=%i, fuz=%f  conf %f\n",vadBin,vadFuz0,conf); }
    else if (cl==1) { printf("SPEECH bin=%i fuz=%f  conf %f\n",vadBin,vadFuz0,conf); }
    printf("kla %i nS %i\n",kla,nS);
  }

 // long i;
// TODO: cache the vectors, i.e. build a separate mean&var Sum, until the turn confidence measure is received


  // TODO: update only if 
  if (vadBin) {
    if ( (speechModelWeight != -1.0) && ((kla&&(conf > 0.6)) || (vadFuz>0.7)) ) {
      // train speech model, online update of mean and variance
      for (i=0; i<MIN(nFt,vec->N); i++) {
        meanS[i] += src[i];
        FLOAT_DMEM m = meanS[i] / (FLOAT_DMEM)(nS+1);
        varS[i] += (src[i] - m)*(src[i] - m);
      }
      nS++;
    }
  } else {
    if (updateI) {
      if ( (stopModelWeight != -1.0) && (conf > 0.6) ) {
        // train agent / stop model, online update of mean and variance
        for (i=0; i<MIN(nFt,vec->N); i++) {
          meanI[i] += src[i];
          FLOAT_DMEM m = meanI[i] / (FLOAT_DMEM)(nI+1);
          varI[i] += (src[i] - m)*(src[i] - m);
        }
        nI++;
      }
    } else {
      if ( (noiseModelWeight != -1.0) && ((kla&&(conf>0.6)) || (vadFuz<0.2)) ) {
        // train noise model, online update of mean and variance
        for (i=0; i<MIN(nFt,vec->N); i++) {
          meanN[i] += src[i];
          FLOAT_DMEM m = meanN[i] / (FLOAT_DMEM)(nN+1);
          varN[i] += (src[i] - m)*(src[i] - m);
        }
        nN++;
      }
    }
  }

  // B: gaussian / mahalanobis ...?



  // re-decide VAD result

  if (kla) {
    if (vadFuz0 > 0.4) vadBin=1;
    else vadBin=0;
    //printf("VADbin %i\n",vadBin);
  } else {
    if ((nFt>=0)&&(nFt<vec->N)) vadFuz0 = src[nFt+1]; // pass on vadFuz (?) to turn detector
  }

  dst[0] = (FLOAT_DMEM)vadBin;
  dst[1] = (FLOAT_DMEM)vadFuz0;
  dst[2] = (FLOAT_DMEM)vadFuz0;

  // save to dataMemory
  writer->setNextFrame(vec0);
  delete vec0;

  return 1;
}

cSpeechLearn::~cSpeechLearn()
{
  int i;

printf("saving models...\n");
  // save current models
  if (nS > 0) {
    // normalise before save...
    for (i=0; i<nFt; i++) {
      meanS[i] /= (FLOAT_DMEM)nS;
    }
printf("speech %s\n",saveSpeechModel);
    saveModel(saveSpeechModel, meanS, nFt);
  }
  if (nN > 0) {
    // normalise before save...
    for (i=0; i<nFt; i++) {
      meanN[i] /= (FLOAT_DMEM)nN;
    }
    saveModel(saveNoiseModel, meanN, nFt);
  }
  if (nI > 0) {
    // normalise before save...
    for (i=0; i<nFt; i++) {
      meanI[i] /= (FLOAT_DMEM)nI;
    }
    saveModel(saveStopModel, meanI, nFt);
  }

//  if (spec != NULL) free(spec);
  if (meanS != NULL) free(meanS);
  if (meanN != NULL) free(meanN);
  if (meanI != NULL) free(meanI);
  if (varS != NULL) free(varS);
  if (varN != NULL) free(varN);
  if (varI != NULL) free(varI);
}

