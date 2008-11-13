//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#pragma once

#include <string>
#include <list>
#include <map>
#include <vector>
#include "Signal.h"
#include "EngineParameterSetOfSets.h"
#include "CommunicativeIntention.h"
#include "mmemotions/Constraint.h"
#include "mmemotions/MultimodalEmotionConstraint.h"

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  It represents a multimodal signal, that is, a vector of signals.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class MultimodalSignal : public std::vector<Signal>
{
public:


	/** 
    * Class contructor.
    * 
    */
	MultimodalSignal(void);
	/** 
    * Copy contructor.
    * 
    */
	MultimodalSignal(const MultimodalSignal &rhs);
	/** 
    * Class destructor.
    * 
    */
	~MultimodalSignal(void);
	/** 
    * This function returns true if the multimodal signal is produced
	* on the given modality, otherwise it returns false.
    * 
    */
	bool UseModality(std::string modalityname);
	/** 
    * Using the agent's Dynamicline and the communicative intention
	* importance in input, this function decides if the multimodal
	* signal can be used or not. It basically compares the intention
	* importance with the OAC parameters of the modalities on which
	* the multimodal signal is produced.
    * 
    */
	bool IsActivable(Dynamicline *dl,float importance);
	/** 
    * It returns the preference of the multimodal signal. It uses the
	* agent's Dynamicline given as input.
    * 
    */
	float ComputePreference(Dynamicline *dl);
	/** 
    * This is the list of the names of the modalities on which the
	* multimodal signal is produced.
    * 
    */
	std::list<std::string> UsedModalities();
	/** 
    * It assigns the starting and ending time to the signals
	* composing the multimodal signal. No stroke is given as input
	* so it is calculated as the average between start and end.
    * 
    */
	void Temporize(float start,float duration);
	/** 
    * It assigns the starting, stroke and ending time to the signals
	* composing the multimodal signal.
    * 
    */
	void Temporize(float start,float stroke,float duration);
	/** 
    * It assigns the starting, strokes and ending time to the signals
	* composing the multimodal signal.
    * 
    */
	void Temporize(float start,std::vector<float> strokes,float duration);
	/** 
    * (not used) It computes the multimodal signal stroke timing starting from the given
	* communicative intention. It looks for the first pitch accent co-occurring with
	* the given communicative intention and transfer its timing to the multimodal
	* signal stroke timing.
    * 
    */
	void AssignStrokesFromCommAct(MMSystemSpace::CommunicativeIntention *ca);
	/** 
    * Depending on the importance of the communicative intention and
	* on the vector of the vocal stress given as input it computes
	* the strokes timings of the multimodal signal.
    * 
    */
	void AssignStrokesFromStressVector(MMSystemSpace::CommunicativeIntention *ca,std::vector<float> *stressvector);
	/** 
    * It is used to assign the same id (the name parameter) to all the signals
	* composing the multimodal signal.
    * 
    */
	void AssignNameToSignals(std::string name);
	/** 
    * It copies the parameters (expressivity and other) contained in the
	* agent's Dynamicline into the parameters of the signals composing
	* the multimodal signal.
    *
    */
	void AssignParameters(Baseline *bl);
	/** 
    * It transforms the multimodal signal into the corresponding BML code.
	* If present, it attaches the id parameter giveng as input to the ids
	* of the signals.
    * 
    */
	std::string GetBML(int *uid);
	/** 
    * It transforms the multimodal signal into csv values and saves them to a file.
    *
    */
	void SaveCSV(float time,FILE *preferencefile,FILE *gesturefile,FILE *torsofile,FILE *headfile,FILE *facefile,FILE *gazefile);
	/** 
    * It shuffles the signals which have alternative shapes with probabilities.
    * 
    */
	void ShuffleShapes();
	/** 
    * This is the preference of the multimodal signal.
	*
    */
	float preference;
	
	//added by radek
	/** 
    * adds intensity to signals.
	*
    */
	void AddIntensity(float intensity1);
	
	//added by radek
	/** 
    * consider time constraints for multimodal expressions of emotions
	*
    */
	void addTimeConstraints(std::string emotion);

	MultimodalSignal clone(); 

	bool checkStartCons(std::string reference, float begin_time, MultimodalEmotionConstraint *mec);
	bool checkStopCons(std::string reference, float  begin_time, float end_time, MultimodalEmotionConstraint *mec);

	private:

		std::string findName(std::string reference);

};

/**@#-*/
}
/**@#+*/