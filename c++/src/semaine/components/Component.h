/*
 *  Component.h
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_COMPONENTS_COMPONENT_H
#define SEMAINE_COMPONENTS_COMPONENT_H

#include <list>
#include <queue>

#include <semaine/config.h>

#include <decaf/lang/Thread.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>

#include <cms/CMSException.h>

#include <semaine/cms/SEMAINEMessageAvailableListener.h>
#include <semaine/cms/receiver/Receiver.h>
#include <semaine/cms/sender/Sender.h>
#include <semaine/components/meta/MetaMessenger.h>


using namespace decaf::lang;
using namespace cms;
using namespace semaine::cms;
using namespace semaine::cms::receiver;
using namespace semaine::cms::sender;
using namespace semaine::components::meta;

namespace semaine {
namespace components {

/**
 * The base class for all components.
 * Subclasses need to:
 * <ul>
 * <li>create receivers and/or senders in their constructor;
 * <li>implement {@link #act()} and/or {@link #react(SEMAINEMessage)}
 * to do something meaningful.</li>
 * </ul>
 * @author marc
 *
 */
class Component : public Thread, public SEMAINEMessageAvailableListener
{
public:
	static const std::string STATE_STARTING;
	static const std::string STATE_READY;
	static const std::string STATE_STOPPED;
	static const std::string STATE_FAILURE;
	
	virtual ~Component();
	
	const std::string getName() { return name; }
	
	/**
	 * Method called from the corresponding receiver thread, to notify us
	 * that a message is available.
	 */
	virtual void messageAvailableFrom(Receiver * const source);

	/**
	 * The main method executed for this Thread.
	 */
	void run();
	
	void requestExit();
	
	
	
protected:
	std::list<Receiver *> receivers;
	std::list<Sender *> senders;
	std::queue<Receiver *> inputWaiting;
	CMSLogger * log;
	bool _exitRequested;
	std::string state;
	MetaMessenger meta;
	int waitingTime;
	decaf::util::concurrent::Mutex mutex;
	
	Component(const std::string & componentName) throw (CMSException);

	bool exitRequested();
	
		//////////////////////// Methods to override ////////////////////
	
	/**
	 * Proactive actions. This method is called every {@link #waitingTime}
	 * milliseconds if no messages arrive, and after every message processing.
	 * 
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 * @throws CMSException if communication with the JMS server goes wrong.
	 */
	virtual void act() throw(CMSException) {}

	/**
	 * Reactions to messages.
	 * 
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 * @throws CMSException if communication with the JMS server goes wrong.
	 */
	virtual void react(SEMAINEMessage * message) throw (CMSException) {}


private:
	const std::string name;
	
	void startIO() throw(CMSException);




};




} // namespace components
} // namespace semaine

#endif

