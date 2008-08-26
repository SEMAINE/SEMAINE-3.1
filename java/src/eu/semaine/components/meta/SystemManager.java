/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.meta;

import java.util.Map;
import java.util.TreeMap;
import java.util.Map.Entry;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;

import eu.semaine.components.Component;
import eu.semaine.jms.IOBase;

/**
 * Communication between a component and the
 * system manager, transporting meta messages about the state of the 
 * component and of the system. 
 * 
 * We extend Component so that it is easy to start the system manager,
 * but we do things differently from normal modules. 
 * so we override run() rather than act() and react().
 * @author marc
 *
 */
public class SystemManager extends Component implements MessageListener
{
	private IOBase iobase;
	private MessageProducer producer;
	private MessageConsumer consumer;
	private Map<String, State> componentStates;
	// what was the last value we reported for "system ready"?
	private boolean lastReportSystemReady = false;

	public SystemManager()
	throws JMSException
	{
		super("SystemManager");
		iobase = new IOBase("semaine.meta");
		componentStates = new TreeMap<String, State>();
		consumer = iobase.getSession().createConsumer(iobase.getTopic(), MetaMessenger.COMPONENT_NAME + " IS NOT NULL");
		producer = iobase.getSession().createProducer(iobase.getTopic());
		consumer.setMessageListener(this);
		iobase.startConnection();
		// need to send our own start again because when super() sent it,
		// we weren't listening yet:
		meta.reportState(State.starting);
	}
	
	
	public void reportSystemReady(boolean ready)
	throws JMSException
	{
		log.info("System is "+(ready ? "" : "not ")+"ready");
		Message m = iobase.getSession().createMessage();
		m.setBooleanProperty(MetaMessenger.SYSTEM_READY, ready);
		producer.send(m);
		lastReportSystemReady = ready;
	}

	public void onMessage(Message m)
	{
		try {
			assert m.propertyExists(MetaMessenger.COMPONENT_NAME) : "message should contain header field '"+MetaMessenger.COMPONENT_NAME+"'";
			assert m.propertyExists(MetaMessenger.COMPONENT_STATE) : "message should contain header field '"+MetaMessenger.COMPONENT_STATE+"'";
			String componentName = m.getStringProperty(MetaMessenger.COMPONENT_NAME);
			State componentState = null;
			try {
				componentState = State.valueOf(m.getStringProperty(MetaMessenger.COMPONENT_STATE));
			} catch (IllegalArgumentException e) {
				log.warn("unknown component state '"+m.getStringProperty(MetaMessenger.COMPONENT_STATE)+"'");
				componentState = State.failure;
			}
			componentStates.put(componentName, componentState);
			if (log.isDebugEnabled()) {
				StringBuilder builder = new StringBuilder("Components:\n");
				for (Entry<String,State>entry : componentStates.entrySet()) {
					builder.append(entry.getKey()+": "+entry.getValue()+"\n");
				}
				log.debug(builder);
			}
			if (componentState != State.ready) { // system not ready
				if (lastReportSystemReady == true) {
					reportSystemReady(false);
				} // else, we were not ready anyway
			} else { // this component is ready, how about the others?
				boolean allReady = true;
				for (Entry<String,State> entry : componentStates.entrySet()) {
					if (entry.getValue() != State.ready) {
						allReady = false;
						break; // no point looking further
					}
				}
				if (allReady != lastReportSystemReady) {
					reportSystemReady(allReady);
				}
			}
		} catch (JMSException e) {
			log.error(e);
		}
	}

	public void run()
	{
		try {
			meta.reportState(State.ready);
		} catch (JMSException e) {
			log.error("cannot report component ready state");
			return;
		}
		while (!exitRequested()) {
			synchronized (this) {
				try {
					wait();
				} catch (InterruptedException ie) {}
			}
		}
		try {
			iobase.getConnection().close();
		} catch (JMSException e) {
			log.error("cannot close connection");
		}
	}
}
