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

#include <decaf/lang/System.h>
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
	static const std::string STATE_STALLED;

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
	bool isInput;
	bool isOutput;
	decaf::util::concurrent::Mutex mutex;

	/**
	 * Create a component with the given name.
	 * The component communicates with the default CMS server (see {@link IOBase#IOBase()}).
	 * @param componentName the component name as it is to be used throughout the system.
	 * @param isInput whether the component is an input component (optional, defaults to false)
	 * @param isOutput whether the component is an output component (optional, defaults to false)
	 * @throws JMSException if the connection to the middleware cannot be established
	 */
	Component(const std::string & componentName, bool isInput=false, bool isOutput=false)
	throw (CMSException);

	/**
	 * Create a component with the given name.
	 * The component is neither an input nor an output component,
	 * communicates with the CMS server given in the arguments.
	 * @param componentName the component name as it is to be used throughout the system.
	 * @param cmsUrl the url where to contact the CMS server.
	 * @param cmsUser the username to use (optional)
	 * @param cmsPassword the password to use (optional)
	 * @throws JMSException if the connection to the middleware cannot be established
	 */
	Component(const std::string & componentName,
			const std::string & cmsUrl, const std::string & cmsUser = "", const std::string & cmsPassword = "")
	throw (CMSException);

	/**
	 * Create a component with the given name.
	 * The component communicates with the CMS server given in the arguments.
	 * @param componentName the component name as it is to be used throughout the system.
	 * @param isInput whether the component is an input component
	 * @param isOutput whether the component is an output component
	 * @param cmsUrl the url where to contact the CMS server.
	 * @param cmsUser the username to use (can be the empty string)
	 * @param cmsPassword the password to use (can be the empty string)
	 * @throws JMSException if the connection to the middleware cannot be established
	 */
	Component(const std::string & componentName, bool isInput, bool isOutput,
			const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword)
	throw (CMSException);

	bool exitRequested();

		//////////////////////// Methods to override ////////////////////

	/**
	 * Proactive actions. This method is called every {@link #waitingTime}
	 * milliseconds if no messages arrive, and after every message processing.
	 *
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 * @throws CMSException if communication with the JMS server goes wrong.
	 * @throws std::exception if anything goes wrong in the internal processing of the component.
	 */
	virtual void act() throw(std::exception) {}

	/**
	 * Reactions to messages.
	 *
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 * @throws CMSException if communication with the JMS server goes wrong.
	 * @throws std::exception if anything goes wrong in the internal processing of the component.
	 */
	virtual void react(SEMAINEMessage * message) throw (std::exception) {}

	/**
	 * Subclasses can implement this method in order to run custom code
	 * needed at startup time.
	 * @throws std::exception if anything goes wrong in the internal processing of the component.
	 */
	virtual void customStartIO() throw(std::exception) {}

private:
	const std::string name;

	void startIO() throw(std::exception);




};




} // namespace components
} // namespace semaine

#endif

