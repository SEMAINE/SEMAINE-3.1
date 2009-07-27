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
public class SystemStateInfo extends StateInfo
{

	/**
	 * @param doc
	 * @throws MessageFormatException
	 * @throws JMSException
	 */
	public SystemStateInfo(Document doc)
	throws MessageFormatException
	{
		super(doc, "SystemStateInfo", SemaineML.E_SETUP, SemaineML.namespaceURI, Type.SystemState);
	}

	/**
	 * @param infoItems
	 * @throws JMSException
	 */
	public SystemStateInfo(Map<String, String> infoItems)
	throws JMSException
	{
		super(infoItems, "SystemStateInfo", Type.SystemState);
	}


}
