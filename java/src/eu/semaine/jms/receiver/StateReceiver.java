/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.receiver;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import javax.jms.JMSException;
import javax.jms.Message;

import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;

public class StateReceiver extends XMLReceiver
{
	private StateInfo.Type stateInfoType;
	private Map<String, String> currentBestGuess = new HashMap<String, String>();

	/**
	 * Create a receiver that will listen for all messages in the given Topic.
	 * @param topic the name of the JMS Topic to listen to.
	 */
	public StateReceiver(String topicName, StateInfo.Type stateInfoType) throws JMSException
	{
		super(topicName);
		this.stateInfoType = stateInfoType;
	}
	
	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector. 
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	public StateReceiver(String topicName, String messageSelector, StateInfo.Type stateInfoType)
	throws JMSException
	{
		super(topicName, messageSelector);
		this.stateInfoType = stateInfoType;
	}
	
	public void onMessage(Message m)
	{
		// here we do not enforce a listener, because we provide an API for accessing the content
		if (m != null) {
			try {
				message = createSEMAINEMessage(m);
				updateInformationState(message);
				if (listener != null) {
					// Notify component that new message has arrived
					listener.messageAvailableFrom(this);
					// Block until message has been picked up
					synchronized (this) {
						while (notPickedUp()) {
							try {
								this.wait();
							} catch (InterruptedException ie) {}
						}
					}
				}
			} catch (MessageFormatException mfe) {
				log.warn("Problem creating SEMAINE message: ", mfe);
			}
		}
	}

	@Override
	protected SEMAINEMessage createSEMAINEMessage(Message m)
	throws MessageFormatException
	{
		return new SEMAINEStateMessage(m, stateInfoType);
	}

	
	
	/**
	 * Provide the current best guess for the given info item.
	 * @param infoItem
	 * @return
	 */
	public String getCurrentBestGuess(String infoItem)
	{
		return currentBestGuess.get(infoItem);
	}

	private void updateInformationState(SEMAINEMessage m)
	{
		assert m instanceof SEMAINEStateMessage;
		SEMAINEStateMessage as = (SEMAINEStateMessage) m;
		Map<String, String> info = as.getState().getInfo();
		// Two conditions for adding the entry to our currentBestGuess:
		// 1. we don't have this key yet (i.e., first message)
		// 2. the value is not null
		if (currentBestGuess.isEmpty()) {
			for (Entry<String, String> entry : info.entrySet()) {
				currentBestGuess.put(entry.getKey(), entry.getValue());
			}
		} else {
			for (Entry<String, String> entry : info.entrySet()) {
				if (entry.getValue() != null) {
					currentBestGuess.put(entry.getKey(), entry.getValue());
				}
			}
		}
	}


}
