#pragma once

#include <iostream>
#include <utility>
#include "Psydule.h"
#include "JString.h"
#include "../../../include/Common/definitions.h"

using namespace cmlabs;

// Call to the engine class to allowed reference to it.
class GretaReactiveBehaviour;

class ReactiveBackchannelSender
{
public:
	// default constructor
	ReactiveBackchannelSender();
	/// Used constructor
	ReactiveBackchannelSender(Psydule *psydule, GretaReactiveBehaviour *grb);
	// destructor
	~ReactiveBackchannelSender();

	// Methods
	void execute();
	std::string ReactiveBackchannelSender::selectCommunicativeIntentions();
	void ReactiveBackchannelSender::SendBackchannelFML(std::string fml);

	// Attributes
	Psydule *psydule;

	// The owner of this object
	GretaReactiveBehaviour *grb;
	
private:

};