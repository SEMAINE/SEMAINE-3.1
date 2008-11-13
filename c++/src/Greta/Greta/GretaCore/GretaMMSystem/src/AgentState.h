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

// AgentState.h: interface for the AgentState class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "IniManager.h"
#include "EngineParameterSetOfSets.h"

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

/**
 *  An Agent state class. 
 *  Used compute the agent state : i.e. the level of interrest, the backchannels.
 *  Herited from JThread (cmlabs)  
 *  @author ???
 *  @date 2008
 */
class AgentState
{
public:
	/**
	* Default constructor
	*/
	AgentState();
	/**
	* Constructor to use
	* @param std::string state The Agent State in string format
	*/
	AgentState(std::string state);
	/** 
	* Destructor
	*/
	virtual ~AgentState();
	/**
	* This method updates the agent's state 
	*/
	void AgentState::UpdateState(std::string state);
	/**
	* This method updates a single element of the agent's state 
	*/
	void AgentState::setSingle(std::string opname, std::string value);

	float AgentState::getValue(std::string opname);
	/**
	* The init state methods
	*/
	void AgentState::Print();
	/**
	* The update Tendency methods
	* @param float newtendency 
	*/
	void AgentState::UpdateTendency(float newtendency);
	/**
	* The interrest level computing method.
	* @param float x
	*/
	void AgentState::ComputeInterestLevel(float x);
	/**
	* Add a backchannel, into the backchannel Map
	* @param int time : the time stamp of the bachannel to add
	* @param std::string lastbc : the backchannel to add
	*/
	void AgentState::AddBackchannel(int time, std::string lastbc);
	/**
	* To get the agent's name
	* @return std::string : the agent's name.
	*/
	std::string AgentState::getAgentName();
	/**
	* Register the agent's name
	* @param std::string name : the agent's name to set
	*/
	void AgentState::setAgentName(std::string name);
	/**
	* Register the user interrest level
	* @param float uil : the user interrest level to set.
	*/
	void AgentState::setUserInterestLevel(float uil);
	/**
	* The user interrest level getter
	* @return float userinterestlevel : a copy of the user interest level
	*/
	float AgentState::getUserInterestLevel();
	/**
	* The user's internal emotional state setter
	* @param std::string emotion : the user's emotional state to set
	*/
	void AgentState::setEmotionState(std::string emotion);
	/**
	* The user's internal emotional state getter
	* @return std::string emotionstate : a copy of the user's emotional state
	*/
	std::string AgentState::getEmotionState();
	/**
	* To get the agent's turn 
	* @return int: 1-Speaker 0-Listener
	*/
	int AgentState::getAgentTurn();
	/**
	* The internal state of the agent
	* @see EngineParameterSetOfSets
	*/
	EngineParameterSetOfSets internalstate;
	/**
	* The backchannel Map
	*/
	std::map<int,std::string> mapBackchannel;
	std::vector<std::string> vecListenerFunctionsCombination;


private:
	/**
	* The agent's name
	*/
	std::string name;
	/**
	* The userinterestlevel
	*/
	float userinterestlevel;
	/**
	* The internal emotion state
	*/
	std::string emotionstate;
	/**
	* The externel time
	*/
	int externaltime;
	/**
	* The internal time
	*/
	int internaltime;
	/**
	* The agent's turn 1-Speaker 0-Listener
	*/
	int turn;
};

