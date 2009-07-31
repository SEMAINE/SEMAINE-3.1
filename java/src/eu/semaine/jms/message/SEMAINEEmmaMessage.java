/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.message;

import java.util.List;

import javax.jms.Message;

import org.w3c.dom.Element;

import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.datatypes.xml.SemaineML;
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
		return XMLTool.getChildElementByLocalNameNS(doc.getDocumentElement(), EMMA.E_ONEOF, EMMA.namespaceURI);
	}
	
	public Element getSequence()
	{
		return XMLTool.getChildElementByLocalNameNS(doc.getDocumentElement(), EMMA.E_SEQUENCE, EMMA.namespaceURI);
	}
	
	public Element getTopLevelInterpretation()
	{
		return XMLTool.getChildElementByLocalNameNS(doc.getDocumentElement(), EMMA.E_INTERPRETATION, EMMA.namespaceURI);
	}


	public List<Element> getEmotionElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, EmotionML.E_EMOTION, EmotionML.namespaceURI);
	}

	public List<Element> getFeatureElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_FEATURE, SemaineML.namespaceURI);
	}

	public List<Element> getBehaviourElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_BEHAVIOUR, SemaineML.namespaceURI);
	}
	
	public List<Element> getTextElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_TEXT, SemaineML.namespaceURI);
	}
	
	public List<Element> getSpeakingElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_SPEAKING, SemaineML.namespaceURI);
	}

}
