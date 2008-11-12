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

#ifndef SEMAINE_COMPONENTS_SMILE_TUMFEATUREEXTRACTOR_H
#define SEMAINE_COMPONENTS_SMILE_TUMFEATUREEXTRACTOR_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
#include <semaine/cms/sender/FeatureSender.h>
#include <semaine/cms/sender/EmmaSender.h>


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include <tum/smile/src/featum_common.h>
#include <tum/smile/src/featum_util.h>      

#ifdef LIVE_REC
#include <tum/smile/src/live-input.hpp>
#endif

#ifdef DEBUG_SILDET
#include <tum/smile/src/wave-output.hpp>
#endif

#include <tum/smile/src/silence-detector.hpp>
#include <tum/smile/src/option-parser.hpp>
#include <tum/smile/src/input-framer.hpp>
#include <tum/smile/src/LLDex.hpp>
#include <tum/smile/src/LLDs.hpp>
#include <tum/smile/src/deltas.hpp>
#include <tum/smile/src/functionals.hpp>

#include <tum/smile/src/SVMpredict.hpp>
#include <tum/smile/AMQoutput.hpp>

/** definition of featureMemory levels **/
// TODO: define all levels in ONE central location...!
// #define LLD_LEVEL0 0    // defined elsewhere!
// #define LLD_LEVEL1 1    // defined elsewhere!
// #define LLD_LEVEL2 2    // defined elsewhere!
#define LLD0_DE 3
#define LLD1_DE 4
#define LLD2_DE 5
#define LLD0_DEDE 6
#define LLD1_DEDE 7
#define LLD2_DEDE 8

#define LLD0_FUNC_L0 9
#define LLD1_FUNC_L0 10
#define LLD2_FUNC_L0 11
#define LLD0_DE_FUNC_L0 12
#define LLD1_DE_FUNC_L0 13
#define LLD2_DE_FUNC_L0 14
#define LLD0_DEDE_FUNC_L0 15
#define LLD1_DEDE_FUNC_L0 16
#define LLD2_DEDE_FUNC_L0 17

#define LLD0_FUNC_L1 18
#define LLD1_FUNC_L1 19
#define LLD2_FUNC_L1 20
#define LLD0_DE_FUNC_L1 21
#define LLD1_DE_FUNC_L1 22
#define LLD2_DE_FUNC_L1 23
#define LLD0_DEDE_FUNC_L1 24
#define LLD1_DEDE_FUNC_L1 25
#define LLD2_DEDE_FUNC_L1 26

#define LLD0_FUNC_L0f 27
#define LLD0_DE_FUNC_L0f 28
#define LLD0_DEDE_FUNC_L0f 29

#define ftMEM_nLevels 30

/*** class definition ***/

using namespace cms;
using namespace semaine::components;
//using namespace tum::components:smile;
using namespace semaine::cms::sender;

namespace semaine {
namespace components {
namespace smile {

class TumFeatureExtractor : public Component
{
public:
	TumFeatureExtractor(pOptions _opts) throw (CMSException); 
	virtual ~TumFeatureExtractor();

protected:
	virtual void act() throw (CMSException);
	virtual void customStartIO() throw (CMSException);

private:
	FeatureSender * featureSender;
	EmmaSender * emmaSender;

	pOptions opts;
	cLiveInput *liveIn;
	cInputFramer * framedInput;
	cLLDex * lldex;
	cLLDs * llds;
	cFeatureMemory * ftMem;
	cDeltas * deltas;
	cFunctionals * functs;

	cAmqOutput * amqLowLevel;

	cSvmPredict * svmPredA;
	cSvmPredict * svmPredV;

	#ifdef DEBUG_SILDET
	cWaveOutput *waveOut;
	cPcmBuffer * frame2;
	#endif

	cSilenceDetector *sildet;
	cPcmBuffer * frame1;

	cFtIdentifier *meanE;
	cFtIdentifier *meanF0; 
	cFtIdentifier *meanF0n; 
	cFtIdentifier *meanF0s;  
	cFtIdentifier *frameE;

	int  getFrame_flag;
	int framerId1, framerId2;

	LONG_IDX turntime;
	int speaking;
};

} // namespace smile
} // namespace components
} // namespace semaine


#endif

