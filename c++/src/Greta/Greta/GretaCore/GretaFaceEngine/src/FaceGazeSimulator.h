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

// FaceGazeSimulator.h: interface for the FaceGazeSimulator class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4786)

#include "FaceEngineGlobals.h"
#include "APMLCommunicativeAct.h"
#include "FAPFrame.h"
//#include "FaceEngine.h"
#include <vector>

//!
//! Structure used for selecting a possible candidate for the gaze state
//!

typedef struct 
{
	//! - 'id' is the state (from 0 to 3 associated, respectevly, to the gaze states (0,0), (0,1), (1,0), (1,1)
	int id;
	//! - 'p' is the probability of 'id'
	double p;
} 
gaze_candidate;


struct gazestate
{
	int state;
	float duration;
	float pitch;
	float roll;
};

/**
*
* class :FaceGazeSimulator
*
*/

class FaceGazeSimulator  
{
public:

	/**
	*
	* contructor 
	*
	* @param  e
	* @param  a
	*/

	FaceGazeSimulator(void* e,void* a);

	/**
	*
	* destructor 
	*/

	virtual ~FaceGazeSimulator();

	/**
	* this method 
	* 
	*
	* @return 
	* @param phonemes
	*/

	int InitGazeSimulation(phoneme *phonemes);

	/**
	* this method 
	* 
	*
	* @return 
	* @param phonemes
	*/

	int InitGazeSimulationBML(phoneme *phonemes);

	/**
	* this method 
	* 
	*
	* @return 
	* @param 
	* @param  animationlength
	*/

	int InitGazeSimulationBML(CommunicativeAct *,float animationlength);

	/**
	* this method 
	* 
	*
	* @param phonemes
	*/

	void SimulGaze(phoneme *phonemes);

	/**
	* this method 
	* 
	*
	*/

	void SimulGazeBML();

	unsigned char* speaker_state;
	unsigned char* listener_state;

	unsigned char *first_speaker_state;
	unsigned char *first_listener_state;

	int num_of_transitions;


	/**
	* this method 
	* 
	*
	* @param phonemes
	*/

	void GazeGeneratorSpeaker(phoneme *phonemes);

	/**
	* this method 
	* 
	*
	*/

	void GazeGeneratorSpeakerBML();

	/**
	* this method 
	* 
	*
	* @return 
	* @param phonemes
	* @param fapvector
	* @param faps_head_expr
	* @param fapmask_head_expr
	* @param faps_head_ns
	* @param fapmask_head_ns
	* @param  int FramesTotalNumber
	*/

	int CheckHeadPosition(phoneme *phonemes,std::vector<FAPFrame> *fapvector,int **faps_head_expr, int **fapmask_head_expr,int **faps_head_ns, int **fapmask_head_ns, int FramesTotalNumber);

	/**
	* this method 
	* 
	*
	* @return 
	* @param fapvector
	* @param faps_head_expr
	* @param fapmask_head_expr
	* @param faps_head_ns
	* @param fapmask_head_ns
	* @param  int FramesTotalNumber
	*/

	int CheckHeadPositionBML(std::vector<FAPFrame> *fapvector,int **faps_head_expr, int **fapmask_head_expr,int **faps_head_ns, int **fapmask_head_ns, int FramesTotalNumber);

	/**
	* this method 
	* 
	*
	* @param phonemes
	*/

	void GazeGeneratorListener(phoneme *phonemes);
	std::vector<gazestate> speakergaze,listenergaze;
	std::vector<float> interestlevel;
	std::vector<float> effectivenesslevel;

	bool breaked;
	float timebreaked;
	bool bml;	

	/**
	* this method 
	* 
	*
	* @return 
	* @param  prev_state_speaker
	* @param  prev_state_listener
	* @param  LIMIT
	* @param  SCALE
	*/

	int GetGazeState(int prev_state_speaker, int prev_state_listener, float LIMIT,float SCALE);

	/**
	* this method 
	* 
	*
	* @param speaker_or_listener
	* @param new_state
	*/

	void FaceGazeSimulator::SetState(unsigned char *speaker_or_listener,int new_state);

	/**
	* this method 
	* 
	*
	* @return 
	* @param speaker_or_listener
	*/

	int FaceGazeSimulator::GetState(unsigned char *speaker_or_listener);

	/**
	* this method 
	* 
	*
	* @param  n
	* @param  int state
	*/

	void FaceGazeSimulator::CreateStateTransitions(int n, int state);

	/**
	* this method 
	* 
	*
	* @param speaker_or_listener
	*/

	void FaceGazeSimulator::NextState(unsigned char **speaker_or_listener);


private:

	/**
	* this method 
	* 
	*
	* @return 
	* @param s
	*/

	CommunicativeAct *FindCommActFromText(char *s);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  phoneme_start
	*/

	CommunicativeAct *FindCommActFromPhoneme(float phoneme_start);


	void *engine;

	void *animationcalculator;
	float INTEREST_THRESHOLD;
	float EFFECTIVENESS_THRESHOLD;
	float gaze_resolution;
};
