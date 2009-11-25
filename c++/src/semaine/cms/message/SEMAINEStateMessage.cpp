/*
 * SEMAINEStateMessage.cpp
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#include "SEMAINEStateMessage.h"

#include <semaine/datatypes/stateinfo/AgentStateInfo.h>
#include <semaine/datatypes/stateinfo/UserStateInfo.h>
#include <semaine/datatypes/stateinfo/DialogStateInfo.h>
#include <semaine/datatypes/stateinfo/ContextStateInfo.h>
#include <semaine/datatypes/stateinfo/SystemStateInfo.h>

using namespace cms;
using namespace semaine::datatypes::stateinfo;
using namespace semaine::cms::exceptions;
using namespace XERCES_CPP_NAMESPACE;

namespace semaine {

namespace cms {

namespace message {

SEMAINEStateMessage::SEMAINEStateMessage(const Message * message, StateInfo::Type stateInfoType)
throw(SystemConfigurationException, MessageFormatException)
: SEMAINEXMLMessage(message), stateInfoType(stateInfoType), state(NULL)
{
	try {
		state = createState(document);
		log = CMSLogger::getLog(state->toString());
		std::string messageVersion = message->getStringProperty(state->toString()+"APIVersion");
		if (state->getAPIVersion() != messageVersion) {
			log->warn("API versions differ: I have '"+state->getAPIVersion()+
					"', message from "+message->getStringProperty(SEMAINEMessage::SOURCE)+
					" has '"+messageVersion+"'. Some information may be lost.");
		}
	} catch (CMSException &e) {
		e.printStackTrace();
		throw MessageFormatException("Problem reading message");
	}
}

SEMAINEStateMessage::~SEMAINEStateMessage() {
	if (state != NULL) {
		delete state;
		state = NULL;
	}
}

StateInfo * SEMAINEStateMessage::createState(DOMDocument * doc)
throw(MessageFormatException)
{
	switch (stateInfoType) {
	case StateInfo::AgentState: return new AgentStateInfo(doc);
	case StateInfo::DialogState: return new DialogStateInfo(doc);
	case StateInfo::UserState: return new UserStateInfo(doc);
	case StateInfo::ContextState: return new ContextStateInfo(doc);
	case StateInfo::SystemState: return new SystemStateInfo(doc);
	}
	throw MessageFormatException("Unknown state info type: "+stateInfoType);
}




}

}

}
