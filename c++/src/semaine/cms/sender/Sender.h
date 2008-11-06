/*
 *  Sender.h
 *  semaine
 *
 *  Created by Marc Schröder on 11.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_SENDER_SENDER_H
#define SEMAINE_CMS_SENDER_SENDER_H

#include <semaine/config.h>

#include <cms/MessageProducer.h>
#include <cms/Message.h>

#include <semaine/cms/IOBase.h>
#include <semaine/cms/message/SEMAINEMessage.h>
#include <semaine/cms/exceptions/SEMAINEException.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

using namespace cms;
using namespace semaine::cms;
using namespace semaine::cms::message;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace sender {
/**
 * This class handles the common part to all message senders in
 * the SEMAINE system.
 * @author marc
 *
 */
class Sender : public IOBase
{
public:

	/**
	 * Create a new Sender to the given topic on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: default value empty;</li>
	 * <li>jms Password: default value empty.</li>
	 * </ul>
	 * Datatype and source will be sent with every message.
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @throws CMSException
	 */
	Sender(const std::string & topicName, const std::string & datatype, const std::string & source)
	throw (CMSException);

	/**
	 * Create a new CMS IOBase connection with the given topic on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topicName the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @throws CMSException
	 */
	Sender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
		const std::string & topicName, const std::string & datatype, const std::string & source)
	throw (CMSException);

		
	/**
	 * The name of the data type sent.
	 * @return a non-null and non-empty string.
	 */
	const std::string getDatatype()
	{
		return datatype;
	}
	
	/**
	 * The name of the source component sending the data.
	 * @return a non-null and non-empty string.
	 */
	const std::string getSource()
	{
		return source;
	}

	/**
	 * Determine that the data is periodic, and set the period in which data is (intended to be) sent, in milliseconds.
	 * @param aPeriod the period in which data is to be sent.
	 * @throws SEMAINEException if aPeriod is <= 0.
	 */
	void setPeriodic(int aPeriod) throw(SEMAINEException)
	{
		if (aPeriod <= 0)
			throw SEMAINEException("Period must be positive");
		this->period = aPeriod;
	}
	
	/**
	 * Determine that the data is event-based, not periodic.
	 * Individual messages can transport different kinds of events,
	 * @see {@link #sendTextMessage()}.
	 */
	void setEventBased()
	{
		this->period = 0;
	}

	/**
	 * Test whether this sender is set to send messages periodically. 
	 * @return true if sender is set to periodic, false otherwise.
	 */
	bool isPeriodic()
	{
		return period != 0;
	}
	
	/**
	 * Test whether this sender is set to send messages in an event-based way. 
	 * @return true if sender is set to event-based, false otherwise.
	 */
	bool isEventBased()
	{
		return period == 0;
	}

	/**
	 * For periodic senders, get the period of sending.
	 * @return An integer representing the period of sending, in milliseconds. 
	 */
	int getPeriod()
	{
		if (isEventBased())
			throw SEMAINEException("Cannot report period for event-based Sender!");
		return period;
	}
	
	/**
	 * Set the time to live for all messages sent by this sender, 
	 * i.e. the number of milliseconds before the message is considered
	 * obsolete by the JMS system. Messages that are not delivered by their
	 * expiration time are deleted by the JMS system.
	 * @param aTimeToLive time to live, in milliseconds.
	 * A value of 0 means unlimited time, i.e. the messages will not expire.
	 */
	void setTimeToLive(long long aTimeToLive) throw(CMSException)
	{
		producer->setTimeToLive(aTimeToLive);
	}
	
	/**
	 * Get the time to live for all messages sent by this sender, 
	 * i.e. the number of milliseconds before the message is considered
	 * obsolete by the JMS system. Messages that are not delivered by their
	 * expiration time are deleted by the JMS system.
	 * @return time to live, in milliseconds. 
	 * A value of 0 means unlimited time, i.e. the messages will not expire.
	 */
	long long getTimeToLive() throw(CMSException)
	{
		return producer->getTimeToLive();
	}
	
	/**
	 * Send a text message via this sender.
	 * This will send a message to the registered topic 
	 * with the following message properties:
	 * <ul>
	 *   <li><code>datatype</code> is a String property containing the value produced by {@link #getDatatype()};</li>
	 *   <li><code>source</code> is a String property containing the value produced by {@link #getSource()};</li>
	 *   <li><code>usertime</code> is a long property containing the value of parameter <code>usertime</code>;</li>
	 *   <li>if the message is periodic ({@link #isPeriodic()} returns <code>true</code>),
	 *   <code>period</code> is an int property containing the value returned by {@link #getPeriod()};</li>
	 *   <li>else, the message is event-based. <code>event</code> is a String property; as
	 *   this method does not specify an event type, the default value <code>single</code> is assumed.</li>
	 * </ul>
	 * Furthermore, if {@link #getTimeToLive()} returns a non-zero value, the message will
	 * contain a header field <code>JMSExpiration</code> containing the time when the message
	 * will expire.
	 * @param text the message text.
	 * @param usertime the "user" time that this message refers to,
	 * in milliseconds since 1970.
	 * @throws SystemConfigurationException if the connection is not started or the sender is in event-based mode.
	 */
	void sendTextMessage(const std::string & text, long long usertime)
	throw(CMSException, SystemConfigurationException);
	
	/**
	 * Send a text message via this sender, for event-based messages.
	 * This will send a message to the registered topic 
	 * with the following message properties:
	 * <ul>
	 *   <li><code>datatype</code> is a String property containing the value produced by {@link #getDatatype()};</li>
	 *   <li><code>source</code> is a String property containing the value produced by {@link #getSource()};</li>
	 *   <li><code>usertime</code> is a long property containing the value of parameter <code>usertime</code>;</li>
	 *   <li><code>event</code> is a String property containing
	 *   the String representation of the event parameter to this method.</li>
	 * </ul>
	 * Furthermore, if {@link #getTimeToLive()} returns a non-zero value, the message will
	 * contain a header field <code>JMSExpiration</code> containing the time when the message
	 * will expire.
	 * @param text the message text.
	 * @param usertime the "user" time that this message refers to,
	 * in milliseconds since 1970.
	 * @param event the type of event represented by this message.
	 * @throws IllegalStateException if the connection is not started or the sender is in periodic mode.
	 */
	void sendTextMessage(const std::string & text, long long usertime, const std::string & eventType)
	throw(CMSException, SystemConfigurationException);



protected:
	MessageProducer * producer;
	/**
	 * The name of the data type being sent.
	 */
	const std::string datatype;
	/**
	 * The name of the source component from where the data originated.
	 */
	const std::string source;
	/**
	 * If data is periodic, the period in which data is sent, in milliseconds.
	 * A value of 0 means not periodic. 
	 * Data can be either periodic or event-based, but not both at the same time.
	 */
	int period;

	void initialise() throw (CMSException);

	/**
	 * Fill in the usual message properties as far as possible.
	 * Subclasses are encouraged to override this method, but should
	 * normally call this method as well (as <code>super.fillMessageProperties(message, usertime)</code>).
	 * @param a pointer to a message object in preparation for sending.
	 * @param usertime the "user" time that this message refers to,
	 * in milliseconds since 1970.
	 */
	virtual void fillMessageProperties(Message * message, long long usertime)
	throw(CMSException)
	{
		message->setStringProperty(SEMAINEMessage::DATATYPE, getDatatype());
		message->setStringProperty(SEMAINEMessage::SOURCE, getSource());
		message->setLongProperty(SEMAINEMessage::USERTIME, usertime);
	}
	


};

} // namespace sender
} // namespace cms
} // namespace semaine

#endif

