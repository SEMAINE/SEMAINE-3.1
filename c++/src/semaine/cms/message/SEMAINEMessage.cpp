/*
 *  SEMAINEMessage.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SEMAINEMessage.h"

namespace semaine {
namespace cms {
namespace message {

const std::string SEMAINEMessage::USERTIME = "usertime";
const std::string SEMAINEMessage::DATATYPE = "datatype";
const std::string SEMAINEMessage::SOURCE = "source";
const std::string SEMAINEMessage::EVENT = "event";
const std::string SEMAINEMessage::PERIOD = "period";


	SEMAINEMessage::SEMAINEMessage(const Message * message)
	throw(MessageFormatException)
	{
		if (message == NULL)
			throw MessageFormatException("Message is null");
		this->message = message->clone();
		verifyProperties();
	}

	SEMAINEMessage::~SEMAINEMessage()
	{
		delete message;
	}

	void SEMAINEMessage::verifyProperties() throw(MessageFormatException)
	{
		try {
			getUsertime();
		} catch (CMSException & e) {
			throw MessageFormatException("Problem with message property '"+USERTIME+"': "+e.getStackTraceString());
		}
		try {
			getDatatype();
		} catch (CMSException & e) {
			throw MessageFormatException("Problem with message property '"+DATATYPE+"': "+e.getStackTraceString());
		}
		try {
			getSource();
		} catch (CMSException & e) {
			throw new MessageFormatException("Problem with message property '"+SOURCE+"': "+e.getStackTraceString());
		}
		try {
			if (!isPeriodic() && !isEventBased())
				throw MessageFormatException("Message contains neither periodic nor event-based header properties.");
		} catch (CMSException & e) {
			throw MessageFormatException("Problem determining whether properties '"+PERIOD+"' or '"+EVENT+"' exist: "+e.getMessage());
		}
	}







} // namespace message
} // namespace cms
} // namespace semaine

