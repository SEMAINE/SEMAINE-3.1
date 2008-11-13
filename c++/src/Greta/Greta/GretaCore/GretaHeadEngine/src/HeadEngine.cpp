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

#include ".\Headengine.h"
#include "datacontainer.h"
#include "datacontainer.h"
#include <crtdbg.h>

extern DataContainer *datacontainer;

/**@#-*/
using namespace HeadSpace;
/**@#+*/

HeadEngine::HeadEngine(void)
{
	//this->LoadGestuary("head/gestuary.xml");
	//usa datacontainer
	gestuary=(datacontainer->getHeadGestuary())->getGestuary();
	dw=0;
}

HeadEngine::~HeadEngine(void)
{
	//do not cancel it; datcontainer will do that
	//gestuary.clear();
}
/*
int HeadEngine::LoadGestuary(std::string gestuaryfilename)
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
		
		HeadGesture tg((*iter)->GetAttribute("id"));

		XMLGenericTree *headnode;
		headnode=(*iter)->FindNodeCalled("head");
		std::list<XMLGenericTree*>::iterator iter2;
		for(iter2=headnode->child.begin();iter2!=headnode->child.end();iter2++)
		{
			HeadGesturePhase tp;
			tp.type=tp.GetPhaseId((*iter2)->GetAttribute("phase"));
			if(tp.type==stroke)
				found_stroke=true;
			if(tp.type==unknown_phase)
				printf("HeadEngine::Error unknown phase %s in gesture %s\n",
				(*iter2)->GetAttribute("phase").c_str(),tg.id.c_str());
			else
			{
				if(tp.type==preparation)
				{
				}
				std::list<XMLGenericTree*>::iterator iter3;
				for(iter3=(*iter2)->child.begin();iter3!=(*iter2)->child.end();iter3++)
				{
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
						tp.AddPoint((*iter3)->GetAttributef("x"),
									(*iter3)->GetAttributef("y"),
									(*iter3)->GetAttributef("z"));
					}
				}
				tg.phases.push_back(tp);
			}
		}
		if(found_stroke==true)
			this->gestuary[tg.id]=tg;
		else
			printf("HeadEngine::Warning Skipping gesture %s (no stroke phase found)\n",tg.id.c_str());
	}

	delete t;
	return 1;
}

*/

int HeadEngine::ScheduleHeadGesture(std::string reference, float start, float duration, float s)
{
	if(this->gestuary.find(reference)==this->gestuary.end())
	{
		printf("HeadEngine:: Schedule Warning, gesture %s unknown\n",reference.c_str());
		return 0;
	}
	HeadGesture tg(gestuary[reference]);
	tg.reference=reference;
	tg.start=start;
	tg.duration=duration;
	tg.strokes.push_back(s);
	AddGesture(tg);
	return 1;
}

int HeadEngine::ScheduleHeadGesture(std::string reference, float start, float duration, std::vector<float> *strokes)
{
	if(this->gestuary.find(reference)==this->gestuary.end())
	{
		printf("HeadEngine:: Schedule Warning, gesture %s unknown\n",reference.c_str());
		return 0;
	}
	HeadGesture tg(reference,start,duration,strokes);
	AddGesture(tg);
	return 1;
}

int HeadEngine::ScheduleHeadGesture(MMSystemSpace::Signal s)
{
	if(this->gestuary.find(s.reference)==this->gestuary.end())
	{
		printf("HeadEngine:: Schedule Warning, gesture %s unknown\n",s.reference.c_str());
		return 0;
	}
	HeadGesture tg(s);
	AddGesture(tg);
	return 1;
}

int HeadEngine::ScheduleHeadGesture(MMSystemSpace::Signal s,std::string gestureid)
{

	
	if(this->gestuary.find(gestureid)==this->gestuary.end())
	{
		printf("HeadEngine:: Schedule Warning, gesture %s unknown\n",gestureid.c_str());
		return 0;
	}
	
	HeadGesture tg(s,gestuary[gestureid]);
	
	AddGesture(tg);
	
	return 1;
}

void HeadEngine::AddGesture(HeadGesture tg)
{
	
	if(this->gestures.size()==0)
	{
		this->gestures.push_back(tg);
		return;
	}
	

	std::vector<HeadGesture>::iterator iter;
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
					
					printf("HeadEngine:: Error, could not schedule gesture %s, it overlaps with gesture %s\n",
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
			printf("HeadEngine:: Error, could not schedule gesture %s, it overlaps with gesture %s\n",
					tg.id.c_str(),(*iter).id.c_str());
			break;
		}
	}
}

void HeadEngine::RenderAnimation(float start, float end, float fps)
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

	//VisualizeBAP(49);
	//VisualizeBAP(48);
}

void HeadEngine::VisualizeBAP(int which)
{
	if(animation.size()==0)
		return;

	if(dw==0)
	{
		dw=new DataViewerWindow();
		
		dw->show();
	}
	char fapname[10];
	sprintf(fapname,"%d",which);

	dw->AddGraph(fapname,0,0,0,0,animation.size(),-50000,50000);

	dw->ShowPointsOfGraph(fapname);

	int i;

	for(i=0;i<animation.size();i++)
	{
		dw->AddPointToGraph(fapname,i,animation[i].GetFAP(which));
	}
}

void HeadEngine::CreateEmptyAnimation(float duration)
{
	int i;
	animation.clear();
	for(i=0;i<duration*fps;i++)
	{
		FAPFrame ff;
		//bf.SetBAP("vt7.rotateX",degrees,90);
		//bf.SetMask(t7tilt,1);
		animation.push_back(ff);
	}
	animationlength=duration;
}

void HeadEngine::TemporizeGestures()
{
	std::vector<HeadGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).ExpandStrokes();
		(*iter).Temporize();
	}
}

void HeadEngine::AddMissingPreparationRetraction()
{
	std::vector<HeadGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).AddMissingPreparationRetraction(this->fps);
	}
}

void HeadEngine::AddMissingHold()
{
	std::vector<HeadGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).AddMissingHold(this->fps);
	}
}

void HeadEngine::CollectKeyframes()
{
	keyframes.clear();
	std::vector<HeadGesture>::iterator iter;
	for(iter=gestures.begin();iter!=gestures.end();iter++)
	{
		(*iter).CollectKeyframes(keyframes,this->fps);
	}
}



void HeadEngine::InsertKeyframes()
{
	int i;
	std::vector<FAPFrame>::iterator iter;

	for(i=0;i<3;i++)
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
		for(i=48;i<51;i++)
		{
			if((*iter).GetMask(i)==1)
			{
	/*			if(i==118)
				{

					dw->AddPointToGraph("bap",(*iter).GetFrameNumber(),(*iter).GetBAP(i));
				}
*/
				if(i==142)
					printf("");
				interpolator[i-48].AddPoint((*iter).framenumber,
				(*iter).GetFAP(i),
				(*iter).GetTCBParam(Tension),
				(*iter).GetTCBParam(Continuity),
				(*iter).GetTCBParam(Bias));
			}
		}
	}
}

void HeadEngine::InterpolateKeyframes()
{
	int i,j;
	for(i=0;i<3;i++)
	{
		interpolator[i].Prepare();
		if(interpolator[i].in_use==false)
			continue;
		for(j=0;j<animationlength*fps;j++)
		{
			animation[j].SetFAP(i+48,interpolator[i].GetFrameN(j));
		}
	}
}

std::vector<FAPFrame> *HeadEngine::GetAnimation()
{
	return &animation;
}

void HeadEngine::Cleanup(void)
{
	int i;
	animation.clear();
	keyframes.clear();
	gestures.clear();
	animationlength=0;
	for(i=0;i<3;i++)
	{
		interpolator[i].Reset();
	}

}