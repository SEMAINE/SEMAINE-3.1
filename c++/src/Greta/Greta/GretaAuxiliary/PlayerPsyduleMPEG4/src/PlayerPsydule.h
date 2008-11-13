#pragma once

#include <iostream>
#include <fstream>
#include <Windows.h>
#include "BML_AgentEngine.h"
#include "Communicator.h"
#include "Component.h"
#include "CppAIRPlug.h"
#include "Collection.h"
#include "Psydule.h"


class PlayerPsydule : public Psydule
{
public:
	PlayerPsydule(JString name, JString host, int port);
	~PlayerPsydule();
	int PlayerRegister();
	JString name,host;
	int port;
	Message* msg;
	JString content;
	Message* triggerMessage;
	Message* multMessages;
};


