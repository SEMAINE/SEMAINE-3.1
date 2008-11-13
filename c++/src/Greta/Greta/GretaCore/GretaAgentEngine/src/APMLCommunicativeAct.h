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

// CommunicativeAct.h: interface for the CommunicativeAct class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FaceEngineGlobals.h"
#include "FaceExpression.h"
#include "RandomGen.h"
//#include "belief.h"

#include <vector>

typedef enum CommunicativeActphasetype {caphase_notype=0,caphase_attack,caphase_decay,caphase_sustain,caphase_release};

struct CommunicativeActphase
{
	CommunicativeActphasetype type;
	float duration;
};

class CommunicativeAct  
{
public:
	CommunicativeAct();
	virtual ~CommunicativeAct();
	//! Communicative function (affective, performative, etc etc)
	char *function; 
	//! Value of communication function (example: for affective 'joy') or text string if 'function' is equal to 'text'.
	char *value; 
	//! flag to indicate if the communicative act is inside a 'rheme' or 'theme' part
	int rheme_or_theme;
	//! Start time of the communicative act
	float start_time;
	//! Duration time of the communicative act
	float dur_time;
	//! Probability of the communicative act
	float prob; 
	//! Next communicative act
	void* next; 
	//! Previous communicative act
	void* prev;
	//! Name of an object if 'function' is 'deictic'
	char* obj; 
	//! Phonemes file (only if function is 'text')
	std::string phonemes_file;
	//! Phonemes list (only if function is 'text')
	phoneme *phonemes_list;	
	//! Text list (only if function is 'text')
    text_l *text_list;      
	//! List of communicative acts which conflict with current communicative act
	void* *conflict_comm_act_list; 
	//! Active facial channels used for the communicative act 
	channel active_channels;
	//! Communicative act type: head, face, gaze gesture
	std::string type;

	FaceExpression *fe;

	//!Expressivity Parameters
	float expressivitySPC;
	float expressivityTMP;
	float expressivityFLD;
	float expressivityPWR;
	float expressivityREP;

	int head_m_type;
	float head_m_amplitude;
	float head_m_period;

	std::vector<float> strokes;

	//EXPR_TIMINGS A D S R
	std::vector<CommunicativeActphase> phases;

	//activation of a communicative act    ---   Elisabetta 01/2006
	float activation;

	CommunicativeActphase *GetPhase(CommunicativeActphasetype type);
	
	float GetPhaseDur(CommunicativeActphasetype type);

	void AddPhase(CommunicativeActphasetype type,float duration);

	void SetStrokes(std::vector<float> strokevector);

	void Print();


	void		SetExpressivitySPC(float SPC);
	void		SetExpressivityTMP(float TMP);
	void		SetExpressivityFLD(float FLD);
	void		SetExpressivityPWR(float PWR);
	void		SetExpressivityREP(float REP);

	int **face_expr_faps;

	private :
	//intnsity of a communicative act - Radek
	
	float  intensity ;

public:

	float getIntensity();
	void setIntensity(float intensity1);
};
