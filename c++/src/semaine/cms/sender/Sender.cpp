/*
 *  Sender.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 11.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "Sender.h"



namespace semaine {
namespace cms {
namespace sender {

	Sender::Sender(const std::string & topicName, const std::string & datatype, const std::string & source)
	throw (CMSException) :
	IOBase(topicName),
	datatype(datatype),
	source(source),
	period(0)
	{
		initialise();
	}

	Sender::Sender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName, const std::string & datatype, const std::string & source)
	throw (CMSException) :
	IOBase(cmsUrl, cmsUser, cmsPassword, topicName),
	datatype(datatype),
	source(source),
	period(0)
	{
		initialise();
	}


	void Sender::initialise()
	throw (CMSException)
	{
		producer = session->createProducer(topic);
		producer->setDeliveryMode(DeliveryMode::PERSISTENT);
	}


	void Sender::sendTextMessage(const std::string & text, long long usertime)
	throw(CMSException, SystemConfigurationException)
	{
		sendTextMessage(text, usertime, "", -1);
	}

	void Sender::sendTextMessage(const std::string & text, long long usertime, const std::string & contentID, long long contentCreationTime)
	throw(CMSException, SystemConfigurationException)
	{
		if (!isConnectionStarted)
			throw SystemConfigurationException("Connection is not started!");
		TextMessage * message = session->createTextMessage(text);
		fillMessageProperties(message, usertime, contentID, contentCreationTime);
		if (isPeriodic())
			message->setIntProperty(SEMAINEMessage::PERIOD, getPeriod());
		else // event-based
			message->setStringProperty(SEMAINEMessage::EVENT, SEMAINE_CMS_EVENT_SINGLE);
		producer->send(message);
		delete message;
	}

	void Sender::sendTextMessage(const std::string & text, long long usertime, const std::string & eventType)
	throw(CMSException, SystemConfigurationException)
	{
		sendTextMessage(text, usertime, eventType, "", -1);
	}

	void Sender::sendTextMessage(const std::string & text, long long usertime, const std::string & eventType, const std::string & contentID, long long contentCreationTime)
	throw(CMSException, SystemConfigurationException)
	{
		if (!isConnectionStarted)
			throw SystemConfigurationException("Connection is not started!");
		if (isPeriodic())
			throw SystemConfigurationException("This method is for event-based messages, but sender is in periodic mode.");
		TextMessage * message = session->createTextMessage(text);
		fillMessageProperties(message, usertime, contentID, contentCreationTime);
		message->setStringProperty(SEMAINEMessage::EVENT, eventType);
		producer->send(message);
		delete message;
	}



} // namespace sender
} // namespace cms
} // namespace semaine

