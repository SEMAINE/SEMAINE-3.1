/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.sender;

import javax.jms.BytesMessage;
import javax.jms.JMSException;

import eu.semaine.jms.message.SEMAINEMessage;

/**
 * @author marc
 *
 */
public class BytesSender extends Sender 
{

	/**
	 * @param topicName
	 * @param datatype
	 * @param source
	 * @throws JMSException
	 */
	public BytesSender(String topicName, String datatype, String source)
			throws JMSException {
		super(topicName, datatype, source);
	}

	/**
	 * @param jmsUrl
	 * @param jmsUser
	 * @param jmsPassword
	 * @param topicName
	 * @param datatype
	 * @param source
	 * @throws JMSException
	 */
	public BytesSender(String jmsUrl, String jmsUser, String jmsPassword,
			String topicName, String datatype, String source)
			throws JMSException {
		super(jmsUrl, jmsUser, jmsPassword, topicName, datatype, source);
	}

	/**
	 * Send a bytes message via this sender.
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
	 * @param bytes the message data, as a byte array.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @throws IllegalStateException if the connection is not started or the sender is in event-based mode.
	 */
	public void sendBytesMessage(byte[] bytes, long usertime)
	throws JMSException
	{
		sendBytesMessage(bytes, usertime, null, -1);
	}
	
	/**
	 * Send a bytes message via this sender.
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
	 * @param bytes the message data, as a byte array.
	 * @param usertime the "user" time that this message refers to,
	 * in milliseconds since system startup.
	 * @param contentID a unique identifier for the message's content.
	 * If this is not null, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @throws IllegalStateException if the connection is not started or the sender is in event-based mode.
	 */
	public void sendBytesMessage(byte[] bytes, long usertime, String contentID, long contentCreationTime)
	throws JMSException {
		sendBytesMessage(bytes, usertime, contentID, contentCreationTime, null);
	}

	/**
	 * Send a bytes message via this sender.
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
	 * @param bytes the message data, as a byte array.
	 * @param usertime the "user" time that this message refers to,
	 * in milliseconds since system startup.
	 * @param contentID a unique identifier for the message's content.
	 * If this is not null, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @param contentType an optional content type for the message's content;
	 * The value may be one of SEMAINEMessage.CONTENT_TYPE_UTTERANCE, SEMAINEMessage.CONTENT_TYPE_LISTENERVOCALISATION,
	 * SEMAINEMessage.CONTENT_TYPE_VISUALONLY, or any other string. Can be null, in which case no content type will be sent.
	 * @throws IllegalStateException if the connection is not started or the sender is in event-based mode.
	 */
	public void sendBytesMessage(byte[] bytes, long usertime, String contentID, long contentCreationTime, String contentType)
	throws JMSException
	{
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		BytesMessage message = session.createBytesMessage();
		message.writeBytes(bytes);
		fillMessageProperties(message, usertime, contentID, contentCreationTime, contentType);
		if (isPeriodic())
			message.setIntProperty(SEMAINEMessage.PERIOD, getPeriod());
		else // event-based
			message.setStringProperty(SEMAINEMessage.EVENT, Event.single.toString());
		producer.send(message);
	}

}
