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
	public static enum State {starting, ready, stopped, failure};
	
	protected List<Receiver> receivers;
	protected List<Sender> senders;
	protected BlockingQueue<Receiver> inputWaiting;
	protected JMSLogger log;
	private boolean exitRequested = false;
	protected State state = State.stopped;
	protected MetaMessenger meta;
	protected int waitingTime = 100;
	
	protected Component(String componentName)
	throws JMSException
	{
		super(componentName);
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
		meta.reportTopics(receivers, senders);
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
			// Check at every loop that the total system is ready
			synchronized (meta) {
				while (!meta.isSystemReady()) {
					log.info("waiting for system to become ready");
					try {
						meta.wait();
					} catch (InterruptedException ie) {
					}
					if (meta.isSystemReady()) {
						log.info("system ready - let's go");
					}
				}
			}
			try {
				// Check if we should do something proactively:
				act();
			} catch (Throwable e) {
				log.error("error when trying to act", e);
				try {
					state = State.failure;
					meta.reportState(state, "error when trying to act", e);
				} catch (JMSException me) {
					log.error("cannot report failure state", me);
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
			
			try {
				// Now, do something meaningful with the message,
				// and possibly send output via the Senders.
				react(message);
			} catch (Throwable e) {
				log.error("error when trying to react", e);
				log.debug("(message was: ", MessageLogComponent.message2logString(message), ")");
				try {
					state = State.failure;
					meta.reportState(state, "error when trying to react", e,
							" (message was: ", MessageLogComponent.message2logString(message), ")");
				} catch (JMSException me) {
					log.error("cannot report failure state", me);
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
