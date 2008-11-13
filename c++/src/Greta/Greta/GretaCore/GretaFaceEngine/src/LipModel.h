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

// LipModel.h: interface for the LipModel class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4786)

#include "XMLGenericParser.h"
#include "APMLCommunicativeAct.h"
#include "FaceExpression.h"
#include "FaceExpressionDictionary.h"
#include "FAPFrame.h"
#include <string>
#include <vector>
#include <map>
#include "Phoneme.h"
#include "LipData.h"

#include "DataViewerWindow.h"

#include "FaceEngineGlobals.h"
#include "EngineParameter.h"

//Intensità dell'influenza delle vocali sulle consonanti
#define STRONG 4.5
#define MILD 3.5
#define WEAK 2.5


typedef struct PhoData 
{
	char phoneme[5];
	float time;
} PhoData;

/**
*
* class :FAPPhoneme
*
*/

class FAPPhoneme
{
public:
	std::string phoneme;
	float time[3][64];
	float target[3][64];
};

/**
* class :LipModel
*
*  It computes lip movement starting from the text the agent has to say
 *  @author Elisabetta Bevacqua elisabetta.bevacqua@libero.it
 *  @date 2008
 */


class LipModel  
{

public:

/**
 *
 *
* @return 
* @param fapvector
* @param first_comm_act
*/

	LipModel(std::vector<FAPFrame> *fapvector,CommunicativeAct *first_comm_act);

/**
*
* destructor 
*/

	virtual ~LipModel();
	
	std::vector<FAPPhoneme> FAPPhonemes;

	//FAPFrameVector Frames;
	//PhonemeVector *PhonVector;
	XMLGenericTree *ExpressionTree;
	FaceExpressionDictionary *expressionsdictionary;

	std::vector<Phoneme> Phonemes;

	LipData *lipdata;

/**
 *
 *
* @return 
* @param  phonemefilename
* @param  speech_delay=0.0f
*/

	int LipModel::CalculateLips(std::string phonemefilename,float speech_delay=0.0f);

private:

	EngineParameter *SPC;

/**
 *
 *
* @param  iterphonemes
* @param  std::vector<Phoneme>::iterator begin
* @param  std::vector<Phoneme>::iterator end
*/

	void LipModel::Vowel_Coarticulation(std::vector<Phoneme>::iterator iterphonemes, std::vector<Phoneme>::iterator begin, std::vector<Phoneme>::iterator end);

/**
 *
 *
* @param  iterphonemes
*/

	void LipModel::Consonant_Coarticulation(std::vector<Phoneme>::iterator iterphonemes);

/**
 *
 *
* @param  inPho
* @param  std::vector<Phoneme>::iterator end
*/

	void LipModel::Cons_Cons_Coarticulation(std::vector<Phoneme>::iterator inPho, std::vector<Phoneme>::iterator end);
	

/**
 *
 *
* @param  inPho
* @param  std::vector<Phoneme>::iterator end
*/

	void LipModel::Lip2Fap(std::vector<Phoneme>::iterator inPho, std::vector<Phoneme>::iterator end);

/**
 *
 *
* @return 
*/

	int LipModel::InterpolateLipFap();
	
/**
 *
 *
* @return 
* @param  filePho
*/

	int LipModel::Conversion(std::string filePho);

/**
 *
 *
* @return 
* @param  pho
*/

	std::string LipModel::ConvertIta2Ita(std::string pho);

/**
 *
 *
* @param  pho
*/

	void LipModel::ConvertEng2Ita(char* pho);

/**
 *
 *
* @return 
* @param  pho
*/

	int LipModel::ConvertFra2Ita(char* pho);

/**
 *
 *
* @return 
* @param  pho
*/

	int LipModel::ConvertGer2Ita(char* pho);

/**
 *
 *
* @return 
* @param  pho
*/

	int LipModel::ConvertPol2Ita(char* pho);

/**
 *
 *
* @return 
* @param  pho
*/

	int LipModel::ConvertSwe2Ita(char* pho);


/**
 *
 *
* @return 
*/

	float LipModel::ReadPhonemes();

/**
 *
 *
* @return 
* @param  istant = 0.0
*/

	int LipModel::ComputeLipMovemet(float istant = 0.0);

/**
 *
 *
* @return 
* @param  istant = 0.0
*/

	int LipModel::StoreLipFaps(float istant = 0.0);
	
	
	//auxaliary functions

/**
 *
 *
* @return 
* @param  vow
*/

	std::string LipModel::set_vowel(std::string vow);

/**
 *
 *
* @return 
* @param  inSeq
*/

	int LipModel::is_vowel(std::string inSeq);

/**
 *
 *
* @return 
* @param  inSeq
*/

	int LipModel::is_labiodental(std::string inSeq);

/**
 *
 *
* @return 
* @param  b
* @param  float x
*/

	float LipModel::A_coart(float b, float x);

/**
 *
 *
* @return 
* @param  b
* @param  float x
*/

	float LipModel::CO_coart(float b, float x);

/**
 *
 *
* @return 
* @param  inSeq
*/

	int LipModel::is_bilabial(std::string inSeq);

/**
 *
 *
* @return 
*/

	float LipModel::CalculateRate();


/**
 *
 *
*/

	void LipModel::FreeAll();

	float animationtime;

	TCBInterpolator *KeyFrameInt[64];


	CommunicativeAct *first_comm_act;
	std::vector<FAPFrame> *fapvector;

	std::vector<PhoData> PhonemeVec;
	
	
	//parametri per cui si richiede la curva
	int par_selected[8];
	
	char current_performative[256];
	char current_affect[256];
	int index_UE;
	char unknown_emotion[50][256];

	XMLGenericTree *consonantTree;
	XMLGenericParser *phonemeParser;

	float ENGINE_ARTICULATION_OAC;
	float ENGINE_INTENSIVITY_SPC;
	float ENGINE_TENSION_PWR;
	std::string ENGINE_LIP_FLOW;
	std::string ENGINE_LIP_PARAMETERS;
	std::string FESTIVAL_LANGUAGE;
	std::string ENGINE_LIP_WEIGHT;
	std::string LIP_EMOTION_MODEL;
	std::string FESTIVAL;
	std::string EULER;
	std::string EULER_LANGUAGE;
	std::string MARY;
	std::string MARY_LANGUAGE;

	bool isLipFap[69];


	//*********************************************************************************************************
//							FUNZIONI PER LA CONVERSIONE DEI FONEMI NEL FILE .PHO
//*********************************************************************************************************
#define NUM_ENG2ITA 100
#define NUM_FRA2ITA 50
#define NUM_ITA2ITA 50
#define NUM_DE2IT 55
#define NUM_POL2ITA 50
#define NUM_SWE2ITA 50

	char *eng2ita[NUM_ENG2ITA][2];
	char *fra2ita[NUM_FRA2ITA][2];
	char *ita2ita[NUM_ITA2ITA][2];
	char *de2it[NUM_DE2IT][2];
	char *pol2ita[NUM_POL2ITA][2];
	char *swe2ita[NUM_POL2ITA][2];
};
