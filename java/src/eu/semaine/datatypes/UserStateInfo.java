/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

import java.util.Map;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

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
		super(doc, "UserStateInfo", APIVersion);
	}

	/**
	 * @param infoItems
	 * @throws JMSException
	 */
	public UserStateInfo(Map<String, String> infoItems)
	throws JMSException
	{
		super(infoItems, "UserStateInfo", APIVersion);
	}

	/**
	 * @see eu.semaine.datatypes.StateInfo#createDocumentFromInfo()
	 */
	@Override
	protected void createDocumentFromInfo()
	{
		doc = XMLTool.newDocument(SemaineML.USERSTATE, SemaineML.namespace, SemaineML.version);
		Element root = doc.getDocumentElement();
		String item = info.get("emotion-quadrant");
		if (item != null) {
			Element category = XMLTool.appendChildElement(root, EmotionML.CATEGORY, EmotionML.namespace);
			category.setAttribute(EmotionML.SET, "emotion-quadrant");
			category.setAttribute(EmotionML.NAME, item);
		}
		item = info.get("interest");
		if (item != null) {
			Element category = XMLTool.appendChildElement(root, EmotionML.CATEGORY, EmotionML.namespace);
			category.setAttribute(EmotionML.SET, "interest");
			category.setAttribute(EmotionML.NAME, item);
		}
		item = info.get("engagement");
		if (item != null) {
			Element dimensions = XMLTool.appendChildElement(root, EmotionML.DIMENSIONS, EmotionML.namespace);
			dimensions.setAttribute(EmotionML.SET, "engagement");
			Element engagement = XMLTool.appendChildElement(dimensions, "engagement");
			engagement.setAttribute(EmotionML.VALUE, item);
		}
		item = info.get("turn-event");
		if (item != null) {
			Element event = XMLTool.appendChildElement(root, SemaineML.EVENT, SemaineML.namespace);
			event.setAttribute(SemaineML.NAME, "turn-event");
			event.setAttribute(SemaineML.VALUE, item);
			String time = info.get("turn-event time");
			if (time != null) {
				event.setAttribute(SemaineML.TIME, time);
			}
		}
		item = info.get("behaviour");
		if (item != null) {
			Element behaviour = XMLTool.appendChildElement(root, SemaineML.BEHAVIOUR, SemaineML.namespace);
			behaviour.setAttribute(SemaineML.NAME, item);
			String intensity = info.get("behaviour intensity");
			if (intensity != null) {
				behaviour.setAttribute(SemaineML.INTENSITY, intensity);
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
		if (namespace.equals(EmotionML.namespace)) {
			if (tagname.equals(EmotionML.DIMENSIONS)) {
				String set = XMLTool.needAttribute(el, EmotionML.SET);
				if (set.equals("engagement")) {
					Element engagement = XMLTool.needChildElementByTagNameNS(el, "engagement", EmotionML.namespace);
					String value = XMLTool.needAttribute(engagement, EmotionML.VALUE);
					// And finally...
					info.put("engagement", value);
					return true;
				}
			}
		} else if (namespace.equals(SemaineML.namespace)) {
			if (tagname.equals(SemaineML.EVENT)) {
				String name = XMLTool.needAttribute(el, SemaineML.NAME);
				String value = XMLTool.needAttribute(el, SemaineML.VALUE);
				info.put(name, value);
				String time = XMLTool.getAttributeIfAvailable(el, SemaineML.TIME);
				if (time != null) {
					info.put(name + " time", time);
				}
				return true;
			} else if (tagname.equals(SemaineML.BEHAVIOUR)) {
				String name = XMLTool.needAttribute(el, SemaineML.NAME);
				info.put("behaviour", name);
				String intensity = XMLTool.getAttributeIfAvailable(el, "intensity");
				if (intensity != null) {
					info.put("behaviour intensity", intensity);
				}
			}
		}
		return false;
	}
}
