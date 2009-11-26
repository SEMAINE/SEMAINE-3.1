/*
 * StateSender.cpp
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#include "StateSender.h"

#include <semaine/cms/message/SEMAINEMessage.h>
#include <semaine/cms/event.h>

using namespace semaine::cms::exceptions;
using namespace semaine::datatypes::stateinfo;
using namespace semaine::util;
using namespace semaine::cms::message;
using namespace XERCES_CPP_NAMESPACE;

namespace semaine {

namespace cms {

namespace sender {

void StateSender::sendStateInfo(StateInfo * s, long long usertime, const std::string & contentID, long long contentCreationTime)
throw(CMSException, SystemConfigurationException)
{
	if (s == NULL)
		throw SystemConfigurationException("state passed as argument is null");
	DOMDocument * document = s->getDocument();
	if (document == NULL)
		throw SystemConfigurationException("document passed as argument is null");
	if (isPeriodic())
		throw SystemConfigurationException("state sender is expected to be event-based, not periodic");
	if (!isConnectionStarted)
		throw SystemConfigurationException("Connection is not started!");
	TextMessage * message = session->createTextMessage(XMLTool::dom2string(document));
	Sender::fillMessageProperties(message, usertime, contentID, contentCreationTime);
	message->setStringProperty(s->toString()+"APIVersion", s->getAPIVersion());
	message->setStringProperty(SEMAINEMessage::EVENT, SEMAINE_CMS_EVENT_SINGLE);
	producer->send(message);

}

}

}

}
