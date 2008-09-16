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


	Component::Component(const std::string & componentName) throw (CMSException) :
		name(componentName),
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


	void Component::startIO() throw(CMSException)
	{
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
		state = STATE_READY;
		meta.reportState(state);
	}

	void Component::run()
	{
		try {
			std::cerr << "starting component " << name << "..." << std::endl; 
			startIO();
			std::cerr << "    ... " << name << " ready." << std::endl;
		} catch (CMSException & ex) {
			log->error("Cannot startup component:", &ex);
			requestExit();
		}
		
		while (!exitRequested()) {
			// Check at every loop that the total system is ready
			synchronized (&meta) {
				while (!meta.isSystemReady()) {
					log->info("waiting for system to become ready");
					try {
						meta.wait();
					} catch (decaf::lang::Exception & ie) {
					}
					if (meta.isSystemReady()) {
						log->info("system ready - let's go");
					}
				}
			}
			try {
				// Check if we should do something proactively:
				act();
			} catch (CMSException & e) {
				log->error("error when trying to act", & e);
				try {
					state = STATE_FAILURE;
					meta.reportState(state);
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
			
			try {
				// Now, do something meaningful with the message,
				// and possibly send output via the Senders.
				react(message);
				delete message;
			} catch (CMSException & e) {
				log->error("error when trying to react", &e);
				try {
					state = STATE_FAILURE;
					meta.reportState(state);
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

