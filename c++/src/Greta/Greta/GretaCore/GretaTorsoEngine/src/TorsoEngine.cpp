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

#include "torsoengine.h"
#include <crtdbg.h>
#include "BAPConverter.h"
#include "DataContainer.h"

extern DataContainer *datacontainer;

BAPConverter TorsoGlobalBAPconverter;

/**@#-*/
using namespace TorsoSpace;
/**@#+*/

TorsoEngine::TorsoEngine(void)
{
	

	dw=0;
	animationlength=0;
	fps=25;


	//restposition.LoadPose("rest.txt");
	//restposition.compensatehead=1;
	//restposition.compensateshoulders=1;
	//this->LoadGestuary("torso/gestuary.xml");

	//usa datacontainer instead
	restposition = datacontainer->getRestPosition();
	gestuary=(datacontainer->getTorsoGestuary())->getGestuary();
}

TorsoEngine::~TorsoEngine(void)
{
	//do not cancel it; datcontainer will do that
	//gestuary.clear();
}



//see torsogestuary in datamodule

/*
int TorsoEngine::LoadGestuary(std::string gestuaryfilename)
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
*/


int TorsoEngine::ScheduleTorsoGesture(std::string reference, float start, float duration, float s)
{
	if(this->gestuary.find(reference)==this->gestuary.end())
	{
		printf("TorsoEngine:: Schedule Warning, gesture %s unknown\n",reference.c_str());
		return 0;
	}
	TorsoGesture tg(gestuary[reference]);
	tg.reference=reference;
	tg.start=start;
	tg.duration=duration;
	tg.strokes.push_back(s);
	AddGesture(tg);
	return 1;
}

int TorsoEngine::ScheduleTorsoGesture(std::string reference, float start, float duration, std::vector<float> *strokes)
{
	if(this->gestuary.find(reference)==this->gestuary.end())
	{
		printf("TorsoEngine:: Schedule Warning, gesture %s unknown\n",reference.c_str());
		return 0;
	}
	TorsoGesture tg(reference,start,duration,strokes);
	AddGesture(tg);
	return 1;
}

int TorsoEngine::ScheduleTorsoGesture(MMSystemSpace::Signal s)
{
	if(this->gestuary.find(s.reference)==this->gestuary.end())
	{
		printf("TorsoEngine:: Schedule Warning, gesture %s unknown\n",s.reference.c_str());
		return 0;
	}
	TorsoGesture tg(s);
	AddGesture(tg);
	return 1;
}

int TorsoEngine::ScheduleTorsoGesture(MMSystemSpace::Signal s,std::string gestureid)
{

	
	if(this->gestuary.find(gestureid)==this->gestuary.end())
	{
		printf("TorsoEngine:: Schedule Warning, gesture %s unknown\n",gestureid.c_str());
		return 0;
	}
	
	TorsoGesture tg(s,gestuary[gestureid]);
	
	AddGesture(tg);
	
	return 1;
}

void TorsoEngine::AddGesture(TorsoGesture tg)
{
	
	if(this->gestures.size()==0)
	{
		this->gestures.push_back(tg);
		return;
	}
	

	std::vector<TorsoGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		if((tg.start+tg.duration)<(*iter).start)
		{
			this->gestures.insert(iter,tg);
			return;
		}
		while((tg.start>=((*iter).start+(*iter).duration))&&((iter+1)!=gestures.end()))
			iter++;
		if(tg.start>=((*iter).start+(*iter).duration))
		{
			
			if((iter+1)!=gestures.end())
			{
				
				if((tg.start+tg.duration)>(*(iter+1)).start)
				{
					
					printf("TorsoEngine:: Error, could not schedule gesture %s, it overlaps with gesture %s\n",
					tg.id.c_str(),(*iter).id.c_str());
					return;
				}
				this->gestures.insert(iter,tg);
				return;
			}
			else
			{
				this->gestures.push_back(tg);
				return;
			}
			
		}
		else
		{
			printf("TorsoEngine:: Error, could not schedule gesture %s, it overlaps with gesture %s\n",
					tg.id.c_str(),(*iter).id.c_str());
			break;
		}
	}
}

void TorsoEngine::RenderAnimation(float start, float end, float fps)
{

	if(end<=start)
		return;

	this->fps=fps;
	
	CreateEmptyAnimation(end-start);

	AddMissingPreparationRetraction();
	
	TemporizeGestures();

	AddMissingHold();

	CollectKeyframes();

	//AddNoise();

	InsertKeyframes();

	InterpolateKeyframes();

	//VisualizeBAP(142);
}

void TorsoEngine::VisualizeBAP(int which)
{
	if(animation.size()==0)
		return;

	if(dw==0)
	{
		dw=new DataViewerWindow();
		
		dw->show();
	}

	dw->AddGraph("bap",0,0,0,0,animation.size(),-50000,50000);

	dw->ShowPointsOfGraph("bap");

	int i;

	for(i=0;i<animation.size();i++)
	{
		dw->AddPointToGraph("bap",i,animation[i].GetBAP(which));
	}
}

void TorsoEngine::CreateEmptyAnimation(float duration)
{
	int i;
	animation.clear();
	for(i=0;i<duration*fps;i++)
	{
		BAPFrame bf(&TorsoGlobalBAPconverter);
		//bf.SetBAP("vt7.rotateX",degrees,90);
		//bf.SetMask(t7tilt,1);
		animation.push_back(bf);
	}
	animationlength=duration;
}

void TorsoEngine::TemporizeGestures()
{
	std::vector<TorsoGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).Temporize();
	}
}

void TorsoEngine::AddMissingPreparationRetraction()
{
	std::vector<TorsoGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).AddMissingPreparationRetraction(restposition,this->fps);
	}
}

void TorsoEngine::AddMissingHold()
{
	std::vector<TorsoGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).AddMissingHold(this->fps);
	}
}

void TorsoEngine::CollectKeyframes()
{
	keyframes.clear();
	std::vector<TorsoGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).CollectKeyframes(keyframes,this->fps);
	}
}



void TorsoEngine::InsertKeyframes()
{
	int i;
	std::vector<BAPFrame>::iterator iter;

	for(i=0;i<72;i++)
	{
		interpolator[i].AddPoint(0,0,0,0,0);
		//interpolator[i].AddPoint(1,0,0,0,0);
		//interpolator[i].AddPoint((animationlength*fps)-2,0,0,0,0);
		interpolator[i].AddPoint((animationlength*fps)-1,0,0,0,0);
	}

/*
	if(dw==0)
	{
		dw=new DataViewerWindow();
		
		dw->show();
		dw->AddGraph("bap",0,0,0,0,animation.size(),-50000,50000);
		dw->ShowPointsOfGraph("bap");
	}
*/

	for(iter=keyframes.begin();iter!=keyframes.end();iter++)
	{
		for(i=98;i<170;i++)
		{
			if((*iter).GetMask(i)==1)
			{
	/*			if(i==118)
				{

					dw->AddPointToGraph("bap",(*iter).GetFrameNumber(),(*iter).GetBAP(i));
				}

				if(i==142)
					printf("");*/
				interpolator[i-98].AddPoint((*iter).GetFrameNumber(),
				(*iter).GetBAP(i),
				(*iter).GetTCBParam(Tension),
				(*iter).GetTCBParam(Continuity),
				(*iter).GetTCBParam(Bias));
			}
		}
	}
}

void TorsoEngine::InterpolateKeyframes()
{
	int i,j;
	for(i=0;i<72;i++)
	{
		interpolator[i].Prepare();
		if(interpolator[i].in_use==false)
			continue;
		for(j=0;j<animationlength*fps;j++)
		{
			animation[j].SetBAP(i+98,interpolator[i].GetFrameN(j));
		}
	}
}

std::vector<BAPFrame> *TorsoEngine::GetAnimation()
{
	return &animation;
}
void TorsoEngine::Cleanup(void)
{
	int i;
	animation.clear();
	keyframes.clear();
	gestures.clear();
	animationlength=0;
	for(i=0;i<72;i++)
	{
		interpolator[i].Reset();
	}

}
