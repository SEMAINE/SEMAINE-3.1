/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.sender;

import java.io.StringWriter;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.TextMessage;

import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;
import org.w3c.dom.bootstrap.DOMImplementationRegistry;
import org.w3c.dom.ls.DOMImplementationLS;
import org.w3c.dom.ls.LSOutput;
import org.w3c.dom.ls.LSSerializer;

import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;

/**
 * An abstraction of Sender for feature vectors.
 * @author marc
 *
 */
public class XMLSender extends Sender
{
	
	protected LSSerializer serializer;
	protected DOMImplementationLS domImplLS;

	
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
		
		try {
			DOMImplementation implementation = DOMImplementationRegistry.newInstance().getDOMImplementation("XML 3.0");
			domImplLS = (DOMImplementationLS) implementation.getFeature("LS", "3.0");
			serializer = domImplLS.createLSSerializer();
		} catch (Exception e) {
			throw new SystemConfigurationException("Problem instantiating XML serializer code", e);
		}
		
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
	 * @param usertime the time in "user" space that these features
	 * refer to, in milliseconds since 1970.
	 * @throws JMSException
	 */
	public void sendXML(Document document, long usertime)
	throws JMSException
	{
		sendXML(document, usertime, Event.single);
	}
	
	/**
	 * Send an XML document with the given event tag.
	 * @param document the document to send.
	 * @param usertime the time in "user" space that these features
	 * refer to, in milliseconds since 1970.
	 * @param event the kind of event represented by this message
	 * @throws JMSException
	 * @throws NullPointerException if document or event is null
	 * @throws IllegalStateException if the sender is set to periodic mode.
	 */
	public void sendXML(Document document, long usertime, Event event)
	throws JMSException
	{
		if (document == null)
			throw new NullPointerException("document passed as argument is null");
		if (event == null)
			throw new NullPointerException("document passed as argument is null");
		if (isPeriodic())
			throw new IllegalStateException("XML sender is expected to be event-based, not periodic");
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		if (isPeriodic())
			throw new IllegalStateException("This method is for event-based messages, but sender is in periodic mode.");
		LSOutput output = domImplLS.createLSOutput();
		output.setEncoding("UTF-8");
		StringWriter buf = new StringWriter();
		output.setCharacterStream(buf);
		serializer.write(document, output);
		sendTextMessage(buf.toString(), usertime, event);
	}
	
	@Override
	protected void fillMessageProperties(Message message, long usertime)
	throws JMSException
	{
		super.fillMessageProperties(message, usertime);
		message.setBooleanProperty(SEMAINEXMLMessage.IS_XML, true);
	}
}

