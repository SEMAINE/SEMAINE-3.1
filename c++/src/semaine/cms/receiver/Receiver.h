/*
 *  Receiver.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_RECEIVER_RECEIVER_H
#define SEMAINE_CMS_RECEIVER_RECEIVER_H

#include <semaine/config.h>

#include <cms/MessageListener.h>

#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>


#include <semaine/cms/IOBase.h>
#include <semaine/cms/CMSLogger.h>
#include <semaine/cms/SEMAINEMessageAvailableListener.h>
#include <semaine/cms/message/SEMAINEMessage.h>

#include <semaine/cms/exceptions/MessageFormatException.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

using namespace cms;
using namespace semaine::cms;
using namespace semaine::cms::message;
using namespace semaine::cms::exceptions;

using namespace decaf;

namespace semaine {
	namespace cms {
		namespace receiver {
			/**
			 * This class handles the common part to all message receivers in
			 * the SEMAINE system.
			 *
			 * Two models of use are possible:
			 * <ul>
			 * <li>synchronous API: client code calls the methods {@link #receive()},
			 * {@link #receive(long)} or {@link #receiveNoWait()} directly;</li>
			 * <li>asynchronous API: client code registers a {@link SEMAINEMessageAvailableListener};
			 * whenever a new message arrives, the Receiver thread will call that listener's
			 * {@link SEMAINEMessageAvailableListener#messageAvailableFrom(Receiver)} method.
			 * The listener code then calls the Receiver's {@link #getMessage()} method from the
			 * thread that should actually do the processing of the message. Note that in the
			 * asynchronous API, the Receiver will block until getMessage() was called.</li>
			 * </ul>
			 *
			 * The Receiver itself covers basic text messages only.
			 * <strong>Subclasses</strong> should override {@link #createSEMAINEMessage(Message)}
			 * to do a meaningful analysis of the JMS message in terms of a suitable subclass of
			 * {@link SEMAINEMessage}.
			 *
			 * @author marc
			 *
			 */
			class Receiver : public IOBase, public MessageListener
			{
			public:
				/**
				 * Create a receiver that will listen for all messages in the given Topic.
				 * @param topic the name of the CMS Topic to listen to.
				 */
				Receiver(const std::string & topicName) throw (CMSException);

				/**
				 * Create a receiver that will listen only to the messages in the given Topic
				 * that will pass the given messageSelector.
				 * @param topicName the name of the JMS Topic to listen to.
				 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
				 * for the detailed description.
				 */
				Receiver(const std::string & topicName, const std::string & messageSelector) throw (CMSException);

				virtual ~Receiver();

				/**
				 * Get the message selector pattern active in this receiver to filter messages.
				 * @return a string representation of the selector pattern.
				 * @throws CMSException
				 */
				std::string getMessageSelector()
				throw(CMSException)
				{
					return consumer->getMessageSelector();
				}


				////////////////// Synchronous message consumption ////////////////

				/**
				 * Receives the next message produced for this message consumer.
				 * This call blocks indefinitely until a message is produced or until this message consumer is closed.
				 * @return the next message produced for this message consumer, or null if this message consumer is concurrently closed
				 * @throws CMSException if the CMS provider fails to receive the next message due to some internal error.
				 * @throws SystemConfigurationException if the connection was not started before calling this method.
				 */
				SEMAINEMessage * receive() throw(CMSException, SystemConfigurationException);

				/**
				 * Receives the next message that arrives within the specified timeout interval.
				 * This call blocks until a message arrives, the timeout expires, or this message consumer is closed. A timeout of zero never expires, and the call blocks indefinitely.
				 * @param timeout the timeout value (in milliseconds)
				 * @return the next message produced for this message consumer, or null if the timeout expires or this message consumer is concurrently closed
				 * @throws CMSException if the CMS provider fails to receive the next message due to some internal error.
				 * @throws SystemConfigurationException if the connection was not started before calling this method.
				 */
				SEMAINEMessage * receive(int timeout) throw(CMSException, SystemConfigurationException);

				/**
				 * Receives the next message if one is immediately available.
				 * @return the next message produced for this message consumer, or null if one is not available
				 * @throws CMSException if the CMS provider fails to receive the next message due to some internal error.
				 * @throws SystemConfigurationException if the connection was not started before calling this method.
				 */
				SEMAINEMessage * receiveNoWait() throw(CMSException, SystemConfigurationException);


				////////////////// Asynchronous message consumption ////////////////

				void setMessageListener(SEMAINEMessageAvailableListener * aListener)
				throw(CMSException);

				SEMAINEMessageAvailableListener * getMessageListener()
				{
					return listener;
				}

				virtual void onMessage(const Message * m);

				SEMAINEMessage * getMessage();

				bool notPickedUp();



			protected:

				MessageConsumer * consumer;
				SEMAINEMessageAvailableListener * listener;
				CMSLogger * log;
				SEMAINEMessage * message;
				decaf::util::concurrent::Mutex mutex;


				///////////////// The abstraction step: create SEMAINE message ///////

				/**
				 * Create a SEMAINE message from the given CMS message.
				 * Subclasses may want to override this in order to
				 * provide subclasses of SEMAINEMessage for their data.
				 * @param message the CMS method to convert into a SEMAINE message.
				 * @throws MessageFormatException if the message format is problematic
				 * @throws NullPointerException if message is null.
				 */
				virtual SEMAINEMessage * createSEMAINEMessage(const Message * message)
				throw(MessageFormatException);



			};

		} // namespace receiver
	} // namespace cms
} // namespace semaine



#endif

