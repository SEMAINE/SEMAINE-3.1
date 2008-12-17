/*
 *  TumFeatureExtractor.cpp
 *  semaine
 *
 *  original: DummyFeatureExtractor.cpp
 *    Created by Marc Schröder on 19.09.08.
 *
 *  Modified by Florian Eyben on 11.11.08
 *  
 */

#include "TumFeatureExtractor.h"
#undef MODULE
#define MODULE "TumFeatureExtractor"

#include <cstdlib>
#include <sstream>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/EmotionML.h>
#include <semaine/datatypes/xml/SemaineML.h>

using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

namespace semaine {
namespace components {
namespace smile {


TumFeatureExtractor::TumFeatureExtractor(pOptions _opts) throw(CMSException) : 
	Component("TumFeatureExtractor"),
	opts(_opts),
	liveIn(NULL),
	#ifdef DEBUG_SILDET
	waveOut(NULL),
	frame2(NULL),
	#endif
	framedInput(NULL),
	lldex(NULL),
	llds(NULL),
	ftMem(NULL),
	deltas(NULL),
	functs(NULL),
	amqLowLevel(NULL),
	sildet(NULL),
	svmPredA(NULL),
	svmPredV(NULL),
	svmPredI(NULL),
	frame1(NULL)

{

	int period = 1; // ms
	featureSender = new FeatureSender("semaine.data.analysis.features.voice", "", getName(), period);
	waitingTime = period;
	senders.push_back(featureSender);
	featureSender->setTimeToLive(100); // discard messages after 100 ms

	emmaSender = new EmmaSender("semaine.data.state.user.emma", getName());
	senders.push_back(emmaSender);

	// A scalable list of features -- for load testing
/*
	int num = 100;
	std::vector<std::string> dummyFeatureNames(num);
	for (int i=0; i<num; i++) {
		std::ostringstream namestream;
		namestream << "feature.name.of.feature" << i;
		dummyFeatureNames[i] = namestream.str();
	}
	featureSender->setFeatureNames(dummyFeatureNames);
	featureSender->setTimeToLive(100); // discard messages after 100 ms
*/
}


TumFeatureExtractor::~TumFeatureExtractor()
{
	if (liveIn != NULL) delete liveIn;
	if (amqLowLevel != NULL) delete amqLowLevel;

	if (meanE != NULL) delete meanE;
	if (meanF0 != NULL) delete meanF0;
	if (meanF0n != NULL) delete meanF0n;
	if (meanF0s != NULL) delete meanF0s;
	if (frameE != NULL) delete frameE;

	if (framedInput != NULL) delete framedInput; 
	if (svmPredA != NULL) delete svmPredA;
	if (svmPredV != NULL) delete svmPredV;

	if (lldex != NULL) delete lldex;
	if (llds != NULL) delete llds;
	if (ftMem != NULL) delete ftMem;
	if (deltas != NULL) delete deltas;
	if (functs != NULL) delete functs;

	#ifdef DEBUG_SILDET
	if (waveOut != NULL) delete waveOut;
	if (frame2 != NULL) delete frame2;
	#endif

	if (pipeOut != NULL) delete pipeOut;
	if (sildet != NULL) delete sildet;

	if (frame1 != NULL) delete frame1;
	if (framePipe != NULL) delete framePipe;
}

void TumFeatureExtractor::customStartIO() throw(CMSException)
{
	//printf("customStartIO Here... %i",(long)opts); fflush(stdout);

	/***************************** soundcard audio input *****************************/
	FEATUM_DEBUG(2,"opening live input audio: %i Hz, %i bit, %i channel(s)",opts->samplerate, opts->bits, opts->channels);

	liveIn = new cLiveInput(NULL, opts->device, opts->buffersize, opts->samplerate, opts->bits, opts->channels, LI_STREAM_STARTED);
  	/*------------------------------------end--------------------------------------*/


	/***************************** create input framer *****************************/
	FEATUM_DEBUG(4,"creating input framer component");
	framedInput = new cInputFramer(*liveIn);

	framedInput->setAudioParameters(1, WORK_SAMPLETYPE, MEMORGA_INTERLV);
	int n_ll_levels = 0;   // low level levels (see below)

	// add framer clients:
	framerId1 = framedInput->addClientSecStep(opts->frameSize, opts->frameStep, 0); 
	n_ll_levels++;
 	framedInput->setPreEmphasis(framerId1, opts->preEmphasis);

	FEATUM_DEBUG(5,"frame size id0 = %i",framedInput->_data.client[framerId1]->frameLength);

	if (opts->pipeaudio) {
	  framerIdPipe = framedInput->addClientSecStep(opts->frameStep, opts->frameStep, 0);
	  framedInput->setWindowing(framerIdPipe, WIN_NONE);
	} else { framerIdPipe = -1; }

	#ifdef DEBUG_SILDET
	framerId2 = framedInput->addClientSecStep(opts->frameStep, opts->frameStep, 0);
	framedInput->setWindowing(framerId2, WIN_NONE);
	#endif
  	/*------------------------------------end--------------------------------------*/


  	/***************************** create LLDex ************************************/
	FEATUM_DEBUG(4,"creating LLDex component");
  	// -> temporary extractor data struct
  	int *nHist = (int*)calloc(1,sizeof(int)*n_ll_levels);
  	int i;
  	for (i=0; i<n_ll_levels; i++) {
    		nHist[i] = 10;  // TODO: autocreate from constants? frameLength/etc..?
  	}
  	lldex = new cLLDex(*framedInput, n_ll_levels, nHist);
  	free(nHist);
  	/*------------------------------------end--------------------------------------*/


	/***************************** create LLDs (main lld config) *******************/
        FEATUM_DEBUG(4,"creating LLDs component");
	llds = new cLLDs(*lldex, 1);

	// setup LLDs : enable, NULL obj (to auto create)
	llds->setupLLD("energy",1,NULL,LLD_LEVEL0);
	llds->setupLLD("fft",1,NULL,LLD_LEVEL0);
	llds->setupLLD("pitch",1,NULL,LLD_LEVEL0);
	//llds->setupLLD("mfcc",1,NULL,LLD_LEVEL0);
	pLLDmfcc mf = (pLLDmfcc)llds->setupLLD("mfcc",1,NULL,LLD_LEVEL0);
	LLDmfcc_configure( mf, opts->nMel, opts->nMFCC, opts->cepLifter, opts->firstMFCC, opts->usePower );
	pLLDmfccz mZ = (pLLDmfccz)llds->setupLLD("mfccz",1,NULL,LLD_LEVEL0);
	LLDmfccz_configure( mZ, opts->cmsAlpha, opts->cmsInitial ); 
	llds->setupLLD("time",1,NULL,LLD_LEVEL0);
	llds->setupLLD("spectral",1,NULL,LLD_LEVEL0);


	// check dependencies of extractors
	if (llds->checkDependencies() == 0) {
		FEATUM_ERR_FATAL(0,"LLD extractor dependency check failed");
		log->error("LLD extractor dependency check failed");
		throw SystemConfigurationException("LLD extractor dependency check failed");
	}
	llds->countEnabled();
	/*------------------------------------end--------------------------------------*/


	/***************************** create featureMemory ****************************/
	LONG_IDX cap[] = {100000,100000}; 
	// TODO: function to automatically compute capacity 
	// based on: a) time (sec.),  b) whole file
	int ids[] = {framerId1,0};    // -> todo ?  auto assign from framer object

	FEATUM_DEBUG(4,"creating featureMemory component");
 	ftMem = new cFeatureMemory(FTMEM_RING_BUFFER, ftMEM_nLevels);

 	// setup the levels and allocate memory for each level
	llds->setupFeatureMemLevels( *ftMem, *framedInput, ids, cap, 1 );
	llds->configureFeatureMemoryNames( *ftMem, LLD_LEVEL0 );
	/*------------------------------------end--------------------------------------*/


	/************************ create differentiator object for LLD *****************/
	FEATUM_DEBUG(4,"creating delta extractor");
	deltas = new cDeltas( 2 ); // two ids for delta and delta delta

	if (!deltas->setupID( *ftMem, 0, LLD0_DE, LLD_LEVEL0, 3)) {
		FEATUM_ERR_FATAL(0,"Failed setting up Delta extractor! (out of memory?)");
		log->error("Failed setting up delta extractor! (out of memory?)");
		throw SystemConfigurationException("Failed setting up delta extractor! (out of memory?");
	}
	if (!deltas->setupID( *ftMem, 1, LLD0_DEDE, LLD0_DE, 3)) {
		FEATUM_ERR_FATAL(0,"Failed setting up delta delta extractor! (out of memory?)");
		log->error("Failed setting up delta delta extractor! (out of memory?)");
		throw SystemConfigurationException("Failed setting up delta delta extractor! (out of memory?");                 
	} 
	/*------------------------------------end--------------------------------------*/  


	/***************************** create functionals object ***********************/
	#ifdef ENABLE_FUNCTIONALS
	FEATUM_DEBUG(4,"creating functionals extractor");
	functs = new cFunctionals( 9 );

	if (!opts->HRF)
	{
		// enable all functionals for all hierarchies (>180.000 features!)
		functs->enable("maxmin",NULL);
		functs->enable("mean",NULL);
		functs->enable("nzmean",NULL);
		functs->enable("mmmdist",NULL);
		functs->enable("quartile",NULL);
		functs->enable("percentile",NULL);
		functs->enable("qmean",NULL);
		functs->enable("moments",NULL);
		functs->enable("centroid",NULL);
		functs->enable("regression",NULL);
		functs->enable("peaks",NULL);
		functs->enable("times",NULL);
		functs->enable("segments",NULL);
		functs->enable("dct",NULL);

	} else {

		int functsL0[] = {0,1,2,6,7,8, -1};
		int functsL1[] = {3,4,5, -1};

		// enbale the functionals required on the first hierarchy...
		//functs->enable("maxmin",functsL0);
		functs->enable("percentile",NULL);
		functs->enable("mean",functsL0);
		//functs->enable("mmmdist",functsL0);
		//functs->enable("quartile",functsL0);
		functs->enable("moments",functsL0);
		functs->enable("centroid",functsL0);
		functs->enable("regression",functsL0);
		//functs->enable("times",functsL0);
		//functs->enable("segments",functsL0);
		functs->enable("dct",functsL0);

		// enbale the functionals required on the second hierarchy...
		functs->enable("maxmin",functsL1);
		functs->enable("mean",functsL1);
		functs->enable("moments",functsL1);
		functs->enable("quartile",functsL0);
		//functs->enable("centroid",functsL1);
		//functs->enable("regression",functsL1);
		//functs->enable("dct",functsL1);
	}

  	float funcL0_T = 0.5;
  	float funcL0_fs = 0.5;
	//if (opts->HRF) {   // TODO:::: fix bug in computeFunctionals, when not all levels are set up!
	  	// basic functionals (only required when hierarchical functionals are enabled!)
		functs->setupID( *ftMem, 0, LLD0_FUNC_L0, LLD_LEVEL0, funcL0_T, funcL0_fs, -1);
		functs->setupID( *ftMem, 1, LLD0_DE_FUNC_L0, LLD0_DE, funcL0_T, funcL0_fs, -1);
		functs->setupID( *ftMem, 2, LLD0_DEDE_FUNC_L0, LLD0_DEDE, funcL0_T, funcL0_fs, -1);

		// hierarchical functionals:
		functs->setupID( *ftMem, 3, LLD0_FUNC_L1, LLD0_FUNC_L0, 4.0, 16.0, -1);
		functs->setupID( *ftMem, 4, LLD0_DE_FUNC_L1, LLD0_DE_FUNC_L0, 4.0, 16.0, -1);
		functs->setupID( *ftMem, 5, LLD0_DEDE_FUNC_L1, LLD0_DEDE_FUNC_L0, 4.0, 16.0, -1);
	//}

	// full turn, non-hierarchical:
	functs->setupID( *ftMem, 6, LLD0_FUNC_L0f, LLD_LEVEL0, funcL0_T, funcL0_fs, -1);
	functs->setupID( *ftMem, 7, LLD0_DE_FUNC_L0f, LLD0_DE, funcL0_T, funcL0_fs, -1);
	functs->setupID( *ftMem, 8, LLD0_DEDE_FUNC_L0f, LLD0_DEDE, funcL0_T, funcL0_fs, -1);

	// do initialisation of names and count enabled functionals
	functs->finalizeConfiguration( );

	#endif
	/*------------------------------------end--------------------------------------*/



	/*************************** create output objects *****************************/
	// ActiveMQ output for 10ms low-level features (periodic)
	FEATUM_DEBUG(4,"creating ActiveMQ output component");
	amqLowLevel = new cAmqOutput( *ftMem, featureSender, &meta );
	#define nAMQels 12
	sElID amqEls[nAMQels];
	// energy:
	amqEls[0] = outputObject(LLD_LEVEL0, ft_lld_energy, 0, -1, 0, EL_ENABLED);
	amqEls[1] = outputObject(LLD0_DE, ft_lld_energy, 0, -1, 0, EL_ENABLED);
	amqEls[2] = outputObject(LLD0_DEDE, ft_lld_energy, 0, -1, 0, EL_ENABLED);
	// pitch:
	amqEls[3] = outputObject(LLD_LEVEL0, ft_lld_pitch, 0, -1, 0, EL_ENABLED);
	amqEls[4] = outputObject(LLD0_DE, ft_lld_pitch, 0, -1, 0, EL_ENABLED);
	amqEls[5] = outputObject(LLD0_DEDE, ft_lld_pitch, 0, -1, 0, EL_ENABLED);

	// MFCC:
	amqEls[6] = outputObject(LLD_LEVEL0, ft_lld_mfccz, 1, -1, 0, EL_ENABLED);
	amqEls[7] = outputObject(LLD_LEVEL0, ft_lld_mfccz, 0, 0, 0, EL_ENABLED);
	amqEls[8] = outputObject(LLD0_DE, ft_lld_mfccz, 1, -1, 0, EL_ENABLED);
	amqEls[9] = outputObject(LLD0_DE, ft_lld_mfccz, 0, 0, 0, EL_ENABLED);
	amqEls[10] = outputObject(LLD0_DEDE, ft_lld_mfccz, 1, -1, 0, EL_ENABLED);
	amqEls[11] = outputObject(LLD0_DEDE, ft_lld_mfccz, 0, 0, 0, EL_ENABLED);

//	amqEls[6] = outputObjectFull(LLD_LEVEL0, ft_lld_mfccz);
	//amqEls[7] = outputObjectFull(LLD0_DE, ft_lld_mfccz);
	//amqEls[8] = outputObjectFull(LLD0_DEDE, ft_lld_mfccz);

	/*
	// time domain features (zero crossings, etc.)
	amqEls[9] = outputObjectFull(LLD_LEVEL0, ft_lld_time);
	amqEls[10] = outputObjectFull(LLD0_DE, ft_lld_time);
	amqEls[11] = outputObjectFull(LLD0_DEDE, ft_lld_time);

	// spectral
	amqEls[12] = outputObjectFull(LLD_LEVEL0, ft_lld_spectral);
	amqEls[13] = outputObjectFull(LLD0_DE, ft_lld_spectral);
	amqEls[14] = outputObjectFull(LLD0_DEDE, ft_lld_spectral);
	*/

	amqLowLevel->configure(amqEls, nAMQels);


	#ifdef ENABLE_FUNCTIONALS
	#define nFUNCels0 15
	sElID funcEls0[nFUNCels0];

	int l0=LLD0_FUNC_L0f;
	int l1=LLD0_DE_FUNC_L0f;
	int l2=LLD0_DEDE_FUNC_L0f;

	if (opts->HRF) {
		l0 = LLD0_FUNC_L1;
		l1 = LLD0_DE_FUNC_L1;
		l2 = LLD0_DEDE_FUNC_L1;
	}

	funcEls0[0] = outputObjectFull(l0, ft_lld_energy);
	funcEls0[1] = outputObjectFull(l1, ft_lld_energy);
	funcEls0[2] = outputObjectFull(l2, ft_lld_energy);

	funcEls0[3] = outputObjectFull(l0, ft_lld_pitch);
	funcEls0[4] = outputObjectFull(l1, ft_lld_pitch);
	funcEls0[5] = outputObjectFull(l2, ft_lld_pitch);

	funcEls0[6] = outputObjectFull(l0, ft_lld_mfcc);
 	funcEls0[7] = outputObjectFull(l1, ft_lld_mfcc);
	funcEls0[8] = outputObjectFull(l2, ft_lld_mfcc);

	funcEls0[9] = outputObjectFull(l0, ft_lld_time);
	funcEls0[10] = outputObjectFull(l1, ft_lld_time);
	funcEls0[11] = outputObjectFull(l2, ft_lld_time);

	funcEls0[12] = outputObjectFull(l0, ft_lld_spectral);
	funcEls0[13] = outputObjectFull(l1, ft_lld_spectral);
	funcEls0[14] = outputObjectFull(l2, ft_lld_spectral);

	if (opts->svmmodelA != NULL) {
		FEATUM_MESSAGE(1,"loading svm model for arousal ...");
		try {
		  svmPredA = new cSvmPredict( *ftMem, opts->svmmodelA, opts->svmscaleA );
		  svmPredA->loadSelection(opts->svmpredfselA);
		  svmPredA->configure(funcEls0, nFUNCels0);
		} catch (int e) {
		  if (e==1) { 
		  		FEATUM_ERROR(1,"AROUSAL recognition is DISABLED!"); 
 				delete svmPredA; svmPredA = NULL; 
	      }
		}
	}
	if (opts->svmmodelV != NULL) {
		FEATUM_MESSAGE(1,"loading svm model for valence ...");
		try {
		  svmPredV = new cSvmPredict( *ftMem, opts->svmmodelV, opts->svmscaleV );
		  svmPredV->loadSelection(opts->svmpredfselV);
		  svmPredV->configure(funcEls0, nFUNCels0);
		} catch (int e) {
		  if (e==1) { 
		  		FEATUM_ERROR(1,"VALENCE recognition is DISABLED!"); 
	  	 		delete svmPredV; svmPredV = NULL; 
	      }
		}
	}
	if (opts->svmmodelI != NULL) {
		FEATUM_MESSAGE(1,"loading svm model for interest ...");
		try {
		  svmPredI = new cSvmPredict( *ftMem, opts->svmmodelI, opts->svmscaleI );
		  svmPredI->loadSelection(opts->svmpredfselI);
		  svmPredI->configure(funcEls0, nFUNCels0);
		} catch (int e) {
		  if (e==1) { 
		  		FEATUM_ERROR(1,"INTEREST recognition is DISABLED!"); 
		   		delete svmPredI; svmPredI = NULL; 
	      }
		}
	}
	#endif

	if (opts->sildet) {
		FEATUM_DEBUG(4,"initializing silence detector");
		sildet = new cSilenceDetector(*lldex, opts->sil_thresh);
	}

	#ifdef DEBUG_SILDET
	frame2 = NULL;
	if (!opts->sildet) 
		if (waveOut == NULL) {
			FEATUM_DEBUG(4,"initializing debug wave output");
			waveOut = new cWaveOutput( "dbg_out.wav", 16000, SAMPLETYPE_I16, 1 );
		}
	#endif
	if ((pipeOut == NULL)&&(opts->pipeaudio != NULL)) {
		pipeOut = new cWaveOutput( opts->pipeaudio, 16000, SAMPLETYPE_I16, 1, 1 );
	}

	meanE = new cFtIdentifier(LLD0_FUNC_L0f);
	meanF0 = new cFtIdentifier(LLD0_FUNC_L0f);
	meanF0n = new cFtIdentifier(LLD0_FUNC_L0f);
	meanF0s = new cFtIdentifier(LLD0_FUNC_L0f);
	frameE = new cFtIdentifier(LLD_LEVEL0);

	getFrame_flag = 0;

}

void TumFeatureExtractor::act() throw(CMSException)
{
	FEATUM_DEBUG(1,"act() called!");

	FEATUM_DEBUG(9,"calling getFrame (id1=%i)",framerId1);
	if (framedInput->getFrame(framerId1, &frame1)) {
		getFrame_flag = 1;
		#ifdef DEBUG_SILDET
		framedInput->getFrame(framerId2, &frame2);
		#endif
		if (opts->pipeaudio) {
			framedInput->getFrame(framerIdPipe, &framePipe);
			if ((framePipe != NULL)&&(pipeOut != NULL)) pipeOut->writeFrame(*framePipe);
		}
	}

	if (frame1 == NULL) FEATUM_DEBUG(9,"FRAME1 ==  NULL\n");

    	// add frame to short term ringbuffer
    	if (lldex->addFrame(LLD_LEVEL0, *frame1 )) {

		llds->callExtractors(LLD_LEVEL0);
		llds->flushToFtMem( *ftMem, LLD_LEVEL0 );
		

		deltas->autoCompute();
		//if (speaking) 
		   if (amqLowLevel != NULL) amqLowLevel->sendCurrentFrame(speaking);

		if (opts->displayenergy) {
			if (!frameE->found()) { ftMem->findFeature( "logEnergy", *frameE ); }
			FLOAT_TYPE_FTMEM *fen = ftMem->getFeaturesByID( *frameE, 0 );
			printf("frameE = %f\n",*fen); fflush(stdout);
		}

		#ifdef ENABLE_FUNCTIONALS
		int fFlag = 0;
		if (opts->HRF) {
			fFlag += functs->computeCurrentOnly( 0 );
			fFlag += functs->computeCurrentOnly( 1 );
			fFlag += functs->computeCurrentOnly( 2 );
		}
		#endif


		#ifdef ENABLE_FUNCTIONALS
		if (opts->sildet) {
			int sres = sildet->process(LLD_LEVEL0);

			if (sres == 2) { // begin of turn (10 frames before current position)
				printf("** detected turn start [t=%f]--\n",(*frame1)._data.timestamp); fflush(stdout);

       			// ++AMQ++ send turn start emma message
   				{
				  DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
				  DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
				  DOMElement * behaviour = XMLTool::appendChildElement(interpretation, SemaineML::E_BEHAVIOUR, SemaineML::namespaceURI);
				  XMLTool::setAttribute(behaviour, SemaineML::A_NAME, "speaking");
				  emmaSender->sendXML(document, meta.getTime());				  
				}

				if (opts->HRF) {
					functs->setFrameStart( 3, -10 );
					functs->setFrameStart( 4, -10 );
					functs->setFrameStart( 5, -10 );
				} else {
					functs->setFrameStart( 6, -10 );
					functs->setFrameStart( 7, -10 );
					functs->setFrameStart( 8, -10 );
				}
				turntime = 0;
				speaking = 1;

				#ifdef DEBUG_SILDET
				// open output wave file
				if (waveOut == NULL)
					waveOut = new cWaveOutput( "dbg_out.wav", 16000, SAMPLETYPE_I16, 1 );
				#endif

			}

			#ifdef DEBUG_SILDET
			// save pcm frame  (non overlap...! non pre emph, non windowed!!!)
			if (waveOut != NULL) waveOut->writeFrame(*frame2);
			#endif

			if (speaking == 1) turntime++;

			if (( sres == 1 )||(turntime>1000)) {  // ||(turnlength > 10s)
				printf("-- detected turn end [t=%f] [l=%i]--\n",(*frame1)._data.timestamp,turntime); fflush(stdout);

       				// ++AMQ++ send turn end emma message
	       			{
				  DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
				  DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
				  DOMElement * behaviour = XMLTool::appendChildElement(interpretation, SemaineML::E_BEHAVIOUR, SemaineML::namespaceURI);
				  XMLTool::setAttribute(behaviour, SemaineML::A_NAME, "silent");
				  emmaSender->sendXML(document, meta.getTime());				  
				}

				#ifdef DEBUG_SILDET
				// close output wave file, increment counter
				delete waveOut; waveOut = NULL;
				#endif

				if (opts->HRF) {
					functs->computeNow(  0, 1 );
					functs->computeNow(  1, 1 );
					functs->computeNow(  2, 1 );
					functs->computeNow(  3, 1 );
					functs->computeNow(  4, 1 );
					functs->computeNow(  5, 1 );
				} else {
					functs->computeNow(  6, 1 );
					functs->computeNow(  7, 1 );
					functs->computeNow(  8, 1 );
				}
				speaking = 0;

				// get voicing and energy parameters + mean pitch
				// warning: the names are not unique, they occur on other levels (0.5s functionals...)
				if (!meanE->found()) { 
					ftMem->findFeature( "logEnergy-max", *meanE ); 
				}
				if (!meanF0->found()) { 
					ftMem->findFeature( "F0freq-nzmean", *meanF0 ); 
				}
				if (!meanF0n->found()) { 
					ftMem->findFeature( "F0freq-nzN", *meanF0n ); 
				}
				if (!meanF0s->found()) { 
					ftMem->findFeature( "F0strength-percentile95", *meanF0s ); 
				}
				FLOAT_TYPE_FTMEM *en = ftMem->getFeaturesByID( *meanE, 0 );
				FLOAT_TYPE_FTMEM *f0 = ftMem->getFeaturesByID( *meanF0, 0 );
				FLOAT_TYPE_FTMEM *f0n = ftMem->getFeaturesByID( *meanF0n, 0 );
				FLOAT_TYPE_FTMEM *f0s = ftMem->getFeaturesByID( *meanF0s, 0 );
				if (en != NULL) printf(" MAX E = %f\n",*en); fflush(stdout);
				if (f0 != NULL) printf(" NZMEAN F0 = %f\n",*f0 * MAX_PITCH); fflush(stdout);
				if (f0n != NULL) printf(" voiced \% = %f\n",*f0n);fflush(stdout);
				if (f0s != NULL) printf(" voicing = %f\n",*f0s);fflush(stdout);


				if ( (f0 != NULL)&&(f0n != NULL)&&(en!= NULL)&&(f0s!=NULL)
					&&(turntime > 100)
					//&& (*f0* MAX_PITCH > 40.0)
					&& (*f0* MAX_PITCH < 495.0)
					&&
					(
						//(*f0n > 0.001) &&
						(*f0s > 0.40) && (*en > -11.0)  )
						|| ( (*en > -9.0) && (*f0s > 0.5) )
					) {


					printf("\n !VOICE!\n");
					// classify:
					if (svmPredA != NULL) {
						svmPredA->setIdxToLastFrame();
						printf(">>>> arousal: "); 
						fflush(stdout);
						svmPredA->processNextFrame(); 
						fflush(stdout);
					}
					if (svmPredV != NULL) {
						svmPredV->setIdxToLastFrame();
						printf(">>>> valence: "); fflush(stdout);
						svmPredV->processNextFrame(); fflush(stdout);
					}
					if (svmPredI != NULL) {
						svmPredI->setIdxToLastFrame();
						printf(">>>> interest (0-2): "); fflush(stdout);
						svmPredI->processNextFrame(); fflush(stdout);
					}
					printf("\n");

					float a,v,i;
					if (svmPredA != NULL) {
					  a = svmPredA->getLastResult();
					}
					if (svmPredV != NULL) {
					  v = svmPredV->getLastResult();
					}
					if (svmPredI != NULL) {
					  i = svmPredI->getLastResult();
					}

					// ++AMQ++  send (arousal, valence, interest) as EMMA

					char strtmp[50];
					sprintf(strtmp,"%.2f",a);
					std::string aroStr(strtmp);
					sprintf(strtmp,"%.2f",v);
					std::string valStr(strtmp);
					sprintf(strtmp,"%1.0f",i);
					std::string interestStr(strtmp);

					// Create and fill a simple EMMA EmotionML document
					DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
					DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
					XMLTool::setAttribute(interpretation, EMMA::A_START, "time not implemented");
					DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
					if ((svmPredA != NULL)||(svmPredV != NULL)) {
					  DOMElement * dimensions = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSIONS, EmotionML::namespaceURI);
					  XMLTool::setAttribute(dimensions, EmotionML::A_SET, "valenceArousalPotency");
					  if (svmPredA != NULL) {
					    DOMElement * arousal = XMLTool::appendChildElement(dimensions, EmotionML::E_AROUSAL, EmotionML::namespaceURI);
					    XMLTool::setAttribute(arousal, EmotionML::A_VALUE, aroStr);
					  }
					  if (svmPredV != NULL) {
					    DOMElement * valence = XMLTool::appendChildElement(dimensions, EmotionML::E_VALENCE, EmotionML::namespaceURI);
					    XMLTool::setAttribute(valence, EmotionML::A_VALUE, valStr);
					  }
					}
					if (svmPredI != NULL) {
					  DOMElement * category = XMLTool::appendChildElement(emotion, EmotionML::E_CATEGORY, EmotionML::namespaceURI);
					  XMLTool::setAttribute(category, EmotionML::A_NAME, "interest");
					  XMLTool::setAttribute(category, EmotionML::A_VALUE, interestStr);
					}

					// Now send it
					emmaSender->sendXML(document, meta.getTime());

/*
          printf("<emotion>\n");
          printf("  <dimensions set=\"valenceArousalPotency\">\n");
          printf("    <arousal value=\"%f\">\n",a);
          printf("    <valence value=\"%f\">\n",v);
          printf("    <potency value=\"%f\">\n",0.0);
          printf("  </dimensions>\n");
          printf("</emotion>\n\n");
          fflush(stdout);
*/
				}    else {
          				// TODO: skip frame...
        			}
				turntime = 0;


			}
		}
		#endif

	}



}


} // namespace smile
} // namespace components
} // namespace semaine
