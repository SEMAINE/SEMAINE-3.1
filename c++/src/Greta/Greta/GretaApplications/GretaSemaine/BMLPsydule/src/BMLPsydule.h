#pragma once

#include <iostream>
#include <fstream>
#include <Windows.h>
#include "BML_AgentEngine.h"
#include "Psydule.h"


class BMLPsydule : public Psydule
{
public:
	BMLPsydule(JString name, JString host, int port);
	~BMLPsydule();
	int ReadMessage(Message *triggerMessage);
	JString name,host;
	int port;
};


