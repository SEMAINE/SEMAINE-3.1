/*
 *  SEMAINEMessage.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SEMAINEMessage.h"

using namespace cms;

namespace semaine {
namespace cms {
namespace message {

const std::string SEMAINEMessage::USERTIME = "usertime";
const std::string SEMAINEMessage::DATATYPE = "datatype";
const std::string SEMAINEMessage::SOURCE = "source";
const std::string SEMAINEMessage::EVENT = "event";
const std::string SEMAINEMessage::PERIOD = "period";
const std::string SEMAINEMessage::CONTENT_ID = "content-id";
const std::string SEMAINEMessage::CONTENT_CREATION_TIME = "content-creation-time";
const std::string SEMAINEMessage::CONTENT_TYPE = "content-type";
const std::string SEMAINEMessage::CONTENT_TYPE_UTTERANCE = "utterance";
const std::string SEMAINEMessage::CONTENT_TYPE_LISTENERVOCALISATION = "listener-vocalisation";
const std::string SEMAINEMessage::CONTENT_TYPE_VISUALONLY = "visual-only";


	SEMAINEMessage::SEMAINEMessage(const Message * message)
	throw(semaine::cms::exceptions::MessageFormatException)
	{
		if (message == NULL)
			throw semaine::cms::exceptions::MessageFormatException("Message is null");
		this->message = message->clone();
		verifyProperties();
	}

	SEMAINEMessage::~SEMAINEMessage()
	{
		delete message;
	}

	void SEMAINEMessage::verifyProperties() throw(semaine::cms::exceptions::MessageFormatException)
	{
		try {
			getUsertime();
		} catch (CMSException & e) {
			throw semaine::cms::exceptions::MessageFormatException("Problem with message property '"+USERTIME+"': "+e.getStackTraceString());
		}
		try {
			getDatatype();
		} catch (CMSException & e) {
			throw semaine::cms::exceptions::MessageFormatException("Problem with message property '"+DATATYPE+"': "+e.getStackTraceString());
		}
		try {
			getSource();
		} catch (CMSException & e) {
			throw semaine::cms::exceptions::MessageFormatException("Problem with message property '"+SOURCE+"': "+e.getStackTraceString());
		}
		try {
			if (!isPeriodic() && !isEventBased())
				throw semaine::cms::exceptions::MessageFormatException("Message contains neither periodic nor event-based header properties.");
		} catch (CMSException & e) {
			throw semaine::cms::exceptions::MessageFormatException("Problem determining whether properties '"+PERIOD+"' or '"+EVENT+"' exist: "+e.getMessage());
		}
	}







} // namespace message
} // namespace cms
} // namespace semaine

