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
public class UserStateInfo extends StateInfo
{
	public static final String APIVersion = "0.1";


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

	/**
	 * @see eu.semaine.datatypes.stateinfo.StateInfo#createDocumentFromInfo()
	 */
	@Override
	protected void createDocumentFromInfo()
	{
		doc = XMLTool.newDocument(SemaineML.E_USERSTATE, SemaineML.namespaceURI, SemaineML.version);
		Element root = doc.getDocumentElement();
		String item = info.get("emotion-quadrant");
		if (item != null) {
			Element category = XMLTool.appendChildElement(root, EmotionML.E_CATEGORY, EmotionML.namespaceURI);
			category.setAttribute(EmotionML.A_SET, "emotion-quadrant");
			category.setAttribute(EmotionML.A_NAME, item);
		}
		item = info.get("interest");
		if (item != null) {
			Element category = XMLTool.appendChildElement(root, EmotionML.E_CATEGORY, EmotionML.namespaceURI);
			category.setAttribute(EmotionML.A_SET, "interest");
			category.setAttribute(EmotionML.A_NAME, item);
		}
		item = info.get("engagement");
		if (item != null) {
			Element dimensions = XMLTool.appendChildElement(root, EmotionML.E_DIMENSIONS, EmotionML.namespaceURI);
			dimensions.setAttribute(EmotionML.A_SET, "engagement");
			Element engagement = XMLTool.appendChildElement(dimensions, "engagement");
			engagement.setAttribute(EmotionML.A_VALUE, item);
		}
		item = info.get("turn-event");
		if (item != null) {
			Element event = XMLTool.appendChildElement(root, SemaineML.E_EVENT, SemaineML.namespaceURI);
			event.setAttribute(SemaineML.A_NAME, "turn-event");
			event.setAttribute(SemaineML.A_VALUE, item);
			String time = info.get("turn-event time");
			if (time != null) {
				event.setAttribute(SemaineML.A_TIME, time);
			}
		}
		item = info.get("behaviour");
		if (item != null) {
			Element behaviour = XMLTool.appendChildElement(root, SemaineML.E_BEHAVIOUR, SemaineML.namespaceURI);
			behaviour.setAttribute(SemaineML.A_NAME, item);
			String intensity = info.get("behaviour intensity");
			if (intensity != null) {
				behaviour.setAttribute(SemaineML.A_INTENSITY, intensity);
			}
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
		info.put("engagement", null);
		info.put("turn-event", null);
		info.put("turn-event time", null);
		info.put("behaviour", null);
		info.put("behaviour intensity", null);
	}

	@Override
	protected boolean analyseElement(Element el)
	throws MessageFormatException
	{
		// Give the super implementation the first try:
		if (super.analyseElement(el)) return true;
		
		String namespace = el.getNamespaceURI();
		String tagname = el.getTagName();
		if (namespace.equals(EmotionML.namespaceURI)) {
			if (tagname.equals(EmotionML.E_DIMENSIONS)) {
				String set = XMLTool.needAttribute(el, EmotionML.A_SET);
				if (set.equals("engagement")) {
					Element engagement = XMLTool.needChildElementByTagNameNS(el, "engagement", EmotionML.namespaceURI);
					String value = XMLTool.needAttribute(engagement, EmotionML.A_VALUE);
					// And finally...
					info.put("engagement", value);
					return true;
				}
			}
		} else if (namespace.equals(SemaineML.namespaceURI)) {
			if (tagname.equals(SemaineML.E_EVENT)) {
				String name = XMLTool.needAttribute(el, SemaineML.A_NAME);
				String value = XMLTool.needAttribute(el, SemaineML.A_VALUE);
				info.put(name, value);
				String time = XMLTool.getAttributeIfAvailable(el, SemaineML.A_TIME);
				if (time != null) {
					info.put(name + " time", time);
				}
				return true;
			} else if (tagname.equals(SemaineML.E_BEHAVIOUR)) {
				String name = XMLTool.needAttribute(el, SemaineML.A_NAME);
				info.put("behaviour", name);
				String intensity = XMLTool.getAttributeIfAvailable(el, "intensity");
				if (intensity != null) {
					info.put("behaviour intensity", intensity);
				}
				return true;
			}
		}
		return false;
	}
}
