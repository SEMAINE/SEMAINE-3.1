/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import javax.jms.JMSException;
import javax.jms.Message;


import eu.semaine.datatypes.SEMAINEEmmaMessage;
import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.exceptions.MessageFormatException;

/**
 * @author marc
 *
 */
public class EmmaReceiver extends XMLReceiver {

	/**
	 * @param topicName
	 * @throws JMSException
	 */
	public EmmaReceiver(String topicName) throws JMSException {
		super(topicName);
	}

	/**
	 * @param topicName
	 * @param messageSelector
	 * @throws JMSException
	 */
	public EmmaReceiver(String topicName, String messageSelector)
			throws JMSException {
		super(topicName, messageSelector);
	}
	
	
	@Override
	protected SEMAINEMessage createSEMAINEMessage(Message message)
	throws MessageFormatException
	{
		return new SEMAINEEmmaMessage(message);
	}


}
