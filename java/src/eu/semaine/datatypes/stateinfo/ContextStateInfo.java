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
public class ContextStateInfo extends StateInfo
{
	public static final String APIVersion = "0.2";


	/**
	 * @param doc
	 * @throws MessageFormatException
	 * @throws JMSException
	 */
	public ContextStateInfo(Document doc)
	throws MessageFormatException
	{
		super(doc, "ContextStateInfo", APIVersion, SemaineML.E_CONTEXT, SemaineML.namespaceURI, Type.ContextState);
	}

	/**
	 * @param infoItems
	 * @throws JMSException
	 */
	public ContextStateInfo(Map<String, String> infoItems)
	throws JMSException
	{
		super(infoItems, "ContextStateInfo", APIVersion, Type.ContextState);
	}

	/**
	 * @see eu.semaine.datatypes.stateinfo.StateInfo#createDocumentFromInfo()
	 */
	@Override
	protected void createDocumentFromInfo()
	{
		throw new UnsupportedOperationException("not yet implemented");
	}

}
