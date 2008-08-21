/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components;

import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import javax.jms.JMSException;

import eu.semaine.jms.Receiver;
import eu.semaine.jms.SEMAINEMessage;
import eu.semaine.jms.SEMAINEMessageAvailableListener;
import eu.semaine.jms.Sender;

/**
 * @author marc
 *
 */
public abstract class Component extends Thread implements SEMAINEMessageAvailableListener
{
	protected List<Receiver> receivers;
	protected List<Sender> senders;
	
	protected BlockingQueue<Receiver> inputWaiting = new LinkedBlockingQueue<Receiver>();
	
	private boolean exitRequested = false;
	
	
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
			Receiver r = null;
			try {
				// block until input becomes available
				r = inputWaiting.take();
			} catch (InterruptedException ie) {
				// if we have no input, we'll keep on waiting
				continue;
			}
			assert r != null : "cannot use null receiver";
			assert receivers.contains(r) : "the receiver that alerted us is not one of our receivers -- it is a "+r;
			SEMAINEMessage message = r.getMessage();
			assert message != null : "Receiver "+r+" alerted me but has no message";
			
			// Now, do something meaningful with the message,
			// and possibly send output via the Senders.
			processMessage(message);
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
	
	protected abstract void processMessage(SEMAINEMessage message);
	
}
