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
	
	/**
	 * Get the top-level interpretation in this EMMA document
	 * @return the interpretation element, or null if there is no such element.
	 */
	public Element getTopLevelInterpretation()
	{
		return XMLTool.getChildElementByLocalNameNS(doc.getDocumentElement(), EMMA.E_INTERPRETATION, EMMA.namespaceURI);
	}


	/**
	 * Get the emotion elements that are children of the given interpretation element.
	 * @param interpretation an interpretation element
	 * @return a list of emotion elements, or an empty element if there are no emotion children below interpretation.
	 */
	public List<Element> getEmotionElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, EmotionML.E_EMOTION, EmotionML.namespaceURI);
	}
	
	public List<Element> getCategoryElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, EmotionML.E_CATEGORY, EmotionML.namespaceURI);
	}
	
	public List<Element> getGenderElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, "gender", SemaineML.namespaceURI);
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
	
	public List<Element> getPitchElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_PITCH, SemaineML.namespaceURI);
	}

}
