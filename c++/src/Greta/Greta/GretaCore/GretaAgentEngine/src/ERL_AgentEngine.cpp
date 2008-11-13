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

// ERL_AgentEngine.cpp: implementation of the ERL_AgentEngine class.
//
//////////////////////////////////////////////////////////////////////

#include ".\ERL_AgentEngine.h"
#include "Agent.h"
#include "IniManager.h"
#include "FileNames.h"
extern IniManager inimanager;

extern FileNames filenames;

ERL_AgentEngine::ERL_AgentEngine(void)
{
}

ERL_AgentEngine::~ERL_AgentEngine(void)
{
	delete this->ERLtree;
}

int ERL_AgentEngine::Compute(std::string ERLfilename)
{
	Agent agent;
	
	agent.Load(ERLfilename);

//	APMLTree *tree;

	APMLParser *parser2;

	parser2=new APMLParser();

	parser2->EnableTimingsCalculation();

	std::string APMLfilename=ERLfilename.substr(0,ERLfilename.find_last_of("/\\")+1);

	APMLfilename=APMLfilename+"apml_";

	APMLfilename=APMLfilename+ERLfilename.substr(ERLfilename.find_last_of("/\\")+1);

	parser2->ParseFile(APMLfilename);

	this->ERLfilename=ERLfilename;

	ERLParser *parser;

	parser=new ERLParser();

	parser->phonemesfilename=parser2->phonemesfilename;

	this->ERLtree=parser->ParseFile(this->ERLfilename);

	this->ERLtree->SaveWithAttributes("tmp/gino.txt");

	if(this->ERLtree==0)
	{
		printf("error parsing ERL.\n");
		return 0;
	}

	//printf("finished parsing ERL.\n");

	delete parser;

	delete parser2;

	return 1;
}

