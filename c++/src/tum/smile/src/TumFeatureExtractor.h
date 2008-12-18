/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
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
 *******************************************************************************/

/*
 *  TumFeatureExtractor.h
 *  semaine
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
#include <strings.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "featum_common.h"
#include "featum_util.h"      

#ifdef LIVE_REC
#include "live-input.hpp"
#endif

#include "wave-output.hpp"

#include "silence-detector.hpp"
#include "option-parser.hpp"
#include "input-framer.hpp"
#include "LLDex.hpp"
#include "LLDs.hpp"
#include "deltas.hpp"
#include "functionals.hpp"

#include "SVMpredict.hpp"
#include "AMQoutput.hpp"

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
	cSvmPredict * svmPredI;
	
	#ifdef DEBUG_SILDET
	cWaveOutput *waveOut;
	cPcmBuffer * frame2;
	#endif

	cWaveOutput *pipeOut;
	cSilenceDetector *sildet;
	cPcmBuffer * frame1;
	cPcmBuffer * framePipe;

	cFtIdentifier *meanE;
	cFtIdentifier *meanF0; 
	cFtIdentifier *meanF0n; 
	cFtIdentifier *meanF0s;  
	cFtIdentifier *frameE;

	int  getFrame_flag;
	int framerId1, framerId2;
	int framerIdPipe;

	LONG_IDX turntime;
	int speaking;
};

} // namespace smile
} // namespace components
} // namespace semaine


#endif

