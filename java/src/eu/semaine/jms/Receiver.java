/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.jms.ConnectionMetaData;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;

/**
 * This class handles the common part to all message receivers in
 * the SEMAINE system.
 * @author marc
 *
 */
public class Receiver extends IOBase implements MessageListener
{
	protected MessageConsumer consumer;
	
	protected SEMAINEMessageListener listener;
		
	/**
	 * Create a receiver that will listen for all messages in the given Topic.
	 * @param topic the name of the JMS Topic to listen to.
	 */
	public Receiver(String topicName) throws JMSException
	{
		super(topicName);
		consumer = session.createConsumer(topic);
		connection.start();
	}
	
	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector. 
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	public Receiver(String topicName, String messageSelector)
	throws JMSException
	{
		super(topicName);
		consumer = session.createConsumer(topic, messageSelector);
		connection.start();
	}
	
	/**
	 * Get the message selector pattern active in this receiver to filter messages.
	 * @return a string representation of the selector pattern.
	 * @throws JMSException
	 */
	public String getMessageSelector()
	throws JMSException
	{
		return consumer.getMessageSelector();
	}
	
	
	////////////////// Synchronous message consumption ////////////////
	
	/**
	 * Receives the next message produced for this message consumer.
     * This call blocks indefinitely until a message is produced or until this message consumer is closed.
     * @return the next message produced for this message consumer, or null if this message consumer is concurrently closed
     * @throws JMSException if the JMS provider fails to receive the next message due to some internal error.
	 */
	public SEMAINEMessage receive()
	throws JMSException
	{
		Message m = consumer.receive();
		if (m == null) return null;
		return new SEMAINEMessage(m);
	}
	
	/**
	 * Receives the next message that arrives within the specified timeout interval.
	 * This call blocks until a message arrives, the timeout expires, or this message consumer is closed. A timeout of zero never expires, and the call blocks indefinitely.
	 * @param timeout the timeout value (in milliseconds)
	 * @return the next message produced for this message consumer, or null if the timeout expires or this message consumer is concurrently closed 
     * @throws JMSException if the JMS provider fails to receive the next message due to some internal error.
	 */
	public SEMAINEMessage receive(long timeout)
	throws JMSException
	{
		Message m = consumer.receive(timeout);
		if (m == null) return null;
		return new SEMAINEMessage(m);
	}

	/**
	 * Receives the next message if one is immediately available.
     * @return the next message produced for this message consumer, or null if one is not available
     * @throws JMSException if the JMS provider fails to receive the next message due to some internal error.
	 */
	public SEMAINEMessage receiveNoWait()
	throws JMSException
	{
		Message m = consumer.receiveNoWait();
		if (m == null) return null;
		return new SEMAINEMessage(m);
	}

	////////////////// Asynchronous message consumption ////////////////

	public void setMessageListener(SEMAINEMessageListener aListener)
	throws JMSException
	{
		this.listener = aListener;
		if (listener == null)
			consumer.setMessageListener(null);
		else
			consumer.setMessageListener(this);
	}
	
	public SEMAINEMessageListener getMessageListener()
	{
		return listener;
	}
	
	public void onMessage(Message message)
	{
		assert listener != null : "asynchronous mode, but no SEMAINE message listener registered!";
		SEMAINEMessage m = (message != null ? new SEMAINEMessage(message) : null);
		listener.onMessage(m);
	}
	
	
	/////////////////////// Simplistic test code ////////////////////////
	public static void main(String[] args) throws Exception
	{
		Receiver r = new Receiver("testtopic");
		ConnectionMetaData meta = r.getConnection().getMetaData();
		String provider = meta.getJMSProviderName();
		String providerVersion = meta.getProviderVersion();
		String jmsVersion = meta.getJMSVersion();
		System.out.println("Connected to "+r.getJMSUrl()+", JMS server info: "+
				provider+", version "+providerVersion +
				"(JMS version "+jmsVersion+")");
		System.out.println("Reading messages from topic "+r.getTopicName());
		DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");
		while (true) {
			SEMAINEMessage m = r.receive();
			System.out.println("usertime: "+dateFormat.format(new Date(m.getUsertime()))+" -- message: "+m.getText());
		}
	}

}
