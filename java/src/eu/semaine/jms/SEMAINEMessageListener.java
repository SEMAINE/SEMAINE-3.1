/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;


public interface SEMAINEMessageListener
{
	/**
	 * method called when a SEMAINE message is received by a receiver.
	 * @param message
	 */
	public void onMessage(SEMAINEMessage message);
}
