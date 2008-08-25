/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes;

import javax.jms.Message;

import org.w3c.dom.Element;

import eu.semaine.exceptions.MessageFormatException;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class SEMAINEUserStateMessage extends SEMAINEStateMessage
{
	public static final String APIVersion = "0.1";

	/**
	 * @param message
	 * @param whatState
	 * @param apiVersion
	 * @throws MessageFormatException
	 */
	public SEMAINEUserStateMessage(Message message)
	throws MessageFormatException
	{
		super(message, "UserState", APIVersion);
	}

	
	/**
	 * Set up the possible values that we can know about.
	 * Things that are not previewed here will not be read from the document.
	 * When this changes, the APIVersion must change with it.
	 */
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
