/*
 *  SEMAINEMessage.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */


#ifndef SEMAINE_CMS_MESSAGE_SEMAINEMESSAGE_H
#define SEMAINE_CMS_MESSAGE_SEMAINEMESSAGE_H

#include <typeinfo>

#include <semaine/config.h>

#include <cms/Message.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/Destination.h>
#include <cms/Topic.h>

#include <semaine/cms/exceptions/MessageFormatException.h>


using namespace cms;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace message {

/**
 * A representation of a message as used in SEMAINE code.
 * It is a higher-level access method for JMS messages,
 * providing interpretations of information accessible in the message headers.
 *
 * This base class provides access to the Message body only as plain text
 * (for text messages). Subclasses should:
 * <ul>
 * <li>add any specific message format verification code in their constructor;</li>
 * <li>provide their own API for accessing the message content.</li>
 * </ul>
 *
 * @author marc
 *
 */
class SEMAINEMessage
{
public:
	/**
	 * The name of the property used for time in "user" time that the message refers to.
	 */
	static const std::string USERTIME;
	/**
	 * The name of the property used for the type of data being sent.
	 */
	static const std::string DATATYPE;
	/**
	 * The name of the property used for the component that sent the data.
	 */
	static const std::string SOURCE;
	/**
	 * The name of the property used for the type of event being sent (for event-based messages).
	 */
	static const std::string EVENT;
	/**
	 * The name of the property used for the period length, in milliseconds (for periodic messages).
	 */
	static const std::string PERIOD;

		/**
	 * Create a SEMAINE specific abstraction from the given message.
	 * @param message a message containing SEMAINE-specific properties.
	 * @throws NullPointerException if message is null.
	 * @throws MessageFormatException if the message does not have the proper format
	 */
	SEMAINEMessage(const Message * message) throw(MessageFormatException);

	virtual ~SEMAINEMessage();

	/**
	 * Get the user time that the message refers to, in milliseconds since 1970.
	 * @return the user time
	 * @throws CMSException if the CMS provider fails to get the property value due to some internal error.
	 */
	long long getUsertime() throw(CMSException)
	{
		return message->getLongProperty(USERTIME);
	}

	/**
	 * Get the name of the data type received.
	 * @return a string containing the name of the data type
	 * @throws CMSException if the CMS provider fails to get the property value due to some internal error.
	 */
	std::string getDatatype() throw(CMSException)
	{
		return message->getStringProperty(DATATYPE);
	}

	/**
	 * Get the name of the source component that sent the data.
	 * @return a string containing the name of the source component
	 * @throws CMSException if the CMS provider fails to get the property value due to some internal error.
	 */
	std::string getSource() throw(CMSException)
	{
		return message->getStringProperty(SOURCE);
	}

	/**
	 * Determine whether the message is periodic. This is determined
	 * by the presence of the <code>period</code> property.
	 * @return <code>true</code> if message is periodic,
	 * <code>false</code> if message is event-based.
	 * @throws CMSException if the CMS provider fails to get the property value due to some internal error.
	 */
	bool isPeriodic() throw(CMSException)
	{
		return message->propertyExists(PERIOD);
	}

	/**
	 * Determine whether the message is event-based. This is determined
	 * by the presence of the <code>event</code> property.
	 * @return <code>true</code> if message is event-based,
	 * <code>false</code> if message is periodic.
	 * @throws CMSException if the CMS provider fails to get the property value due to some internal error.
	 */
	bool isEventBased() throw(CMSException)
	{
		return message->propertyExists(EVENT);
	}

	/**
	 * For event-based messages, determine the event type.
	 * @return an Event object representing the event type.
	 * @throws CMSException if the CMS provider fails to get the property value due to some internal error.
	 * @throws MessageFormatException if the message contains an <code>event</code> property, but the value is not known,
	 * or if the message is not event-based.
	 */
	std::string getEventType() throw (CMSException, MessageFormatException)
	{
		if (!message->propertyExists(EVENT))
			throw MessageFormatException("Message is not event-based, cannot provide event type");
		std::string eventString = message->getStringProperty(EVENT);
		return eventString;
	}

	/**
	 * For periodic messages, determine the period length.
	 * @return the period length, in milliseconds.
	 * @throws CMSException if the JMS provider fails to get the property value due to some internal error.
	 * @throws MessageFormatException if the message is not periodic.
	 */
	int getPeriod() throw (CMSException, MessageFormatException)
	{
		if (!message->propertyExists(PERIOD))
			throw MessageFormatException("Message is not periodic, cannot provide period length");
		return message->getIntProperty(PERIOD);
	}

	/**
	 * Determine whether the message is a text message.
	 * @return true if the message is a text message, false otherwise.
	 */
	bool isTextMessage()
	{
		return dynamic_cast<const TextMessage*>(message) != NULL;
	}

	/**
	 * Determine whether the message is a bytes message.
	 * @return true if the message is a bytes message, false otherwise.
	 */
	bool isBytesMessage()
	{
		return dynamic_cast<const BytesMessage*>(message) != NULL;
	}

	/**
	 * For text messages, provide access to the text sent.
	 * @return the text sent in the body of the message.
	 * @throws CMSException if the JMS provider fails to get the property value due to some internal error.
	 * @throws MessageFormatException if the message is not a text message.
	 */
	std::string getText() throw(CMSException, MessageFormatException)
	{
		const TextMessage * tm = dynamic_cast<const TextMessage*>(message);
		if (tm == NULL) {
			throw MessageFormatException(std::string("Cannot get text for a message of type ")+typeid(*message).name());
		}
		return tm->getText();
	}

	std::string getTopicName() throw(CMSException)
	{
		const Destination * dest = message->getCMSDestination();
		if (dest == NULL) return std::string("unknown");
		const Topic * topic = dynamic_cast<const Topic *>(dest);
		if (topic == NULL) return std::string("unknown");
		return topic->getTopicName();
	}

	/**
	 * Provide access to the low-level message encapsulated in this object.
	 * @return the low-level message object.
	 */
	const Message * getMessage()
	{
		return message;
	}




protected:
	const Message * message;

private:
		/**
	 * Check that the message conforms to our expectations.
	 * This method is private because overriding it makes no sense:
	 * as it is called from the constructor, the version in the subclass
	 * would not be visible from our constructor.
	 * @throws MessageFormatException if a problem with the properties is found.
	 */
	void verifyProperties() throw(MessageFormatException);



};


} // namespace message
} // namespace cms
} // namespace semaine



#endif

