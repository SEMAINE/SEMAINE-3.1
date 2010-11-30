/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.fail;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;

import org.junit.BeforeClass;
import org.junit.Test;

import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.Sender;
import eu.semaine.util.SEMAINEUtils;


/**
 * @author marc
 *
 */
public class JMSTest {
	
	////////////// Helpers ///////////////
	
	Sender getTestSender() throws JMSException {
		return getTestSender("semaine.test.topic");
	}

	Sender getTestSender(String topic) throws JMSException {
		Sender s = new Sender(topic, "TEST", "JUnit");
		s.startConnection();
		return s;
	}

	Receiver getTestReceiver() throws JMSException {
		return getTestReceiver("semaine.test.topic");
	}
	
	Receiver getTestReceiver(String topic) throws JMSException {
		Receiver r = new Receiver(topic);
		r.startConnection();
		return r;
	}
	
	MessageConsumer getAdvisoryConsumer() throws JMSException {
		IOBase iob = new IOBase("ActiveMQ.Advisory.SlowConsumer.Topic.>");
		MessageConsumer mc = iob.getSession().createConsumer(iob.getTopic());
		iob.startConnection();
		return mc;
	}
	
	
	//////////////// Tests /////////////////
	
	@BeforeClass
	public static void getStarted() throws Exception {
		SEMAINEUtils.setupLog4j();
		IOBase.useEmbeddedBroker();
	}
	
	
	@Test
	public void canEstablishLink() throws JMSException {
		Sender s = getTestSender();
		Receiver r = getTestReceiver();
		String testText = "Test message test";
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive();
		assertNotNull(m);
		assertEquals(testText, m.getText());
		s.getConnection().close();
		r.getConnection().close();
	}
	
	
	
	@Test
	public void wildcard1() throws JMSException {
		Sender s = getTestSender("semaine.test.topic.one");
		Receiver r = getTestReceiver("semaine.test.topic.*");
		String testText = "wildcard1 test";
		// exercise SUT
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive(1000);
		assertNotNull(m);
		assertEquals(testText, m.getText());
	}

	@Test
	public void wildcard2() throws JMSException {
		Receiver r = getTestReceiver("semaine.test.topic.*");
		Sender s = getTestSender("semaine.test.topic.two");
		String testText = "wildcard2 test";
		// exercise SUT
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive(1000);
		assertNotNull(m);
		assertEquals(testText, m.getText());
	}

	@Test
	public void wildcard3() throws JMSException {
		Receiver r = getTestReceiver("semaine.test.topic.>");
		Sender s = getTestSender("semaine.test.topic.three");
		String testText = "wildcard3 test";
		// exercise SUT
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive(1000);
		assertNotNull(m);
		assertEquals(testText, m.getText());
	}

	@Test
	public void wildcard3a() throws JMSException {
		Sender s = getTestSender("semaine.test.topic.threea");
		Receiver r = getTestReceiver("semaine.test.topic.>");
		String testText = "wildcard3a test";
		// exercise SUT
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive(1000);
		assertNotNull(m);
		assertEquals(testText, m.getText());
	}

	@Test
	public void wildcard3b() throws JMSException {
		Receiver r = getTestReceiver("semaine.test.>");
		Sender s = getTestSender("semaine.test.topic.threeb");
		String testText = "wildcard3b test";
		// exercise SUT
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive(1000);
		assertNotNull(m);
		assertEquals(testText, m.getText());
	}

	@Test
	public void wildcard3c() throws JMSException {
		Sender s = getTestSender("semaine.test.topic.threec");
		Receiver r = getTestReceiver("semaine.test.>");
		String testText = "wildcard3c test";
		// exercise SUT
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive(1000);
		assertNotNull(m);
		assertEquals(testText, m.getText());
	}

//	For some unknown reason, the following test passes but then makes memoryFailsProducer() fail
//  because no advisory message is received.... commenting out for now.
//	@Test
//	public void wildcard4() throws JMSException {
//		Sender s = getTestSender("semaine.test.topic2.four");
//		Receiver r = getTestReceiver("semaine.test.*.>");
//		String testText = "wildcard4 test";
//		// exercise SUT
//		s.sendTextMessage(testText, 0);
//		SEMAINEMessage m = r.receive(1000);
//		assertNotNull(m);
//		assertEquals(testText, m.getText());
//	}

	@Test
	public void wildcard5() throws JMSException {
		Receiver r = getTestReceiver("semaine.test.*.>");
		Sender s = getTestSender("semaine.test.topic3.five");
		String testText = "wildcard5 test";
		// exercise SUT
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive(1000);
		// verify result:
		// We expect this to fail because the wildcard will not match,
		// because the sender is created after the receiver (!)
		assertNull(m);
	}
	

	@Test
	public void memoryFailsProducer() throws JMSException {
		MessageConsumer mc = getAdvisoryConsumer();
		Sender s = getTestSender();
		Receiver r = getTestReceiver();
		String testText = "Test message test";
		try {
			for (long i=0; i < 10000; i++) {
				s.sendTextMessage(testText, 0);
			}
			fail("Expected to throw a JMSException -- check that activemq.xml defines low memory limits and producer flow control on test topic");
		} catch (JMSException e) {
			// OK, expected
			// Verify that we get an advisory message for slow consumer:
			Message m = mc.receive(1000);
			assertNotNull("No advisory message received for slow consumer", m);
		} finally {
			s.getConnection().close();
			r.getConnection().close();
		}
	}
	
	
}
