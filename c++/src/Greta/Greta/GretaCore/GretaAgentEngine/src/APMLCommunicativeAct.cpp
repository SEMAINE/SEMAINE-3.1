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

// CommunicativeAct.cpp: implementation of the CommunicativeAct class.
//
//////////////////////////////////////////////////////////////////////

#include "APMLCommunicativeAct.h"

extern RandomGen *randomgen;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CommunicativeAct::CommunicativeAct()
{
	/*
	function=(char*)malloc(100*sizeof(char));
	strcpy(function,"");
	value=(char*)malloc(100*sizeof(char));
	strcpy(value,"");
	*/
	
	function=0;
	value=0;
	rheme_or_theme=0;
	next=0;
	prev=0;
	phonemes_list=0;
	head_m_type=0;
	head_m_amplitude=0;
	head_m_period=0;
	text_list=0;
	conflict_comm_act_list=0;
	obj=0;
	start_time=0.0;
	dur_time=0.0;
	prob=randomgen->GetRand01();
	activation=1;
	phases.clear();
	face_expr_faps=0;
	type="";
	fe=0;
	
	intensity=1;
}

CommunicativeAct::~CommunicativeAct()
{
	if(face_expr_faps!=0)
	{
		for(int i=0;i<2;i++)
		{
			//for(int j=0;j<69;j++)
			//	free(face_expr_faps[i][j]);
			free(face_expr_faps[i]);
		}
		free(face_expr_faps);
	}
}

CommunicativeActphase* CommunicativeAct::GetPhase(CommunicativeActphasetype type)
{
	std::vector<CommunicativeActphase>::iterator iter;
	iter=phases.begin();
	while(iter!=phases.end())
	{
		if((*iter).type==type)
			return &(*iter);
		iter++;
	}
	return 0;
}

float CommunicativeAct::GetPhaseDur(CommunicativeActphasetype type)
{
	std::vector<CommunicativeActphase>::iterator iter;
	iter=phases.begin();
	while(iter!=phases.end())
	{
		if((*iter).type==type)
			return (*iter).duration;
		iter++;
	}
	return 0;
}

void CommunicativeAct::AddPhase(CommunicativeActphasetype type,float duration)
{
	CommunicativeActphase *phase;
	phase=new CommunicativeActphase;
	phase->type=type;
	phase->duration=duration;
	phases.push_back(*phase);
}

void CommunicativeAct::SetStrokes(std::vector<float> strokevector)
{
	this->strokes=strokevector;
}

void CommunicativeAct::Print()
{
	printf("%s=%s from %.2f to %.2f; prob=%.2f; faps=%d\n",this->function,this->value,this->start_time,this->start_time+this->dur_time,this->prob,this->face_expr_faps);
}


//////////////////////////////////////////////////////////////////////
void CommunicativeAct::SetExpressivitySPC(float SPC)
{
	this->expressivitySPC=SPC;
}
//////////////////////////////////////////////////////////////////////
void CommunicativeAct::SetExpressivityTMP(float TMP)
{
	this->expressivityTMP=TMP;
}
//////////////////////////////////////////////////////////////////////
void CommunicativeAct::SetExpressivityFLD(float FLD)
{
	this->expressivityFLD=FLD;
}
//////////////////////////////////////////////////////////////////////
void CommunicativeAct::SetExpressivityPWR(float PWR)
{
	this->expressivityPWR=PWR;
}
//////////////////////////////////////////////////////////////////////
void CommunicativeAct::SetExpressivityREP(float REP)
{
	this->expressivityREP=REP;
}

float CommunicativeAct::getIntensity()
{
	return intensity;
}
void CommunicativeAct::setIntensity(float intensity1)
{
	this->intensity=intensity1;
}