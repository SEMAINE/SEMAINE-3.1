/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.meta;

import java.util.LinkedList;
import java.util.List;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.MessageProducer;

import eu.semaine.components.Component.State;
import eu.semaine.jms.IOBase;
import eu.semaine.jms.JMSLogger;
import eu.semaine.jms.message.SEMAINEMessage;
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
	// Properties sent by the Meta messenger:
	public static final String COMPONENT_NAME = "ComponentName";
	public static final String COMPONENT_STATE = "ComponentState";
	public static final String COMPONENT_STATE_DETAILS = "ComponentStateDetails";
	public static final String RECEIVE_TOPICS = "ReceiveTopics";
	public static final String SEND_TOPICS = "SendTopics";
	public static final String IS_INPUT = "IsInputComponent";
	public static final String IS_OUTPUT = "IsOutputComponent";
	public static final String LAST_SEEN_ALIVE = "LastSeenAlive";
	public static final String AVERAGE_ACT_TIME = "AverageActTime";
	public static final String AVERAGE_REACT_TIME = "AverageReactTime";
	public static final String AVERAGE_TRANSMIT_TIME = "AverageTransmitTime";
	public static final String TOTAL_MESSAGES_RECEIVED = "TotalMessagesReceived";
	
	// Properties sent by the System manager:
	public static final String SYSTEM_READY = "SystemReady";
	public static final String SYSTEM_READY_TIME = "SystemReadyTime";
	public static final String PING = "Ping";
	public static final String REPORT_TOPICS = "DoReportTopics";
	
	private String componentName;
	private MessageProducer producer;
	private MessageConsumer consumer;
	private boolean systemReady = false;
	private long timeDelta;
	private long lastSeenAlive = 0;
	private State lastSeenState;
	private String receiveTopics;
	private String sendTopics;
	private boolean isInput;
	private boolean isOutput;
	private Statistics statistics;
	
	public MetaMessenger(String componentName)
	throws JMSException
	{
		super("semaine.meta");
		this.componentName = componentName;
		consumer = session.createConsumer(topic, SEMAINEMessage.SOURCE+" = 'SystemManager'");
		producer = session.createProducer(topic);
		consumer.setMessageListener(this);
		startConnection();
		statistics = new Statistics(30);
	}
	
	public void IamAlive()
	{
		lastSeenAlive = getTime();
	}
	
	public void reportTopics(List<Receiver> receivers, List<Sender> senders,
			boolean isInput, boolean isOutput)
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
		receiveTopics = rec.toString();
		sendTopics = snd.toString();
		this.isInput = isInput;
		this.isOutput = isOutput;
		reportTopics();
	}
	
	private void reportTopics()
	throws JMSException
	{
		Message m = session.createMessage();
		m.setStringProperty(COMPONENT_NAME, componentName);
		m.setStringProperty(RECEIVE_TOPICS, receiveTopics);
		m.setStringProperty(SEND_TOPICS, sendTopics);
		m.setBooleanProperty(IS_INPUT, isInput);
		m.setBooleanProperty(IS_OUTPUT, isOutput);
		producer.send(m);
	}
	
	public void reportState(State state)
	throws JMSException
	{
		reportState(state, (Object[])null);
	}

	public void reportState(State state, Object... details)
	throws JMSException
	{
		Message m = session.createMessage();
		m.setStringProperty(COMPONENT_NAME, componentName);
		lastSeenState = state;
		m.setStringProperty(COMPONENT_STATE, state.toString());
		if (details != null) {
			m.setStringProperty(COMPONENT_STATE_DETAILS, JMSLogger.toLogMessageText(details));
		}
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
			if (m.propertyExists(PING)) {
				Message reply = session.createMessage();
				reply.setStringProperty(COMPONENT_NAME, componentName);
				reply.setStringProperty(COMPONENT_STATE, lastSeenState.toString());
				reply.setLongProperty(LAST_SEEN_ALIVE, lastSeenAlive);
				long val = statistics.avgActTime();
				if (val != -1) reply.setLongProperty(AVERAGE_ACT_TIME, val);
				val = statistics.avgReactTime();
				if (val != -1) reply.setLongProperty(AVERAGE_REACT_TIME, val);
				val = statistics.avgTransmitTime();
				if (val != -1) reply.setLongProperty(AVERAGE_TRANSMIT_TIME, val);
				reply.setIntProperty(TOTAL_MESSAGES_RECEIVED, statistics.getTotalMessagesReceived());
				producer.send(reply);
			}
			if (m.propertyExists(REPORT_TOPICS)) {
				reportTopics();
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
	
	public Statistics statistics()
	{
		return statistics;
	}
	
	public static class Statistics
	{
		private int memory;
		private LinkedList<Long> actTimes;
		private LinkedList<Long> reactTimes;
		private LinkedList<Long> transmitTimes;
		private int messagesReceived = 0;
		
		public Statistics(int memory)
		{
			this.memory = memory;
			actTimes = new LinkedList<Long>();
			reactTimes = new LinkedList<Long>();
			transmitTimes = new LinkedList<Long>();
		}
		
		public synchronized void actTime(long millis)
		{
			actTimes.addLast(millis);
			if (actTimes.size() > memory) actTimes.removeFirst();
		}
		
		public synchronized void reactTime(long millis)
		{
			reactTimes.addLast(millis);
			if (reactTimes.size() > memory) reactTimes.removeFirst();
		}
		
		public synchronized void transmitTime(long millis)
		{
			transmitTimes.addLast(millis);
			if (transmitTimes.size() > memory) transmitTimes.removeFirst();
		}
		
		public synchronized void countMessageReceived()
		{
			messagesReceived++;
		}
		
		public synchronized int getTotalMessagesReceived()
		{
			return messagesReceived;
		}
		
		public synchronized long avgActTime()
		{
			return average(actTimes);
		}
		public synchronized long avgReactTime()
		{
			return average(reactTimes);
		}
		public synchronized long avgTransmitTime()
		{
			return average(transmitTimes);
		}
		
		private long average(List<Long> values)
		{
			long total = 0;
			int size = values.size();
			if (size == 0) return -1;
			for (long l : values) {
				total += l;
			}
			return total / size;
		}
	}
}
