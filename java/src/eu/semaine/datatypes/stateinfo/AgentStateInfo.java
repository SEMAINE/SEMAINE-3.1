/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.stateinfo;

import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class AgentStateInfo extends StateInfo
{
	public static final String APIVersion = "0.2";


	/**
	 * @param doc
	 * @throws MessageFormatException
	 * @throws JMSException
	 */
	public AgentStateInfo(Document doc)
	throws MessageFormatException
	{
		super(doc, "AgentStateInfo", APIVersion, SemaineML.E_AGENTSTATE, SemaineML.namespaceURI, Type.AgentState);
	}

	/**
	 * @param infoItems
	 * @throws JMSException
	 */
	public AgentStateInfo(Map<String, String> infoItems)
	throws JMSException
	{
		super(infoItems, "AgentStateInfo", APIVersion, Type.AgentState);
	}


	
}
