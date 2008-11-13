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

// AgentStatePsydule.cpp: implementation of the AgentStatePsydule class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include "AgentState.h"
#include "EngineParameter.h"
#include "EngineParameterSet.h"

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

extern IniManager inimanager;

// Default constructor
AgentState::AgentState()
{
}

// Constructor to use
AgentState::AgentState(std::string state)
{
	internalstate.LoadFromBuffer(state);

	this->name="";

	this->userinterestlevel=0.8;

	turn=0;
}

// Destructor
AgentState::~AgentState()
{

}

int AgentState::getAgentTurn() 
{ 
	return(this->turn);
}

//To get the agent's name
std::string AgentState::getAgentName() { return(name); }

//To set the agent's name
void AgentState::setAgentName(std::string name) { this->name=name; }

//The user interest level getter
float AgentState::getUserInterestLevel() { return(userinterestlevel); }

//The user interest level setter
void AgentState::setUserInterestLevel(float uil) { userinterestlevel=uil; }

std::string AgentState::getEmotionState() {return emotionstate;}
void AgentState::setEmotionState(std::string emotion) {this->emotionstate=emotion;}

//The update methods
void AgentState::UpdateState(std::string state)
{
	//correggi lo stato
	internalstate.LoadFromBuffer(state);
}

void AgentState::setSingle(std::string opname, std::string value)
{
	if(value=="")
	{
		std::string name=opname.substr(0,opname.find_first_of("="));
		std::string val=opname.substr(opname.find_first_of("=")+1);
		internalstate.SetAttrValue(name, val);
	}
	else
		internalstate.SetAttrValue(opname, value);
}

float AgentState::getValue(std::string opname)
{
	return (internalstate.GetAttrValue(opname));
}

void AgentState::Print()
{
	internalstate.Print();
	printf("emotion: %s    uil: %.2f\n", this->emotionstate.c_str(), this->userinterestlevel);
}


void AgentState::UpdateTendency(float newtendency)
{
}

/*
void AgentState::ComputeInterestLevel(float x)
{
	char value[256];
	float a, b, c, interestlevel;

	//x/=2;   ????

	b=internalstate.GetAttrValue("interest_level.slope.value");

	c = (float)((1+exp(-b))/(exp(-b)-1));
	a = -2*c;
	interestlevel=(float)(a/(1 + exp(-b*x)) + c);

	sprintf(value,"%.2f",interestlevel);
	internalstate.SetAttrValue("interest_level.interest.value",(std::string)value);
}
*/

void AgentState::AddBackchannel(int time, std::string lastbc)
{
	mapBackchannel[time] = lastbc;
}