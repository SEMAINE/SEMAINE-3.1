/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.meta;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.Map.Entry;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;
import javax.jms.Topic;

import org.apache.activemq.command.ActiveMQMessage;
import org.apache.activemq.command.ConsumerInfo;

import eu.semaine.components.Component;
import eu.semaine.components.MessageLogComponent;
import eu.semaine.gui.monitor.ComponentInfo;
import eu.semaine.gui.monitor.SystemMonitor;
import eu.semaine.gui.monitor.TopicInfo;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.JMSLogger;
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
	private static long PING_PERIOD = 1000; // in ms 

	private IOBase iobase;
	private MessageProducer producer;
	private MessageConsumer consumer;
	private Map<String, ComponentInfo> componentInfos;
	// what was the last value we reported for "system ready"?
	private boolean lastReportSystemReady = false;
	private SystemMonitor systemMonitor;
	private MessageConsumer dataConsumer;
	private MessageConsumer callbackConsumer;
	private long systemZeroTime;
	private Set<String> ignoreStalledComponents;
	private Set<String> componentsToHide;
	private Set<String> topicsToHide;
	
	public SystemManager()
	throws JMSException
	{
		super("SystemManager");
		systemZeroTime = System.currentTimeMillis();

		componentInfos = new TreeMap<String, ComponentInfo>();
		iobase = new IOBase("semaine.meta");
		consumer = iobase.getSession().createConsumer(iobase.getTopic(), MetaMessenger.COMPONENT_NAME + " IS NOT NULL");
		producer = iobase.getSession().createProducer(iobase.getTopic());
		consumer.setMessageListener(this);

		ignoreStalledComponents = new HashSet<String>();
		String isc = System.getProperty("semaine.systemmanager.ignorestalled");
		if (isc != null) {
			StringTokenizer st = new StringTokenizer(isc);
			while (st.hasMoreElements()) {
				ignoreStalledComponents.add(st.nextToken());
			}
		}
		componentsToHide = new HashSet<String>();
		componentsToHide.add("SystemManager");
		componentsToHide.add("MessageLogComponent");

		iobase.startConnection();
		// need to send our own start again because when super() sent it,
		// we weren't listening yet:
		meta.reportState(State.starting);
		
		
		setupAdvisoryListener();

		if (Boolean.getBoolean("semaine.systemmanager.gui")) {
			// We hide components directly here (they never get to the GUI)
			String cth = System.getProperty("semaine.systemmanager.hide.components");
			if (cth != null) {
				StringTokenizer st = new StringTokenizer(cth);
				while (st.hasMoreElements()) {
					componentsToHide.add(st.nextToken());
				}
			}
			// The GUI doesn't draw the hidden topics, but they are listed in the component's detail view:
			topicsToHide = new HashSet<String>();
			String tth = System.getProperty("semaine.systemmanager.hide.topics");
			if (tth != null) {
				StringTokenizer st = new StringTokenizer(tth);
				while (st.hasMoreElements()) {
					topicsToHide.add(st.nextToken());
				}
			}
			// To clean up the system monitor, users can indicate which topics not to use for sorting:
			Set<String> topicsToIgnoreWhenSorting = new HashSet<String>();
			topicsToIgnoreWhenSorting.addAll(topicsToHide);
			String ttiws = System.getProperty("semaine.systemmanager.gui.topics_to_ignore_when_sorting");
			if (ttiws != null) {
				StringTokenizer st = new StringTokenizer(ttiws);
				while (st.hasMoreElements()) {
					topicsToIgnoreWhenSorting.add(st.nextToken());
				}
			}
			ComponentInfo.setTopicsToIgnoreWhenSorting(topicsToIgnoreWhenSorting);

			systemMonitor = new SystemMonitor(null, topicsToHide);
			systemMonitor.start();
			
			MessageListener topicListener = new MessageListener() {
				public void onMessage(Message m) {
					try {
						String topicName = ((Topic)m.getJMSDestination()).getTopicName();
						TopicInfo ti = systemMonitor.getTopicInfo(topicName);
						if (ti != null) {
							SEMAINEMessage sm = new SEMAINEMessage(m);
							ti.addMessage(MessageLogComponent.message2logString(sm), sm.getSource());
						}
					} catch (JMSException e) {
						System.err.println("Cannot log message:");
						e.printStackTrace();
					}
				}
			};
			dataConsumer = iobase.getSession().createConsumer(iobase.getSession().createTopic("semaine.data.>"));
			dataConsumer.setMessageListener(topicListener);
			callbackConsumer = iobase.getSession().createConsumer(iobase.getSession().createTopic("semaine.callback.>"));
			callbackConsumer.setMessageListener(topicListener);
		}
	}
	
	private void setupAdvisoryListener() throws JMSException {
		IOBase iob = new IOBase("ActiveMQ.Advisory.SlowConsumer.Topic.>");
		MessageConsumer mc = iob.getSession().createConsumer(iob.getTopic());
		mc.setMessageListener(new MessageListener() {
			@Override
			public void onMessage(Message msg) {
			    if (msg instanceof ActiveMQMessage) {
		             ActiveMQMessage aMsg =  (ActiveMQMessage)msg;
		             ConsumerInfo info = (ConsumerInfo) aMsg.getDataStructure();
		             String topic = info.getDestination().toString();
		             String name = info.getConsumerId().getConnectionId();
		             int limit = info.getPrefetchSize();
		             int current = info.getCurrentPrefetchSize();
		             String logMsg = "Slow consumer on "+topic+": "+name+" has "+current+" out of "+limit+" messages prefetched";
		             JMSLogger.getLog("ActiveMQ Advisory").warn(logMsg);
		         }
			}
		});
		iob.startConnection();
	}
	
	
	private void reportSystemReady(boolean ready)
	throws JMSException
	{
		log.info("System is "+(ready ? "" : "not ")+"ready");
		Message m = iobase.getSession().createMessage();
		m.setStringProperty(SEMAINEMessage.SOURCE, getName());
		m.setBooleanProperty(MetaMessenger.SYSTEM_READY, ready);
		if (ready) {
			// When we get ready, we set the system time to 0
			// Alternatively, we could set it to System.currentTimeMillis(),
			// which would set all clocks to this process's system clock.
			m.setLongProperty(MetaMessenger.SYSTEM_READY_TIME, getTime());
		}
		producer.send(m);
		lastReportSystemReady = ready;
	}

	private synchronized void pingComponents()
	throws JMSException
	{
		Message m = iobase.getSession().createMessage();
		m.setStringProperty(SEMAINEMessage.SOURCE, getName());
		m.setStringProperty(MetaMessenger.PING, "");
		producer.send(m);
		
		for (ComponentInfo ci : componentInfos.values()) {
			if (ci.receiveTopics() == null && ci.sendTopics() == null) {
				log.info("Component "+ci.toString()+" has no topic info -- will ask");
				// we need to ask for the information
				m = iobase.getSession().createMessage();
				m.setStringProperty(SEMAINEMessage.SOURCE, getName());
				m.setStringProperty(MetaMessenger.REPORT_TOPICS, "");
				producer.send(m);
				
			}
		}
	}
	
	private synchronized void checkComponentsAlive()
	{
		assert lastReportSystemReady == true : "Checking for stalled components makes no sense when the system is not ready";
		long time = getTime();
		for (ComponentInfo ci : componentInfos.values()) {
			if (time - ci.lastSeenAlive() > MetaMessenger.TIMEOUT_PERIOD
					&& !ignoreStalledComponents.contains(ci.toString())) {
				ci.setState(Component.State.stalled, 
						"Component was last seen at time "+ci.lastSeenAlive());
			}
		}
	}
	
	private long getTime()
	{
		return System.currentTimeMillis() - systemZeroTime;
	}
	
	public void onMessage(Message m)
	{
		try {
			assert m.propertyExists(MetaMessenger.COMPONENT_NAME) : "message should contain header field '"+MetaMessenger.COMPONENT_NAME+"'";
			String componentName = m.getStringProperty(MetaMessenger.COMPONENT_NAME);
			if (componentsToHide != null && componentsToHide.contains(componentName)) {
				return;
			}
			
			ComponentInfo ci = componentInfos.get(componentName);
			if (ci == null) {
				synchronized(this) {
					ci = new ComponentInfo(componentName, null, null, false, false);
					componentInfos.put(componentName, ci);
					if (systemMonitor != null) {
						// Tell GUI there is something new
						systemMonitor.addComponentInfo(ci);
					}
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
				String details = null;
				if (m.propertyExists(MetaMessenger.COMPONENT_STATE_DETAILS)) {
					details = m.getStringProperty(MetaMessenger.COMPONENT_STATE_DETAILS);
				}
				ci.setState(componentState, details);
				if (componentState != State.ready) { // system not ready
					if (lastReportSystemReady == true) {
						logComponentStates();
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
						logComponentStates();
						reportSystemReady(allReady);
					}
				}
			}
			if (m.propertyExists(MetaMessenger.RECEIVE_TOPICS)) {
				String receiveTopicsString = m.getStringProperty(MetaMessenger.RECEIVE_TOPICS);
				String[] receiveTopics = null;
				if (receiveTopicsString != null && receiveTopicsString.length() > 0) {
					receiveTopics = receiveTopicsString.split(" ");
				}
				ci.setReceiveTopics(receiveTopics);
			}
			if (m.propertyExists(MetaMessenger.SEND_TOPICS)) {
				String sendTopicsString = m.getStringProperty(MetaMessenger.SEND_TOPICS);
				String[] sendTopics = null;
				if (sendTopicsString != null && sendTopicsString.length() > 0) {
						sendTopics = sendTopicsString.split(" ");
				}
				ci.setSendTopics(sendTopics);
			}
			if (m.propertyExists(MetaMessenger.IS_INPUT)) {
				ci.setIsInput(m.getBooleanProperty(MetaMessenger.IS_INPUT));
			}
			if (m.propertyExists(MetaMessenger.IS_OUTPUT)) {
				ci.setIsOutput(m.getBooleanProperty(MetaMessenger.IS_OUTPUT));
			}
			if (m.propertyExists(MetaMessenger.LAST_SEEN_ALIVE)) {
				ci.setLastSeenAlive(m.getLongProperty(MetaMessenger.LAST_SEEN_ALIVE));
			}
			if (m.propertyExists(MetaMessenger.AVERAGE_ACT_TIME)) {
				ci.setAverageActTime(m.getLongProperty(MetaMessenger.AVERAGE_ACT_TIME));
			}
			if (m.propertyExists(MetaMessenger.AVERAGE_REACT_TIME)) {
				ci.setAverageReactTime(m.getLongProperty(MetaMessenger.AVERAGE_REACT_TIME));
			}
			if (m.propertyExists(MetaMessenger.AVERAGE_TRANSMIT_TIME)) {
				ci.setAverageTransmitTime(m.getLongProperty(MetaMessenger.AVERAGE_TRANSMIT_TIME));
			}
			if (m.propertyExists(MetaMessenger.TOTAL_MESSAGES_RECEIVED)) {
				ci.setTotalMessagesReceived(m.getIntProperty(MetaMessenger.TOTAL_MESSAGES_RECEIVED));
			}
		} catch (JMSException e) {
			log.error(e);
		}
	}


	private void logComponentStates() {
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
	}

	@Override
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
					wait(PING_PERIOD);
				} catch (InterruptedException ie) {}
			}
			meta.IamAlive();
			try {
				pingComponents();
			} catch (JMSException e) {
				log.error("cannot ping components");
			}
			if (lastReportSystemReady) { // System manager thinks everything is fine
				checkComponentsAlive();
			}
			updateSystemStatus();
		}
		try {
			iobase.getConnection().close();
		} catch (JMSException e) {
			log.error("cannot close connection");
		}
	}
	
	private synchronized void updateSystemStatus()
	{
		if (systemMonitor == null) return;
		StringBuilder sb = new StringBuilder();
		sb.append("System status: ");
		if (lastReportSystemReady) {
			sb.append("ready\n");
			sb.append("time: ").append(getTime()).append("\n");
			sb.append("average...\n");
			long actTime = 0;
			int nAct = 0;
			long reactTime = 0;
			int nReact = 0;
			long transmitTime = 0;
			int nTransmit = 0;
			int numMsg = 0;
			int nNumMsg = 0;
			for (ComponentInfo ci : componentInfos.values()) {
				long val = ci.getAverageActTime();
				if (val != -1) {
					actTime += val;
					nAct++;
				}
				val = ci.getAverageReactTime();
				if (val != -1) {
					reactTime += val;
					nReact++;
				}
				val = ci.getAverageTransmitTime();
				if (val != -1) {
					transmitTime += val;
					nTransmit++;
				}
				int msg = ci.getTotalMessagesReceived();
				if (msg != -1) {
					numMsg += msg;
					nNumMsg++;
				}
			}
			if (nAct == 0) nAct = 1;
			if (nReact == 0) nReact = 1;
			if (nTransmit == 0) nTransmit = 1;
			if (nNumMsg == 0) nNumMsg = 1;
			sb.append("...time spent in act(): ").append(actTime/nAct).append(" ms\n");
			sb.append("...time spent in react(): ").append(reactTime/nReact).append(" ms\n");
			sb.append("...message travel time: ").append(transmitTime/nTransmit).append(" ms\n");
			sb.append("...number of messages received: ").append(numMsg/nNumMsg).append("\n");
		} else {
			sb.append("not ready:\n");
			for (ComponentInfo ci : componentInfos.values()) {
				if (ci.getState() != State.ready) {
					sb.append("    ").append(ci.toString()).append(" ").append(ci.getState()).append("\n");
				}
			}
		}
		systemMonitor.setSystemStatus(sb.toString());
	}
}
