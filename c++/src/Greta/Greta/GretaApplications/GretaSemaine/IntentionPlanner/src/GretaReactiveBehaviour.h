#pragma once

#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <utility>
#include "DataContainer.h"
#include "Psydule.h"
#include "CentralClock.h"
#include "../../../include/Common/definitions.h"
#include "ReactiveBackchannelSender.h"
#include "MimicrySender.h"
#include "AgentState.h"
#include "ReactiveBehaviourTrigger.h"
#include "TimeController.h"
#include "listener/ListenerData.h"

class GretaReactiveBehaviour  
{
public:
	// default constructor
	GretaReactiveBehaviour();
	// destructor
	~GretaReactiveBehaviour();

	// Methods :
	// Initialise the application
	int initApplication();
	// Initialise the receiver interfaces
	int initRegistration();
	// Initialise the backchannel sender interfaces
	int initReactiveBackchannelSender();
	// Initialise the mimicry signal sender interfaces
	int initMimicrySender();
	// Receive messages and launch the right thread to treat data
	int GretaReactiveBehaviour::ReadMessage(Message *msg);
	int GretaReactiveBehaviour::ReadInputMessages();
	// Initialise the reactive backchannel trigger
	int GretaReactiveBehaviour::initReactiveBehaviourTrigger();
	// Initialise the time controller that check time passing between backchannels
	int GretaReactiveBehaviour::initTimeController();
	// Initialise the agent state
	int GretaReactiveBehaviour::initAgentState();
	// The execution method
	int run();
	// The quit method
	int quit();

	// Getters
	CentralClock* getpCentralClock();
	AgentState* getpAgentState();
	int getLastBCTime();
	ReactiveBackchannelSender* getReactiveBackchannelSender();
	MimicrySender* getMimicrySender();

	ListenerData* listenerdata;
	//Contains info about the agent's state
	AgentState agentstate;

private:
	// Attributes :
	// The FML receiver 
//	IPreceiver *ipreceiver ;
	// The psyclone module adress
	JString psyduleHost,GretaName;	
	// The psyclone module port
	int psydulePort;
	// The Psydule Input interface
	Psydule *psydule;
	// The Psydule Greta interface
	Psydule *psyduleInput;
	// Computer central clock
	CentralClock pc;
	// The backchannel FML sender
	ReactiveBackchannelSender reactivebackchannelsender;
	// The mimicry signal sender
	MimicrySender mimicrysender;
	//Generats backchannel requests
	ReactiveBehaviourTrigger reactivebehaviourtrigger;
	//Check time passing
	TimeController timecontroller;
	

	//time of the last performed backchannel
	int lastbctime;
	
};