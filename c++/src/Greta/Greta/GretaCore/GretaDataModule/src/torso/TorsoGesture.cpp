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

// TorsoGesture.cpp: implementation of the  TorsoGesture class.
//
//////////////////////////////////////////////////////////////////////

#include ".\torsogesture.h"

/**@#-*/
using namespace TorsoSpace;
/**@#+*/

TorsoGesture::TorsoGesture()
{
//	prev=next=0;
	concretized=false;
}

TorsoGesture::TorsoGesture(std::string id)
{
	this->id=id;
//	prev=next=0;
	concretized=false;
}

TorsoGesture::TorsoGesture(std::string id, float start, float duration, std::vector<float> *strokes)
{
	this->id=id;
	this->start=start;
	this->duration=duration;
	this->strokes=*strokes;
//	prev=next=0;
	concretized=false;
}

TorsoGesture::TorsoGesture(std::string id, float start, float duration, float stroke)
{
	this->id=id;
	this->strokes.clear();
	this->strokes.push_back(stroke);
//	prev=next=0;
	concretized=false;
}

TorsoGesture::TorsoGesture(const TorsoGesture &rhs):MMSystemSpace::Signal((MMSystemSpace::Signal)rhs)
{
	if(rhs.phases.empty()==false)
	{
		std::vector<TorsoGesturePhase>::const_iterator iter2;
		for(iter2=rhs.phases.begin();iter2!=rhs.phases.end();iter2++)
		{
			TorsoGesturePhase tgp((*iter2));
			this->phases.push_back(tgp);
		}
	}		
//	prev=next=0;
	concretized=rhs.concretized;
}


TorsoGesture::TorsoGesture(const MMSystemSpace::Signal &signal,const TorsoGesture &gesture):MMSystemSpace::Signal(signal)
{
	if(gesture.phases.empty()==false)
	{
		std::vector<TorsoGesturePhase>::const_iterator iter;
		for(iter=gesture.phases.begin();iter!=gesture.phases.end();iter++)
		{
			TorsoGesturePhase tgp((*iter));
			this->phases.push_back(tgp);
		}
	}
//	prev=next=0;
	concretized=gesture.concretized;
}

TorsoGesture::TorsoGesture(const MMSystemSpace::Signal &rhs):MMSystemSpace::Signal(rhs)
{
//	prev=next=0;
}

TorsoGesture::~TorsoGesture(void)
{
	phases.clear();
}

int TorsoGesture::Temporize()
{
	float temporal;

	if(this->parameters.find("TMP.value")!=this->parameters.end())
	{
		this->parameters["TMP.value"].SetScaling(1.5,0.5);
		temporal=this->parameters["TMP.value"].GetScaledValue();
	}
	else
		temporal=1.0f;

	if(this->strokes.empty()==true)
	{
		printf("TorsoEngine::TorsoGesture Error: no stroke timing found for gesture %s\n",this->reference.c_str());
		return 0;
	}

	TorsoGesturePhase *phase;

	ScalePhases(stroke,temporal);
	ScalePhases(preparation,temporal);
	ScalePhases(retraction,temporal);

	phase=GetPhase(stroke);
	
	if(phase!=0)
	{
		float stroketime;
		stroketime=this->strokes[this->strokes.size()/2];
		phase->Temporize(this->start+stroketime-phase->Tduration,this->start+stroketime);
	}

	phase=GetPhase(preparation);
	
	if(phase!=0)
	{
		phase->Temporize(this->start,this->start+phase->Tduration);
	}

	phase=GetPhase(retraction);
	
	if(phase!=0)
	{
		phase->Temporize(this->start+this->duration-phase->Tduration,this->start+this->duration);
	}

	if(CheckTiming()==false)
	{
		printf("TorsoGesture: cannot concretize gesture %s, phases timing problem\n",this->reference.c_str());
		concretized=false;
	}
	else
		concretized=true;
	return 1;
}

void TorsoGesture::AddMissingPreparationRetraction(TorsoMovementPoint restpose,int fps)
{

	bool found;
	found=false;
	std::vector<TorsoGesturePhase>::iterator iter;
	for(iter=this->phases.begin();iter!=this->phases.end();iter++)
	{
		if((*iter).type==preparation)
		{
			found=true;
			break;
		}
	}
	if(found==false)
	{
		TorsoGesturePhase tp;
		tp.Tduration=0.2f;
		tp.type=preparation;
		tp.points.push_back(restpose);
		tp.points.push_back(restpose);
		this->phases.insert(this->phases.begin(),tp);
	}

	found=false;

	for(iter=this->phases.begin();iter!=this->phases.end();iter++)
	{
		if((*iter).type==retraction)
		{
			found=true;
			break;
		}
	}
	if(found==false)
	{
		TorsoGesturePhase tp;
		tp.Tduration=0.2f;
		tp.type=retraction;
		tp.points.push_back(restpose);
		this->phases.push_back(tp);
	}

}

void TorsoGesture::AddMissingHold(int fps)
{
	if(this->concretized==false)
		return;
	bool found;
	found=false;
	std::vector<TorsoGesturePhase>::iterator iter,prev;
	for(iter=(this->phases.begin())+1;iter!=this->phases.end();iter++)
	{
		prev=iter-1;
		if(((*iter).begin-(*prev).end)>0.2)
		{
			TorsoGesturePhase tp;
			tp.Tduration=(*iter).begin-(*prev).end;
			tp.type=hold;
			tp.points.push_back((*prev).points[(*prev).points.size()-1]);
			tp.points.push_back((*prev).points[(*prev).points.size()-1]);
			tp.Temporize((*prev).end,(*iter).begin);
			this->phases.insert(iter,tp);
			iter=(this->phases.begin());
		}

	}
}


void TorsoGesture::CollectKeyframes(std::vector<BAPFrame> &keyframes,float fps)
{
	float scale,continuity,tension;

	if(this->concretized==false)
		return;

	if(this->parameters.find("SPC.value")!=this->parameters.end())
	{
		this->parameters["SPC.value"].SetScaling(0,1.0);
		scale=this->parameters["SPC.value"].GetScaledValue();
	}
	else
		scale=1.0f;

	if(this->parameters.find("FLD.value")!=this->parameters.end())
	{
		this->parameters["FLD.value"].SetScaling(-1,0);
		continuity=this->parameters["FLD.value"].GetScaledValue();
	}
	else
		continuity=1.0f;

	if(this->parameters.find("PWR.value")!=this->parameters.end())
	{
		this->parameters["PWR.value"].SetScaling(0.1f,0.9f);
		tension=this->parameters["PWR.value"].GetScaledValue();
	}
	else
		tension=1.0f;

	std::vector<TorsoGesturePhase>::iterator iter;
	for(iter=this->phases.begin();iter!=this->phases.end();iter++)
	{
		(*iter).CollectKeyframes(keyframes,fps,scale,continuity,tension);
	}
}

TorsoGesturePhase* TorsoGesture::GetPhase(TorsoGesturePhaseType which)
{
	std::vector<TorsoGesturePhase>::iterator iter;
	
	for(iter=this->phases.begin();iter!=this->phases.end();iter++)
	{
		if((*iter).type==which)
			return &(*iter);
	}
	return 0;
}

bool TorsoGesture::ScalePhases(TorsoGesturePhaseType which,float scaling)
{
	bool found;
	std::vector<TorsoGesturePhase>::iterator iter;
	found=false;
	for(iter=this->phases.begin();iter!=this->phases.end();iter++)
	{
		if((*iter).type==which)
		{
			(*iter).Tduration*=scaling;
			found=true;
		}
	}
	return found;
}

bool TorsoGesture::CheckTiming()
{
	std::vector<TorsoGesturePhase>::iterator iter,prev;
	for(iter=this->phases.begin()+1;iter!=this->phases.end();iter++)
	{
		prev=iter-1;
		if((*prev).end>(*iter).begin)
		{
			//printf("Torso Engine: gesture %s, phase %s (%.2f-%.2f) overlaps with phase %s (%.2f-%.2f)\n",
			//	this->reference.c_str(),((*prev).GetPhaseName()).c_str(),(*prev).begin,(*prev).end,
			//	((*iter).GetPhaseName()).c_str(),(*iter).begin,(*iter).end);
			//printf("the whole gesture start at %.2f and ends at %.2f\n",
			//	this->start,this->start+this->duration);

			return false;
		}
	}
	return true;
}
