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
public class SEMAINEUserStateMessage extends SEMAINEStateMessage
{

	/**
	 * @param message
	 * @param whatState
	 * @param apiVersion
	 * @throws MessageFormatException
	 */
	public SEMAINEUserStateMessage(Message message)
	throws MessageFormatException
	{
		super(message);
	}

	protected StateInfo createState(Document document)
	throws MessageFormatException
	{
		return new UserStateInfo(document);
	}
	
	@Override
	public UserStateInfo getState()
	{
		return (UserStateInfo)state;
	}

}
