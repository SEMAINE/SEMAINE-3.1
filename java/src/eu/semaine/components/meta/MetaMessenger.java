/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.meta;

import java.util.List;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;

import eu.semaine.components.Component.State;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.Sender;

/**
 * Communication between a component and the
 * system manager, transporting meta messages about the state of the 
 * component and of the system. 
 * @author marc
 *
 */
public class MetaMessenger extends IOBase implements MessageListener
{
	public static final String COMPONENT_NAME = "ComponentName";
	public static final String COMPONENT_STATE = "ComponentState";
	public static final String SYSTEM_READY = "SystemReady";
	public static final String SYSTEM_READY_TIME = "SystemReadyTime";
	public static final String RECEIVE_TOPICS = "ReceiveTopics";
	public static final String SEND_TOPICS = "SendTopics";
	
	private String componentName;
	private MessageProducer producer;
	private MessageConsumer consumer;
	private boolean systemReady = false;
	private long timeDelta;
	
	public MetaMessenger(String componentName)
	throws JMSException
	{
		super("semaine.meta");
		this.componentName = componentName;
		consumer = session.createConsumer(topic);
		producer = session.createProducer(topic);
		consumer.setMessageListener(this);
		startConnection();
	}
	
	public void reportTopics(List<Receiver> receivers, List<Sender> senders)
	throws JMSException
	{
		StringBuilder rec = new StringBuilder();
		for (Receiver r : receivers) {
			if (rec.length()>0) rec.append(" ");
			rec.append(r.getTopicName());
		}
		StringBuilder snd = new StringBuilder();
		for (Sender s : senders) {
			if (snd.length()>0) snd.append(" ");
			snd.append(s.getTopicName());
		}
		Message m = session.createMessage();
		m.setStringProperty(COMPONENT_NAME, componentName);
		m.setStringProperty(RECEIVE_TOPICS, rec.toString());
		m.setStringProperty(SEND_TOPICS, snd.toString());
		producer.send(m);
	}
	
	public void reportState(State state)
	throws JMSException
	{
		Message m = session.createMessage();
		m.setStringProperty(COMPONENT_NAME, componentName);
		m.setStringProperty(COMPONENT_STATE, state.toString());
		producer.send(m);
	}

	public void onMessage(Message m)
	{
		try {
			if (m.propertyExists(SYSTEM_READY)) {
				setSystemReady(m.getBooleanProperty(SYSTEM_READY));
			}
			if (m.propertyExists(SYSTEM_READY_TIME)) {
				setTime(m.getLongProperty(SYSTEM_READY_TIME));
			}
			
		} catch (JMSException e) {
			e.printStackTrace(System.err);
		}
	}

	private synchronized void setSystemReady(boolean ready)
	{
		systemReady = ready;
		notify();
	}
	
	public synchronized boolean isSystemReady()
	{
		return systemReady;
	}
	
	private void setTime(long systemTime)
	{
		timeDelta = systemTime - System.currentTimeMillis();
	}
	
	/**
	 * Get the current time in common, normalised time space.
	 * Processes should use only this method for determining the time.
	 * The value is counting milliseconds since some arbitrary point
	 * in time; therefore, the absolute time is not informative,
	 * but differences of time values are. 
	 * @return
	 */
	public long getTime()
	{
		return System.currentTimeMillis() + timeDelta;
	}
}
