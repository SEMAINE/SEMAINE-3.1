/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.message;

import javax.jms.JMSException;
import javax.jms.Message;

import org.w3c.dom.Document;


import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.JMSLogger;

/**
 * @author marc
 *
 */
public abstract class SEMAINEStateMessage extends SEMAINEXMLMessage
{
	
	protected JMSLogger log;
	protected StateInfo state;
	
	/**
	 * @param message
	 * @throws MessageFormatException
	 */
	public SEMAINEStateMessage(Message message)
	throws MessageFormatException
	{
		super(message);
		try {
			state = createState(doc);
			log = JMSLogger.getLog(state.toString());
			String messageVersion = message.getStringProperty(state.toString()+"APIVersion");
			if (!state.getAPIVersion().equals(messageVersion)) {
				log.warn("API versions differ: I have '"+state.getAPIVersion()+
						"', message from "+message.getStringProperty(SEMAINEMessage.SOURCE)+
						" has '"+messageVersion+"'. Some information may be lost.");
			}
		} catch (JMSException e) {
			throw new MessageFormatException("Problem reading message", e);
		}
	}
	
	protected abstract StateInfo createState(Document doc)
	throws MessageFormatException;
	
	public StateInfo getState()
	{
		return state;
	}

}
