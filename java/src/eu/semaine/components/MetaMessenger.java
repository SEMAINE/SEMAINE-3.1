/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;

import eu.semaine.components.Component.State;
import eu.semaine.jms.IOBase;

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
	
	private String componentName;
	private MessageProducer producer;
	private MessageConsumer consumer;
	private boolean systemReady = false;
	
	public MetaMessenger(String componentName)
	throws JMSException
	{
		super("semaine.meta");
		this.componentName = componentName;
		consumer = session.createConsumer(topic, SYSTEM_READY + " IS NOT NULL");
		producer = session.createProducer(topic);
		consumer.setMessageListener(this);
		startConnection();
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
			assert m.propertyExists(SYSTEM_READY) : "message should contain header field '"+SYSTEM_READY+"'";
			setSystemReady(m.getBooleanProperty(SYSTEM_READY));
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
}
