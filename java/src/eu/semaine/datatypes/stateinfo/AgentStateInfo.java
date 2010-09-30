/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.stateinfo;

import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;

/**
 * @author marc
 *
 */
public class AgentStateInfo extends StateInfo
{

	/**
	 * @param doc
	 * @throws MessageFormatException
	 * @throws JMSException
	 */
	public AgentStateInfo(Document doc)
	throws MessageFormatException
	{
		super(doc, "AgentStateInfo", SemaineML.E_AGENTSTATE, SemaineML.namespaceURI, Type.AgentState);
	}

	/**
	 * @param infoItems
	 * @throws JMSException
	 */
	public AgentStateInfo(Map<String, String> infoItems)
	throws IllegalArgumentException
	{
		super(infoItems, "AgentStateInfo", Type.AgentState);
	}


	
}
