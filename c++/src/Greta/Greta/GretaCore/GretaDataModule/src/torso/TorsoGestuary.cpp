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

// TorsoGestuary.cpp: implementation of the  TorsoGestuary class.
//
//////////////////////////////////////////////////////////////////////

#include "TorsoGestuary.h"
#include "TorsoGesture.h"

using namespace TorsoSpace;

TorsoGestuary::TorsoGestuary(){}

TorsoGestuary::~TorsoGestuary(){
	gestuary.clear();
}

std::map<std::string,TorsoGesture> TorsoGestuary::getGestuary(){return gestuary;}


int TorsoGestuary::LoadGestuary(std::string gestuaryfilename, TorsoMovementPoint restposition)
{
	XMLGenericParser p;
	XMLGenericTree *t;
	bool found_stroke;

	p.SetValidating(true);
	t=p.ParseFile(gestuaryfilename);
	if(t==0)
	{
		delete t;
		return 0;
	}
	
	std::list<XMLGenericTree*>::iterator iter;

	for(iter=t->child.begin();iter!=t->child.end();iter++)
	{
		found_stroke=false;
		
		TorsoGesture tg((*iter)->GetAttribute("id"));

		XMLGenericTree *torsonode;
		torsonode=(*iter)->FindNodeCalled("torso");
		std::list<XMLGenericTree*>::iterator iter2;
		for(iter2=torsonode->child.begin();iter2!=torsonode->child.end();iter2++)
		{
			TorsoGesturePhase tp;
			tp.type=tp.GetPhaseId((*iter2)->GetAttribute("phase"));
			if(tp.type==stroke)
				found_stroke=true;
			if(tp.type==unknown_phase)
				printf("TorsoEngine::Error unknown phase %s in gesture %s\n",
				(*iter2)->GetAttribute("phase").c_str(),tg.id.c_str());
			else
			{
				if(tp.type==preparation)
				{
					tp.points.push_back(restposition);
				}
				tp.trajectory=tp.GetTrajectoryId((*iter2)->GetAttribute("trajectory"));
				std::list<XMLGenericTree*>::iterator iter3;
				for(iter3=(*iter2)->child.begin();iter3!=(*iter2)->child.end();iter3++)
				{
					if((*iter3)->name=="spine")
					{
						if((*iter3)->GetAttribute("manner")=="closed")
							tp.stance=closed;
						else
							tp.stance=opened;
					}
					if((*iter3)->name=="timing")
					{
						tp.Tdefault=(*iter3)->GetAttributef("default");
						tp.Tmin=(*iter3)->GetAttributef("min");
						tp.Tmax=(*iter3)->GetAttributef("max");
						tp.Tduration=(*iter3)->GetAttributef("duration");
						tp.Tfixed=(int)(*iter3)->GetAttributef("fixed");
					}
					if((*iter3)->name=="point")
					{
						TorsoMovementPoint tmp;
						tmp.Hposition=tmp.GetHpositionId((*iter3)->GetAttribute("left_right"));
						tmp.Vposition=tmp.GetVpositionId((*iter3)->GetAttribute("forward_backward"));
						tmp.Sposition=tmp.GetSpositionId((*iter3)->GetAttribute("tilt"));
						tmp.SpineRotation=tmp.GetSpineRotationId((*iter3)->GetAttribute("spine_rotation"));
						tmp.compensatehead=(bool)(*iter3)->GetAttributef("compensate_head");
						tmp.compensateshoulders=(bool)(*iter3)->GetAttributef("compensate_shoulders");
						tmp.spatial_fixed=(bool)(*iter3)->GetAttributef("spatial_fixed");
						if(tmp.LoadPose(tp.stance,(*iter3)->GetAttributef("scaling"))==0)
						{
							break;
						}
						if((tp.type!=stroke)&&(tp.points.size()>1))
							printf("TorsoEngine::Warning Multiple points phase in Gesture %s (only strokes can have multiple points)\n",
							tg.id.c_str());
						else
							if(tp.type==retraction)
								printf("TorsoEngine::Warning Retraction phase cannot contain poses (gesture %s)\n",
								tg.id.c_str());
							else
								tp.points.push_back(tmp);
					}
				}
				tg.phases.push_back(tp);
			}
		}
		if(found_stroke==true)
			this->gestuary[tg.id]=tg;
		else
			printf("TorsoEngine::Warning Skipping gesture %s (no stroke phase found)\n",tg.id.c_str());
	}

	delete t;
	return 1;
}