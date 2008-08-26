/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.jms.sender;

import javax.jms.ConnectionMetaData;
import javax.jms.DeliveryMode;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageProducer;
import javax.jms.TextMessage;

import eu.semaine.jms.IOBase;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEMessage;


/**
 * This class handles the common part to all message senders in
 * the SEMAINE system.
 * @author marc
 *
 */
public class Sender extends IOBase
{
	protected MessageProducer producer;
	/**
	 * The name of the data type being sent.
	 */
	protected String datatype;
	/**
	 * The name of the source component from where the data originated.
	 */
	protected String source;
	/**
	 * If data is periodic, the period in which data is sent, in milliseconds.
	 * A value of 0 means not periodic. 
	 * Data can be either periodic or event-based, but not both at the same time.
	 */
	protected int period = 0;
	
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
	public Sender(String topicName, String datatype, String source)
	throws JMSException
	{
		super(topicName);
		initialise(datatype, source);
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
	public Sender(String jmsUrl, String jmsUser, String jmsPassword, String topicName, String datatype, String source)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName);
		initialise(datatype, source);
	}

	
	protected void initialise(String datatype, String source)
	throws JMSException
	{
		if (datatype == null || source == null) 
			throw new NullPointerException("datatype and source must be non-null");
		this.datatype = datatype;
		this.source = source;
		this.producer = session.createProducer(topic);
		// Do not allow for messages to be lost:
		this.producer.setDeliveryMode(DeliveryMode.PERSISTENT);
	}
	
	/**
	 * The name of the data type sent.
	 * @return a non-null and non-empty string.
	 */
	public String getDatatype()
	{
		return datatype;
	}
	
	/**
	 * The name of the source component sending the data.
	 * @return a non-null and non-empty string.
	 */
	public String getSource()
	{
		return source;
	}
	
	/**
	 * Determine that the data is periodic, and set the period in which data is (intended to be) sent, in milliseconds.
	 * @param aPeriod the period in which data is to be sent.
	 * @throws IllegalArgumentException if aPeriod is <= 0.
	 */
	public void setPeriodic(int aPeriod)
	{
		if (aPeriod <= 0)
			throw new IllegalArgumentException("Period must be positive");
		this.period = aPeriod;
	}
	
	/**
	 * Determine that the data is event-based, not periodic.
	 * Individual messages can transport different kinds of events,
	 * @see {@link #sendTextMessage(String, long, eu.semaine.jms.IOBase.Event)}.
	 */
	public void setEventBased()
	{
		this.period = 0;
	}

	/**
	 * Test whether this sender is set to send messages periodically. 
	 * @return true if sender is set to periodic, false otherwise.
	 */
	public boolean isPeriodic()
	{
		return period != 0;
	}
	
	/**
	 * Test whether this sender is set to send messages in an event-based way. 
	 * @return true if sender is set to event-based, false otherwise.
	 */
	public boolean isEventBased()
	{
		return period == 0;
	}

	/**
	 * For periodic senders, get the period of sending.
	 * @return An integer representing the period of sending, in milliseconds. 
	 */
	public int getPeriod()
	{
		if (isEventBased())
			throw new IllegalStateException("Cannot report period for event-based Sender!");
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
	public void setTimeToLive(long aTimeToLive)
	throws JMSException
	{
		producer.setTimeToLive(aTimeToLive);
	}
	
	/**
	 * Get the time to live for all messages sent by this sender, 
	 * i.e. the number of milliseconds before the message is considered
	 * obsolete by the JMS system. Messages that are not delivered by their
	 * expiration time are deleted by the JMS system.
	 * @return time to live, in milliseconds. 
	 * A value of 0 means unlimited time, i.e. the messages will not expire.
	 */
	public long getTimeToLive()
	throws JMSException
	{
		return producer.getTimeToLive();
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
	 * @throws IllegalStateException if the connection is not started or the sender is in event-based mode.
	 */
	public void sendTextMessage(String text, long usertime)
	throws JMSException
	{
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		TextMessage message = session.createTextMessage(text);
		fillMessageProperties(message, usertime);
		if (isPeriodic())
			message.setIntProperty(SEMAINEMessage.PERIOD, getPeriod());
		else // event-based
			message.setStringProperty(SEMAINEMessage.EVENT, Event.single.toString());
		producer.send(message);
	}
	
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
	public void sendTextMessage(String text, long usertime, Event eventType)
	throws JMSException
	{
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		if (isPeriodic())
			throw new IllegalStateException("This method is for event-based messages, but sender is in periodic mode.");
		TextMessage message = session.createTextMessage(text);
		fillMessageProperties(message, usertime);
		message.setStringProperty(SEMAINEMessage.EVENT, eventType.toString());
		producer.send(message);
	}
	
	/**
	 * Fill in the usual message properties as far as possible.
	 * Subclasses are encouraged to override this method, but should
	 * normally call this method as well (as <code>super.fillMessageProperties(message, usertime)</code>).
	 * @param a message object in preparation for sending.
	 * @param usertime the "user" time that this message refers to,
	 * in milliseconds since 1970.
	 */
	protected void fillMessageProperties(Message message, long usertime)
	throws JMSException
	{
		message.setStringProperty(SEMAINEMessage.DATATYPE, getDatatype());
		message.setStringProperty(SEMAINEMessage.SOURCE, getSource());
		message.setLongProperty(SEMAINEMessage.USERTIME, usertime);
	}
	
	
	/////////////////////// Simplistic test code ////////////////////////
	public static void main(String[] args) throws Exception
	{
		Sender s = new Sender("testtopic", "test data", "command line");
		s.startConnection();
		ConnectionMetaData meta = s.getConnection().getMetaData();
		String provider = meta.getJMSProviderName();
		String providerVersion = meta.getProviderVersion();
		String jmsVersion = meta.getJMSVersion();
		System.out.println("Connected to "+s.getJMSUrl()+", JMS server info: "+
				provider+", version "+providerVersion +
				"(JMS version "+jmsVersion+")");
		java.io.BufferedReader buf = new java.io.BufferedReader(new java.io.InputStreamReader(System.in));
		System.out.println("Sending messages to topic "+s.getTopicName());
		String line;
		while ((line = buf.readLine()) != null) {
			s.sendTextMessage(line, System.currentTimeMillis());
		}
	}
}
