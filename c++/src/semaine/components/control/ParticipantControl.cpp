/*
 * ParticipantControl.cpp
 *
 *  Created on: 28.07.2009
 *      Author: marc
 */

#include <semaine/components/control/ParticipantControl.h>

#include <semaine/cms/message/SEMAINEStateMessage.h>
#include <semaine/datatypes/stateinfo/ContextStateInfo.h>

#include <decaf/lang/Thread.h>

#include <string>
#include <iostream>

using namespace cms;
using namespace semaine::cms::sender;
using namespace semaine::cms::receiver;
using namespace semaine::cms::message;
using namespace semaine::datatypes::stateinfo;

namespace semaine {

namespace components {

namespace control {


class StdinLineReader : public decaf::lang::Thread
{
public:
	StdinLineReader(ParticipantControl * pc)
	: pc(pc), exitRequested(false) {}

	void run() {
		std::string line;
		while (!exitRequested) {
			getline(std::cin, line);
			if (std::cin.eof()) {
				exitRequested = true;
				pc->requestExit();
			} else {
				std::string user = "";
				std::string character = "";
				if (line.find("user") != std::string::npos) {
					if (line.find("present") != std::string::npos) user = "present";
					else user = "absent";
				}
				if (line.find("Poppy") != std::string::npos) character = "Poppy";
				else if (line.find("Prudence") != std::string::npos) character = "Prudence";
				else if (line.find("Obadiah") != std::string::npos) character = "Obadiah";
				else if (line.find("Spike") != std::string::npos) character = "Spike";
				pc->sendContextInfo(user, character);
			}
		}
	}
private:
	ParticipantControl * pc;
	bool exitRequested;
};


ParticipantControl::ParticipantControl()
throw (CMSException)
: semaine::components::Component("ParticipantControlC++", false, false)
{
	contextSender = new StateSender("semaine.data.state.context", StateInfo::ContextState, getName());
	senders.push_back(contextSender);
	contextReceiver = new StateReceiver("semaine.data.state.context", StateInfo::ContextState);
	receivers.push_back(contextReceiver);
}

ParticipantControl::~ParticipantControl() {
}

void ParticipantControl::react(SEMAINEMessage * message) throw (std::exception)
{
	SEMAINEStateMessage * sm = dynamic_cast<SEMAINEStateMessage *>(message);
	if (sm == NULL) throw MessageFormatException(std::string("Expected state message, got a ")+typeid(*message).name());
	StateInfo * state = sm->getState();
	// Either like this:
	//std::string userPresent = state->getInfo("userPresent");
	//std::string character = state->getInfo("character");
	std::cout << "Received context message: ";
	// Or, more flexibly, like this:
	std::string values[] = {"userPresent", "character"};
	int n = 2;
	for (int i=0; i<n; i++) {
		if (state->hasInfo(values[i])) std::cout << " " << values[i] << "=" << state->getInfo(values[i]);
	}
	std::cout << std::endl;
}

void ParticipantControl::customStartIO() throw(std::exception)
{
	StdinLineReader * slr = new StdinLineReader(this);
	slr->start();
}

void ParticipantControl::sendContextInfo(const std::string & userPresent, const std::string & character)
{
	std::map<std::string, std::string> info;
	bool haveSomething = false;
	if (userPresent == "present" || userPresent == "absent") {
		info["userPresent"] = userPresent;
		haveSomething = true;
	}
	if (character == "Spike" || character == "Poppy" || character == "Prudence" || character == "Obadiah") {
		info["character"] = character;
		haveSomething = true;
	}
	if (haveSomething) {
		ContextStateInfo csi(info);
		contextSender->sendStateInfo(&csi, meta.getTime());
	}
}




}

}

}
