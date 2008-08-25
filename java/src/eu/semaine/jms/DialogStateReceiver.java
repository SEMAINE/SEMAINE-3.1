/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import java.util.SortedSet;

import javax.jms.JMSException;
import javax.jms.Message;

import eu.semaine.datatypes.DialogStateInfo;
import eu.semaine.datatypes.SEMAINEDialogStateMessage;
import eu.semaine.datatypes.SEMAINEMessage;
import eu.semaine.datatypes.SEMAINEStateMessage;
import eu.semaine.datatypes.StateInfo;
import eu.semaine.datatypes.DialogStateInfo.DialogAct;
import eu.semaine.exceptions.MessageFormatException;

public class DialogStateReceiver extends XMLReceiver
{
	/**
	 * Create a receiver that will listen for all messages in the given Topic.
	 * @param topic the name of the JMS Topic to listen to.
	 */
	public DialogStateReceiver(String topicName) throws JMSException
	{
		super(topicName);
	}
	
	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector. 
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	public DialogStateReceiver(String topicName, String messageSelector)
	throws JMSException
	{
		super(topicName, messageSelector);
	}

	@Override
	protected SEMAINEMessage createSEMAINEMessage(Message m)
	throws MessageFormatException
	{
		return new SEMAINEDialogStateMessage(m);
	}

	public SortedSet<DialogAct> getDialogHistory()
	{
		StateInfo s = ((SEMAINEStateMessage)message).getState();
		return ((DialogStateInfo)s).getDialogHistory();
	}
	
}
