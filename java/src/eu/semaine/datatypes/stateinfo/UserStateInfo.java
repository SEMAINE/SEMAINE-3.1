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
public class UserStateInfo extends StateInfo
{
	public static final String APIVersion = "0.2";


	/**
	 * @param doc
	 * @throws MessageFormatException
	 * @throws JMSException
	 */
	public UserStateInfo(Document doc)
	throws MessageFormatException
	{
		super(doc, "UserStateInfo", APIVersion, SemaineML.E_USERSTATE, SemaineML.namespaceURI, Type.UserState);
	}

	/**
	 * @param infoItems
	 * @throws JMSException
	 */
	public UserStateInfo(Map<String, String> infoItems)
	throws JMSException
	{
		super(infoItems, "UserStateInfo", APIVersion, Type.UserState);
	}


}
