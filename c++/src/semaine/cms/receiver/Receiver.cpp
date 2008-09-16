/*
 *  receiver.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "Receiver.h"


using namespace semaine::cms::receiver;

Receiver::Receiver(const std::string & topicName) 
throw (CMSException) :
	IOBase(topicName),
	listener(NULL),
	message(NULL)
{
	consumer = session->createConsumer(topic);
	log = CMSLogger::getLog("Receiver");
}


Receiver::Receiver(const std::string & topicName, const std::string & messageSelector)
throw (CMSException) :
	IOBase(topicName),
	listener(NULL),
	message(NULL)
{
	consumer = session->createConsumer(topic, messageSelector);
	log = CMSLogger::getLog("Receiver");
}

Receiver::~Receiver()
{
	delete consumer;
	if (message != NULL) delete message;
	
}


SEMAINEMessage * Receiver::createSEMAINEMessage(const Message * message)
throw(MessageFormatException)
{
	if (message == NULL)
		throw MessageFormatException("cannot create semaine message from null");
	return new SEMAINEMessage(message);
}


SEMAINEMessage * Receiver::receive() throw(CMSException, SystemConfigurationException)
{
		if (!isConnectionStarted)
			throw SystemConfigurationException("Connection is not started!");
		Message * m = consumer->receive();
		if (m == NULL) return NULL;
		return createSEMAINEMessage(m);
}

	SEMAINEMessage * Receiver::receive(int timeout) throw(CMSException, SystemConfigurationException)
	{
		if (!isConnectionStarted)
			throw SystemConfigurationException("Connection is not started!");
		Message * m = consumer->receive(timeout);
		if (m == NULL) return NULL;
		return createSEMAINEMessage(m);
	}

	SEMAINEMessage * Receiver::receiveNoWait() throw(CMSException, SystemConfigurationException)
	{
		if (!isConnectionStarted)
			throw SystemConfigurationException("Connection is not started!");
		Message * m = consumer->receiveNoWait();
		if (m == NULL) return NULL;
		return createSEMAINEMessage(m);
	}

	void Receiver::setMessageListener(SEMAINEMessageAvailableListener * aListener)
	throw(CMSException)
	{
		listener = aListener;
		if (listener == NULL)
			consumer->setMessageListener(NULL);
		else
			consumer->setMessageListener(this);
	}

	void Receiver::onMessage(const Message * m)
	{
		assert(listener != NULL); // asynchronous mode, but no SEMAINE message listener registered
		if (m != NULL) {
			try {
				message = createSEMAINEMessage(m);
				// Notify component that new message has arrived
				listener->messageAvailableFrom(this);
				// Block until message has been picked up
				synchronized (&mutex) {
					while (notPickedUp()) {
						try {
							mutex.wait();
						} catch (lang::Exception & ie) {}
					}
				}
			} catch (MessageFormatException & mfe) {
				log->warn("Problem creating SEMAINE message: ", &mfe);
			}
		}
	}

	SEMAINEMessage * Receiver::getMessage()
	{
		synchronized (&mutex) {
			SEMAINEMessage * m = this->message;
			this->message = NULL;
			mutex.notify();
			return m;
		}
		return NULL; // just to keep compiler happy
	}

				bool Receiver::notPickedUp()
				{
					synchronized(&mutex) {
						return message != NULL;
					}
					return true; // just to keep compiler happy
				}



