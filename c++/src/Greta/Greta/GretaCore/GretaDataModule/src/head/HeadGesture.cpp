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

// HeadGesture.cpp: implementation of the  HeadGesture class.
//
//////////////////////////////////////////////////////////////////////

#include "HeadGesture.h"

/**@#-*/
using namespace HeadSpace;
/**@#+*/

HeadGesture::HeadGesture()
{
//	prev=next=0;
	concretized=false;
}

HeadGesture::HeadGesture(std::string id)
{
	this->id=id;
//	prev=next=0;
	concretized=false;
}

HeadGesture::HeadGesture(std::string id, float start, float duration, std::vector<float> *strokes)
{
	this->id=id;
	this->start=start;
	this->duration=duration;
	this->strokes=*strokes;
//	prev=next=0;
	concretized=false;
}

HeadGesture::HeadGesture(std::string id, float start, float duration, float stroke)
{
	this->id=id;
	this->strokes.clear();
	this->strokes.push_back(stroke);
//	prev=next=0;
	concretized=false;
}

HeadGesture::HeadGesture(const HeadGesture &rhs):MMSystemSpace::Signal((MMSystemSpace::Signal)rhs)
{
	if(rhs.phases.empty()==false)
	{
		std::vector<HeadGesturePhase>::const_iterator iter2;
		for(iter2=rhs.phases.begin();iter2!=rhs.phases.end();iter2++)
		{
			HeadGesturePhase tgp((*iter2));
			this->phases.push_back(tgp);
		}
	}		
//	prev=next=0;
	concretized=rhs.concretized;
}


HeadGesture::HeadGesture(const MMSystemSpace::Signal &signal,const HeadGesture &gesture):MMSystemSpace::Signal(signal)
{
	if(gesture.phases.empty()==false)
	{
		std::vector<HeadGesturePhase>::const_iterator iter;
		for(iter=gesture.phases.begin();iter!=gesture.phases.end();iter++)
		{
			HeadGesturePhase tgp((*iter));
			this->phases.push_back(tgp);
		}
	}
//	prev=next=0;
	concretized=gesture.concretized;
}

HeadGesture::HeadGesture(const MMSystemSpace::Signal &rhs):MMSystemSpace::Signal(rhs)
{
//	prev=next=0;
}

HeadGesture::~HeadGesture(void)
{
	phases.clear();
}

int HeadGesture::Temporize()
{
	std::vector<HeadGesturePhase>::iterator iter;
	float temporal;

	if(this->parameters.find("TMP.value")!=this->parameters.end())
	{
		this->parameters["TMP.value"].SetScaling(1.2,0.2);
		temporal=this->parameters["TMP.value"].GetScaledValue();
	}
	else
		temporal=1.0f;

	if(this->strokes.empty()==true)
	{
		printf("HeadEngine::HeadGesture Error: no stroke timing found for gesture %s\n",this->reference.c_str());
		return 0;
	}

	HeadGesturePhase *phase;

	ScalePhases(stroke,temporal);
	ScalePhases(preparation,temporal);
	ScalePhases(retraction,temporal);

	iter=this->phases.begin();
	int strokecount;
	strokecount=0;

	while(iter!=this->phases.end())
	{
		iter=GetNextPhase(iter,stroke);
		if(iter!=this->phases.end())
		{
			(*iter).Temporize(this->start+this->strokes[strokecount]-(*iter).Tduration,this->start+this->strokes[strokecount]);
			iter++;
			strokecount++;
		}
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
		printf("HeadGesture: cannot concretize gesture %s\n",this->reference.c_str());
		concretized=false;
	}
	else
		concretized=true;
	return 1;
}

void HeadGesture::AddMissingPreparationRetraction(int fps)
{

	bool found;
	found=false;
	std::vector<HeadGesturePhase>::iterator iter;
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
		HeadGesturePhase tp;
		tp.Tduration=0.05f;
		tp.type=preparation;
		HeadGesturePoint hgp;
		hgp.rotx=0;
		hgp.roty=0;
		hgp.rotz=0;
		tp.points.push_back(hgp);
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
		HeadGesturePhase tp;
		tp.Tduration=0.05f;
		tp.type=retraction;
		HeadGesturePoint hgp;
		hgp.rotx=0;
		hgp.roty=0;
		hgp.rotz=0;
		tp.points.push_back(hgp);
		this->phases.push_back(tp);
	}

}

void HeadGesture::AddMissingHold(int fps)
{
	if(this->concretized==false)
		return;
	bool found;
	found=false;
	std::vector<HeadGesturePhase>::iterator iter,prev;
	for(iter=(this->phases.begin())+1;iter!=this->phases.end();iter++)
	{
		prev=iter-1;
		if(((*iter).begin-(*prev).end)>0.2)
		{
			HeadGesturePhase tp;
			tp.Tduration=((*iter).begin-(*prev).end);

			if((*prev).type==stroke)
			{
				tp.type=retraction;
				tp.AddPoint(0,0,0);
			}
			else
			{
				tp.type=hold;

				tp.AddPoint((*prev).points[(*prev).points.size()-1].rotx,
						(*prev).points[(*prev).points.size()-1].roty,
						(*prev).points[(*prev).points.size()-1].rotz);
			}			
			tp.Temporize((*prev).end,(*iter).begin);
			this->phases.insert(iter,tp);
			iter=(this->phases.begin());
		}

	}
}


void HeadGesture::CollectKeyframes(std::vector<FAPFrame> &keyframes,float fps)
{
	float scale,continuity,tension;

	if(this->concretized==false)
		return;

	if(this->parameters.find("SPC.value")!=this->parameters.end())
	{
		this->parameters["SPC.value"].SetScaling(0.5,1.8);
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

	std::vector<HeadGesturePhase>::iterator iter;
	for(iter=this->phases.begin();iter!=this->phases.end();iter++)
	{
		(*iter).CollectKeyframes(keyframes,fps,scale,continuity,tension);
	}
}

HeadGesturePhase* HeadGesture::GetPhase(HeadGesturePhaseType which)
{
	std::vector<HeadGesturePhase>::iterator iter;
	
	for(iter=this->phases.begin();iter!=this->phases.end();iter++)
	{
		if((*iter).type==which)
			return &(*iter);
	}
	return 0;
}

std::vector<HeadGesturePhase>::iterator HeadGesture::GetPhaseReverse(HeadGesturePhaseType which)
{
	std::vector<HeadGesturePhase>::iterator iter;
	
	for(iter=this->phases.end();iter!=this->phases.begin();iter--)
	{
		if((*iter).type==which)
			return iter;
	}
	return this->phases.begin();
}

std::vector<HeadGesturePhase>::iterator HeadGesture::GetNextPhase(std::vector<HeadGesturePhase>::iterator startiter,HeadGesturePhaseType which)
{
	std::vector<HeadGesturePhase>::iterator iter;
	
	for(iter=startiter;iter!=this->phases.end();iter++)
	{
		if((*iter).type==which)
			return iter;
	}
	return this->phases.end();
}


void HeadGesture::ExpandStrokes()
{
	std::vector<HeadGesturePhase>::iterator iter;

	int strokecount;
	iter=this->phases.begin();
	for(strokecount=0;strokecount<this->strokes.size();strokecount++)
	{
		iter=GetNextPhase(iter,stroke);
		if(iter!=this->phases.end())
		{
			iter++;
		}
		else
		{
			iter=this->GetPhaseReverse(stroke);
			HeadGesturePhase tp(*iter);
			tp.Tduration=(*iter).Tduration;
			tp.type=stroke;
			iter++;
			this->phases.insert(iter,tp);
			strokecount=-1;
			iter=this->phases.begin();
		}
	}
	while(iter!=this->phases.end())
	{
		iter=GetNextPhase(iter,stroke);
		if(iter!=this->phases.end())
		{
			this->phases.erase(iter);
		}
	}
}


bool HeadGesture::ScalePhases(HeadGesturePhaseType which,float scaling)
{
	bool found;
	std::vector<HeadGesturePhase>::iterator iter;
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

bool HeadGesture::CheckTiming()
{
	std::vector<HeadGesturePhase>::iterator iter,prev;
	for(iter=this->phases.begin()+1;iter!=this->phases.end();iter++)
	{
		prev=iter-1;
		if((*prev).end>(*iter).begin)
		{
			if(((*prev).end-(*iter).begin)<0.15f)
				if((((*prev).end-0.15)-(*prev).begin)>0.2)
				{
					(*prev).end-=0.15;
					continue;
				}
			/*printf("HeadGesture: phase %d (%.2f-%.2f) overlaps with phase %d (%.2f-%.2f\n",
				(*prev).type,(*prev).begin,(*prev).end,
				(*iter).type,(*iter).begin,(*iter).end
				);*/

			return false;
		}
	}
	return true;
}
