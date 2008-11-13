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

// LipData.h: interface for the LipData class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include "IniManager.h"

#define NUM_PAR 12
#define NUM_VOW 13
#define TONGUE_PAR 5 
#define LIP_PAR 7
#define NUM_EMOT 7

/**
* class :Vowel
*
*/

class Vowel
{
public:
	std::string vowel;
	std::string close_consonant;
	std::string emotion;
	float duration;
	float time[NUM_PAR][3];
	float target[NUM_PAR][3];
};

/**
* class :Consonant
*
*/

class Consonant
{
public:
	std::string consonant;
	std::string close_vowel;
	std::string emotion;
	float duration;
	float time[NUM_PAR];
	float target[NUM_PAR];
};

/**
* class :Tongue
*
*/

class Tongue
{
public:
	std::string phoneme;
	float target[TONGUE_PAR][2];
};

/**
* class :MixEmotion
*
*/

class MixEmotion
{
public:
	std::string emotion;
	float tension;
	float artic;
	int AsDestra;
	int AsSinistra;
	float EmotPar[NUM_EMOT][LIP_PAR];
};

/**
* class :LipData
*
*/

class LipData  
{

private:
	std::vector<Vowel> Vowels;
	std::vector<Consonant> Consonants;
	std::vector<Tongue> Tongues;
	std::vector<MixEmotion> MixEmotions;
	

public:


/**
* contructor 
*
*/

	LipData();

/**
* destructor 
*/

	virtual ~LipData();

/**
 *  
 * 
 *
* @return 
*/

	int LipData::ReadPhonemes();

/**
 *  
 * 
 *
* @return 
*/

	int LipData::LoadVowelTarget();

/**
 *  
 * 
 *
* @return 
*/

	int LipData::LoadConsonantTarget();

/**
 *  
 * 
 *
* @return 
*/

	int LipData::LoadTongueTarget();

/**
 *  
 * 
 *
* @return 
*/

	int LipData::LoadRealDataEmotion();

/**
 *  
 * 
 *
* @return 
* @param  vowel
* @param  std::string close_consonant
* @param  std::string emotion
*/

	std::vector<Vowel>::iterator LipData::FindVowel(std::string vowel, std::string close_consonant, std::string emotion);

/**
 *  
 * 
 *
* @return 
* @param  consonant
* @param  std::string close_vowel
* @param  std::string emotion
*/

	std::vector<Consonant>::iterator LipData::FindConsonant(std::string consonant, std::string close_vowel, std::string emotion);

/**
 *  
 * 
 *
* @return 
* @param  phoneme
*/

	std::vector<Tongue>::iterator LipData::FindTongue(std::string phoneme);

/**
 *  
 * 
 *
* @return 
* @param  emotion
*/

	std::vector<MixEmotion>::iterator LipData::FindEmotion(std::string emotion);
	

/**
 *  
 * 
 *
* @return 
* @param  x1
* @param  float y1
* @param  float x2
* @param  float y2
* @param  float x
*/

	float LipData::direction(float x1, float y1, float x2, float y2, float x);




/**
 *  
 * 
 *
*/

	void LipData::PrintVowels();

/**
 *  
 * 
 *
*/

	void LipData::PrintConsonants();

/**
 *  
 * 
 *
*/

	void LipData::PrintTongue();

/**
 *  
 * 
 *
*/

	void LipData::PrintEmotion();
};
