/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

import java.util.List;

import javax.jms.Message;

import org.w3c.dom.Element;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class SEMAINEEmmaMessage extends SEMAINEXMLMessage {

	/**
	 * @param message
	 * @throws MessageFormatException
	 */
	public SEMAINEEmmaMessage(Message message) throws MessageFormatException
	{
		super(message);
	}

	public Element getOneOf()
	{
		return XMLTool.getChildElementByTagNameNS(doc.getDocumentElement(), EMMA.ONEOF, EMMA.namespace);
	}
	
	public Element getTopLevelInterpretation()
	{
		return XMLTool.getChildElementByTagNameNS(doc.getDocumentElement(), EMMA.INTERPRETATION, EMMA.namespace);
	}


	public List<Element> getEmotionElements(Element interpretation)
	{
		return XMLTool.getChildrenByTagNameNS(interpretation, EmotionML.EMOTION, EmotionML.namespace);
	}

	public List<Element> getFeatureElements(Element interpretation)
	{
		return XMLTool.getChildrenByTagNameNS(interpretation, SemaineML.FEATURE, SemaineML.namespace);
	}

	public List<Element> getBehaviourElements(Element interpretation)
	{
		return XMLTool.getChildrenByTagNameNS(interpretation, SemaineML.BEHAVIOUR, SemaineML.namespace);
	}

}
