/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components;

import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import javax.jms.JMSException;

import eu.semaine.components.meta.MetaMessenger;
import eu.semaine.jms.JMSLogger;
import eu.semaine.jms.SEMAINEMessageAvailableListener;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.Sender;

/**
 * The abstract base class for all components.
 * Subclasses need to:
 * <ul>
 * <li>create receivers and/or senders in their constructor;
 * <li>implement {@link #act()} and/or {@link #react(SEMAINEMessage)}
 * to do something meaningful.</li>
 * </ul>
 * @author marc
 *
 */
public class Component extends Thread implements SEMAINEMessageAvailableListener
{
	/**
	 * States of components that can be reported to the system meta component.
	 * @author marc
	 *
	 */
	public static enum State {starting, ready, stopped, failure, stalled};
	
	protected boolean isInput;
	protected boolean isOutput;
	protected List<Receiver> receivers;
	protected List<Sender> senders;
	protected BlockingQueue<Receiver> inputWaiting;
	protected JMSLogger log;
	private boolean exitRequested = false;
	private boolean hasSystemJustBecomeReady = false;
	protected State state = State.stopped;
	protected MetaMessenger meta;
	protected int waitingTime = 100;
	
	protected Component(String componentName)
	throws JMSException
	{
		this(componentName, false, false);
	}
	
	protected Component(String componentName, boolean isInput, boolean isOutput)
	throws JMSException
	{
		super(componentName);
		this.isInput = isInput;
		this.isOutput = isOutput;
		receivers = new LinkedList<Receiver>();
		senders = new LinkedList<Sender>();
		inputWaiting = new LinkedBlockingQueue<Receiver>();
		log = JMSLogger.getLog(getName());
		meta = new MetaMessenger(getName());
		state = State.starting;
		meta.reportState(state);
	}
	
	
	/**
	 * Method called from the corresponding receiver thread, to notify us
	 * that a message is available.
	 */
	public void messageAvailableFrom(Receiver source)
	{
		try {
			inputWaiting.put(source);
		} catch (InterruptedException ie) {
			// should not occur
		}
	}
	
	
	private void startIO() throws Exception
	{
		long startTime = System.currentTimeMillis();
		meta.reportTopics(receivers, senders, isInput, isOutput);
		customStartIO();
		for (Receiver r : receivers) {
			r.setMessageListener(this);
			r.startConnection();
		}
		for (Sender s : senders) {
			s.startConnection();
		}
		long endTime = System.currentTimeMillis();
		long startupDuration = endTime - startTime;
		state = State.ready;
		meta.reportState(state, "Startup took "+startupDuration+" ms");
	}
	
	/**
	 * Any custom startup code can go here.
	 * @throws Exception if anything goes wrong in the internal processing of the component.
	 */
	protected void customStartIO() throws Exception{}

	public void run()
	{
		try {
			startIO();
		} catch (Throwable ex) {
			log.error("Cannot startup component:", ex);
			try {
				state = State.failure;
				meta.reportState(state, "Cannot startup component:", ex);
			} catch (JMSException me) {
				log.error("cannot report failure state", me);
			}
			requestExit();
		}
		
		while (!exitRequested()) {
			// Report that we are alive before we check system ready status,
			// (in case we were stalled due to a long act() or react() this
			// means we "report back" to the rest of the system)
			meta.IamAlive();
			hasSystemJustBecomeReady = false;
			// Check at every loop that the total system is ready
			synchronized (meta) {
				if (!meta.isSystemReady()) {
					log.info(meta.getTime(), "waiting for system to become ready");
				}
				while (!meta.isSystemReady()) {
					try {
						meta.wait();
					} catch (InterruptedException ie) {
					}
					if (meta.isSystemReady()) {
						log.info(meta.getTime(), "system ready - let's go");
						hasSystemJustBecomeReady = true;
					}
				}
			}
			try {
				// Check if we should do something proactively:
				long before = System.currentTimeMillis();
				act();
				long timeSpentInAct = System.currentTimeMillis() - before;
				meta.statistics().actTime(timeSpentInAct);
			} catch (Throwable e) {
				log.error(meta.getTime(), "error when trying to act", e);
				try {
					state = State.failure;
					meta.reportState(state, "error when trying to act", e);
				} catch (JMSException me) {
					log.error(meta.getTime(), "cannot report failure state", me);
				}
				requestExit();
				return;
			}
			
			Receiver r = null;
			try {
				// block until input becomes available
				r = inputWaiting.poll(waitingTime, TimeUnit.MILLISECONDS);
			} catch (InterruptedException ie) {
				// if we have no input, we'll keep on waiting
				continue;
			}
			if (r == null) continue; 
			assert receivers.contains(r) : "the receiver that alerted us is not one of our receivers -- it is a "+r;
			SEMAINEMessage message = r.getMessage();
			assert message != null : "Receiver "+r+" alerted me but has no message";
			meta.statistics().countMessageReceived();
			
			try {
				// time that the message travelled, in "user" time (ms since system startup -- this should be independent of clock asynchrony)
				long timeMessageTravelled = meta.getTime() - message.getUsertime();
				meta.statistics().transmitTime(timeMessageTravelled);
				// Now, do something meaningful with the message,
				// and possibly send output via the Senders.
				long before = System.currentTimeMillis();
				react(message);
				long timeSpentInReact = System.currentTimeMillis() - before;
				meta.statistics().reactTime(timeSpentInReact);
			} catch (Throwable e) {
				log.error(meta.getTime(), "error when trying to react", e);
				log.debug(meta.getTime(), "(message was: ", MessageLogComponent.message2logString(message), ")");
				try {
					state = State.failure;
					meta.reportState(state, "error when trying to react", e,
							" (message was: ", MessageLogComponent.message2logString(message), ")");
				} catch (JMSException me) {
					log.error(meta.getTime(), "cannot report failure state", me);
				}
				requestExit();
				return;
			}
		}
	}
	
	protected synchronized boolean exitRequested()
	{
		return exitRequested;
	}
	
	/**
	 * Ask this component to exit as soon as possible.
	 */
	public synchronized void requestExit()
	{
		exitRequested = true;
		notify();
	}
	
	/**
	 * Method that informs subclasses that the system has just become ready. This is true the first time act() is called after the system has become ready, and false otherwise.
	 * The recommended place to check this, if necessary, is therefore {@link Component#act()}.
	 * @return true if the system has just become ready, and false otherwise.
	 */
	protected boolean hasSystemJustBecomeReady() {
		return hasSystemJustBecomeReady;
	}
	
	//////////////////////// Methods to override ////////////////////
	
	/**
	 * Proactive actions. This method is called every {@link #waitingTime}
	 * milliseconds if no messages arrive, and after every message processing.
	 * 
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 * @throws JMSException if communication with the JMS server goes wrong.
	 * @throws Exception if anything goes wrong in the internal processing of the component.
	 */
	protected void act()
	throws Exception
	{
		
	}

	/**
	 * Reactions to messages.
	 * 
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 * @throws JMSException if communication with the JMS server goes wrong.
	 * @throws Exception if anything goes wrong in the internal processing of the component.
	 */
	protected void react(SEMAINEMessage message)
	throws Exception
	{
		
	}
	
	
}
