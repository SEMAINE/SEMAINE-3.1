#pragma once

#include <iostream>
#include <utility>
//#include "JThread.h"
#include "Psydule.h"
#include "JString.h"
#include "InputData.h"
#include "../../../include/Common/definitions.h"

using namespace cmlabs;

// Call to the engine class to allowed reference to it.
class GretaReactiveBehaviour;

class MimicrySender //: public JThread  
{
public:
	// default constructor
	MimicrySender();
	/// Used constructor
	MimicrySender(Psydule *psydule, GretaReactiveBehaviour *grb);
	// destructor
	~MimicrySender();

	// Methods
//	void run();
	int running();
	int MimicrySender::addInputData(InputData indata);
	std::string MimicrySender::CalulateBML(InputData data, int time);
	void MimicrySender::SendMimicryBML(std::string bml);

	// Attributes
	Psydule *psydule;

	// The owner of this object
	GretaReactiveBehaviour *grb;

private:

	int senttime;
	std::list<InputData> inputDataList;
	//InputData data;
};