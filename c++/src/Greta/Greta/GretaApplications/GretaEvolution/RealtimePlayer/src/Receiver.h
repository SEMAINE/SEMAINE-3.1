#pragma once

#include "Psydule.h"
#include "FramesManager.h"
#include "CommandsManager.h"
#include "JThread.h"
#include "CentralClock.h"
#include "../../../include/common/definitions.h"

using namespace cmlabs;



class Receiver : public JThread  
{
public:
	Receiver(Psydule *psydule,FramesManager* fm, CommandsManager* cm,CentralClock *pc,JString GretaName);
	virtual ~Receiver();
	void run();
	Psydule *psydule;
	FramesManager* fm;
	CommandsManager* cm;
	CentralClock *pc;
	int sound_time;
private:
	JString GretaName;

};
