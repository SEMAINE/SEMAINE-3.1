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
#include <semaine/cms/message/SEMAINEMessage.h>

#include <semaine/components/Component.h>

using namespace semaine::cms::receiver;
using namespace semaine::cms::sender;
using namespace semaine::components;

namespace semaine {
namespace components {
namespace meta {

    // Properties sent by the Meta messenger:
	const std::string MetaMessenger::COMPONENT_NAME = "ComponentName";
	const std::string MetaMessenger::COMPONENT_STATE = "ComponentState";
	const std::string MetaMessenger::COMPONENT_STATE_DETAILS = "ComponentStateDetails";
	const std::string MetaMessenger::RECEIVE_TOPICS = "ReceiveTopics";
	const std::string MetaMessenger::SEND_TOPICS = "SendTopics";
	const std::string MetaMessenger::IS_INPUT = "IsInputComponent";
	const std::string MetaMessenger::IS_OUTPUT = "IsOutputComponent";
	const std::string MetaMessenger::LAST_SEEN_ALIVE = "LastSeenAlive";
	const std::string MetaMessenger::AVERAGE_ACT_TIME = "AverageActTime";
	const std::string MetaMessenger::AVERAGE_REACT_TIME = "AverageReactTime";
	const std::string MetaMessenger::AVERAGE_TRANSMIT_TIME = "AverageTransmitTime";
	const std::string MetaMessenger::TOTAL_MESSAGES_RECEIVED = "TotalMessagesReceived";
	// Properties sent by the System manager:
	const std::string MetaMessenger::SYSTEM_READY = "SystemReady";
	const std::string MetaMessenger::SYSTEM_READY_TIME = "SystemReadyTime";
	const std::string MetaMessenger::PING = "Ping";
	const std::string MetaMessenger::REPORT_TOPICS = "DoReportTopics";

	const long long MetaMessenger::TIMEOUT_PERIOD = 3000; // ms

	MetaMessenger::MetaMessenger(const std::string & aComponentName) throw(CMSException) :
		IOBase("semaine.meta"),
		componentName(aComponentName),
		systemReady(false),
		lastSeenAlive(0),
		lastSeenState("")
	{
		consumer = session->createConsumer(topic, semaine::cms::message::SEMAINEMessage::SOURCE + " = 'SystemManager'");
		producer = session->createProducer(topic);
		consumer->setMessageListener(this);
		startConnection();
		_statistics = new Statistics(30);
	}

	void MetaMessenger::IamAlive()
	{
		if (timeDelta != 0) { // only set time when we have a meaningful system time
			lastSeenAlive = getTime();
		}
		lastSeenState = Component::STATE_READY;
	}

	void MetaMessenger::reportState(const std::string & state, const std::string & message, const std::exception * exc)
	throw(CMSException)
	{
		Message * m = session->createMessage();
		m->setStringProperty(COMPONENT_NAME, componentName);
		lastSeenState = state;
		m->setStringProperty(COMPONENT_STATE, state);
		if (message.length() > 0) {
			m->setStringProperty(COMPONENT_STATE_DETAILS, CMSLogger::toLogMessageText(message, exc));
		}
		producer->send(m);
		delete m;
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
		receiveTopics = rec.str();
		sendTopics = snd.str();
		this->isInput = isInput;
		this->isOutput = isOutput;
		_reportTopics();
	}

	void MetaMessenger::_reportTopics() throw(CMSException)
	{
		Message * m = session->createMessage();
		m->setStringProperty(COMPONENT_NAME, componentName);
		m->setStringProperty(RECEIVE_TOPICS, receiveTopics);
		m->setStringProperty(SEND_TOPICS, sendTopics);
		m->setBooleanProperty(IS_INPUT, isInput);
		m->setBooleanProperty(IS_OUTPUT, isOutput);
		producer->send(m);
		delete m;

	}

	void MetaMessenger::onMessage(const Message * m)
	{
		try {
			if (m->propertyExists(SYSTEM_READY)) {
				setSystemReady(m->getBooleanProperty(SYSTEM_READY));
			}
			if (m->propertyExists(SYSTEM_READY_TIME)) {
				setTime(m->getLongProperty(SYSTEM_READY_TIME));
			}
			if (m->propertyExists(PING)) {
				Message * reply = session->createMessage();
				reply->setStringProperty(COMPONENT_NAME, componentName);
				// Do some sensibility checks to see if the component really seems alive
				if (lastSeenState == Component::STATE_READY && systemReady) {
					long long time = getTime();
					if (lastSeenAlive > time // maybe an outdated value after a clock reset?
							|| time - lastSeenAlive > TIMEOUT_PERIOD) {
						lastSeenState = Component::STATE_STALLED;
					}
				}
				reply->setStringProperty(COMPONENT_STATE, lastSeenState);
				reply->setLongProperty(LAST_SEEN_ALIVE, lastSeenAlive);
				long val = _statistics->avgActTime();
				if (val != -1) reply->setLongProperty(AVERAGE_ACT_TIME, val);
				val = _statistics->avgReactTime();
				if (val != -1) reply->setLongProperty(AVERAGE_REACT_TIME, val);
				val = _statistics->avgTransmitTime();
				if (val != -1) reply->setLongProperty(AVERAGE_TRANSMIT_TIME, val);
				reply->setIntProperty(TOTAL_MESSAGES_RECEIVED, _statistics->getTotalMessagesReceived());
				producer->send(reply);
				delete reply;
			}
			if (m->propertyExists(REPORT_TOPICS)) {
				_reportTopics();
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


	void Statistics::actTime(long millis)
	{
		actTimes.push_back(millis);
		if (actTimes.size() > memory)
			actTimes.pop_front();
	}

	void Statistics::reactTime(long millis)
	{
		reactTimes.push_back(millis);
		if (reactTimes.size() > memory)
			reactTimes.pop_front();
	}

	void Statistics::transmitTime(long millis)
	{
		transmitTimes.push_back(millis);
		if (transmitTimes.size() > memory)
			transmitTimes.pop_front();
	}

	long Statistics::average(std::list<long> & values)
	{
		long total = 0;
		int size = values.size();
		if (size == 0) return -1;
		std::list<long>::iterator it;
		for (it = values.begin(); it != values.end(); it++) {
			total += (*it);
		}
		return total/size;
	}

} // namespace meta
} // namespace components
} // namespace semaine

