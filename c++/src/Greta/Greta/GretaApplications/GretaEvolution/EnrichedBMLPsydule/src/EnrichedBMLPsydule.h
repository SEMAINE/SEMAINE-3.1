#pragma once

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include "BML_AgentEngine.h"
#include "Psydule.h"
#include "XMLGenericParser.h"

struct markerstruct{
	std::string name;
	float time;
};

struct phonemesstruct{
	std::string ph;
	float duration;
};

struct behaviortagsstruct{
	std::string channel;
	std::string id;
	std::string signalname;
	std::string start;
	std::string stroke;
	std::string end;
};


class EnrichedBMLPsydule : public Psydule
{
public:
	EnrichedBMLPsydule(JString name, JString host, int port);
	~EnrichedBMLPsydule();
	int ReadMessage(Message *triggerMessage);
	JString name,host;
	int port;
	int ComputeEnrichedBML(std::string enrichedbml);
	void PreVistTree(XMLGenericTree *t);
	std::vector<markerstruct> markers;
	std::vector<phonemesstruct> phonemes;
	std::vector<behaviortagsstruct> behaviortags;
	void SavePhonemes();
	std::string ComputeTimings();
	float globaltime;
	JString PrepareInternalBML(XMLGenericTree *t);
	void SendInternalBML(JString tosend);
	std::string ExtractMarkerName(std::string s);
	float GetMarkerTime(std::string s);
};


