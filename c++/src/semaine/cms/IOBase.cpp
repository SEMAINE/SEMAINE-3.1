/*
 *  iobase.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 29.08.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "IOBase.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <cms/ConnectionFactory.h>

using namespace semaine::cms;
using namespace cms;

IOBase::IOBase(const std::string & topicName)
throw (CMSException) :
	cmsUrl(DEFAULT_CMS_URL),
	cmsUser(DEFAULT_CMS_USER),
	cmsPassword(DEFAULT_CMS_PASSWORD)
{
	initialise(topicName);
}

IOBase::IOBase(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName)
throw (CMSException) :
	cmsUrl(cmsUrl),
	cmsUser(cmsUser),
	cmsPassword(cmsPassword)
{
	initialise(topicName);
}

IOBase::~IOBase()
{
	connection->stop();
	delete topic;
	delete session;
	delete connection;
}

void IOBase::initialise(const std::string & topicName)
throw (CMSException)
{
	ConnectionFactory* factory = new activemq::core::ActiveMQConnectionFactory(cmsUrl, cmsUser, cmsPassword);
	connection = factory->createConnection();
    delete factory;
	session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
	topic = session->createTopic(topicName);
}

void IOBase::startConnection()
throw (CMSException)
{
    connection->start();
	isConnectionStarted = true;
}

std::string IOBase::getTopicName()
throw (CMSException)
{
	return topic->getTopicName();
}

