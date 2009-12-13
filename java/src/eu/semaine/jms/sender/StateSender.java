/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.sender;


import javax.jms.JMSException;
import javax.jms.TextMessage;

import org.w3c.dom.Document;

import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.util.XMLTool;

/**
 * An abstraction of Sender for state data.
 * @author marc
 *
 */
public class StateSender extends XMLSender
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
	 */
	public StateSender(String topicName, StateInfo.Type datatype, String source)
	throws JMSException
	{
		super(topicName, datatype.toString(), source);
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
	 */
	public StateSender(String jmsUrl, String jmsUser, String jmsPassword, String topicName, String datatype, String source)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName, datatype, source);
	}

	/**
	 * Send the given state information.
	 * @param s a state information
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @throws IllegalStateException if the connection is not started or the sender is in periodic mode.
	 * @throws JMSException
	 */
	public void sendStateInfo(StateInfo s, long usertime)
	throws JMSException
	{
		sendStateInfo(s, usertime, null, -1);
	}

	/**
	 * Send the given state information.
	 * @param s a state information
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @param contentID a unique identifier for the message's content.
	 * If this is not null, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @throws IllegalStateException if the connection is not started or the sender is in periodic mode.
	 * @throws JMSException
	 */
	public void sendStateInfo(StateInfo s, long usertime, String contentID, long contentCreationTime)
	throws JMSException
	{
		if (s == null)
			throw new NullPointerException("state passed as argument is null");
		Document document = s.getDocument();
		if (document == null)
			throw new NullPointerException("document passed as argument is null");
		if (isPeriodic())
			throw new IllegalStateException("XML sender is expected to be event-based, not periodic");
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		if (isPeriodic())
			throw new IllegalStateException("This method is for event-based messages, but sender is in periodic mode.");
		TextMessage message = session.createTextMessage(XMLTool.document2String(document));
		fillMessageProperties(message, usertime, contentID, contentCreationTime);
		message.setStringProperty(s.toString()+"APIVersion", s.getAPIVersion());
		message.setStringProperty(SEMAINEMessage.EVENT, Event.single.toString());
		producer.send(message);
	}
}
