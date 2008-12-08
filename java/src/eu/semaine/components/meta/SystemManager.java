/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.meta;

import java.util.Arrays;
import java.util.Date;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.Map.Entry;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import javax.jms.TextMessage;
import javax.jms.Topic;

import eu.semaine.components.Component;
import eu.semaine.components.MessageLogComponent;
import eu.semaine.gui.monitor.ComponentInfo;
import eu.semaine.gui.monitor.SystemMonitor;
import eu.semaine.gui.monitor.TopicInfo;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.message.SEMAINEMessage;

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
	private static final Set<String> IGNORE_COMPONENTS = new HashSet<String>(
			Arrays.asList("SystemManager", "MessageLogComponent"));

	private IOBase iobase;
	private MessageProducer producer;
	private MessageConsumer consumer;
	private Map<String, ComponentInfo> componentInfos;
	// what was the last value we reported for "system ready"?
	private boolean lastReportSystemReady = false;
	private SystemMonitor systemMonitor;
	private MessageConsumer dataConsumer;

	public SystemManager()
	throws JMSException
	{
		super("SystemManager");
		iobase = new IOBase("semaine.meta");
		componentInfos = new TreeMap<String, ComponentInfo>();
		consumer = iobase.getSession().createConsumer(iobase.getTopic(), MetaMessenger.COMPONENT_NAME + " IS NOT NULL");
		producer = iobase.getSession().createProducer(iobase.getTopic());
		consumer.setMessageListener(this);
		iobase.startConnection();
		// need to send our own start again because when super() sent it,
		// we weren't listening yet:
		meta.reportState(State.starting);
		if (Boolean.getBoolean("semaine.systemmanager.gui")) {
			systemMonitor = new SystemMonitor(null);
			systemMonitor.start();
			dataConsumer = iobase.getSession().createConsumer(iobase.getSession().createTopic("semaine.data.>"));
			dataConsumer.setMessageListener(new MessageListener() {
				public void onMessage(Message m) {
					try {
						String topicName = ((Topic)m.getJMSDestination()).getTopicName();
						TopicInfo ti = systemMonitor.getTopicInfo(topicName);
						if (ti != null) {
							StringBuilder buf = new StringBuilder();
							SEMAINEMessage sm = new SEMAINEMessage(m);
							ti.addMessage(MessageLogComponent.message2logString(sm), sm.getSource());
						}
					} catch (JMSException e) {
						System.err.println("Cannot log message:");
						e.printStackTrace();
					}
				}
			});
		}
	}
	
	
	public void reportSystemReady(boolean ready)
	throws JMSException
	{
		log.info("System is "+(ready ? "" : "not ")+"ready");
		Message m = iobase.getSession().createMessage();
		m.setBooleanProperty(MetaMessenger.SYSTEM_READY, ready);
		if (ready) {
			// When we get ready, we set the system time to 0
			// Alternatively, we could set it to System.currentTimeMillis(),
			// which would set all clocks to this process's system clock.
			m.setLongProperty(MetaMessenger.SYSTEM_READY_TIME, 0);
		}
		producer.send(m);
		lastReportSystemReady = ready;
	}

	public void onMessage(Message m)
	{
		try {
			assert m.propertyExists(MetaMessenger.COMPONENT_NAME) : "message should contain header field '"+MetaMessenger.COMPONENT_NAME+"'";
			String componentName = m.getStringProperty(MetaMessenger.COMPONENT_NAME);
			ComponentInfo ci = componentInfos.get(componentName);
			if (ci == null) {
				ci = new ComponentInfo(componentName, null, null, false, false);
				componentInfos.put(componentName, ci);
				if (systemMonitor != null && !IGNORE_COMPONENTS.contains(componentName)) {
					// Tell GUI there is something new
					systemMonitor.addComponentInfo(ci);
				}
			}
			if (m.propertyExists(MetaMessenger.COMPONENT_STATE)) {
				State componentState = null;
				try {
					componentState = State.valueOf(m.getStringProperty(MetaMessenger.COMPONENT_STATE));
				} catch (IllegalArgumentException e) {
					log.warn("unknown component state '"+m.getStringProperty(MetaMessenger.COMPONENT_STATE)+"'");
					componentState = State.failure;
				}
				ci.setState(componentState);
				String details = "";
				if (m.propertyExists(MetaMessenger.COMPONENT_STATE_DETAILS)) {
					details = m.getStringProperty(MetaMessenger.COMPONENT_STATE_DETAILS);
					ci.setStateDetails(details);
				}
				if (log.isDebugEnabled()) {
					StringBuilder builder = new StringBuilder("Components:\n");
					for (Entry<String,ComponentInfo>entry : componentInfos.entrySet()) {
						builder.append(entry.getKey()+": "+entry.getValue().getState());
						String entryDetails = entry.getValue().getStateDetails();
						if (entryDetails != null)
							builder.append(" (").append(entryDetails).append(")");
						builder.append("\n");
					}
					log.debug(builder);
				}
				if (componentState != State.ready) { // system not ready
					if (lastReportSystemReady == true) {
						reportSystemReady(false);
					} // else, we were not ready anyway
				} else { // this component is ready, how about the others?
					boolean allReady = true;
					for (Entry<String,ComponentInfo> entry : componentInfos.entrySet()) {
						if (entry.getValue().getState() != State.ready) {
							allReady = false;
							break; // no point looking further
						}
					}
					if (allReady != lastReportSystemReady) {
						reportSystemReady(allReady);
					}
				}
			}
			if (m.propertyExists(MetaMessenger.RECEIVE_TOPICS)) {
				String receiveTopicsString = m.getStringProperty(MetaMessenger.RECEIVE_TOPICS);
				String[] receiveTopics = receiveTopicsString.length() > 0 ?
						receiveTopicsString.split(" ") : null;
				ci.setReceiveTopics(receiveTopics);
			}
			if (m.propertyExists(MetaMessenger.SEND_TOPICS)) {
				String sendTopicsString = m.getStringProperty(MetaMessenger.SEND_TOPICS);
				String[] sendTopics = sendTopicsString.length() > 0 ?
						sendTopicsString.split(" ") : null;
				ci.setSendTopics(sendTopics);
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
