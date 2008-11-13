//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// TorsoGesturePhase.cpp: implementation of the  TorsoGesturePhase class.
//
//////////////////////////////////////////////////////////////////////

#include ".\torsogesturephase.h"
#include "BAPconverter.h"

extern BAPConverter TorsoGlobalBAPconverter;

/**@#-*/
using namespace TorsoSpace;
/**@#+*/

TorsoGesturePhase::TorsoGesturePhase(void)
{
	Tmin=0.5f;
	Tmax=1.0f;
	Tdefault=1.0f;
	Tduration=0.0f;
	concretized=false;
}

TorsoGesturePhase::TorsoGesturePhase(const TorsoGesturePhase &rhs)
{
	this->Tdefault=rhs.Tdefault;
	this->Tduration=rhs.Tduration;
	this->Tfixed=rhs.Tfixed;
	this->Tmax=rhs.Tmax;
	this->Tmin=rhs.Tmin;
	this->begin=rhs.begin;
	this->end=rhs.end;

	this->concretized=rhs.concretized;

	this->trajectory=rhs.trajectory;
	this->type=rhs.type;
	//this->stance=rhs.stance;

	if(rhs.points.empty()==false)
	{
		std::vector<TorsoMovementPoint>::const_iterator iter;

		for(iter=rhs.points.begin();iter!=rhs.points.end();iter++)
		{
			TorsoMovementPoint tmp((*iter));
			this->points.push_back(tmp);
		}
	}
}

TorsoGesturePhase::~TorsoGesturePhase(void)
{
	points.clear();
}

void TorsoGesturePhase::Temporize(float begin,float end)
{
	float count;
	this->begin=begin;
	this->end=end;
	this->Tduration=end-begin;
	std::vector<TorsoMovementPoint>::iterator iter;

	if(this->type==preparation)
	{
		this->points[0].time=begin;
		iter=this->points.begin()+1;
		count=2;
	}
	else
	{
		iter=this->points.begin();
		count=1;
	}

	for(;iter!=this->points.end();iter++)
	{
		(*iter).time=begin+count*((Tduration)/points.size());
		count ++;
	}
	concretized=true;
}

void TorsoGesturePhase::CollectKeyframes(std::vector<BAPFrame> &keyframes,float fps,float scalefactor,float continuity,float tension)
{
	int i;

	float scale;

	std::vector<TorsoMovementPoint>::iterator iter;
	for(iter=this->points.begin();iter!=this->points.end();iter++)
	{
		if((*iter).spatial_fixed)
			scale=1;
		else
			scale=scalefactor;

		BAPFrame *bf=new BAPFrame(&TorsoGlobalBAPconverter);
		bf->CopyAngles((*iter).baps,scale);
		bf->SetFrameNumber((*iter).time*fps);

		if((*iter).SpineRotation==r_right)
		{
			for(i=168;i>116;i=i-3)
				bf->SetBAP(i,-3000*scale);
		}

		if((*iter).SpineRotation==r_left)
		{
			for(i=168;i>116;i=i-3)
				bf->SetBAP(i,3000*scale);
		}

		if(((*iter).compensateshoulders==false)&&((*iter).compensatehead==false))
		{
			bf->SetBAP("vc7.rotateX",0);
			bf->SetBAP("vc2.rotateX",0);
		}
		if(((*iter).compensateshoulders==true)&&((*iter).compensatehead==false))
		{
			bf->SetBAP("vc6.rotateX",-bf->GetBAP("vc7.rotateX"));
		}
		if(((*iter).compensateshoulders==false)&&((*iter).compensatehead==true))
		{
			bf->SetBAP("vc6.rotateX",bf->GetBAP("vc7.rotateX"));
			bf->SetBAP("vc7.rotateX",0);
		}
		if(((*iter).compensateshoulders==true)&&((*iter).compensatehead==true))
		{
		}

		bf->SetContinuity(continuity);
		bf->SetTension(tension);

		keyframes.push_back(*bf);
	}
}


TorsoGesturePhaseType TorsoGesturePhase::GetPhaseId(std::string phasename)
{
	if(phasename=="all_lenght")
		return all_lenght;
	if(phasename=="preparation")
		return preparation;
	if(phasename=="stroke")
		return stroke;
	if(phasename=="hold")
		return hold;
	if(phasename=="retraction")
		return retraction;
	return unknown_phase;
}

std::string TorsoGesturePhase::GetPhaseName(TorsoGesturePhaseType type_id)
{
	if(type_id==all_lenght)
		return "all_lenght";
	if(type_id==preparation)
		return "preparation";
	if(type_id==stroke)
		return "stroke";
	if(type_id==hold)
		return "hold";
	if(type_id==retraction)
		return "retraction";
	return "unknown_phase";
}

std::string TorsoGesturePhase::GetPhaseName()
{
	if(this->type==all_lenght)
		return "all_lenght";
	if(this->type==preparation)
		return "preparation";
	if(this->type==stroke)
		return "stroke";
	if(this->type==hold)
		return "hold";
	if(this->type==retraction)
		return "retraction";
	return "unknown_phase";
}

TorsoTrajectoryType TorsoGesturePhase::GetTrajectoryId(std::string trajectoryname)
{
	if(trajectoryname=="linear")
		return linear;
	if(trajectoryname=="curved")
		return curved;
	if(trajectoryname=="circular")
		return circular;
	return linear;
}