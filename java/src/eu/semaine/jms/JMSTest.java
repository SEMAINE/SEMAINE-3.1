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
public class JMSTest {
	
	////////////// Helpers ///////////////
	
	Sender getTestSender() throws JMSException {
		Sender s = new Sender("semaine.test.topic", "TEST", "JUnit");
		s.startConnection();
		return s;
	}
	
	Receiver getTestReceiver() throws JMSException {
		Receiver r = new Receiver("semaine.test.topic");
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
		BasicConfigurator.configure();
		Logger.getLogger("org.apache.activemq").setLevel(Level.INFO);
	}
	

	@Test
	public void canEstablishLink() throws JMSException {
		Sender s = getTestSender();
		Receiver r = getTestReceiver();
		String testText = "Test message test";
		s.sendTextMessage(testText, 0);
		SEMAINEMessage m = r.receive();
		assertTrue(m != null);
		assertEquals(testText, m.getText());
		s.getConnection().close();
		r.getConnection().close();
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
		} finally {
			s.getConnection().close();
			r.getConnection().close();
		}
		// Verify that we get an advisory message for slow consumer:
		Message m = mc.receive(1000);
		assertTrue("No advisory message received for slow consumer", m != null);
	}
	
	
}
