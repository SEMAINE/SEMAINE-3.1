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

// Agent.cpp: 
//
//////////////////////////////////////////////////////////////////////

#include ".\agent.h"

Agent::Agent(void)
{
	//general expressivity tendency
	general_OAC=0.5f;
	general_SPC=0.0f;
	general_TMP=0.0f;
	general_FLD=0.0f;
	general_PWR=0.0f;
	general_REP=0.0f;
	//for the lips
	liptension=medium;
	liparticulation=medium;
	//modality preference
	hierarchy_face=1.0f;
	hierarchy_gesture=1.0f;
	hierarchy_posture=1.0f;
	hierarchy_gaze=1.0f;
	hierarchy_head=1.0f;
	//influence of expressivity in each modality
	factor_face=1.0f;
	factor_gesture=1.0f;
	factor_posture=1.0f;
	factor_gaze=1.0f;
	factor_head=1.0f;
}



Agent::~Agent(void)
{
}

int Agent::Load(std::string xmlfilename)
{
	XMLGenericParser parser;
	parser.SetValidating(false);
	XMLGenericTree *tree=parser.ParseFile(xmlfilename);
	if(tree==0)
		return 0;
	XMLGenericTree *agentdef=tree->FindNodeCalled("agentDefinition");
	if(agentdef==0)
	{
		delete tree;
		return 0;
	}
	
	this->name=agentdef->GetAttribute("name");

	if((agentdef->GetAttribute("gender"))=="female")
		this->gender=female;
	if((agentdef->GetAttribute("gender"))=="male")
		this->gender=male;

	this->age=(int)agentdef->GetAttributef("age");

	if((agentdef->GetAttribute("hand"))=="left")
		this->hand=left;
	if((agentdef->GetAttribute("hand"))=="right")
		this->hand=right;

	this->origin=agentdef->GetAttribute("origin");

	this->general_OAC=(agentdef->FindNodeCalled("descriptionFactors"))->GetAttributef("overallActivity");
	this->general_SPC=(agentdef->FindNodeCalled("descriptionFactors"))->GetAttributef("spatial");
	this->general_TMP=(agentdef->FindNodeCalled("descriptionFactors"))->GetAttributef("temporal");
	this->general_FLD=(agentdef->FindNodeCalled("descriptionFactors"))->GetAttributef("fluidity");
	this->general_PWR=(agentdef->FindNodeCalled("descriptionFactors"))->GetAttributef("power");
	this->general_REP=(agentdef->FindNodeCalled("descriptionFactors"))->GetAttributef("repetitivity");

	if(((agentdef->FindNodeCalled("articulationParam"))->GetAttribute("tension"))=="low")
		this->liptension=low;
	if(((agentdef->FindNodeCalled("articulationParam"))->GetAttribute("tension"))=="medium")
		this->liptension=medium;
	if(((agentdef->FindNodeCalled("articulationParam"))->GetAttribute("tension"))=="strong")
		this->liptension=strong;

	if(((agentdef->FindNodeCalled("articulationParam"))->GetAttribute("articulation"))=="low")
		this->liparticulation=low;
	if(((agentdef->FindNodeCalled("articulationParam"))->GetAttribute("articulation"))=="medium")
		this->liparticulation=medium;
	if(((agentdef->FindNodeCalled("articulationParam"))->GetAttribute("articulation"))=="strong")
		this->liparticulation=strong;

	this->hierarchy_face=((agentdef->FindNodeCalled("modalityHierarchy"))->FindNodeCalled("face"))->GetAttributef("value");
	this->hierarchy_gesture=((agentdef->FindNodeCalled("modalityHierarchy"))->FindNodeCalled("gesture"))->GetAttributef("value");
	this->hierarchy_posture=((agentdef->FindNodeCalled("modalityHierarchy"))->FindNodeCalled("posture"))->GetAttributef("value");
	this->hierarchy_gaze=((agentdef->FindNodeCalled("modalityHierarchy"))->FindNodeCalled("gaze"))->GetAttributef("value");
	this->hierarchy_head=((agentdef->FindNodeCalled("modalityHierarchy"))->FindNodeCalled("head"))->GetAttributef("value");

	this->factor_face=((agentdef->FindNodeCalled("modalityFactorExp"))->FindNodeCalled("face"))->GetAttributef("value");
	this->factor_gesture=((agentdef->FindNodeCalled("modalityFactorExp"))->FindNodeCalled("gesture"))->GetAttributef("value");
	this->factor_posture=((agentdef->FindNodeCalled("modalityFactorExp"))->FindNodeCalled("posture"))->GetAttributef("value");
	this->factor_gaze=((agentdef->FindNodeCalled("modalityFactorExp"))->FindNodeCalled("gaze"))->GetAttributef("value");
	this->factor_head=((agentdef->FindNodeCalled("modalityFactorExp"))->FindNodeCalled("head"))->GetAttributef("value");

	return 1;
}
