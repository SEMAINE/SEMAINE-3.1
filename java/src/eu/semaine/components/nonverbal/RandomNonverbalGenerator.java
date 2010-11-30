/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.nonverbal;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.sender.EmmaSender;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class RandomNonverbalGenerator extends Component {
	private enum Modality { au, head, speaking, vocalization, pitch };
	private final String[] auValues = new String[] { "0", "1", "2", "4", "12", "25" };
	private final String[] headValues = new String[] { "NOD", "SHAKE", "TILT-LEFT", "TILT-RIGHT", "APPROACH", "RETRACT" };
	private final String[] speakingValues = new String[] { "start", "stop" };
	private final String[] vocalizationValues = new String[] { "(laughter)", "(sigh)" };
	private final String[] pitchValues = new String[] { "rise", "fall", "rise-fall", "fall-rise", "high", "mid", "low" };
	
	private EmmaSender emmaSender;
	private final long averageSendingPeriod;
	private List<Modality> activeModalities;
	
	private long nextTime;
	private Random random;
	
	public RandomNonverbalGenerator() throws JMSException {
		super("RandomNonverbal");
		emmaSender = new EmmaSender("semaine.data.state.user.emma.nonverbal.random", getName());
		senders.add(emmaSender);
		averageSendingPeriod = Long.getLong("semaine.random-nonverbal.period", 1000);
		activeModalities = new ArrayList<Modality>();
		if (Boolean.getBoolean("semaine.random-nonverbal.modality.au")) {
			activeModalities.add(Modality.au);
		}
		if (Boolean.getBoolean("semaine.random-nonverbal.modality.head")) {
			activeModalities.add(Modality.head);
		}
		if (Boolean.getBoolean("semaine.random-nonverbal.modality.speaking")) {
			activeModalities.add(Modality.speaking);
		}
		if (Boolean.getBoolean("semaine.random-nonverbal.modality.vocalization")) {
			activeModalities.add(Modality.vocalization);
		}
		if (Boolean.getBoolean("semaine.random-nonverbal.modality.pitch")) {
			activeModalities.add(Modality.pitch);
		}

		nextTime = 0;
		random = new Random();
	}

	@Override
	protected void act() throws JMSException {
		long currentTime = meta.getTime();
		if (currentTime >= nextTime) {
			Document doc = createRandomNonverbalXML();
			if (doc != null) {
				emmaSender.sendXML(doc, currentTime);
			}
			nextTime = currentTime + (long) (averageSendingPeriod * (1 + 0.5 * random.nextGaussian()));
		} else if (nextTime - currentTime > 2*averageSendingPeriod) {
			// system clock was reset since we last set nextTime -- reset it
			nextTime = currentTime;
			
		}
	}
	

	private Document createRandomNonverbalXML() {
		if (activeModalities.size() == 0) {
			return null;
		}
		
		int i = random.nextInt(activeModalities.size());
		Modality m = activeModalities.get(i);
		switch (m) {
		case au:
			return createRandomAU();
		case head:
			return createRandomHead();
		case speaking:
			return createRandomSpeaking();
		case vocalization:
			return createRandomVocalization();
		case pitch:
			return createRandomPitch();
		default:
			throw new IllegalStateException("Should never get here");
		}
	}

	private Document createRandomAU() {
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element interpretation = XMLTool.appendChildElement(doc.getDocumentElement(), EMMA.E_INTERPRETATION);
		interpretation.setAttribute(EMMA.A_OFFSET_TO_START, String.valueOf(meta.getTime()));
		interpretation.setAttribute(EMMA.A_CONFIDENCE, "1.0");

		Element bml = XMLTool.appendChildElement(interpretation, BML.E_BML, BML.namespaceURI);
		Element face = XMLTool.appendChildElement(bml, BML.E_FACE);
		
		String auValue = auValues[random.nextInt(auValues.length)];
		face.setAttribute("au", auValue);

		return doc;
	}
	
	private Document createRandomHead() {
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element interpretation = XMLTool.appendChildElement(doc.getDocumentElement(), EMMA.E_INTERPRETATION);
		long time = meta.getTime();
		int duration = random.nextInt(1000);
		interpretation.setAttribute(EMMA.A_OFFSET_TO_START, String.valueOf(time));
		interpretation.setAttribute(EMMA.A_DURATION, String.valueOf(duration));
		interpretation.setAttribute(EMMA.A_CONFIDENCE, "1.0");

		Element bml = XMLTool.appendChildElement(interpretation, BML.E_BML, BML.namespaceURI);
		Element head = XMLTool.appendChildElement(bml, BML.E_HEAD);
		
		String typeValue = headValues[random.nextInt(headValues.length)];
		head.setAttribute(BML.A_TYPE, typeValue);
		head.setAttribute(BML.A_START, String.valueOf(time)); // TODO: milliseconds or seconds?
		head.setAttribute(BML.A_END, String.valueOf(time+duration)); // TODO: milliseconds or seconds?

		return doc;
	}
	
	private Document createRandomSpeaking() {
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element interpretation = XMLTool.appendChildElement(doc.getDocumentElement(), EMMA.E_INTERPRETATION);
		interpretation.setAttribute(EMMA.A_OFFSET_TO_START, String.valueOf(meta.getTime()));
		interpretation.setAttribute(EMMA.A_CONFIDENCE, "1.0");

		Element speaking = XMLTool.appendChildElement(interpretation, SemaineML.E_SPEAKING, SemaineML.namespaceURI);
		
		String value = speakingValues[random.nextInt(speakingValues.length)];
		speaking.setAttribute(SemaineML.A_STATUS_CHANGE, value);

		return doc;
	}
	
	private Document createRandomVocalization() {
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element interpretation = XMLTool.appendChildElement(doc.getDocumentElement(), EMMA.E_INTERPRETATION);
		interpretation.setAttribute(EMMA.A_OFFSET_TO_START, String.valueOf(meta.getTime()));
		interpretation.setAttribute(EMMA.A_CONFIDENCE, "1.0");

		Element vocalization = XMLTool.appendChildElement(interpretation, SemaineML.E_VOCALIZATION, SemaineML.namespaceURI);
		
		String value = vocalizationValues[random.nextInt(vocalizationValues.length)];
		vocalization.setAttribute(SemaineML.A_NAME, value);

		return doc;
	}
	
	private Document createRandomPitch() {
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element interpretation = XMLTool.appendChildElement(doc.getDocumentElement(), EMMA.E_INTERPRETATION);
		interpretation.setAttribute(EMMA.A_OFFSET_TO_START, String.valueOf(meta.getTime()));
		interpretation.setAttribute(EMMA.A_CONFIDENCE, "1.0");

		Element pitch = XMLTool.appendChildElement(interpretation, SemaineML.E_PITCH, SemaineML.namespaceURI);
		
		String value = pitchValues[random.nextInt(pitchValues.length)];
		pitch.setAttribute(SemaineML.A_DIRECTION, value);

		return doc;
	}
	
}
