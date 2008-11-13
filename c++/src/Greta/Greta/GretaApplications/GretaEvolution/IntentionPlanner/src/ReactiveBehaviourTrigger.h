#pragma once

#include "JThread.h"
#include "CentralClock.h"
#include "InputData.h"
#include "listener\AdaptRules.h"
#include "listener\BCRules.h"
#include "aipersonmemory.h"
#include <iostream>
#include <utility>


using namespace cmlabs;
// Call to the engine class to allowed reference to it.
class GretaReactiveBehaviour;

class ReactiveBehaviourTrigger// : public JThread  
{
public:
	// default constructor
	ReactiveBehaviourTrigger();
	// constructor to use
	ReactiveBehaviourTrigger(GretaReactiveBehaviour *grb);
	// destructor
	virtual ~ReactiveBehaviourTrigger();
//	void run();

	/**
	* Add a message to send into the Message buffer.
	* @param std::string mes the message to send
	*/
	int ReactiveBehaviourTrigger::addMessage(std::string mes);

	void ReactiveBehaviourTrigger::messages();

	//void ReactiveBehaviourTrigger::setActiveThread();
	//void ReactiveBehaviourTrigger::setContent(JString cont);

	// The owner of this object
	GretaReactiveBehaviour *grb;

	

private:
	std::string ReactiveBehaviourTrigger::HeadMovement(InputData *data);
	void ReactiveBehaviourTrigger::CheckAdaptRules(InputData *data);
	std::string ReactiveBehaviourTrigger::CheckBCRules(InputData *data);
	float ReactiveBehaviourTrigger::ComputeBCprobability(std::string event);
	
	//EventAnalyser EA;
	AIPersonMemory *aiPersonMemory;
	
	//int activethread;
	//JString content;	
	float lastheadDeg;
	float lastangularspeed;
	int lastheadrotationtime;

	std::list<std::string> messageList;
};
