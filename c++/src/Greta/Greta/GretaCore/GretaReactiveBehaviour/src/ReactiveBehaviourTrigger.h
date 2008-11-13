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


// ReactiveBehaviourTrigger.h: interface for the ReactiveBehaviourTrigger class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "AgentState.h"
#include "ReactiveBackchannel.h"
#include "CentralClock.h"
#include "InputData.h"
#include "Mimicry.h"
#include "listener\AdaptRules.h"
#include "listener\BCRules.h"
#include "listener\ListenerData.h"
#include "aipersonmemory.h"
#include <iostream>
#include <map>
#include <utility>
#include "DataBackchannel.h"

/**
 *  Contains informations on the trigger.
 *  The trigger analyses info about the external events to decide 
 *  when a mimicry or a reactive backchannel must be triggered
 *  @author Elisabetta Bevacqua elisabetta.bevacqua@libero.it
 *  @date 2008
 */
class ReactiveBehaviourTrigger
{
public:
	/** 
    * Class default contructor.
    */
	ReactiveBehaviourTrigger();
	/** 
    * Class  contructor.
    * @param agentstate the object containing data about the agent's internal state
    */
	ReactiveBehaviourTrigger(AgentState *agentstate);
	/** 
    * Class destructor.
    */
	virtual ~ReactiveBehaviourTrigger();

	/**
	* Add a message to send into the Message buffer.
	* @param std::string mes the message to send
	*/
	int ReactiveBehaviourTrigger::addMessage(std::string mes);
	/** 
    * The list of messages about Input Data coming from the image analyser
    * 
    */
	std::list<std::string> messageList;
	/** 
    * It analyses the data coming from the image analyser and decide when they trig 
	* a backchannel signal, according to the rules and to the probability that a
	* user's behaviour generats a backchannel response
    * 
    */
	std::map<std::string,DataBackchannel*> ReactiveBehaviourTrigger::ReceiveMessages();

private:
	/** 
    * Each object of this class is a communicative function
	* it is used to create a vector containing the comm func
    */
	class CommFunction
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
    * It analyses head positions to understand when the head rotates toward a different zone
	* the space around the head is devided in three zone A, B and C according to the
	* angle of rotation
    * @param data the data containing the last head position tracked by the head
	* movement analyser. This function is constructed to works with WATSON
    */
	std::string ReactiveBehaviourTrigger::HeadMovement(InputData *data);
	/** 
    * It translate the user's behaviour in agent's behaviour
	* for example a smile for the user is a mouth=smile for BML for Greta
	* how to translate the behaviour is specified in an external file: listener/analysisrules.xml
    * 
    */
	void ReactiveBehaviourTrigger::CheckAdaptRules(InputData *data);
	/** 
    * It checks if the user's behaviour just analysed belongs to those that generate
	* a backchannel signals. Those behaviours are specified in the rules that
	* define also the probability that a user's behaviour generats a backchannel response
	* listener/rules.xml
    * 
    */
	int ReactiveBehaviourTrigger::CheckBCRules(InputData *data);


	std::map<std::string,DataBackchannel*> ReactiveBehaviourTrigger::CheckComplexRule(std::vector<std::string> *usersigVec);
	InputData *ReactiveBehaviourTrigger::AnalyseWatsonData(InputData *data);
	DataBackchannel *ReactiveBehaviourTrigger::DataForBML(BCComplexRules *rule);
	DataBackchannel *ReactiveBehaviourTrigger::DataForFML(int priority);
	void ReactiveBehaviourTrigger::GetCommunicativeFunctions(std::vector<std::string> *vecResultCommFun);

    /**
	*  An artificial intelligence personnal memories object. 
	*  Used to implement the compute the user's estimated interest level
	*  @author Christopher Peters
	*  @date 2005
	*/
	AIPersonMemory *aiPersonMemory;
	/** 
    * Contains info about the listener: analysis rules, backchannel rules, internal state
    * 
    */
	ListenerData* listenerdata;
	/** 
    * Contains info about the agent's state
    * 
    */
	AgentState *agentstate;
	/** 
    * It computes the necessary data to write a BML for a mimicry backchannel
    * 
    */
	Mimicry *mimicry;
	/** 
    * It computes the necessary data to write a FML for a reactive backchannel
    * 
    */
	ReactiveBackchannel *reactivebackchannel;

	/** 
    * The last head rotation computed, in degree
    * 
    */
	float lastheadDeg;
	/** 
    * The last head tilt computed, in degree
    * 
    */
	float lasttiltDegree;
	/** 
    * It contains the DataBackchannel to write a BML (for mimicry) and a FML
	* (for reactive) for the last backchannel generated by the analysis 
	* of a list of input messages (messageList)
    * 
    */
	std::map<std::string,DataBackchannel*> messageMap;
	std::map<std::string,DataBackchannel*> bmlfmlMap;

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
};
