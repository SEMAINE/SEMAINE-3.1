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

// BC_CommunicativeAct.h: interface for the BC_CommunicativeAct class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FaceEngineGlobals.h"
#include "RandomGen.h"
//#include "belief.h"

#include <vector>

/*typedef enum BC_CommunicativeActphasetype {caphase_notype=0,caphase_attack,caphase_decay,caphase_sustain,caphase_release};

struct BC_CommunicativeActphase
{
	BC_CommunicativeActphasetype type;
	float duration;
};*/

class BC_CommunicativeAct  
{
public:
	BC_CommunicativeAct();
	virtual ~BC_CommunicativeAct();
	//! BackChannel Communicative function (contact, perception... attitude)
	char *function; 
	//! Value of communication function (example: for contact 'yes' or 'no', for attitude 'agreement', 'disagreement'...)
	char *value; 
	//! Start time of the BackChannel communicative act
	float start_time;
	//! Duration time of the BackChannel communicative act
	float dur_time;
	//! Probability of the BackChannel communicative act
	float prob; 
	//! Next BackChannel communicative act
	void* next; 
	//! Previous BackChannel communicative act
	void* prev;
	//! Previous communicative act
	void* prev_comm_act;
	/*
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
	*/

	//EXPR_TIMINGS A D S R
//	std::vector<BC_CommunicativeActphase> phases;

	//activation of a communicative act    ---   Elisabetta 01/2006
	int activation;
/*
	BC_CommunicativeActphase *GetPhase(BC_CommunicativeActphasetype type);
	
	float GetPhaseDur(BC_CommunicativeActphasetype type);

	void AddPhase(BC_CommunicativeActphasetype type,float duration);
*/
	void Print();

	int **face_expr_faps;
	
};
