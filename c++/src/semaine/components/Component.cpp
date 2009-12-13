/*
 *  Component.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "Component.h"

#include <cassert>

namespace semaine {
namespace components {

const std::string Component::STATE_STARTING = "starting";
const std::string Component::STATE_READY = "ready";
const std::string Component::STATE_STOPPED = "stopped";
const std::string Component::STATE_FAILURE = "failure";
const std::string Component::STATE_STALLED = "stalled";


	Component::Component(const std::string & componentName, bool isInput, bool isOutput) throw (CMSException) :
		name(componentName),
		isInput(isInput),
		isOutput(isOutput),
		_exitRequested(false),
		waitingTime(100),
		state(STATE_STOPPED),
		meta(componentName)
	{
		log = CMSLogger::getLog(name);
		state = STATE_STARTING;
		meta.reportState(state);
	}



	Component::~Component()
	{
	   std::list<Receiver *>::iterator i;
		for (i = receivers.begin(); i != receivers.end(); i++) {
			Receiver * r = *i;
			delete r;
		}
	   std::list<Sender *>::iterator j;
		for (j = senders.begin(); j != senders.end(); j++) {
			Sender * s = *j;
			delete s;
		}
		delete log;
	}



	void Component::messageAvailableFrom(Receiver * source)
	{
		synchronized (&mutex) {
			inputWaiting.push(source);
			mutex.notify();
		}
	}


	void Component::startIO() throw(std::exception)
	{
		long long startTime = System::currentTimeMillis();
		meta.reportTopics(receivers, senders, isInput, isOutput);
		customStartIO();
	    std::list<Receiver *>::iterator i;
		for (i = receivers.begin(); i != receivers.end(); i++) {
			Receiver * r = *i;
			r->setMessageListener(this);
			r->startConnection();
		}
	   std::list<Sender *>::iterator j;
		for (j = senders.begin(); j != senders.end(); j++) {
			Sender * s = *j;
			s->startConnection();
		}
		long long endTime = System::currentTimeMillis();
		long long startupDuration = endTime - startTime;
		state = STATE_READY;
		std::stringstream buf;
		buf << "Startup took " << startupDuration << " ms";
		meta.reportState(state, buf.str());
	}

	void Component::run()
	{
		try {
			std::cerr << "starting component " << name << "..." << std::endl;
			startIO();
			std::cerr << "    ... " << name << " ready." << std::endl;
		} catch (std::exception & ex) {
			log->error("Cannot startup component:", &ex);
			try {
				state = STATE_FAILURE;
				meta.reportState(state, "Cannot startup component:", &ex);
			} catch (CMSException & me) {
				log->error("cannot report failure state", & me);
			}
			requestExit();
		}

		while (!exitRequested()) {
			// Report that we are alive before we check system ready status,
			// (in case we were stalled due to a long act() or react() this
			// means we "report back" to the rest of the system)
			meta.IamAlive();
      //printf("run %i\n",exitRequested());
			// Check at every loop that the total system is ready
			bool exitFlag = false;
			synchronized (&meta) {
				while (!meta.isSystemReady()) {
					log->info("waiting for system to become ready");
					try {
						meta.wait(1000);
					} catch (decaf::lang::Exception & ie) {
						ie.printStackTrace();
					}
					if (meta.isSystemReady()) {
						log->info("system ready - let's go");
					}
					if (exitRequested()) {
						exitFlag = true;
						break;
					}
				}
			}
			if (exitFlag) break;

			try {
				// Check if we should do something proactively:
				long long before = System::currentTimeMillis();
				act();
				long long timeSpentInAct = System::currentTimeMillis() - before;
				meta.statistics()->actTime((long)timeSpentInAct);
			} catch (std::exception & e) {
				log->error("error when trying to act", & e);
				try {
					state = STATE_FAILURE;
					meta.reportState(state, "error when trying to act", &e);
				} catch (CMSException & me) {
					log->error("cannot report failure state", & me);
				}
				requestExit();
				return;
			}

			Receiver * r = NULL;
			synchronized (&mutex) {
				if (inputWaiting.empty()) {
				// block until input becomes available
				mutex.wait(waitingTime);
				}
				// check again
				if (!inputWaiting.empty()) {
					r = inputWaiting.front();
					inputWaiting.pop();
				}
			}
			if (r == NULL) continue;
			//assert(receivers.contains(r)); // the receiver that alerted us is not one of our receivers;
			SEMAINEMessage * message = r->getMessage();
			assert(message != NULL); // Receiver alerted me but has no message
			meta.statistics()->countMessageReceived();

			try {
				// time that the message travelled, in "user" time (ms since system startup -- this should be independent of clock asynchrony)
				long long timeMessageTravelled = meta.getTime() - message->getUsertime();
				meta.statistics()->transmitTime((long)timeMessageTravelled);
				// Now, do something meaningful with the message,
				// and possibly send output via the Senders.
				long long before = System::currentTimeMillis();
				react(message);
				delete message;
				long long timeSpentInReact = System::currentTimeMillis() - before;
				meta.statistics()->reactTime((long)timeSpentInReact);
			} catch (std::exception & e) {
				log->error("error when trying to react", &e);
				try {
					state = STATE_FAILURE;
					std::string errMsg = CMSLogger::toLogMessageText("error when trying to react", &e)
						+ "(message was: " + CMSLogger::message2logString(message) + ")";
					meta.reportState(state, errMsg);
				} catch (CMSException & me) {
					log->error("cannot report failure state", & me);
				}
				requestExit();
				return;
			}
		}
	}

	bool Component::exitRequested()
	{
		synchronized (&mutex) {
			return _exitRequested;
		}
		return false; // just to keep compiler happy
	}

	void Component::requestExit()
	{
		synchronized (&mutex) {
			_exitRequested = true;
			mutex.notify();
		}
	}

} // namespace components
} // namespace semaine

