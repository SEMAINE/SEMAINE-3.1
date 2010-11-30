/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.receiver;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.jms.ConnectionMetaData;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.JMSLogger;
import eu.semaine.jms.SEMAINEMessageAvailableListener;
import eu.semaine.jms.message.SEMAINEMessage;

/**
 * This class handles the common part to all message receivers in
 * the SEMAINE system.
 * 
 * Two models of use are possible:
 * <ul>
 * <li>synchronous API: client code calls the methods {@link #receive()},
 * {@link #receive(long)} or {@link #receiveNoWait()} directly;</li>
 * <li>asynchronous API: client code registers a {@link SEMAINEMessageAvailableListener};
 * whenever a new message arrives, the Receiver thread will call that listener's 
 * {@link SEMAINEMessageAvailableListener#messageAvailableFrom(Receiver)} method.
 * The listener code then calls the Receiver's {@link #getMessage()} method from the 
 * thread that should actually do the processing of the message. Note that in the
 * asynchronous API, the Receiver will block until getMessage() was called.</li>
 * </ul>
 * 
 * The Receiver itself covers basic text messages only.
 * <strong>Subclasses</strong> should override {@link #createSEMAINEMessage(Message)}
 * to do a meaningful analysis of the JMS message in terms of a suitable subclass of
 * {@link SEMAINEMessage}.
 * 
 * @author marc
 *
 */
public class Receiver extends IOBase implements MessageListener
{
	protected MessageConsumer consumer;
	
	protected SEMAINEMessageAvailableListener listener;
	
	protected JMSLogger log;
	
	protected SEMAINEMessage message = null;
		
	/**
	 * Create a receiver that will listen for all messages in the given Topic
	 * on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: read from system property <code>jms.url</code>, default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: read from system property <code>jms.user</code>, default value <code>null</code>;</li>
	 * <li>jms Password: read from system property <code>jms.password</code>, default value <code>null</code>.</li>
	 * </ul>
	 * @param topic the name of the JMS Topic to listen to.
	 */
	public Receiver(String topicName) throws JMSException
	{
		super(topicName);
		consumer = session.createConsumer(topic);
		log = JMSLogger.getLog("Receiver");
	}
	
	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector,
	 * on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: read from system property <code>jms.url</code>, default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: read from system property <code>jms.user</code>, default value <code>null</code>;</li>
	 * <li>jms Password: read from system property <code>jms.password</code>, default value <code>null</code>.</li>
	 * </ul>
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	public Receiver(String topicName, String messageSelector)
	throws JMSException
	{
		super(topicName);
		consumer = session.createConsumer(topic, messageSelector);
		log = JMSLogger.getLog("Receiver");
	}
	
	/**
	 * Create a receiver that will listen for all messages in the given Topic
	 * on the given JMS server.
	 * @param jmsUrl the url where to contact the JMS server
	 * @param jmsUser the username to use (can be null)
	 * @param jmsPassword the password to use (can be null)
	 * @param topic the name of the JMS Topic to listen to.
	 */
	public Receiver(String jmsUrl, String jmsUser, String jmsPassword, String topicName) throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName);
		consumer = session.createConsumer(topic);
		log = JMSLogger.getLog("Receiver");
	}
	
	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector,
	 * on the given JMS server.
	 * @param jmsUrl the url where to contact the JMS server
	 * @param jmsUser the username to use (can be null)
	 * @param jmsPassword the password to use (can be null)
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	public Receiver(String jmsUrl, String jmsUser, String jmsPassword, String topicName, String messageSelector)
	throws JMSException
	{
		super(jmsUrl, jmsUser, jmsPassword, topicName);
		consumer = session.createConsumer(topic, messageSelector);
		log = JMSLogger.getLog("Receiver");
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
	
	///////////////// The abstraction step: create SEMAINE message ///////
	
	/**
	 * Create a SEMAINE message from the given JMS message.
	 * Subclasses may want to override this in order to 
	 * provide subclasses of SEMAINEMessage for their data.
	 * @param message the JMS method to convert into a SEMAINE message.
	 * @throws MessageFormatException if the message format is problematic
	 * @throws NullPointerException if message is null.
	 */
	protected SEMAINEMessage createSEMAINEMessage(Message message)
	throws MessageFormatException
	{
		if (message == null)
			throw new NullPointerException("cannot create semaine message from null");
		return new SEMAINEMessage(message);
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
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		Message m = consumer.receive();
		if (m == null) return null;
		return createSEMAINEMessage(m);
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
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		Message m = consumer.receive(timeout);
		if (m == null) return null;
		return createSEMAINEMessage(m);
	}

	/**
	 * Receives the next message if one is immediately available.
     * @return the next message produced for this message consumer, or null if one is not available
     * @throws JMSException if the JMS provider fails to receive the next message due to some internal error.
	 */
	public SEMAINEMessage receiveNoWait()
	throws JMSException
	{
		if (!isConnectionStarted)
			throw new IllegalStateException("Connection is not started!");
		Message m = consumer.receiveNoWait();
		if (m == null) return null;
		return createSEMAINEMessage(m);
	}

	////////////////// Asynchronous message consumption ////////////////

	public void setMessageListener(SEMAINEMessageAvailableListener aListener)
	throws JMSException
	{
		this.listener = aListener;
		if (listener == null)
			consumer.setMessageListener(null);
		else
			consumer.setMessageListener(this);
	}
	
	public SEMAINEMessageAvailableListener getMessageListener()
	{
		return listener;
	}
	
	public synchronized void onMessage(Message m)
	{
		assert listener != null : "asynchronous mode, but no SEMAINE message listener registered!";
		if (m != null) {
			try {
				message = createSEMAINEMessage(m);
				// Notify component that new message has arrived
				listener.messageAvailableFrom(this);
				// Block until message has been picked up
				synchronized (this) {
					while (notPickedUp()) {
						try {
							this.wait();
						} catch (InterruptedException ie) {}
					}
				}
			} catch (MessageFormatException mfe) {
				log.warn("Problem creating SEMAINE message: ", mfe);
			}
		}
	}
	
	
	public synchronized SEMAINEMessage getMessage()
	{
		SEMAINEMessage m = this.message;
		this.message = null;
		this.notify();
		return m;
	}
	
	public synchronized boolean notPickedUp()
	{
		return message != null;
	}
	
	/////////////////////// Simplistic test code ////////////////////////
	public static void main(String[] args) throws Exception
	{
		Receiver r = new Receiver("testtopic");
		r.startConnection();
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
