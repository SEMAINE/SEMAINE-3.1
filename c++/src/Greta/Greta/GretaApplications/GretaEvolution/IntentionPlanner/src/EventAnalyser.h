// EventAnalyser.h: interface for the EventAnalyser class.
//
//////////////////////////////////////////////////////////////////////

#pragma once



#include <iostream>
#include <fstream>
#include <Windows.h>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include "Psydule.h"
#include "InputData.h"
#include "RandomGen.h"
#include "XMLGenericTree.h"
#include "listener\BCRules.h"

/*! \brief generation of listener's reactive backchannel signals
	*
	* this is the main object for generate listener's reactive backchannel
	* signals. Responses are selected according to a set of rules and
	* adio and visual data in input that give information about the 
	* speaker's behaviour.
	* The type of the triggering data from Psyclone is Greta.Data.Input 
	* Messages come as standard http requests.
	* This class inherits from Psydule class to use all the functions
	* needed to communicate with a Psyclone whiteboared
	*
*/

// Call to the engine class to allowed reference to it.
class GretaReactiveBehaviour;


class Event
{
public:
	Event();
	virtual ~Event();

	std::string name;
	std::string stringvalue;
	float floatvalue;
	float time;


	void Event::SetName(std::string n);
	void Event::SetStringValue(std::string sv);
	void Event::SetFloatValue(float fv);
	void Event::SetTime(float t);

	std::string Event::GetName();
	std::string Event::GetStringValue();
	float Event::GetFloatValue();
	float Event::GetTime();
};


class EventAnalyser
{
public:
	EventAnalyser(GretaReactiveBehaviour *grb);
	virtual ~EventAnalyser();

	double last_head_rotation[3];
	double new_head_rotation[3];

	int last_rotation_time; //JTime
	int time_head_still;
	
	int EventAnalyser::LoadRulesList(std::string filepath);
	InputData* EventAnalyser::RulesAnalysis(InputData *data);
	
	int EventAnalyser::AnalyseHeadPosition(InputData *data);
	float EventAnalyser::ComputeBackchannelProbability(InputData *data);

	std::map<std::string, BCRules> mapBackchannelRules;

	// The owner of this object
	GretaReactiveBehaviour *grb;

};
