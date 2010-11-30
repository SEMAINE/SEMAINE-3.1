/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.nonverbal;

import java.util.ArrayList;
import java.util.List;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.sender.EmmaSender;
import eu.semaine.util.SEMAINEUtils;
import eu.semaine.util.XMLTool;

/**
 * A component combining evidence on the user's non-verbal behaviour 
 * from different sources into a consolidated statement.
 * Both input and output are EMMA messages transporting various kinds of non-verbal data.
 * @author marc
 *
 */
public class NonverbalFusion extends Component {

	private EmmaReceiver emmaReceiver;
	private EmmaSender emmaSender;
	
	/**
	 * @throws JMSException
	 */
	public NonverbalFusion() throws JMSException {
		super("NonverbalFusion");
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma.nonverbal.*");
		receivers.add(emmaReceiver);
		emmaSender = new EmmaSender("semaine.data.state.user.emma.nonverbal", getName());
		senders.add(emmaSender);
	}


	@Override
	protected void react(SEMAINEMessage m) throws JMSException {
		if (!(m instanceof SEMAINEEmmaMessage)) {
			return;
		}
		SEMAINEEmmaMessage em = (SEMAINEEmmaMessage) m;
		// We currently preview only two types of structure for non-verbal EMMA messages:
		// 1. a single top-level interpretation;
		// 2. a top-level group containing a collection of interpretations.
		// Sequences or one-of will be silently ignored.
		List<Element> interpretations;
		Element topInterpretation = em.getTopLevelInterpretation();
		if (topInterpretation != null) {
			interpretations = new ArrayList<Element>();
			interpretations.add(topInterpretation);
		} else {
			Element group = em.getGroup();
			if (group == null) {
				return;
			}
			interpretations = XMLTool.getChildrenByLocalNameNS(group, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
			if (interpretations.isEmpty()) {
				return;
			}
		}
		log.debug("EMMA message with non-verbal info received from "+em.getTopicName());
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element parent = doc.getDocumentElement();
		boolean hasNVInfo = false;
		if (interpretations.size() > 1) {
			parent = XMLTool.appendChildElement(parent, EMMA.E_GROUP);
			parent.setPrefix("emma");
		}
		for (Element interpretation : interpretations) {
			float confidence = SEMAINEUtils.parseFloat(interpretation.getAttribute(EMMA.A_CONFIDENCE), 1);
			List<Element> nonverbals = em.getNonverbalElements(interpretation);
			if (nonverbals.isEmpty()) {
				continue;
			}
			hasNVInfo = true;
			for (Element nonverbal : nonverbals) {
				float factor = verifyMultimodalConsistency(nonverbal);
				confidence *= factor;
			}
			Element newInterpretation = XMLTool.appendChildElement(parent, EMMA.E_INTERPRETATION);
			newInterpretation.setPrefix("emma");
			// Make sure confidence is in interval [0, 1]
			confidence = Math.min(1, Math.max(0, confidence));
			newInterpretation.setAttribute(EMMA.A_CONFIDENCE, String.valueOf(confidence));
			String[] attributesToCopy = new String[] {EMMA.A_OFFSET_TO_START, EMMA.A_DURATION};
			for (String attribute : attributesToCopy) {
				if (interpretation.hasAttribute(attribute)) {
					newInterpretation.setAttribute(attribute, interpretation.getAttribute(attribute));
				}
			}
			for (Element nonverbal : nonverbals) {
				Element newNonverbal = (Element) doc.adoptNode(nonverbal);
				newInterpretation.appendChild(newNonverbal);
			}
		}
		if (!hasNVInfo) {
			log.debug("Ignoring EMMA without non-verbal information from "+em.getTopicName());
			return;
		}
		emmaSender.sendXML(doc, em.getUsertime(), em.getEventType(), em.getContentID(), em.getContentCreationTime());
	}
	

	/**
	 * Verify whether the given element of non-verbal behaviour seems consistent with the other
	 * non-verbal behaviour that has been observed in the recent past.
	 * 
	 * For example, this could deduce that the co-occurrence of smile and laugh is highly consistent,
	 * whereas the co-occurrence of smile and sigh is inconsistent.
	 * @param nonverbal an Element representing a piece of non-verbal information, as returned by
	 * {@link SEMAINEEmmaMessage#getNonverbalElements(Element)}.
	 * @return a non-negative float as a factor modifying the confidence that this nonverbal element
	 * is consistent with the other recent observations.
	 */
	protected float verifyMultimodalConsistency(Element nonverbal) {
		// TODO: remember values, compare with recent history
		return 1;
	}
}
