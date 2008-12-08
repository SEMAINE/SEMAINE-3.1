/*
 *  MetaMessenger.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "MetaMessenger.h"

#include <decaf/lang/System.h>

using namespace semaine::cms::receiver;
using namespace semaine::cms::sender;

namespace semaine {
namespace components {
namespace meta {

	const std::string MetaMessenger::COMPONENT_NAME = "ComponentName";
	const std::string MetaMessenger::COMPONENT_STATE = "ComponentState";
	const std::string MetaMessenger::COMPONENT_STATE_DETAILS = "ComponentStateDetails";
	const std::string MetaMessenger::SYSTEM_READY = "SystemReady";
	const std::string MetaMessenger::SYSTEM_READY_TIME = "SystemReadyTime";
	const std::string MetaMessenger::RECEIVE_TOPICS = "ReceiveTopics";
	const std::string MetaMessenger::SEND_TOPICS = "SendTopics";
	const std::string MetaMessenger::IS_INPUT = "IsInputComponent";
	const std::string MetaMessenger::IS_OUTPUT = "IsOutputComponent";

	MetaMessenger::MetaMessenger(const std::string & aComponentName) throw(CMSException) :
		IOBase("semaine.meta"),
		componentName(aComponentName),
		systemReady(false)
	{
		consumer = session->createConsumer(topic, SYSTEM_READY + " IS NOT NULL");
		producer = session->createProducer(topic);
		consumer->setMessageListener(this);
		startConnection();
	}


	void MetaMessenger::reportState(const std::string & state, const std::string & message, const std::exception * exc)
	throw(CMSException)
	{
		Message * m = session->createMessage();
		m->setStringProperty(COMPONENT_NAME, componentName);
		m->setStringProperty(COMPONENT_STATE, state);
		if (message.length() > 0) {
			m->setStringProperty(COMPONENT_STATE_DETAILS, CMSLogger::toLogMessageText(message, exc));
		}
		producer->send(m);
	}

	void MetaMessenger::reportTopics(std::list<Receiver *> & receivers,
			std::list<Sender *> & senders,
			bool isInput, bool isOutput)
	throw(CMSException)
	{
		std::stringstream rec;
	    std::list<Receiver *>::iterator i;
	    bool first = true;
		for (i = receivers.begin(); i != receivers.end(); i++) {
			Receiver * r = *i;
			if (!first) rec << " ";
			first = false;
			rec << r->getTopicName();
		}
		std::stringstream snd;
	    std::list<Sender *>::iterator j;
	    first = true;
		for (j = senders.begin(); j != senders.end(); j++) {
			Sender * s = *j;
			if (!first) snd << " ";
			first = false;
			snd << s->getTopicName();
		}
		Message * m = session->createMessage();
		m->setStringProperty(COMPONENT_NAME, componentName);
		m->setStringProperty(RECEIVE_TOPICS, rec.str());
		m->setStringProperty(SEND_TOPICS, snd.str());
		m->setBooleanProperty(IS_INPUT, isInput);
		m->setBooleanProperty(IS_OUTPUT, isOutput);
		producer->send(m);

	}

	void MetaMessenger::onMessage(const Message * m)
	{
		try {
			assert(m->propertyExists(SYSTEM_READY)); // "message should contain header field '"+SYSTEM_READY+"'";
			setSystemReady(m->getBooleanProperty(SYSTEM_READY));
			if (m->propertyExists(SYSTEM_READY_TIME)) {
				setTime(m->getLongProperty(SYSTEM_READY_TIME));
			}

		} catch (CMSException & e) {
			e.printStackTrace(std::cerr);
		}
	}


	bool MetaMessenger::isSystemReady()
	{
		synchronized(this) {
			return systemReady;
		}
		return false; // just to keep compiler happy
	}

	void MetaMessenger::setSystemReady(bool ready)
	{
		synchronized(this) {
			systemReady = ready;
			notify();
		}
	}

	void MetaMessenger::setTime(long long systemTime)
	{
		timeDelta = systemTime - decaf::lang::System::currentTimeMillis();
	}

	long long MetaMessenger::getTime()
	{
		return decaf::lang::System::currentTimeMillis() + timeDelta;
	}


} // namespace meta
} // namespace components
} // namespace semaine

