#pragma once

#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <utility>
#include "Psydule.h"
#include "../../../include/Common/definitions.h"

// Call to the engine class to allowed reference to it.
class GretaReactiveBehaviour;

class ReceiverInput  
{
public:
	// default constructor
	ReceiverInput();
	ReceiverInput(GretaReactiveBehaviour *grb);
	// destructor
	~ReceiverInput();

	// Methods :

	// Initialise the receiver interfaces
	int initRegistrationInputWhiteboard();
	// Receive messages and launch the right thread to treat data
	int ReceiverInput::ReadMessage();

	// The owner of this object
	GretaReactiveBehaviour *grb;

private:
	// Attributes :
	// The psyclone module adress
	JString psyduleHost;	
	// The psyclone module port
	int psydulePort;
	// The Psydule Input interface
	Psydule *psyduleInput;
};