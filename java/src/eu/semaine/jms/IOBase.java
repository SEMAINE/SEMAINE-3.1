/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.Session;
import javax.jms.Topic;

import org.apache.activemq.ActiveMQConnection;
import org.apache.activemq.ActiveMQConnectionFactory;

/**
 * This class handles the low-level interaction with the JMS provider for
 * the components of the SEMAINE system.
 * 
 * @author marc
 *
 */
public class IOBase
{
	public static enum Event { start, end, single };
	
	protected String jmsUrl;
	protected String jmsUser;
	protected String jmsPassword;
	protected Connection connection;
	protected Session session;
	protected Topic topic;
	protected String topicName;
	protected boolean isConnectionStarted = false;
	protected JMSException exception = null;
	
	/**
	 * Create a new JMS IOBase connection with the given topic on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: read from system property <code>jms.url</code>, default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: read from system property <code>jms.user</code>, default value <code>null</code>;</li>
	 * <li>jms Password: read from system property <code>jms.password</code>, default value <code>null</code>.</li>
	 * </ul>
	 * @param topicName the name of the topic to send to.
	 * @throws JMSException
	 * @throws NullPointerException if any of the parameters jmsUrl or topicName are null. 
	 */
	public IOBase(String topicName) throws JMSException
	{
		jmsUrl = System.getProperty("jms.url", "tcp://localhost:61616");
		jmsUser = System.getProperty("jms.user", null);
		jmsPassword = System.getProperty("jms.password", null);
		initialise(topicName);
		
	}
	
	/**
	 * Create a new JMS IOBase connection with the given topic on the given JMS server.
	 * @param jmsUrl the url where to contact the JMS server
	 * @param jmsUser the username to use (can be null)
	 * @param jmsPassword the password to use (can be null)
	 * @param topicName the name of the topic to send to.
	 * @throws JMSException
	 * @throws NullPointerException if any of the parameters jmsUrl or topicName are null. 
	 */
	public IOBase(String jmsUrl, String jmsUser, String jmsPassword, String topicName)
	throws JMSException
	{
		this.jmsUrl = jmsUrl;
		this.jmsUser = jmsUser;
		this.jmsPassword = jmsPassword;
		initialise(topicName);
	}
	
	private void initialise(String topicName)
	throws JMSException
	{
		if (topicName == null)
			throw new NullPointerException("Topic must be given, cannot be null");
		if (jmsUrl == null)
			throw new NullPointerException("Need JMS server url to connect to, got null");
		ConnectionFactory factory = new ActiveMQConnectionFactory(jmsUser, jmsPassword, jmsUrl);
		this.connection = factory.createConnection();
		// Send asynchronously, so that even in the case of a flooded broker, the send() returns,
		// and we can detect the error condition (ExceptionListener sets exception, detected at next send())
		((ActiveMQConnection)connection).setUseAsyncSend(true);
		connection.setExceptionListener(new ExceptionListener() {
			@Override
			public void onException(JMSException e) {
				JMSLogger.getLog("Connection").error("Exception Listener: ", e);
				IOBase.this.setException(e);
			}
		});
		Runtime.getRuntime().addShutdownHook(new Thread() {
			@Override public void run() { 
				try {
					connection.close();
				} catch (JMSException e) {
					System.err.println("Problem during shutdown: cannot close connection");
					e.printStackTrace();
				}
			}
		});
		this.session = connection.createSession(false /*not transacted*/, Session.AUTO_ACKNOWLEDGE);
		this.topic = session.createTopic(topicName);
		this.topicName = topicName;
	}
	
	/**
	 * Get the name of the topic.
	 * @return a string containing the topic name.
	 * @throws JMSException
	 */
	public String getTopicName()
	{
		return topicName;
	}

	/**
	 * Get the URL of the JMS server connected to.
	 * @return a string containing the url
	 */
	public String getJMSUrl()
	{
		return jmsUrl;
	}
		
	public Connection getConnection()
	{
		return connection;
	}
	
	public Session getSession()
	{
		return session;
	}

	public Topic getTopic()
	{
		return topic;
	}
	
	public void startConnection()
	throws JMSException
	{
		connection.start();
		isConnectionStarted = true;
		
	}
	
	/**
	 * Method called by the ExceptionListener to inform us of the fact that an exception occurred.
	 * @param e
	 */
	private void setException(JMSException e) {
		this.exception = e;
	}
	
}
