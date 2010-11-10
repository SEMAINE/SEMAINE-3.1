/*
 *  CMSLogger.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "CMSLogger.h"

#include <iostream>
#include <cassert>
#include <sstream>

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <cms/ConnectionFactory.h>
#include <cms/Topic.h>
#include <cms/CMSException.h>

using namespace semaine::cms;
using namespace cms;

/////////////////// Static stuff //////////////////////

Connection * CMSLogger::connection = NULL;
Session * CMSLogger::session = NULL;
std::map<const std::string, CMSLogger *> CMSLogger::loggers;

CMSLogger * CMSLogger::getLog(const std::string& source)
{
	CMSLogger * l = loggers[source];
	if (l == NULL) {
		// First, check if we need to set up connection
		if (connection == NULL) {
		    try {
				ConnectionFactory* factory = new activemq::core::ActiveMQConnectionFactory(DEFAULT_CMS_URL, DEFAULT_CMS_USER, DEFAULT_CMS_PASSWORD);
				connection = factory->createConnection();
				delete factory;
				session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
				connection->start();
				assert(connection != NULL);
				assert(session != NULL);
			} catch(CMSException & exc) {
				std::cerr << "Cannot set up CMS logger connection and session: " << exc.getStackTraceString() << std::endl;
			}
		}
		l = new CMSLogger(source);
		loggers[source] = l;
	}
	assert(l != NULL);
	return l;
}


//////////////////////// Member stuff ///////////////////////

CMSLogger::CMSLogger(const std::string & source) :
	source(source)
{
	std::string basename = "semaine.log."+source;
	try {
	    if (session != NULL) {
			Topic * t = session->createTopic(basename+".error");
			errorMP = session->createProducer(t);
			delete t;
			t = session->createTopic(basename+".warn");
			warnMP = session->createProducer(t);
			delete t;
			t = session->createTopic(basename+".info");
			infoMP = session->createProducer(t);
			delete t;
			t = session->createTopic(basename+".debug");
			debugMP = session->createProducer(t);
			delete t;
		}
	} catch (CMSException & e) {
		std::cerr << "Cannot set up CMS log connections for "+source+"; will use fallback instead" << std::endl;
		e.printStackTrace();
	}
}


CMSLogger::~CMSLogger()
{
	loggers.erase(source); // remove me from map
	if (errorMP != NULL) {
	    // assume all are non-null
		delete errorMP;
		delete warnMP;
		delete infoMP;
		delete debugMP;
	}
}

	void CMSLogger::error(const std::string & message, const std::exception * exc)
	{
		log(errorMP, SEMAINE_CMS_LOGLEVEL_ERROR, message, exc);
	}

	void CMSLogger::warn(const std::string & message, const std::exception * exc)
	{
		log(warnMP, SEMAINE_CMS_LOGLEVEL_WARN, message, exc);
	}

	void CMSLogger::info(const std::string & message, const std::exception * exc)
	{
		log(infoMP, SEMAINE_CMS_LOGLEVEL_INFO, message, exc);
	}

	void CMSLogger::debug(const std::string & message, const std::exception * exc)
	{
		log(debugMP, SEMAINE_CMS_LOGLEVEL_DEBUG, message, exc);
	}

std::string CMSLogger::toLogMessageText(const std::string & message, const std::exception *exc)
{
    std::string logMessageText;
    if (exc == NULL) {
		    logMessageText = message;
		} else {
			const CMSException * cmsExc = dynamic_cast<const CMSException *>(exc);
			if (cmsExc != NULL) {
				logMessageText = message + " " + cmsExc->getMessage() + " " + cmsExc->getStackTraceString();
			} else { // another exception
				logMessageText = message + " " + exc->what();
			}
		}
    return logMessageText;
}

std::string CMSLogger::message2logString(SEMAINEMessage * m)
{
	try {
		std::stringstream buf;
		buf << "[" << m->getUsertime() << "] ";
		buf << m->getDatatype() << " ";
		buf << m->getSource() << "->" << m->getTopicName() << " ";
		if (m->isEventBased()) {
			buf << m->getEventType() << " event ";
		} else {
			buf << "period=" << m->getPeriod() << " ";
		}
		const Message * message = m->getMessage();
		const TextMessage * tm = dynamic_cast<const TextMessage *>(message);
		if (tm != NULL) {
			buf << "\n";
			buf << tm->getText();
			buf << "\n";
		} else {
			buf << typeid(*message).name();
			buf << "\n";
		}
		return buf.str();
	} catch (CMSException & je) {
		return toLogMessageText("Exception trying to log message content: ", &je);
	}


}

void CMSLogger::log(MessageProducer * mp, const std::string & level, const std::string & message, const std::exception * exc)
	{
		std::string logMessageText = toLogMessageText(message, exc);
	    try {
			if (session != NULL) {
				TextMessage * message = session->createTextMessage(logMessageText);
				mp->send(message);
				delete message;
			} else {
				std::cerr << "[" << level << "] " << logMessageText << std::endl;
			}
		} catch (CMSException & e) {
		    std::cerr << "problem with CMS logger: " << e.getStackTraceString() << std::endl;
			std::cerr << "[" << level << "] " << logMessageText << std::endl;
		}

	}






bool CMSLogger::isDebugEnabled()
{
	// in the future, we may be able to use advisory information to know
	// if anyone is listening to this topic
	return true;
}

