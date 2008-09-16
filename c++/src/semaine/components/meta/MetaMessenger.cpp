/*
 *  MetaMessenger.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "MetaMessenger.h"

namespace semaine {
namespace components {
namespace meta {

	const std::string MetaMessenger::COMPONENT_NAME = "ComponentName";
	const std::string MetaMessenger::COMPONENT_STATE = "ComponentState";
	const std::string MetaMessenger::SYSTEM_READY = "SystemReady";

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


	void MetaMessenger::reportState(const std::string & state) throw(CMSException)
	{
		Message * m = session->createMessage();
		m->setStringProperty(COMPONENT_NAME, componentName);
		m->setStringProperty(COMPONENT_STATE, state);
		producer->send(m);
	}


	void MetaMessenger::onMessage(const Message * m)
	{
		try {
			assert(m->propertyExists(SYSTEM_READY)); // "message should contain header field '"+SYSTEM_READY+"'";
			setSystemReady(m->getBooleanProperty(SYSTEM_READY));
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




} // namespace meta
} // namespace components
} // namespace semaine

