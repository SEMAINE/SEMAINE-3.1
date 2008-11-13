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

// HeadGestuarePhase.cpp: implementation of the  HeadGesturePhase class.
//
//////////////////////////////////////////////////////////////////////

#include "HeadGesturephase.h"

/**@#-*/
using namespace HeadSpace;
/**@#+*/

HeadGesturePhase::HeadGesturePhase(void)
{
	Tmin=0.5f;
	Tmax=1.0f;
	Tdefault=1.0f;
	Tduration=0.0f;
	concretized=false;
}

HeadGesturePhase::HeadGesturePhase(const HeadGesturePhase &rhs)
{
	this->Tdefault=rhs.Tdefault;
	this->Tduration=rhs.Tduration;
	this->Tfixed=rhs.Tfixed;
	this->Tmax=rhs.Tmax;
	this->Tmin=rhs.Tmin;
	this->begin=rhs.begin;
	this->end=rhs.end;

	this->concretized=rhs.concretized;

	this->type=rhs.type;
	//this->stance=rhs.stance;
	std::vector<HeadGesturePoint>::const_iterator iter;
	for(iter=rhs.points.begin();iter!=rhs.points.end();iter++)
	{
		HeadGesturePoint *hgp;
		hgp=new HeadGesturePoint();
		hgp->rotx=(*iter).rotx;
		hgp->roty=(*iter).roty;
		hgp->rotz=(*iter).rotz;
		hgp->start=(*iter).start;
		hgp->end=(*iter).end;
		this->points.push_back(*hgp);
	}
}

HeadGesturePhase::~HeadGesturePhase(void)
{

}

void HeadGesturePhase::Temporize(float begin,float end)
{
	this->begin=begin;
	this->end=end;
	this->Tduration=end-begin;
	int count;
	float interval;
	count=0;
	interval=(end-begin)/points.size();
	std::vector<HeadGesturePoint>::iterator iter;
	for(iter=points.begin();iter!=points.end();iter++)
	{
		(*iter).start=this->begin+interval*count;
		(*iter).end=this->begin+interval*(count+1);
		count++;
	}
	concretized=true;
}

void HeadGesturePhase::CollectKeyframes(std::vector<FAPFrame> &keyframes,float fps,float scalefactor,float continuity,float tension)
{
	FAPFrame *ff;
	if(type==preparation)
	{
		ff=new FAPFrame();
		ff->SetFAP(48,0,1);
		ff->SetFAP(49,0,1);
		ff->SetFAP(50,0,1);
		ff->SetTCBParam(tension,continuity,0);
		ff->framenumber=this->begin*fps;
		keyframes.push_back(*ff);

		ff=new FAPFrame();
		ff->SetFAP(48,scalefactor*(points[0].rotx/360.0f)*6.28f*100000,1);
		ff->SetFAP(49,scalefactor*(points[0].rotx/360.0f)*6.28f*100000,1);
		ff->SetFAP(50,scalefactor*(points[0].rotz/360.0f)*6.28f*100000,1);
		ff->SetTCBParam(tension,continuity,0);
		ff->framenumber=this->end*fps;
		keyframes.push_back(*ff);
	}
	if(type==hold)
	{
		ff=new FAPFrame();
		ff->SetFAP(48,scalefactor*(points[0].roty/360.0f)*6.28f*100000,1);
		ff->SetFAP(49,scalefactor*(points[0].rotx/360.0f)*6.28f*100000,1);
		ff->SetFAP(50,scalefactor*(points[0].rotz/360.0f)*6.28f*100000,1);
		ff->SetTCBParam(tension,continuity,0);
		ff->framenumber=this->end*fps;
		keyframes.push_back(*ff);
	}
	if(type==stroke)
	{
		std::vector<HeadGesturePoint>::iterator iter;
		for(iter=points.begin();iter!=points.end();iter++)
		{
			ff=new FAPFrame();
			ff->SetFAP(48,scalefactor*((*iter).roty/360.0f)*6.28f*100000,1);
			ff->SetFAP(49,scalefactor*((*iter).rotx/360.0f)*6.28f*100000,1);
			ff->SetFAP(50,scalefactor*((*iter).rotz/360.0f)*6.28f*100000,1);
			ff->SetTCBParam(tension,continuity,0);
			ff->framenumber=(*iter).end*fps;
			keyframes.push_back(*ff);
		}
	}
	if(type==retraction)
	{
		ff=new FAPFrame();
		ff->SetFAP(48,0,1);
		ff->SetFAP(49,0,1);
		ff->SetFAP(50,0,1);
		ff->SetTCBParam(tension,continuity,0);
		ff->framenumber=this->end*fps;
		keyframes.push_back(*ff);
	}
}

void HeadGesturePhase::AddPoint(int rotx,int roty,int rotz)
{
	HeadGesturePoint hgp;
	hgp.rotx=rotx;
	hgp.roty=roty;
	hgp.rotz=rotz;
	points.push_back(hgp);
}


HeadGesturePhaseType HeadGesturePhase::GetPhaseId(std::string phasename)
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

std::string HeadGesturePhase::GetPhaseName(HeadGesturePhaseType type_id)
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
