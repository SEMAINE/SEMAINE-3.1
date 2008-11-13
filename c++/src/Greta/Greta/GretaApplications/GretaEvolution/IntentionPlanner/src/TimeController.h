#pragma once

#include "JThread.h"
#include "CentralClock.h"
#include "InputData.h"
#include <iostream>
#include <utility>


using namespace cmlabs;
// Call to the engine class to allowed reference to it.
class GretaReactiveBehaviour ;

class TimeController : public JThread  
{
public:
	// default constructor
	TimeController();
	// constructor to use
	TimeController(GretaReactiveBehaviour *grb);
	// destructor
	virtual ~TimeController();
	void run();
	void TimeController::SetBCrequest();

	// The owner of this object
	GretaReactiveBehaviour *grb;

		

private:

	int bcrequest;
	int timelimit;
};
