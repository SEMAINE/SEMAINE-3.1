#pragma once

#include <iostream>
#include <fstream>
#include <Windows.h>
#include "APML_AgentEngine.h"
#include "Psydule.h"


class APMLPsydule : public Psydule
{
public:
	APMLPsydule(JString name, JString host, int port);
	~APMLPsydule();
	int ReadMessage(Message *triggerMessage);
	JString name,host;
	int port;
/*	int SendFAPs(std::string fapfilename);
	int SendBAPs(std::string bapfilename);
	int SendAudio(std::string audiofilename);*/
};


