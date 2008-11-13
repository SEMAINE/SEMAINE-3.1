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

#include ".\world.h"
#include "IniManager.h"
#include "XMLGenericParser.h"

extern IniManager inimanager;

World::World(void)
{
	objects.clear();
	agents.clear();
}

World::~World(void)
{
	objects.clear();
	agents.clear();
}

void World::draw(void)
{
	objectsiterator iter;
	for(iter=objects.begin();iter!=objects.end();iter++)
		(*iter)->draw();
	agentsiterator iter2;
	for(iter2=agents.begin();iter2!=agents.end();iter2++)
		(*iter2)->draw();
}

void World::AddWorldObject(std::string name,std::string objectshape)
{
	WorldObject *wo;
	wo=new WorldObject(name,objectshape);
	objects.push_back(wo);
}

void World::AddAgent(std::string name,std::string character)
{
	AgentAspect *a;
	a=new AgentAspect(name,character);
	//a->AssignFile((char*)(inimanager.Program_Path+"empty").c_str());
	a->AssignFile((char*)(inimanager.Program_Path+"output/walk.0").c_str());
	//a->EnableAudio(false);
	agents.push_back(a);
}

WorldObject *World::FindWorldObject(std::string name)
{
	bool found;
	objectsiterator iter;
	found=false;
	iter=objects.begin();
	while((iter!=objects.end())&&(found==false))
	{
		if((*iter)->name==name)
			found=true;
		else
			iter++;
	}
	if(found==true)
		return (*iter);
	else
		return 0;
}

AgentAspect *World::FindAgent(std::string name)
{
	bool found;
	agentsiterator iter;
	found=false;
	iter=agents.begin();
	while((iter!=agents.end())&&(found==false))
	{
		if((*iter)->name==name)
			found=true;
		else
			iter++;
	}
	if(found==true)
		return (*iter);
	else
		return 0;
}

void World::LoadWorld(std::string filename)
{
	XMLGenericParser p;
	XMLGenericTree *t,*node;
	std::list<XMLGenericTree *>::iterator iter;
	p.SetValidating(true);
	t=p.ParseFile(filename);
	node=t->FindNodeCalled("AGENTS");
	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		AddAgent((*iter)->GetAttribute("name"),(*iter)->GetAttribute("character"));
		AgentAspect *a;
		a=FindAgent((*iter)->GetAttribute("name"));
		a->MoveAgent((*iter)->GetAttributef("position_x"),(*iter)->GetAttributef("position_y"),(*iter)->GetAttributef("position_z"));
		a->RotateAgent((*iter)->GetAttributef("rotation_x"),(*iter)->GetAttributef("rotation_y"),(*iter)->GetAttributef("rotation_z"));
	}
	node=t->FindNodeCalled("OBJECTS");
	for(iter=node->child.begin();iter!=node->child.end();iter++)
	{
		AddWorldObject((*iter)->GetAttribute("name"),(*iter)->GetAttribute("object_shape"));
		WorldObject *wo;
		wo=FindWorldObject((*iter)->GetAttribute("name"));
		wo->Move((*iter)->GetAttributef("position_x"),(*iter)->GetAttributef("position_y"),(*iter)->GetAttributef("position_z"));
		wo->Rotate((*iter)->GetAttributef("rotation_x"),(*iter)->GetAttributef("rotation_y"),(*iter)->GetAttributef("rotation_z"));
	}

	delete t;
}
