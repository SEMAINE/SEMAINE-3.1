/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import javax.jms.Connection;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;

import org.apache.activemq.ActiveMQConnection;
import org.apache.activemq.command.ActiveMQMessage;
import org.apache.activemq.command.ConsumerInfo;
import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.Sender;


/**
 * @author marc
 *
 */
public class IOBaseTest {
	
	
	/////////////// Utility methods /////////////
	
	private Connection createFreshDefaultConnection() throws JMSException {
		String jmsUrl = System.getProperty("jms.url", "tcp://localhost:61616");
		String jmsUser = System.getProperty("jms.user", null);
		String jmsPassword = System.getProperty("jms.password", null);
		return IOBase.createConnection(jmsUrl, jmsUser, jmsPassword);
	}

	private Connection getDefaultConnection() throws JMSException {
		String jmsUrl = System.getProperty("jms.url", "tcp://localhost:61616");
		String jmsUser = System.getProperty("jms.user", null);
		String jmsPassword = System.getProperty("jms.password", null);
		return IOBase.getConnection(jmsUrl, jmsUser, jmsPassword, null);
	}
	
	private void assertIsVMConnection(Connection c) throws AssertionError {
		ActiveMQConnection amc = (ActiveMQConnection) c;
		if (!amc.getTransport().toString().startsWith("vm:")) {
			throw new AssertionError("Not a VM connection: "+amc.getTransport().toString());
		}
	}
	
	//////////////// Tests /////////////////
	
	@BeforeClass
	public static void getStarted() throws Exception {
		BasicConfigurator.configure();
		Logger.getLogger("org.apache").setLevel(Level.WARN);
		Logger.getLogger("org.springframework").setLevel(Level.WARN);
		IOBase.useEmbeddedBroker();
	}
	
	
	@Test
	public void connectionComesFromStatic() throws JMSException {
		// Setup fixture
		IOBase iob = new IOBase("dummy");
		Connection refConnection = iob.getConnection();
		// exercise SUT
		Connection directConnection = IOBase.getConnection(iob.getJMSUrl(), iob.getJMSUser(), iob.getJMSPassword(), null);
		// verify result
		assertEquals("connection does not come from static code!", refConnection, directConnection);
	}


	@Test
	public void connectionIsSame() throws JMSException {
		// Setup fixture
		IOBase iob1 = new IOBase("dummy1");
		// exercise SUT
		IOBase iob2 = new IOBase("dummy2");
		// verify result
		assertEquals("should have been same connection", iob1.getConnection(), iob2.getConnection());
	}
	
	
	@Test
	public void canDoubleStart() throws JMSException {
		// Setup fixture
		Connection c = getDefaultConnection();
		// exercise SUT
		c.start();
		c.start();
		// test is that there is no exception thrown upon the second start
	}
	
	@Test
	public void isRobustToCloseConnection() throws JMSException {
		// setup
		IOBase iob1 = new IOBase("dummy");
		// exercise
		iob1.getConnection().close();
		// verify
		IOBase iob2 = new IOBase("dummy1");
		// test passes if no exception is thrown
	}
	
	@Test
	public void checkStatusInitial() throws JMSException {
		// setup
		Connection c = createFreshDefaultConnection();
		// verify
		assertEquals("connection should be in stopped state", IOBase.getConnectionStatus(c), IOBase.ConnectionStatus.stopped);
	}
	
	@Test
	public void checkStatusStarted() throws JMSException {
		// setup
		Connection c = createFreshDefaultConnection();
		// exercise
		c.start();
		// verify
		assertEquals("connection should be in started state", IOBase.getConnectionStatus(c), IOBase.ConnectionStatus.started);
	}
	
	@Test
	public void checkStatusStopped() throws JMSException {
		// setup
		Connection c = createFreshDefaultConnection();
		// exercise
		c.start();
		c.stop();
		// verify
		assertEquals("connection should be in stopped state", IOBase.getConnectionStatus(c), IOBase.ConnectionStatus.stopped);
	}

	@Test
	public void checkStatusClosed() throws JMSException {
		// setup
		Connection c = createFreshDefaultConnection();
		// exercise
		c.start();
		c.close();
		// verify
		assertEquals("connection should be in closed state", IOBase.getConnectionStatus(c), IOBase.ConnectionStatus.closed);
	}
	
	@Test
	public void canCreateEmbeddedBroker() throws Exception {
		// Setup
		IOBase.useEmbeddedBroker();
		// verify
		assertTrue("cannot switch to embedded broker", IOBase.isEmbeddedBroker());
	}
	
	@Test
	public void secondStartEmbeddedBrokerIsHarmless() throws Exception {
		// setup
		IOBase.useEmbeddedBroker();
		// exercise
		IOBase.useEmbeddedBroker();
		// no exception = success
	}
	
	@Test
	public void embeddedBrokerCreatesVMConnections() throws Exception {
		// Setup
		IOBase.useEmbeddedBroker();
		// exercise
		Connection c = createFreshDefaultConnection();
		// verify
		assertIsVMConnection(c);
	}
	
}
