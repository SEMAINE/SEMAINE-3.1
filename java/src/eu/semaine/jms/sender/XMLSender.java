/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.sender;

import javax.jms.JMSException;
import javax.jms.Message;

import org.w3c.dom.Document;

import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.util.XMLTool;

/**
 * An abstraction of Sender for feature vectors.
 * @author marc
 *
 */
public class XMLSender extends Sender
{
	
	/**
	 * Create a new Sender to the given topic on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: read from system property <code>jms.url</code>, default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: read from system property <code>jms.user</code>, default value <code>null</code>;</li>
	 * <li>jms Password: read from system property <code>jms.password</code>, default value <code>null</code>.</li>
	 * </ul>
	 * Datatype and source will be sent with every message.
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @throws NullPointerException if any of the parameters topicName, datatype, or source are null. 
	 * @throws IllegalArgumentException if period is <= 0.
	 */
	public XMLSender(String topicName, String datatype, String source)
	throws JMSException, SystemConfigurationException
	{
		super(topicName, datatype, source);
	}

	/**
	 * Create a new Sender to the given topic on the given JMS server.
	 * Datatype and source will be sent with every message.
	 * @param jmsUrl the url where to contact the JMS server
	 * @param jmsUser the username to use (can be null)
	 * @param jmsPassword the password to use (can be null)
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @throws JMSException
	 * @throws NullPointerException if any of the parameters jmsUrl, topicName, datatype, or source are null. 
	 * @throws IllegalArgumentException if period is <= 0.
	 */
	public XMLSender(String jmsUrl, String jmsUser, String jmsPassword, String topicName, String datatype, String source)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName, datatype, source);
	}


	/**
	 * Send an XML document as a "single" event.
	 * @param document the document to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @throws JMSException
	 */
	public void sendXML(Document document, long usertime)
	throws JMSException
	{
		sendXML(document, usertime, Event.single);
	}
	
	/**
	 * Send an XML document as a "single" event.
	 * @param document the document to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @param contentID a unique identifier for the message's content.
	 * If this is not null, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @throws JMSException
	 */
	public void sendXML(Document document, long usertime, String contentID, long contentCreationTime)
	throws JMSException
	{
		sendXML(document, usertime, Event.single, contentID, contentCreationTime);
	}
	
	/**
	 * Send an XML document as a "single" event.
	 * @param document the document to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @param contentID a unique identifier for the message's content.
	 * If this is not null, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @param contentType an optional content type for the message's content;
	 * The value may be one of SEMAINEMessage.CONTENT_TYPE_UTTERANCE, SEMAINEMessage.CONTENT_TYPE_LISTENERVOCALISATION,
	 * SEMAINEMessage.CONTENT_TYPE_VISUALONLY, or any other string. Can be null, in which case no content type will be sent.
	 * @throws JMSException
	 */
	public void sendXML(Document document, long usertime, String contentID, long contentCreationTime, String contentType)
	throws JMSException
	{
		sendXML(document, usertime, Event.single, contentID, contentCreationTime, contentType);
	}

	/**
	 * Send an XML document with the given event tag.
	 * @param document the document to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @param event the kind of event represented by this message
	 * @throws JMSException
	 * @throws NullPointerException if document or event is null
	 * @throws IllegalStateException if the sender is set to periodic mode.
	 */
	public void sendXML(Document document, long usertime, Event event)
	throws JMSException
	{
		sendXML(document, usertime, event, null, -1);
	}

	/**
	 * Send an XML document with the given event tag.
	 * @param document the document to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @param event the kind of event represented by this message
	 * @param contentID a unique identifier for the message's content.
	 * If this is not null, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @throws JMSException
	 * @throws NullPointerException if document or event is null
	 * @throws IllegalStateException if the sender is set to periodic mode.
	 */
	public void sendXML(Document document, long usertime, Event event, String contentID, long contentCreationTime)
	throws JMSException
	{
		sendXML(document, usertime, event, contentID, contentCreationTime, null);
	}
	
	/**
	 * Send an XML document with the given event tag.
	 * @param document the document to send.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @param event the kind of event represented by this message
	 * @param contentID a unique identifier for the message's content.
	 * If this is not null, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @param contentType an optional content type for the message's content;
	 * The value may be one of SEMAINEMessage.CONTENT_TYPE_UTTERANCE, SEMAINEMessage.CONTENT_TYPE_LISTENERVOCALISATION,
	 * SEMAINEMessage.CONTENT_TYPE_VISUALONLY, or any other string. Can be null, in which case no content type will be sent.
	 * @throws JMSException
	 * @throws NullPointerException if document or event is null
	 * @throws IllegalStateException if the sender is set to periodic mode.
	 */
	public void sendXML(Document document, long usertime, Event event, String contentID, long contentCreationTime, String contentType)
	throws JMSException
	{
		if (document == null)
			throw new NullPointerException("document passed as argument is null");
		if (event == null)
			throw new NullPointerException("document passed as argument is null");
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		if (isPeriodic())
			throw new IllegalStateException("This method is for event-based messages, but sender is in periodic mode.");
		sendTextMessage(XMLTool.document2String(document), usertime, event, contentID, contentCreationTime, contentType);
	}
	
	@Override
	protected void fillMessageProperties(Message message, long usertime)
	throws JMSException
	{
		super.fillMessageProperties(message, usertime);
		message.setBooleanProperty(SEMAINEXMLMessage.IS_XML, true);
	}
}

