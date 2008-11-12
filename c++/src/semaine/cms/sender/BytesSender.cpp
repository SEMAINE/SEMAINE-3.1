/*
 *  BytesSender.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 12.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "BytesSender.h"

#include <cms/BytesMessage.h>

using namespace cms;

namespace semaine {
namespace cms {
namespace sender {

BytesSender::BytesSender(const std::string & topicName, const std::string & datatype, const std::string & source)
throw(CMSException, SystemConfigurationException) :
	Sender(topicName, datatype, source)
{
}

BytesSender::BytesSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
	const std::string & datatype, const std::string & source, const std::string & topicName)
throw (CMSException, SystemConfigurationException) :
	Sender(cmsUrl, cmsUser, cmsPassword, topicName, datatype, source)
{
}


BytesSender::~BytesSender()
{
}


void BytesSender::sendBytesMessage(const unsigned char * bytes, std::size_t numBytes, long long usertime, const std::string & event)
throw(CMSException, SystemConfigurationException)
{
	if (!isConnectionStarted)
		throw SystemConfigurationException("Connection is not started!");
	BytesMessage * message = session->createBytesMessage();
	message->setBodyBytes(bytes, numBytes);
	fillMessageProperties(message, usertime);
	if (isPeriodic())
		message->setIntProperty(SEMAINEMessage::PERIOD, getPeriod());
	else // event-based
		message->setStringProperty(SEMAINEMessage::EVENT, event);
	producer->send(message);
	delete message;
}
} // namespace sender
} // namespace cms
} // namespace semaine

