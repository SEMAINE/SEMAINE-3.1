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

// HeadGestuary.cpp: implementation of the  HeadGestuary class.
//
//////////////////////////////////////////////////////////////////////

#include "HeadGestuary.h"
#include "HeadGesture.h"

using namespace HeadSpace;

HeadGestuary::HeadGestuary(){}

HeadGestuary::~HeadGestuary(){
	gestuary.clear();
}

std::map<std::string,HeadGesture> HeadGestuary::getGestuary(){return gestuary;}

int HeadGestuary::LoadGestuary(std::string gestuaryfilename)
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
