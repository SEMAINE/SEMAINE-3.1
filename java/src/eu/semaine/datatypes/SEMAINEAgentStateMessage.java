/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

import javax.jms.Message;

import org.w3c.dom.Document;

import eu.semaine.exceptions.MessageFormatException;

/**
 * @author marc
 *
 */
public class SEMAINEAgentStateMessage extends SEMAINEStateMessage
{

	/**
	 * @param message
	 * @param whatState
	 * @param apiVersion
	 * @throws MessageFormatException
	 */
	public SEMAINEAgentStateMessage(Message message)
	throws MessageFormatException
	{
		super(message);
	}

	protected StateInfo createState(Document document)
	throws MessageFormatException
	{
		return new AgentStateInfo(document);
	}

	
	@Override
	public AgentStateInfo getState()
	{
		return (AgentStateInfo)state;
	}

}
