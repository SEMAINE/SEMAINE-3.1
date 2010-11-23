/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.Session;
import javax.jms.Topic;

import org.apache.activemq.ActiveMQConnection;
import org.apache.activemq.ActiveMQConnectionFactory;
import org.apache.activemq.broker.Broker;
import org.apache.activemq.broker.BrokerFactory;
import org.apache.activemq.broker.BrokerService;

/**
 * This class handles the low-level interaction with the JMS provider for
 * the components of the SEMAINE system.
 * 
 * @author marc
 *
 */
public class IOBase
{
	/**
	 * An exception listener which will react in the following way when an exception occurs:
	 * 1. it will log an error on the JMS logger;
	 * 2. it will call {@link IOBase#setException(JMSException)} on each IOBase object
	 * that has been registered using {@link #addCustomer(IOBase)}.
	 * @author marc
	 *
	 */
	private static final class IOBaseExceptionListener implements ExceptionListener {
		private Set<IOBase> myCustomers = new HashSet<IOBase>();
		
		/**
		 * Add an IOBase to the list of IOBase objects to be notified when an exception occurs.
		 * @param iob an IOBase, must not be null
		 */
		private void addCustomer(IOBase iob) {
			assert iob != null;
			myCustomers.add(iob);
		}
		
		@Override
		public void onException(JMSException e) {
			JMSLogger.getLog("Connection").error("Exception Listener: ", e);
			for (IOBase iob : myCustomers) {
				iob.setException(e);
			}
		}
	}

	public static enum Event { start, end, single };
	
	
	///////////////////////////// Static stuff ///////////////////////////////////
	
	private static BrokerService embeddedBroker = null;
	private static Map<String,Connection> theConnections = new HashMap<String, Connection>();
	
	public static synchronized void useEmbeddedBroker() throws Exception {
		if (embeddedBroker != null) {
			return;
		}
		embeddedBroker = BrokerFactory.createBroker("xbean:/eu/semaine/jms/activemq.xml", true /* start broker */);
	}
	
	
	public static boolean isEmbeddedBroker() {
		return embeddedBroker != null;
	}
	
	
	
	/**
	 * Get a Connection to a JMS server. This will reuse an existing connection if possible,
	 * and only create a new Connection if no connection to the given server coordinates exists yet.
	 * If this process is using an embedded broker, this will use a local VM connection, ignoring the connection info given as parameters.
	 * @param serverUrl the server url, e.g. "tcp://localhost:61616"
	 * @param serverUser the user name, or null
	 * @param serverPassword the password, or null
	 * @param iobase an iobase object that wants to be notified if an exception occurs, or null.
	 * @return a valid connection object. It is undetermined whether the connection is already started.
	 * @throws JMSException if the connection cannot be created for some reason.
	 */
	protected static synchronized Connection getConnection(String serverUrl, String serverUser, String serverPassword, IOBase iobase) throws JMSException {
		String key = serverUrl+serverUser+serverPassword;
		assert theConnections != null;
		Connection c = theConnections.get(key);
		if (c == null || getConnectionStatus(c) == ConnectionStatus.closed) {
			c = createConnection(serverUrl, serverUser, serverPassword);
			assert c != null;
			theConnections.put(key, c);
		}
		if (iobase != null) {
			((IOBaseExceptionListener)c.getExceptionListener()).addCustomer(iobase);
		}
		assert c != null;
		return c;
	}
	
	/**
	 * Create a new Connection to a JMS server. This should not normally be called from user code,
	 * since a Connection is a resource-intensive / heavyweight object. 
	 * If this process is using an embedded broker, this will create a local VM connection, ignoring the connection info given as parameters.
	 * 
	 * Use {@link #getConnection(String, String, String)}
	 * instead to re-use an existing connection if possible. 
	 * 
	 * @param serverUrl the server url, e.g. "tcp://localhost:61616"
	 * @param serverUser the user name, or null
	 * @param serverPassword the password, or null
	 * @return a valid connection object which is not yet started.
	 * @throws JMSException if the connection cannot be created for some reason.
	 */
	protected static synchronized Connection createConnection(String serverUrl, String serverUser, String serverPassword) throws JMSException {
		ConnectionFactory factory;
		if (isEmbeddedBroker()) {
			factory = new ActiveMQConnectionFactory("vm://localhost?create=false");
		} else {
			factory = new ActiveMQConnectionFactory(serverUser, serverPassword, serverUrl);
		}
		final Connection c = factory.createConnection();
		
		// Some configuration settings to improve performance:
		if (c instanceof ActiveMQConnection) {
			ActiveMQConnection amc = (ActiveMQConnection) c;
			// Send asynchronously, so that even in the case of a flooded broker, the send() returns,
			// and we can detect the error condition (ExceptionListener sets exception, detected at next send())
			amc.setUseAsyncSend(true);
			// No need to copy the messaage when sending, since we always discard the message object after sending:
			amc.setCopyMessageOnSend(false);
			// acknowledge in batches:
			amc.setOptimizeAcknowledge(true);
			
		}
		c.setExceptionListener(new IOBaseExceptionListener());
		Runtime.getRuntime().addShutdownHook(new Thread() {
			@Override public void run() { 
				try {
					c.close();
				} catch (JMSException e) {
					System.err.println("Problem during shutdown: cannot close connection");
					e.printStackTrace();
				}
			}
		});
		assert c != null;
		return c;
	}
	
	public enum ConnectionStatus {unknown, started, stopped, closed};
	/**
	 * Try to determine the status of the connection.
	 * @param c a connection.
	 * @return one of the following ConnectionStatus values:
	 * <ul>
	 * <li>unknown if we cannot determine the status;</li>
	 * <li>started if the connection has been started and not stopped or closed;</li>
	 * <li>closed if the connection is closing or has been closed;</li>
	 * <li>stopped if the connection has not yet been started or has been stopped.</li>
	 * </ul>
	 * 
	 */
	public static ConnectionStatus getConnectionStatus(Connection c) {
		if (!(c instanceof ActiveMQConnection)) {
			return ConnectionStatus.unknown;
		}
		ActiveMQConnection amc = (ActiveMQConnection) c;
		if (amc.isStarted()) {
			return ConnectionStatus.started;
		} else if (amc.isClosed() || amc.isClosing()) {
			return ConnectionStatus.closed;
		}
		return ConnectionStatus.stopped;
	}
	
	
	/////////////////////////// Non-static stuff /////////////////////////////////
	
	
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
		this.connection = getConnection(jmsUrl, jmsUser, jmsPassword, this);
		assert connection != null;
		//this.session = connection.createSession(false /*not transacted*/, Session.AUTO_ACKNOWLEDGE);
		this.session = connection.createSession(false /*not transacted*/, Session.DUPS_OK_ACKNOWLEDGE);
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
	
	/**
	 * Get the user name used when connecting to the JMS server.
	 * @return the user name as a string, or null if no user name was given.
	 */
	public String getJMSUser() {
		return jmsUser;
	}

	/**
	 * Get the password used when connecting to the JMS server.
	 * @return the password as a string, or null if no password was given.
	 */
	public String getJMSPassword() {
		return jmsPassword;
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
