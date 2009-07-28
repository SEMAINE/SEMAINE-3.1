/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.message;

import javax.jms.JMSException;
import javax.jms.Message;

import org.w3c.dom.Document;


import eu.semaine.datatypes.stateinfo.AgentStateInfo;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.SystemStateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.jms.JMSLogger;

/**
 * @author marc
 *
 */
public class SEMAINEStateMessage extends SEMAINEXMLMessage
{
	
	protected JMSLogger log;
	protected StateInfo state;
	protected StateInfo.Type stateInfoType;
	
	/**
	 * @param message
	 * @throws MessageFormatException
	 */
	public SEMAINEStateMessage(Message message, StateInfo.Type stateInfoType)
	throws MessageFormatException
	{
		super(message);
		this.stateInfoType = stateInfoType;
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
	
	protected StateInfo createState(Document doc)
	throws MessageFormatException
	{
		switch (stateInfoType) {
		case AgentState: return new AgentStateInfo(doc);
		case DialogState: return new DialogStateInfo(doc);
		case UserState: return new UserStateInfo(doc);
		case ContextState: return new ContextStateInfo(doc);
		case SystemState: return new SystemStateInfo(doc);
		}
		throw new IllegalStateException("Unknown state info type: "+stateInfoType);
	}
	
	public StateInfo getState()
	{
		return state;
	}

}
