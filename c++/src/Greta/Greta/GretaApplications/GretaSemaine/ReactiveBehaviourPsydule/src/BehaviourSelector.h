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


// BehaviourSlector.h: interface for the BehaviourSlector class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#if !defined(BEHAVIOURSELECTOR_H)
#define _BEHAVIOURSELECTOR_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include "AgentBehaviour.h"
#include "AgentState.h"
#include "CentralClock.h"
#include "JThread.h"
#include "Selector.h"
#include "StrictPrioritySelector.h"
#include "RandomSelector.h"
#include "XMLGenericTree.h"
#include "RandomGen.h"
#include "DataBackchannel.h"


using namespace cmlabs;

class ReactiveBehaviourPsydule;

/**
 *  Contains informations on the backchannel selector.
 *  The selector must decide which backchannel signal
 *  must be performed.
 *  It decides among mimicry, reactive and cognitive backchannel
 *  @author Elisabetta Bevacqua elisabetta.bevacqua@libero.it
 *  @date 2008
 */
class BehaviourSelector : public JThread
{
public:

	/** 
    * Class default contructor.
    * 
    */
	BehaviourSelector();
	/** 
    * Class contructor.
    * @param rbp the father object
	* @param pc the system central clock
	* @param as the agent state
    */
	BehaviourSelector(ReactiveBehaviourPsydule *rbp, CentralClock *pc, AgentState *as);
	/** 
    * Class destructor.
    * 
    */
	virtual ~BehaviourSelector();
	/** 
    * The thread run function
    * 
    */
	void BehaviourSelector::run();
	/** 
    * It update the backchannel containers (for mimicry, reactive and
	* cognitive). Here the last triggered backchannel with
	* higher priority is saved and deleted when emitted or when
	* another kind of backchannel is emitted.
	* Triggered backchannel that doesn't become signals must
	* be deleted since they become old
    * 
    */
	int BehaviourSelector::updateBackchannelRequest(DataBackchannel *db);
	/** 
    * It looks for the right container according to the type of the backchannel
	* to see if it contains a triggered backchannel
    * @param type the type of the backchannel MIMICRY, REACTIVE or COGNITIVE
	* @return a DataBackchannel that contains necessary info to write a BML or a FML
    */
	DataBackchannel* BehaviourSelector::FindRequest(std::string type);
	/** 
    * It select which type of backchannel signal must be emitted
	* REACTIVE or MIMICRY. The choise depends on the agent's mimicry
	* level defined in the agent's state
	* @return backchannel written in BML or in FML
    */
	std::string BehaviourSelector::selectBackchannelType();
	/** 
    * It returns the backchannel in BML or FML format 
    * 
    */
	std::string BehaviourSelector::retrieveBackchannelrequest(std::string type);

	/** 
    * It writes the BML for mimicry backchannel
	* @param db the DataBackchannel containing the necessary information to write 
	* the BML
    * 
    */
	std::string BehaviourSelector::WriteBML(DataBackchannel *db);
	std::string BehaviourSelector::WriteSignalInBML(std::string modality, std::string reference, int time, int index);
	/** 
    * It writes the FML for reactive backchannel
	* @param db the DataBackchannel containing the necessary information to write 
	* the FML
    * 
    */
	std::string BehaviourSelector::WriteFML(DataBackchannel *db);
	/** 
    * Contains info about the agent's state
    * 
    */
	AgentState *agentstate;
	/** 
    * Mimicry level in the agent's state, it is used to decide
	* between mimicry and reactive backchannel 
    * 
    */
	float mimicryLevel;
	/** 
    * The object to computes random values
    * 
    */
	RandomGen *randgen;
	/** 
    * Mimicry container: it contains the data about the last triggered mimicry backchannel
	* not sent yet
    * 
    */
	DataBackchannel *mimicry;
	/** 
    * Reactive container: it contains the data about the last triggered reactive backchannel
	* not sent yet
    * 
    */
	DataBackchannel *reactive;
	/** 
    * Cognitive container: it contains the data about the last triggered cognitive backchannel
	* not sent yet
    * 
    */
	DataBackchannel *cognitive;
	/** 
    * Last set backchannel container: it contains the data about the last sent backchannel
    * 
    */
	DataBackchannel *lastBackchannelSent;
	/*
	* System central clock
	*
	*/
	CentralClock *pc;
    /*
	* The father object 
	*
	*/
	ReactiveBehaviourPsydule *rbp;
private :
	/** 
    * Name of the specific current Greta (in case of multiple Gretas)
    * 
    */
	JString GretaName;
};

#endif

