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
	cmsPassword(DEFAULT_CMS_PASSWORD),
	exception(NULL),
	excListener(NULL)
{
	initialise(topicName);
}

IOBase::IOBase(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName)
throw (CMSException) :
	cmsUrl(cmsUrl),
	cmsUser(cmsUser),
	cmsPassword(cmsPassword),
	exception(NULL),
	excListener(NULL)
{
	initialise(topicName);
}

IOBase::~IOBase()
{
	connection->close();
	delete topic;
	delete session;
	delete connection;
	if (exception != NULL) {
		delete exception;
	}
	delete excListener;
}

void IOBase::initialise(const std::string & topicName)
throw (CMSException)
{
	ConnectionFactory* factory = new activemq::core::ActiveMQConnectionFactory(cmsUrl, cmsUser, cmsPassword);
	connection = factory->createConnection();
	delete factory; // <-- NOTE: this memory is NOT freed, if an exception is thrown by createConnection()

	// Send asynchronously, so that even in the case of a flooded broker, the send() returns,
	// and we can detect the error condition (ExceptionListener sets exception, detected at next send())
	// TODO: check if this can be set using a newer version of ActiveMQ-CPP:
	// ((ActiveMQConnection)connection).setUseAsyncSend(true);
	excListener = new ExcListener(this);
	connection->setExceptionListener(excListener);
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

void IOBase::setException(const CMSException & ex) {
	exception = &ex;
}
