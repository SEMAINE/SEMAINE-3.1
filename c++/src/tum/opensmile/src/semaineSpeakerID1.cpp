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

NN learner for speech/non-speech features
last input field should be binary VAD

*/

/* TODO:
failsafe modes:
- no agent interrupt (always filter agent)
- disable NN vad
- disable adaptive thresholds (in vadV1.cpp)
disable update of spectral subtraction during speech & AGENT!
*/

#include <semaineSpeakerID1.hpp>

#define MODULE "cSemaineSpeakerID1"


SMILECOMPONENT_STATICS(cSemaineSpeakerID1)

SMILECOMPONENT_REGCOMP(cSemaineSpeakerID1)
{
	SMILECOMPONENT_REGCOMP_INIT
		scname = COMPONENT_NAME_CSEMAINESPEAKERID1;
	sdescription = COMPONENT_DESCRIPTION_CSEMAINESPEAKERID1;

	// we inherit cVectorProcessor configType and extend it:
	SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
		SMILECOMPONENT_IFNOTREGAGAIN(
		ct->setField("trainDur","duration of initial training phase in frames (for each speech and noise)",500);
	ct->setField("agentBlockTime","initial user speech time during which to block agent in frames (this must be high enough in order to have robust enough models)",1000);
	// TODO: ring buffer noise model updates....?
	ct->setField("maxTrainTime","max time to train user model for, to prevent over adaptation (-1 for infinite)",10000);
	ct->setField("debug","show classification result as debug output",0);
	ct->setField("a0","decay factor of fuzzy vad",0.05);
	ct->setField("a1","attack factor of fuzzy vad (>>a0)",0.2);
	ct->setField("initSpeechModel","file to load initial speech model coefficients from",(const char*)NULL);
	ct->setField("initNoiseModel","file to load initial noise model coefficients from",(const char*)NULL);
	ct->setField("initAgentModel","file to load initial learn inhibit model coefficients from (if frame matches this model, it will be treated as noise, but the speech/noise model will not be updated)","model.agent");
	ct->setField("agentModelWeight","weight of initial agent model, ~ number of frames the model represents; set to -1 to disable on-line model update, set to -2 to read weight from model file, set to 0 to disable initial model",-1);
	ct->setField("noiseModelWeight","weight of initial noise model, ~ number of frames the model represents, set to -1 to disable on-line model update, set to -2 to read weight from model file, set to 0 to disable initial model",2000);
	ct->setField("speechModelWeight","weight of initial speech model, ~ number of frames the model represents, set to -1 to disable on-line model update, set to -2 to read weight from model file, set to 0 to disable initial model",1000);
	ct->setField("saveSpeechModel","file to save speech model coefficients to",(const char*)NULL);
	ct->setField("saveNoiseModel","file to save noise model coefficients to",(const char*)NULL);
	ct->setField("saveAgentModel","file to save learn inhibit model coefficients to",(const char*)NULL);
	ct->setField("numAgents", "number of agent speakers (i.e. number of models in agent model file)",4);
	// failsafe flags:
	ct->setField("alwaysRejectAgent", "never detect a speaker turn while the agent is speaking",0);
	ct->setField("ruleVadOnly","don't use speaker adaptive vad, use only rule based vad with dynamic thresholds (also implies 'alwaysRejectAgent')",0);

	)
		SMILECOMPONENT_MAKEINFO(cSemaineSpeakerID1);
}

SMILECOMPONENT_CREATE(cSemaineSpeakerID1)

//-----

cSemaineSpeakerID1::cSemaineSpeakerID1(const char *_name) :
cDataProcessor(_name),
meanS(NULL), meanN(NULL), meanA(NULL),
varS(NULL), varN(NULL), varA(NULL), debug(0), trainDur(0),
nFt(0), vadFuz0(0.0), a0((FLOAT_DMEM)0.05), a1((FLOAT_DMEM)0.2), kla(0),
nTurns(0), nS(0), nN(0), nA(NULL), numAgents(0),
initSpeechModel(NULL), initNoiseModel(NULL), initAgentModel(NULL),
saveSpeechModel(NULL), saveNoiseModel(NULL), saveAgentModel(NULL),
agentModelWeight((FLOAT_DMEM)0.0), noiseModelWeight((FLOAT_DMEM)0.0), speechModelWeight((FLOAT_DMEM)0.0),
agentTurn(0), agentTurnCnt(0), alwaysRejectAgent(0), ruleVadOnly(0),
runTime(0), doReset(1), userPresence(0)
{

}

void cSemaineSpeakerID1::fetchConfig()
{
	cDataProcessor::fetchConfig();

	trainDur = getInt("trainDur");
	agentBlockTime = getInt("agentBlockTime");
    maxTrainTime = getInt("maxTrainTime");

	debug = getInt("debug");

	a0 = (FLOAT_DMEM)getDouble("a0");
	a1 = (FLOAT_DMEM)getDouble("a1");

	initSpeechModel=getStr("initSpeechModel");
	initNoiseModel=getStr("initNoiseModel");
	initAgentModel=getStr("initAgentModel");
	agentModelWeight=(FLOAT_DMEM)getDouble("agentModelWeight");
	noiseModelWeight=(FLOAT_DMEM)getDouble("noiseModelWeight");
	speechModelWeight=(FLOAT_DMEM)getDouble("speechModelWeight");
	saveSpeechModel=getStr("saveSpeechModel");
	if (saveSpeechModel != NULL) saveSpeechModel = strdup(saveSpeechModel);
	saveNoiseModel=getStr("saveNoiseModel");
	if (saveNoiseModel != NULL) saveNoiseModel = strdup(saveNoiseModel);
	saveAgentModel=getStr("saveAgentModel");
	if (saveAgentModel != NULL) saveAgentModel = strdup(saveAgentModel);

	numAgents = getInt("numAgents");

	alwaysRejectAgent = getInt("alwaysRejectAgent");
	ruleVadOnly = getInt("ruleVadOnly");
}

int cSemaineSpeakerID1::setupNewNames(long nEl)
{
	findInputMapping();

	writer->addField( "vadBin" );
	writer->addField( "vadFuz" );
	writer->addField( "vadSmo" );
	namesAreSet = 1;

	return 1;
}

void cSemaineSpeakerID1::findInputMapping()
{
	const FrameMetaInfo * fmeta = reader->getFrameMetaInfo();
	int i;
	long vad1 = fmeta->findFieldByPartialName( "vadBin" );

	if (vad1 < 0) { vad1 = 0; } // default : fallbak to 0th field

	nFt = fmeta->fieldToElementIdx( vad1 );

	meanS = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
	meanN = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
	meanA = (FLOAT_DMEM**)calloc(1,sizeof(FLOAT_DMEM*)*numAgents);
	for (i=0; i<numAgents; i++) {
		meanA[i] = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
	}
	nA = (long long *)calloc(1,sizeof(long long)*numAgents);
	varS = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
	varN = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
	varA = (FLOAT_DMEM**)calloc(1,sizeof(FLOAT_DMEM*)*numAgents);
	for (i=0; i<numAgents; i++) {
		varA[i] = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
	}
}

int cSemaineSpeakerID1::loadModel(const char *filename, FLOAT_DMEM *coeff, int N, FLOAT_DMEM weight)
{
	int i;
	for (i=0; i<N; i++) coeff[i] = 0.0;

	if ((filename != NULL)&&(N>0)&&(coeff!=NULL)) {
		FILE *f = fopen(filename,"rb");
		if (f==NULL) {
			SMILE_IERR(1,"model file '%s' not found or could not be opened",filename);
			return 0;
		}
		float Nf=0.0; float dummy=0.0;
		fread( &Nf, sizeof(float), 1, f);
		fread( &dummy, sizeof(float), 1, f); // nVec is ignored here (is always 1 currently)
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
		printf("LOAD '%s' 0: %f, 1: %f\n",filename,coeff[0],coeff[1]);
		free(tmp);
		return N;
	}
	return 0;
}

int cSemaineSpeakerID1::loadModelArr(const char *filename, FLOAT_DMEM **coeff, int N, int Nmodels, FLOAT_DMEM weight)
{
	int i,j;
	for (j=0; j<Nmodels; j++) {
		if (coeff[j] != NULL) for (i=0; i<N; i++) coeff[j][i] = 0.0;
	}

	if ((filename != NULL)&&(N>0)&&(Nmodels>0)&&(coeff!=NULL)) {
		FILE *f = fopen(filename,"rb");
		if (f==NULL) {
			SMILE_IDBG(3,"model file '%s' not found or could not be opened",filename);
			return 0;
		}
		float Nf=0.0; float dummy=0.0;
		fread( &Nf, sizeof(float), 1, f);
		fwrite( &dummy, sizeof(float), 1, f); 
		if ((int)Nf != N) {
			SMILE_IWRN(2,"number of coefficients in model file '%s' (%i) is unequal the requested number (%i)!",filename,(int)Nf,N);
		}
		N = MIN( (int)Nf, N );
		Nmodels = MIN(Nmodels,(int)dummy);
		float *tmp = (float*)malloc(sizeof(float)*N);
		for (j=0; j<Nmodels; j++) {
			fread( tmp, sizeof(float)*N, 1, f);
			int i;
			if (weight < 1.0) weight=1.0;
			if (coeff[j] != NULL) {
				for (i=0; i<N; i++) { 
					coeff[j][i] = (FLOAT_DMEM)tmp[i] * weight; 
				}
			}
		}

		fclose(f);
		free(tmp);
		return N;
	}
	return 0;
}

/*
fileformat:
Nc= No. of coefficients (FLOAT, 32-bit)
Nv= Num. of vectors     (FLOAT, 32-bit)
Nc*FLOAT
Nc*FLOAT
....
Nc*FLOAT   (Nv rows...)
*/
int cSemaineSpeakerID1::saveModel(const char *filename, FLOAT_DMEM *coeff, int N, FLOAT_DMEM weight)
{
	if ((filename != NULL)&&(N>0)&&(coeff!=NULL)) {
		//printf("open %s\n",filename);
		FILE *f = fopen(filename,"wb");
		if (f==NULL) {
			SMILE_IDBG(3,"model file '%s' could not be opened for writing",filename);
			return 0;
		}
		printf("saVE '%s' 0: %f, 1: %f\n",filename,coeff[0],coeff[1]);

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

int cSemaineSpeakerID1::saveModelArr(const char *filename, FLOAT_DMEM **coeff, int N, int Nmodels, FLOAT_DMEM weight)
{
	if ((filename != NULL)&&(N>0)&&(Nmodels>0)&&(coeff!=NULL)) {
		//printf("open %s\n",filename);
		FILE *f = fopen(filename,"wb");
		if (f==NULL) {
			SMILE_IDBG(3,"model file '%s' could not be opened for writing",filename);
			return 0;
		}
		float Nf=(float)N;
		fwrite( &Nf, sizeof(float), 1, f);
		Nf = (float)Nmodels; 
		fwrite( &Nf, sizeof(float), 1, f);


		float *tmp = (float*)malloc(sizeof(float)*N);
		int j;
		for (j=0; j<Nmodels; j++) {
			int i;
			if (weight < 1.0) weight = 1.0;
			if (coeff[j] != NULL) {
				for (i=0; i<N; i++) { 
					tmp[i] = (float)( coeff[j][i] / weight ); 
				}
			}
			fwrite( tmp, sizeof(float)*N, 1, f);
		}
		fclose(f);
		free(tmp);
		return 1;
	}
	return 0;
}

// initialise or re-set the speaker adaptive VAD models
int cSemaineSpeakerID1::initialiseVadModels()
{
  long i,j;

  // clear existing models or allocate parameter space for new models
  if (meanS != NULL) {
    for (i=0; i<nFt; i++) { meanS[i] = 0.0; }
  } else {
    meanS = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  }
  if (meanN != NULL) {
    for (i=0; i<nFt; i++) { meanN[i] = 0.0; }
  } else {
    meanN = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  }
  if (meanA != NULL) {
    for (j=0; j<numAgents; j++) {
      for (i=0; i<nFt; i++) { meanA[j][i] = 0.0; }
    }
  } else {
    meanA = (FLOAT_DMEM**)calloc(1,sizeof(FLOAT_DMEM*)*numAgents);
    for (i=0; i<numAgents; i++) {
      meanA[i] = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
    }
  }

  if (nA != NULL) {
    for (i=0; i<numAgents; i++) { nA[i] = 0; }
  } else {
    nA = (long long *)calloc(1,sizeof(long long)*numAgents);
  }
  nS = 0;
  nN = 0;

  if (varS != NULL) {
    for (i=0; i<nFt; i++) { varS[i] = 0.0; }
  } else {
    varS = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  }
  if (varN != NULL) {
    for (i=0; i<nFt; i++) { varN[i] = 0.0; }
  } else {
    varN = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
  }
  if (varA != NULL) {
    for (j=0; j<numAgents; j++) {
      for (i=0; i<nFt; i++) { varA[j][i] = 0.0; }
    }
  } else {
    varA = (FLOAT_DMEM**)calloc(1,sizeof(FLOAT_DMEM*)*numAgents);
    for (i=0; i<numAgents; i++) {
      varA[i] = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*nFt);
    }
  }



  // load initial model coefficients, if enabled
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
  if (agentModelWeight != 0.0) {
    if (loadModelArr(initAgentModel, meanA, nFt, numAgents, agentModelWeight)) {
      int i;
      for (i=0; i<numAgents; i++) {
        if (agentModelWeight == -1.0) nA[i] = 1;
        else nA[i] = (int)round(agentModelWeight);
      }
    }
  }

  // reset some other variables
  runTime = 0;

  // clear doReset flag
  lockMessageMemory();
  doReset = 0;
  unlockMessageMemory();

  return 1;
}

int cSemaineSpeakerID1::myFinaliseInstance()
{
	int ret = cDataProcessor::myFinaliseInstance();
	if (ret) {
		ret = initialiseVadModels();
	}
	return ret;
}


int cSemaineSpeakerID1::processComponentMessage( cComponentMessage *_msg )
{
	if (isMessageType(_msg,"semaineCallback")) {
		// determine origin by message's user-defined name, which can be set in the config file
		SMILE_IDBG(3,"received 'semaineCallback' message '%s'",_msg->msgname);
		if (!strncmp(_msg->msgname,"start",5)) agentTurn = 1;
		if (!strncmp(_msg->msgname,"end",3)) agentTurn = 0;
    if (!strncmp(_msg->msgname,"present",7)) { if (userPresence != 1) { userPresence = 1; doReset=1; } }
    if (!strncmp(_msg->msgname,"absent",6)) { if (userPresence != 0) { userPresence = 0; doReset=1; } }
		return 1;  // message was processed
	}

	return 0; // if message was not processed
}


// a derived class should override this method, in order to implement the actual processing
int cSemaineSpeakerID1::myTick(long long t)
{
	// get next frame from dataMemory
	//printf("tick");
	cVector *vec = reader->getNextFrame();
	if (vec == NULL) return 0;
	//printf(" OK \n");

	int _agentTurn=0;
  int _doReset = 0;
	lockMessageMemory();
	_agentTurn = agentTurn;
  _doReset = doReset;
	unlockMessageMemory();

  if (doReset)  {
    initialiseVadModels();
    SMILE_IMSG(2,"VAD was reset\n");
  }

  runTime++;
	int updateAgent = 0;
	int cl = 0; // classification result

	cVector *vec0 = new cVector(3);  // TODO: move vec0 to class...

	FLOAT_DMEM *src = vec->dataF;
	FLOAT_DMEM *dst = vec0->dataF;

	int vadBin = 0;
	FLOAT_DMEM vadFuz = 0.0;

	if ((nFt>=0)&&(nFt<vec->N)) vadBin = (int)(src[nFt]);
	if ((nFt>=0)&&(nFt<vec->N)) vadFuz = src[nFt+1]; // pass on vadFuz (?) to turn detector

  // hack: ignore vad input...
  vadFuz=0;
  vadBin=0;

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

	// match current frame to all models:

	// A: euclidean distance
	FLOAT_DMEM dS=0.0, dN=0.0;
	if ((nN > 0) && (nS > 0)) {
		for (i=0; i<MIN(nFt,vec->N); i++) {
			FLOAT_DMEM tmp = (src[i] - meanS[i]/(FLOAT_DMEM)nS);
			dS += tmp*tmp;
			tmp = (src[i] - meanN[i]/(FLOAT_DMEM)nN);
			dN += tmp*tmp;
		}
	}
	FLOAT_DMEM *dA = NULL;
	if (nA != NULL) {
		dA = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*numAgents);
		int j;
		for (j=0; j<numAgents; j++) {
			if ((nA[j] > 0)) {
				for (i=0; i<MIN(nFt,vec->N); i++) {
					FLOAT_DMEM tmp = (src[i] - meanA[j][i]/(FLOAT_DMEM)(nA[j]));
					dA[j] += tmp*tmp;
				}
			}
		}
	}

	if (debug>=2) {
		printf("dS %f   dN %f    dA[0] %f    dA[1] %f  \n",dS,dN,dA[0],dA[1]);
	}

	FLOAT_DMEM conf = 0.0;

	// start using models after we have enough training material
	if ((nS > trainDur)&&(nN>trainDur)) {

		if (kla==0) {
			SMILE_IMSG(2,"started using learnt (or pre-loaded) speaker verification model");
			kla=1;
		}

		FLOAT_DMEM x=(FLOAT_DMEM)nS/(FLOAT_DMEM)(trainDur*2);
		if (nS > trainDur*2) x=1.0;

		vadBin=0;
		vadFuz=0.0;

		// TODO: compute decision confidence  and use this for update decision...
		// do we have an agent model?
		if ((numAgents > 0)&&(nA!=NULL)&&(nA[0]>0)) {

			if (dS < dN) {
				int flag = 1; int x; for (x=0; x<numAgents; x++) { if (dS >= dA[x]) flag = 0; }
				if (flag) { // it is speech
					cl=1;
					conf = (FLOAT_DMEM)1.0-(dS/dN)*(dS/dN);
					vadFuz=x * (FLOAT_DMEM)1.0 + ((FLOAT_DMEM)1.0-x)*(FLOAT_DMEM)vadBin;
					if (conf < (FLOAT_DMEM)0.6) vadFuz *= conf; // ??
					vadBin=1;
					updateAgent=0;


				} else {  // noise or one of the agents?
					int flag = 1; int x; for (x=0; x<numAgents; x++) { if (dN >= dA[x]) flag = 0; }
					if (flag) { // it is noise
						updateAgent = 0;
						cl=0;
						conf = (FLOAT_DMEM)1.0-(dN/dS)*(dN/dS);
					} else {
						// it is an agent, which one?
						// find min(dA)
						FLOAT_DMEM min=dA[0]; int minIdx = 0;
						int i;
						for (i=0; i<numAgents; i++) {
							if (dA[i] < min) { min = dA[i]; minIdx = i; }
						}
						updateAgent = 1+minIdx;
						cl=2+minIdx;
						conf = (FLOAT_DMEM)1.0-(min/dS)*(min/dS);
					}
				}
			} 

		} else {  // no agent model, standard 2 class speech/noise problem

			if (dS >= dN) {
				//noise
				updateAgent = 0;
				cl=0;
				conf = (FLOAT_DMEM)1.0-(dN/dS)*(dN/dS);

			} else {
				cl=1;
				conf = (FLOAT_DMEM)1.0-(dS/dN)*(dS/dN);
				vadFuz=x * (FLOAT_DMEM)1.0 + ((FLOAT_DMEM)1.0-x)*(FLOAT_DMEM)vadBin;
				if (conf < (FLOAT_DMEM)0.6) vadFuz *= conf; // ??
				vadBin=1;
				updateAgent=0;

			}

		}

		if ((_agentTurn)&&((nS < agentBlockTime)||(alwaysRejectAgent))) { vadFuz = 0.0; vadFuz0 = 0.0; }
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

	// process the callback messages here...
	// we can determine if an agent is speaking
	// if it is speaking, we will never detect speech, and we will not update the noise nor speech model
	if (_agentTurn) {
		if ((nS <= agentBlockTime)||(alwaysRejectAgent)) vadBin = 0;
		updateAgent=1;
	}

	// ----



	// long i;
	// TODO: cache the vectors, i.e. build a separate mean&var Sum, until the turn confidence measure is received
//????????? block any voice activity in the first 3s of adaptation... is this still required???
  if (runTime < 300) {
    vadBin=0;
    vadFuz=0;
    vadFuz0 = 0;
  }

	// now update the models with the new data...
	if (vadBin) {
		if ( (speechModelWeight != -1.0) && (nS < maxTrainTime || maxTrainTime == -1) && (!_agentTurn) && ( (kla&&(conf > 0.6)) || ((!kla)&&(vadFuz>0.7)) ) ) {
			// train speech model, online update of mean and variance
			for (i=0; i<MIN(nFt,vec->N); i++) {
				meanS[i] += src[i];
				FLOAT_DMEM m = meanS[i] / (FLOAT_DMEM)(nS+1);
				varS[i] += (src[i] - m)*(src[i] - m);
			}
			nS++;
		}
	} else {
		if (updateAgent) {
			int j=updateAgent-1;
			if ( (agentModelWeight != -1.0) && ((conf > 0.6)||(nA[j] < 100)) ) {
				// train agent model, online update of mean and variance
				for (i=0; i<MIN(nFt,vec->N); i++) {
					meanA[j][i] += src[i];
					FLOAT_DMEM m = meanA[j][i] / (FLOAT_DMEM)(nA[j]+1);
					varA[j][i] += (src[i] - m)*(src[i] - m);
				}
				nA[j]++;
			}
		} else {
			if ( (noiseModelWeight != -1.0) && ( (kla&&(conf>0.6)) || ((!kla)&&(vadFuz<0.2)) ) ) {
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
	// = TODO!


	// re-decide VAD result

	if ((!ruleVadOnly)&&(kla)) {
		if (vadFuz0 > 0.4) vadBin=1;
		else vadBin=0;

		//printf("VADbin %i\n",vadBin);
	} else {
		if ((nFt>=0)&&(nFt<vec->N)) vadFuz0 = src[nFt+1]; // pass on vadFuz (?) to turn detector
		if (_agentTurn) {
			vadFuz0 = 0.0; vadBin=0;
			agentTurnCnt=20;
		}
		if ((!_agentTurn)&&(agentTurnCnt>0)) {
			vadFuz0 = 0.0; vadBin=0;
			agentTurnCnt--;
		}
	}

	if (debug) {
		if (cl==0) { printf("noise bin=%i, fuz=%f  conf %f\n",vadBin,vadFuz0,conf); }
		else if (cl>=2) { printf("Agent[%i of %i] bin=%i, fuz=%f  conf %f\n",cl-1,numAgents,vadBin,vadFuz0,conf); }
		else if (cl==1) { printf("SPEECH bin=%i fuz=%f  conf %f\n",vadBin,vadFuz0,conf); }
		printf("kla %i nS %i  nN %i %i  AGENT %i\n",kla,nS,nN,_agentTurn,0);
	}

	dst[0] = (FLOAT_DMEM)vadBin;
	dst[1] = (FLOAT_DMEM)vadFuz0;
	dst[2] = (FLOAT_DMEM)vadFuz0;

	if (dA != NULL) free(dA);

	// save to dataMemory
	writer->setNextFrame(vec0);
	delete vec0;

	return 1;
}

cSemaineSpeakerID1::~cSemaineSpeakerID1()
{
	int i,j;

	//printf("saving models...\n");
	// save current models
	if (nS > 0) {
		// normalise before save...
		/*for (i=0; i<nFt; i++) {
		meanS[i] /= (FLOAT_DMEM)nS;
		}*/
		//printf("speech %s\n",saveSpeechModel);
		saveModel(saveSpeechModel, meanS, nFt, (FLOAT_DMEM)nS);
	}
	if (nN > 0) {
		// normalise before save...
		/*for (i=0; i<nFt; i++) {
		meanN[i] /= (FLOAT_DMEM)nN;
		}*/
		saveModel(saveNoiseModel, meanN, nFt, (FLOAT_DMEM)nN);
	}
	if ((nA != NULL)&&(nA[0]>0)) {
		// normalise before save...
		for (j=0; j<numAgents; j++) {
			for (i=0; i<nFt; i++) {
				meanA[j][i] /= (FLOAT_DMEM)nA[j];
			}
		}
		saveModelArr(saveAgentModel, meanA, nFt, numAgents);
	}

	//  if (spec != NULL) free(spec);
	if (meanS != NULL) free(meanS);
	if (meanN != NULL) free(meanN);
	if (meanA != NULL) {
		for (i=0; i<numAgents; i++) if (meanA[i] != NULL) free(meanA[i]);
		free(meanA);
	}
	if (nA != NULL) free(nA);
	if (varS != NULL) free(varS);
	if (varN != NULL) free(varN);
	if (varA != NULL) {
		for (i=0; i<numAgents; i++) if (varA[i] != NULL) free(varA[i]);
		free(varA);
	}
}

