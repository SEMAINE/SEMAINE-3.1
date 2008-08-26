/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.sender;

import javax.jms.JMSException;

/**
 * An abstraction of Sender for state data.
 * @author marc
 *
 */
public class FMLSender extends XMLSender
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
	 * @param source the name of the component sending the data.
	 * @throws NullPointerException if any of the parameters topicName, datatype, or source are null. 
	 */
	public FMLSender(String topicName, String source)
	throws JMSException
	{
		super(topicName, "FML", source);
	}

	/**
	 * Create a new Sender to the given topic on the given JMS server.
	 * Datatype and source will be sent with every message.
	 * @param jmsUrl the url where to contact the JMS server
	 * @param jmsUser the username to use (can be null)
	 * @param jmsPassword the password to use (can be null)
	 * @param topic the name of the topic to send to.
	 * @param source the name of the component sending the data.
	 * @throws JMSException
	 * @throws NullPointerException if any of the parameters jmsUrl, topicName, datatype, or source are null. 
	 */
	public FMLSender(String jmsUrl, String jmsUser, String jmsPassword, String topicName, String source)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName, "FML", source);
	}

}
