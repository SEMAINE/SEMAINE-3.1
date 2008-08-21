/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;


public interface SEMAINEMessageAvailableListener
{
	/**
	 * This method is called when a SEMAINE message is received by a receiver,
	 * from the receiver's thread. The method is expected to cause the listener
	 * to call (in this or another thread) the Receiver's {@link Receiver#getMessage()}
	 * method. The receiver will block until getMessage() has been called.
	 * 
	 * The listener can decide whether to get the message immediately or
	 * to put the receiver in a queue until the listener is ready to process.
	 * For slow components, the second option avoids piling up messages;
	 * instead, they may be discarded by the JMS server if their time to live
	 * is exceeded.
	 * @param receiver the receiver that has a message available.
	 */
	public void messageAvailableFrom(Receiver receiver);
}
