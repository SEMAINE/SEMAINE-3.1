/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms;

import javax.jms.JMSException;
import javax.jms.Message;

/**
 * An abstraction of the Receiver for feature vectors.
 * @author marc
 *
 */
public class FeatureReceiver extends Receiver
{
	protected String[] featureNames;
	
	/**
	 * Create a receiver that will listen for all messages in the given Topic.
	 * @param topic the name of the JMS Topic to listen to.
	 */
	public FeatureReceiver(String topicName) throws JMSException
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
	public FeatureReceiver(String topicName, String messageSelector)
	throws JMSException
	{
		super(topicName, messageSelector);
	}
	
	
	protected SEMAINEMessage createSEMAINEMessage(Message message)
	throws MessageFormatException
	{
		return new SEMAINEFeatureMessage(message);
	}

}
