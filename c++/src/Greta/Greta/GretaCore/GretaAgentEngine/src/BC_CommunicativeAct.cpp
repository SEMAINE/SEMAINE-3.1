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

// BC_CommunicativeAct.cpp: implementation of the BC_CommunicativeAct class.
//
//////////////////////////////////////////////////////////////////////

#include "BC_CommunicativeAct.h"

extern RandomGen *randomgen;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BC_CommunicativeAct::BC_CommunicativeAct()
{
	/*
	function=(char*)malloc(100*sizeof(char));
	strcpy(function,"");
	value=(char*)malloc(100*sizeof(char));
	strcpy(value,"");
	*/
	function=0;
	value=0;
	next=0;
	prev=0;
	prev_comm_act=0;
	/*phonemes_list=0;
	text_list=0;
	conflict_comm_act_list=0;
	obj=0;*/
	start_time=0.0;
	dur_time=0.0;
	prob=randomgen->GetRand01();
	activation=1;
	//phases.clear();
	face_expr_faps=0;
}

BC_CommunicativeAct::~BC_CommunicativeAct()
{

}

/*
BC_CommunicativeActphase* BC_CommunicativeAct::GetPhase(BC_CommunicativeActphasetype type)
{
	std::vector<BC_CommunicativeActphase>::iterator iter;
	iter=phases.begin();
	while(iter!=phases.end())
	{
		if((*iter).type==type)
			return &(*iter);
		iter++;
	}
	return 0;
}

float BC_CommunicativeAct::GetPhaseDur(BC_CommunicativeActphasetype type)
{
	std::vector<BC_CommunicativeActphase>::iterator iter;
	iter=phases.begin();
	while(iter!=phases.end())
	{
		if((*iter).type==type)
			return (*iter).duration;
		iter++;
	}
	return 0;
}

void BC_CommunicativeAct::AddPhase(BC_CommunicativeActphasetype type,float duration)
{
	BC_CommunicativeActphase *phase;
	phase=new BC_CommunicativeActphase;
	phase->type=type;
	phase->duration=duration;
	phases.push_back(*phase);
}
*/

void BC_CommunicativeAct::Print()
{
	printf("%s=%s from %.2f to %.2f; prob=%.2f\n"/*; faps=%d\n"*/,this->function,this->value,this->start_time,this->start_time+this->dur_time,this->prob/*,this->face_expr_faps*/);
}