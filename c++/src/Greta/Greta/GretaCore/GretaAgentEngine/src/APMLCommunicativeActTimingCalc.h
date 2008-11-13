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

// CommunicativeActTimingCalc.h: interface for the CommunicativeActTimingCalc class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include <libxml/tree.h>
#include "FaceEngineGlobals.h"
#include "EngineParameter.h"
#include "APMLCommunicativeAct.h"
#include "Phoneme.h"
#include <string>
#include <iostream>   
#include <stdio.h>
#include <fstream>
#include <math.h>

/**
*
* class :CommunicativeActTimingCalc
*
*/

class CommunicativeActTimingCalc  
{
public:

	/**
	* contructor 
	*
	* @param e
	*/
	CommunicativeActTimingCalc(void *e);

	/**
	* destructor 
	*/
	virtual ~CommunicativeActTimingCalc();

	/**
	* this method 
	* 
	*
	* @param  first_comm_act
	* @param  &animation_time
	* @param  int &number_of_comm_act
	*/
	void CommunicativeActTimingCalc::CalculateTimings(CommunicativeAct* first_comm_act,float &animation_time, int &number_of_comm_act);
	
	//void CommunicativeActTimingCalc::CalcOnsetApexOffset(float *onset, float *apex, float *offset, float time);

	/**
	* this method 
	* 
	*
	* @param comm_act
	* @param attack
	* @param decay
	* @param sustain
	* @param release
	*/
	void CommunicativeActTimingCalc::CalcAttackDecaySustainRelease(CommunicativeAct *comm_act,float *attack, float *decay, float *sustain, float *release);
	
	//void CommunicativeActTimingCalc::ModifyTime(CommunicativeAct *expr);
	//int CommunicativeActTimingCalc::ModifyTimePho(std::string nameFile, float factor);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  nameFile
	*/
	int CommunicativeActTimingCalc::CopyFilePho(std::string nameFile);

	//int CommunicativeActTimingCalc::ModifyTimeFilePho(char nameFile[1024], float startTime, float duration, float factor);
	
	phoneme *all_phonemes_list;
	PhonemeVector PhoVector;

private:

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	*/
	int CommunicativeActTimingCalc::CalcTextDur(CommunicativeAct *comm_act);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  first_comm_act
	*/
	int CommunicativeActTimingCalc::FixTextDurPho(CommunicativeAct* first_comm_act);

	/**
	* this method 
	* 
	*
	* @param comm_act
	*/
	void CommunicativeActTimingCalc::CalcCommActDur(CommunicativeAct *comm_act);

	/**
	* this method 
	* 
	*
	* @param comm_act
	*/
	void CommunicativeActTimingCalc::CalcBoundaryTiming(CommunicativeAct *comm_act);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	*/
	CommunicativeAct *CommunicativeActTimingCalc::FindConsecutiveExpr(CommunicativeAct *comm_act);

	//void CommunicativeActTimingCalc::CalcOnsetApexOffset2(CommunicativeAct *expr,float *onset, float *apex, float *offset);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	*/
	CommunicativeAct *CommunicativeActTimingCalc::FindEmphasisOverTheme(CommunicativeAct *comm_act);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	*/
	phoneme *CommunicativeActTimingCalc::FirstPhoneme(CommunicativeAct *comm_act);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	*/
	phoneme *CommunicativeActTimingCalc::LastPhoneme(CommunicativeAct *comm_act);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	* @param pho
	*/
	phoneme *CommunicativeActTimingCalc::NextPhoneme(CommunicativeAct *comm_act,phoneme *pho);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	* @param pho
	*/
	phoneme *CommunicativeActTimingCalc::PrevPhoneme(CommunicativeAct *comm_act,phoneme *pho);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	*/
	char *CommunicativeActTimingCalc::FirstText(CommunicativeAct *comm_act);

	/**
	* this method 
	* 
	*
	* @return 
	* @param comm_act
	*/
	char *CommunicativeActTimingCalc::LastText(CommunicativeAct *comm_act);


	void *engine;
	CommunicativeAct* first_text_act;
	CommunicativeAct* last_text_act;
	int number_of_comm_act;

	EngineParameter *TMP;

};
