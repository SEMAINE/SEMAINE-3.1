/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.TextMessage;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.IOBase.Event;

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
public class SEMAINEMessage
{
	/**
	 * The name of the property used for time in "user" time that the message refers to.
	 */
	public static final String USERTIME = "usertime";
	/**
	 * The name of the property used for the type of data being sent.
	 */
	public static final String DATATYPE = "datatype";
	/**
	 * The name of the property used for the component that sent the data.
	 */
	public static final String SOURCE = "source";
	/**
	 * The name of the property used for the type of event being sent (for event-based messages).
	 */
	public static final String EVENT = "event";
	/**
	 * The name of the property used for the period length, in milliseconds (for periodic messages).
	 */
	public static final String PERIOD = "period";
	
	protected Message message;

	/**
	 * Create a SEMAINE specific abstraction from the given message.
	 * @param message a message containing SEMAINE-specific properties.
	 * @throws NullPointerException if message is null.
	 * @throws MessageFormatException if the message does not have the proper format 
	 */
	public SEMAINEMessage(Message message)
	throws MessageFormatException
	{
		if (message == null)
			throw new NullPointerException("Message is null");
		this.message = message;
		verifyProperties();
	}
	
	/**
	 * Check that the message conforms to our expectations.
	 * This method is private because overriding it makes no sense:
	 * as it is called from the constructor, the version in the subclass
	 * would not be visible from our constructor.
	 * @throws MessageFormatException if a problem with the properties is found.
	 */
	private void verifyProperties()
	throws MessageFormatException
	{
		try { 
			getUsertime(); 
		} catch (Exception e) {
			throw new MessageFormatException("Problem with message property '"+USERTIME+"'", e); 
		}
		try { 
			getDatatype(); 
		} catch (Exception e) {
			throw new MessageFormatException("Problem with message property '"+DATATYPE+"'", e); 
		}
		try { 
			getSource(); 
		} catch (Exception e) {
			throw new MessageFormatException("Problem with message property '"+SOURCE+"'", e); 
		}
		try {
			if (!isPeriodic() && !isEventBased())
				throw new MessageFormatException("Message contains neither periodic nor event-based header properties.");
		} catch (Exception e) {
			throw new MessageFormatException("Problem determining whether properties '"+PERIOD+"' or '"+EVENT+"' exist.");
		}
	}
	
	/**
	 * Get the user time that the message refers to, in milliseconds since 1970.
	 * @return the user time
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 * @throws MessageFormatException if the message property exists but has the wrong type.
	 */
	public long getUsertime() throws JMSException
	{
		return message.getLongProperty(USERTIME);
	}
	
	/**
	 * Get the name of the data type received.
	 * @return a string containing the name of the data type
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 */
	public String getDatatype() throws JMSException
	{
		return message.getStringProperty(DATATYPE);
	}

	/**
	 * Get the name of the source component that sent the data.
	 * @return a string containing the name of the source component
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 */
	public String getSource() throws JMSException
	{
		return message.getStringProperty(SOURCE);
	}
	
	/**
	 * Determine whether the message is periodic. This is determined
	 * by the presence of the <code>period</code> property.
	 * @return <code>true</code> if message is periodic,
	 * <code>false</code> if message is event-based.
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 */
	public boolean isPeriodic() throws JMSException
	{
		return message.propertyExists(PERIOD);
	}
	
	/**
	 * Determine whether the message is event-based. This is determined
	 * by the presence of the <code>event</code> property.
	 * @return <code>true</code> if message is event-based,
	 * <code>false</code> if message is periodic.
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 */
	public boolean isEventBased() throws JMSException
	{
		return message.propertyExists(EVENT);
	}
	
	/**
	 * For event-based messages, determine the event type.
	 * @return an Event object representing the event type.
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 * @throws MessageFormatException if the message contains an <code>event</code> property, but the value is not known. 
	 * @throws IllegalStateException if the message is not event-based.
	 */
	public Event getEventType() throws JMSException, IllegalArgumentException, IllegalStateException, MessageFormatException
	{
		if (!message.propertyExists(EVENT)) 
			throw new IllegalStateException("Message is not event-based, cannot provide event type");
		String eventString = message.getStringProperty(EVENT);
		try {
			return Event.valueOf(eventString);
		} catch (IllegalArgumentException iae) {
			throw new MessageFormatException("Message contains unknown event type '"+eventString+"'");
		}
	}

	/**
	 * For periodic messages, determine the period length.
	 * @return the period length, in milliseconds.
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 * @throws IllegalStateException if the message is not periodic.
	 */
	public int getPeriod() throws JMSException, IllegalStateException
	{
		if (!message.propertyExists(PERIOD))
			throw new IllegalStateException("Message is not periodic, cannot provide period length");
		return message.getIntProperty(PERIOD);
	}

	/**
	 * Determine whether the message is a text message.
	 * @return true if the message is a text message, false otherwise.
	 */
	public boolean isTextMessage()
	{
		return message instanceof TextMessage;
	}
	
	/**
	 * For text messages, provide access to the text sent.
	 * @return the text sent in the body of the message.
	 * @throws JMSException if the JMS provider fails to get the property value due to some internal error.
	 * @throws IllegalStateException if the message is not a text message.
	 */
	public String getText() throws JMSException
	{
		if (!(message instanceof TextMessage)) {
			throw new IllegalStateException("Cannot get text for a message of type "+message.getClass().toString());
		}
		return ((TextMessage)message).getText();
	}
	
	/**
	 * Provide access to the low-level message encapsulated in this object.
	 * @return the low-level message object.
	 */
	public Message getMessage()
	{
		return message;
	}
}
