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

import eu.semaine.jms.Receiver;
import eu.semaine.jms.SEMAINEMessage;
import eu.semaine.jms.SEMAINEMessageAvailableListener;
import eu.semaine.jms.Sender;

/**
 * The abstract base class for all components.
 * Subclasses need to:
 * <ul>
 * <li>create receivers and/or senders in their constructor;
 * <li>implement {@link #processMessage(SEMAINEMessage)}
 * and/or {@link #proactiveActions()} to do something meaningful.</li>
 * </ul>
 * @author marc
 *
 */
public class Component extends Thread implements SEMAINEMessageAvailableListener
{
	/**
	 * Time to spend waiting for a message before checking if we can do something proactively.
	 * Time is in milliseconds.
	 */
	public static final long MAXWAITINGTIME = 100;
	
	protected List<Receiver> receivers;
	protected List<Sender> senders;
	
	protected BlockingQueue<Receiver> inputWaiting;
	
	private boolean exitRequested = false;
	
	protected Component(String componentName)
	{
		super(componentName);
		receivers = new LinkedList<Receiver>();
		senders = new LinkedList<Sender>();
		inputWaiting = new LinkedBlockingQueue<Receiver>();
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
	
	
	private void startIO() throws JMSException
	{
		for (Receiver r : receivers) {
			r.setMessageListener(this);
			r.startConnection();
		}
		for (Sender s : senders) {
			s.startConnection();
		}
	}

	public void run()
	{
		try {
			startIO();
		} catch (JMSException ex) {
			// TODO: some logging here?
			System.err.println("Cannot startup component:");
			ex.printStackTrace(System.err);
			exitRequested = true;
		}
		
		while (!exitRequested()) {
			try {
				// Check if we should do something proactively:
				proactiveActions();
			} catch (Exception e) {
				// TODO: log properly
				e.printStackTrace(System.err);
			}
			
			Receiver r = null;
			try {
				// block until input becomes available
				r = inputWaiting.poll(MAXWAITINGTIME, TimeUnit.MILLISECONDS);
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
				processMessage(message);
			} catch (Exception e) {
				// TODO: log properly
				e.printStackTrace(System.err);
			}
		}
	}
	
	private synchronized boolean exitRequested()
	{
		return exitRequested;
	}
	
	/**
	 * Ask this component to exit as soon as possible.
	 */
	public synchronized void requestExit()
	{
		exitRequested = true;
	}
	
	//////////////////////// Methods to override ////////////////////
	
	/**
	 * Reactions to messages.
	 * 
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 */
	protected void processMessage(SEMAINEMessage message)
	{
		
	}
	
	/**
	 * Proactive actions. This method is called every {@link #MAXWAITINGTIME}
	 * milliseconds if no messages arrive, and after every message processing.
	 * 
	 * This base implementation does nothing; subclasses should implement
	 * suitable behaviour here.
	 */
	protected void proactiveActions()
	{
		
	}
	
}