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
	public static final String APIVersion = "0.1";


	/**
	 * @param doc
	 * @throws MessageFormatException
	 * @throws JMSException
	 */
	public AgentStateInfo(Document doc)
	throws MessageFormatException
	{
		super(doc, "AgentStateInfo", APIVersion, SemaineML.E_AGENTSTATE, SemaineML.namespace);
	}

	/**
	 * @param infoItems
	 * @throws JMSException
	 */
	public AgentStateInfo(Map<String, String> infoItems)
	throws JMSException
	{
		super(infoItems, "AgentStateInfo", APIVersion);
	}

	/**
	 * @see eu.semaine.datatypes.stateinfo.StateInfo#createDocumentFromInfo()
	 */
	@Override
	protected void createDocumentFromInfo()
	{
		doc = XMLTool.newDocument(SemaineML.E_AGENTSTATE, SemaineML.namespace, SemaineML.version);
		Element root = doc.getDocumentElement();
		String item = info.get("emotion-quadrant");
		if (item != null) {
			Element category = XMLTool.appendChildElement(root, EmotionML.E_CATEGORY, EmotionML.namespace);
			category.setAttribute(EmotionML.A_SET, "emotion-quadrant");
			category.setAttribute(EmotionML.A_NAME, item);
		}
		item = info.get("interest");
		if (item != null) {
			Element category = XMLTool.appendChildElement(root, EmotionML.E_CATEGORY, EmotionML.namespace);
			category.setAttribute(EmotionML.A_SET, "interest");
			category.setAttribute(EmotionML.A_NAME, item);
		}
		item = info.get("stance to user");
		if (item != null) {
			Element emotion = XMLTool.appendChildElement(root, EmotionML.E_EMOTION, EmotionML.namespace);
			emotion.setAttribute(EmotionML.A_TYPE, "stance");
			Element category = XMLTool.appendChildElement(emotion, EmotionML.E_CATEGORY, EmotionML.namespace);
			category.setAttribute(EmotionML.A_SET, "like-dislike");
			category.setAttribute(EmotionML.A_NAME, item);
			Element object = XMLTool.appendChildElement(emotion, EmotionML.E_OBJECT, EmotionML.namespace);
			object.setAttribute(EmotionML.A_NAME, "user");
		}
		item = info.get("stance to topic");
		if (item != null) {
			Element emotion = XMLTool.appendChildElement(root, EmotionML.E_EMOTION, EmotionML.namespace);
			emotion.setAttribute(EmotionML.A_TYPE, "stance");
			Element category = XMLTool.appendChildElement(emotion, EmotionML.E_CATEGORY, EmotionML.namespace);
			category.setAttribute(EmotionML.A_SET, "like-dislike");
			category.setAttribute(EmotionML.A_NAME, item);
			Element object = XMLTool.appendChildElement(emotion, EmotionML.E_OBJECT, EmotionML.namespace);
			object.setAttribute(EmotionML.A_NAME, "topic");
		}
		item = info.get("emotionally-concordant-with-user");
		if (item != null) {
			Element conc = XMLTool.appendChildElement(root, SemaineML.E_EMOTIONALLY_CONCORDANT_WITH_USER, SemaineML.namespace);
			conc.setAttribute(SemaineML.A_VALUE, item);
		}
	}

	/**
	 * Set up the possible values that we can know about.
	 * Things that are not previewed here will not be read from the document.
	 * When this changes, the APIVersion must change with it.
	 */
	@Override
	protected void setupInfoKeys()
	{
		info.put("emotion-quadrant", null);
		info.put("interest", null);
		info.put("stance to user", null);
		info.put("stance to topic", null);
		info.put("emotionally-concordant-with-user", null);
	}

	@Override
	protected boolean analyseElement(Element el)
	throws MessageFormatException
	{
		// Give the super implementation the first try:
		if (super.analyseElement(el)) return true;
		
		String namespace = el.getNamespaceURI();
		String tagname = el.getTagName();
		if (namespace.equals(EmotionML.namespace)) {
			if (tagname.equals(EmotionML.E_EMOTION)) {
				String type = el.getAttribute(EmotionML.A_TYPE);
				if (type.equals("stance")) {
					Element object = XMLTool.needChildElementByTagNameNS(el, EmotionML.E_OBJECT, EmotionML.namespace);
					String objectName = object.getAttribute(EmotionML.A_NAME);
					String infoKey = "stance to "+objectName;
					if (!info.containsKey(infoKey)) {
						throw new MessageFormatException("unknown object '"+objectName+"'");
					}
					Element category = XMLTool.needChildElementByTagNameNS(el, EmotionML.E_CATEGORY, EmotionML.namespace);
					String set = XMLTool.needAttribute(category, EmotionML.A_SET);
					if (!set.equals("like-dislike")) {
						throw new MessageFormatException("a stance needs a category with set 'like-dislike'");
					}
					String value = XMLTool.needAttribute(category, EmotionML.A_NAME);
					// And finally...
					info.put(infoKey, value);
					return true;
				}
			}
		} else if (namespace.equals(SemaineML.namespace)) {
			if (tagname.equals(SemaineML.E_EMOTIONALLY_CONCORDANT_WITH_USER)) {
				String value = XMLTool.needAttribute(el, SemaineML.A_VALUE);
				info.put("emotionally-concordant-with-user", value);
				return true;
			}
		}
		return false;
	}
}
