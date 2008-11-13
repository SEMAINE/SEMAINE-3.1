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
#include <vector>
#include "LipData.h"

#define WEIGHT_ULH	8
#define WEIGHT_LLH	38
#define WEIGHT_JAW	11
#define WEIGHT_LW	15
#define WEIGHT_ULP	70
#define WEIGHT_LLP	70
#define WEIGHT_CR	1

#include "FaceEngineGlobals.h"
#include "APMLCommunicativeAct.h"
#include "EngineParameter.h"

/**
*  Each object derived from this class contains data for the phonemes in the list 
*  of phonemes that the agent has to pronounce.
*  It contains the phoneme start time, its duration, its target with respect to the 
*  previous and the next phoneme, the relative time for the targets, the communicative act 
*  in which the phoneme appears
*  @author Elisabetta Bevacqua elisabetta.bevacqua@libero.it
*  @date 2008
*/

/**
*
* class :Phoneme
*
*/

class Phoneme
{
public:

	LipData *lipdata;

	std::string Pho;
	float StartTime;
	float Duration;
	float Time[NUM_PAR][3];
	float TargetLeft[NUM_PAR][3];
	float TargetRight[NUM_PAR][3];
	float Intensity;

	Phoneme *PhonemePrec;

	Phoneme *PhonemeSuc;

	bool Emphasis;

	std::string Emotion;
	std::string BlendWithEmotion;
	float BlendingCoeff;

	CommunicativeAct *comm_act;
	EngineParameter *SPC;

	Phoneme *next;
	int RhemeTheme;


	/**
	*
	* constructor
	*
	* @return 
	* @param  lipdata
	* @param  std::string pho
	* @param  float startTime
	* @param  float duration
	* @param comm_act
	*/

	Phoneme(LipData* lipdata, std::string pho, float startTime, float duration, CommunicativeAct *comm_act);

	/**
	*
	* contructor 
	*
	* @param  pho
	* @param  float startTime
	* @param  float Duration
	*/

	Phoneme::Phoneme(std::string pho, float startTime, float Duration);

	/**
	*
	* destructor 
	*/

	~Phoneme(void);

	/**
	*
	*
	*/

	void Phoneme::SetEmotion();

	/**
	*
	*
	* @param  rate
	*/

	void Phoneme::LoadVowelTargets(float rate);

	/**
	*
	*
	* @param  rate
	*/

	void Phoneme::LoadConsonantTargets(float rate);


	/**
	*
	*
	*/

	void Phoneme::LoadVowelTargetsMixEmotion();

	/**
	*
	*
	*/

	void Phoneme::LoadConsonantTargetsMixEmotion();


	/**
	*
	*
	* @param  filename
	*/

	void Phoneme::Print(std::string filename);

	/**
	*
	*
	* @return 
	* @param  vow
	*/

	std::string Phoneme::set_vowel(std::string vow);

	/**
	*
	*
	* @param  duration
	*/

	void Phoneme::SetNewDuration(float duration);


	/**
	*
	*
	* @return 
	* @param  inI
	*/

	int Phoneme::get_weight(int inI);
};

typedef std::vector<Phoneme> PhonemeVector;
