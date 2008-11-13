#pragma once

#include <vector>
//#include "JString.h"
//#include "JThread.h"
//#include "JSemaphore.h"
#include "FAPFrame.h"
#include "BAPframe.h"
#include "CentralClock.h"

#include <decaf/util/concurrent/Mutex.h>
using namespace decaf::util::concurrent;
//using namespace cmlabs;

class RTPlayerCommand
{
public:
	bool operator<(RTPlayerCommand& c){if(this->time<c.time)return true; else return false;}
	std::string name;
	std::string arguments;
	int time;
};

class CommandsManager
{
public:

	CommandsManager(CentralClock *pc);
	virtual ~CommandsManager();
	

	void AddCommand(std::string name,std::string arguments);
	bool GetCommand(std::string &returnc);

private:
	CentralClock *pc;

	//JMutex mutex;
	Mutex mutex;

	std::vector<RTPlayerCommand> commands;
};