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


// ReactiveBackchannel.h: interface for the ReactiveBackchannel class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <utility>
#include "JString.h"
#include "CentralClock.h"
#include "AgentState.h"
#include "InputData.h"
#include "DataBackchannel.h"

/**
 *  Contains informations on the ReactiveBackchannel.
 *  When the ReactiveBehaviourTrigger send a "ping" to this object,
 *  the ReactiveBackchannel selects the communicative functions the
 *  agent wants to transmit through a reactive backchannel
 *  @author Elisabetta Bevacqua elisabetta.bevacqua@libero.it
 *  @date 2008
 */
class ReactiveBackchannel
{
private:
	/** 
    * Each object of this class is a communicative function
	* it is used to create a vector containing the comm func
    */
	class CommFun
	{
	public:
		/** 
		* The name of the communicative function
		*/
		std::string name;
		/** 
		* The importance of the communicative function
		*/
		float importance;
	};

	/** 
    * Contains info about the agent's state
    * 
    */
	AgentState *agentstate;
	/** 
    * time when a nod performed by the user has produced a backchannel
	* needed with Watson since it sends a lot of messages to say there's
	* a nod. To avoid the generation of many backchannels for just one nod
    * the following messages containing a nod signals are not considered
	* for three seconds
    */
	int nodtime;
	/** 
    * time when a shake performed by the user has produced a backchannel
	* needed with Watson since it sends a lot of messages to say there's
	* a shake. To avoid the generation of many backchannels for just one shake
    * the following messages containing a shake signals are not considered
	* for three seconds
    */
	int shaketime;
public:
	/** 
    * Class default contructor.
    * 
    */
	ReactiveBackchannel();
	/** 
    * Class  contructor.
    * @param agentstate the object containing data about the agent's internal state
    */
	ReactiveBackchannel::ReactiveBackchannel(AgentState *agentstate);
	/** 
    * Class destructor.
    * 
    */
	~ReactiveBackchannel();

	/** 
    * From the data in input about the user's behaviour, this function
	* decides the reactive behaviour
    * @param data the input data about the event produced by the user (user's behaviour)
	* @return all the data neded by the selector to write an FML (the DataBackchannel
	* is defined in the selectCommunicativeIntentions function
    */
	DataBackchannel *ReactiveBackchannel::GenerateBackchannelBehaviour(InputData data);
	/** 
    * It finds the communicative intentions the agent wants to transmit in the agent's state
    * @param time the time the backchannel has been requested
	* @param priority the priority of the backchannel
	* @return all the data neded by the selector to write an FML
    */
	DataBackchannel *ReactiveBackchannel::selectCommunicativeIntentions(int time, int priority);
	/** 
    * It randomly select the communicative intentions to transmit
    * @param vecResultCommFun the vector that contains all the communicative functions that
	* the agent actually transmits if we don't choose the comm func randomly the system
	* will ask always for the same set of comm func and Greta will display always the same signal
    */
	void ReactiveBackchannel::GetCommunicativeFunctions(std::vector<std::string> *vecResultCommFun);

};