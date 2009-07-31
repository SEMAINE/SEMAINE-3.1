/*
 *  MetaMessenger.h
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_COMPONENTS_META_METAMESSENGER_H
#define SEMAINE_COMPONENTS_META_METAMESSENGER_H

#include <list>

#include <semaine/config.h>

#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>

#include <cms/MessageListener.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>

#include <semaine/cms/IOBase.h>
#include <semaine/cms/receiver/Receiver.h>
#include <semaine/cms/sender/Sender.h>

using namespace cms;
using namespace semaine::cms;

namespace semaine {
namespace components {
namespace meta {

class Statistics;

class MetaMessenger : public IOBase, public MessageListener, public decaf::util::concurrent::Mutex
{
public:
	// Properties sent by the Meta messenger:
	static const std::string COMPONENT_NAME;
	static const std::string COMPONENT_STATE;
	static const std::string COMPONENT_STATE_DETAILS;
	static const std::string RECEIVE_TOPICS;
	static const std::string SEND_TOPICS;
	static const std::string IS_INPUT;
	static const std::string IS_OUTPUT;
	static const std::string LAST_SEEN_ALIVE;
	static const std::string AVERAGE_ACT_TIME;
	static const std::string AVERAGE_REACT_TIME;
	static const std::string AVERAGE_TRANSMIT_TIME;
	static const std::string TOTAL_MESSAGES_RECEIVED;

	// Properties sent by the System manager:
	static const std::string SYSTEM_READY;
	static const std::string SYSTEM_READY_TIME;
	static const std::string PING;
	static const std::string REPORT_TOPICS;

	static const long long TIMEOUT_PERIOD;

	MetaMessenger(const std::string & componentName) throw(CMSException);

	void reportState(const std::string & state, const std::string & message = "", const std::exception * exc = NULL)
	throw(CMSException);

	void reportTopics(std::list<semaine::cms::receiver::Receiver *> & receivers,
			std::list<semaine::cms::sender::Sender *> & senders,
			bool isInput, bool isOutput)
	throw(CMSException);

	virtual void onMessage(const Message * m);

	bool isSystemReady();

	/**
	 * Get the current time in common, normalised time space.
	 * Processes should use only this method for determining the time.
	 * The value is counting milliseconds since some arbitrary point
	 * in time; therefore, the absolute time is not informative,
	 * but differences of time values are.
	 * @return
	 */
	long long getTime();

	void IamAlive();

	Statistics * statistics() { return _statistics; }


private:
	const std::string componentName;
	MessageProducer * producer;
	MessageConsumer * consumer;
	bool systemReady;
	long long timeDelta;
	long long lastSeenAlive;
	std::string lastSeenState;
	std::string receiveTopics;
	std::string sendTopics;
	bool isInput;
	bool isOutput;
	Statistics * _statistics;

	void setSystemReady(bool ready);
	void setTime(long long systemTime);
	void _reportTopics()
	throw(CMSException);

};


class Statistics
{
public:
	Statistics(int memory) : memory(memory), messagesReceived(0)
	{}

	void actTime(long millis);

	void reactTime(long millis);

	void transmitTime(long millis);

	void countMessageReceived() { messagesReceived++; }

	int getTotalMessagesReceived() { return messagesReceived; }

	long avgActTime() { return average(actTimes); }

	long avgReactTime() { return average(reactTimes); }

	long avgTransmitTime() { return average(transmitTimes); }

private:
	size_t memory;
	std::list<long> actTimes;
	std::list<long> reactTimes;
	std::list<long> transmitTimes;
	int messagesReceived;

	long average(std::list<long> & values);

};

} // namespace meta
} // namespace components
} // namespace semaine



#endif

