/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.jms.message;

import java.util.ArrayList;
import java.util.List;

import javax.jms.Message;

import org.w3c.dom.Element;

import eu.semaine.datatypes.xml.BML;
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

	public Element getGroup()
	{
		return XMLTool.getChildElementByLocalNameNS(doc.getDocumentElement(), EMMA.E_GROUP, EMMA.namespaceURI);
	}

	/**
	 * Get the emotion elements that are children of the given interpretation element.
	 * @param interpretation an interpretation element
	 * @return a list of emotion elements, or an empty list if there are no emotion children below interpretation.
	 */
	public List<Element> getEmotionElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, EmotionML.E_EMOTION, EmotionML.namespaceURI);
	}

	/**
	 * Get a list of any non-verbal elements which are direct children of the given interpretation.
	 * @param interpretation
	 * @return a list of non-verbal elements of various kinds, or an empty list if there are no such elements.
	 * @see #getBMLElements(Element)
	 * @see #getGenderElements(Element)
	 * @see #getSpeakingElements(Element)
	 * @see #getVocalizationElements(Element)
	 * @see #getPitchElements(Element)
	 * @see #getFacePresentElements(Element)
	 */
	public List<Element> getNonverbalElements(Element interpretation) {
		List<Element> nonverbals = new ArrayList<Element>();
		nonverbals.addAll(getGenderElements(interpretation));
		nonverbals.addAll(getSpeakingElements(interpretation));
		nonverbals.addAll(getPitchElements(interpretation));
		nonverbals.addAll(getVocalizationElements(interpretation));
		nonverbals.addAll(getFacePresentElements(interpretation));
		nonverbals.addAll(getBMLElements(interpretation));
		return nonverbals;
	}

	
	/**
	 * Get the BML (behaviour markup language) elements that are children of the given interpretation element. 
	 * @param interpretation
	 * @return a list of BML elements, or an empty list if there are no such children below interpretation.
	 */
	public List<Element> getBMLElements(Element interpretation) {
		return XMLTool.getChildrenByLocalNameNS(interpretation, BML.E_BML, BML.namespaceURI);
	}

	
	public List<Element> getGenderElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, "gender", SemaineML.namespaceURI);
	}

	public List<Element> getSpeakingElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_SPEAKING, SemaineML.namespaceURI);
	}
	
	public List<Element> getPitchElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_PITCH, SemaineML.namespaceURI);
	}

	public List<Element> getVocalizationElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_VOCALIZATION, SemaineML.namespaceURI);
	}

	public List<Element> getFacePresentElements(Element interpretation)
	{
		return XMLTool.getChildrenByLocalNameNS(interpretation, SemaineML.E_FACEPRESENT, SemaineML.namespaceURI);
	}

	
	

}
