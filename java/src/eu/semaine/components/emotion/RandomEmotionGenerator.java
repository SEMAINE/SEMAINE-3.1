/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.emotion;

import java.util.Random;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.jms.sender.EmmaSender;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class RandomEmotionGenerator extends Component {
	private EmmaSender emmaSender;
	private final long averageSendingPeriod;
	private long nextTime;
	private Random random;
	
	public RandomEmotionGenerator() throws JMSException {
		super("RandomEmotion");
		emmaSender = new EmmaSender("semaine.data.state.user.emma.emotion.random", getName());
		senders.add(emmaSender);
		averageSendingPeriod = Long.getLong("semaine.random-emotion.period", 1000);
		nextTime = 0;
		random = new Random();
	}

	@Override
	protected void act() throws JMSException {
		long currentTime = meta.getTime();
		if (currentTime >= nextTime) {
			Document doc = createRandomEmotionXML();
			emmaSender.sendXML(doc, currentTime);
			nextTime = currentTime + (long) (averageSendingPeriod * (1 + 0.5 * random.nextGaussian()));
		} else if (nextTime - currentTime > 2*averageSendingPeriod) {
			// system clock was reset since we last set nextTime -- reset it
			nextTime = currentTime;
			
		}
	}
	
	
	private Document createRandomEmotionXML() {
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element interpretation = XMLTool.appendChildElement(doc.getDocumentElement(), EMMA.E_INTERPRETATION);
		Element emotion = XMLTool.appendChildElement(interpretation, EmotionML.E_EMOTION, EmotionML.namespaceURI);
		
		// Four dimensions plus intensity:
		emotion.setAttribute(EmotionML.A_DIMENSION_VOCABULARY, EmotionML.VOC_FSRE_DIMENSION_DEFINITION);
		Element intensity = XMLTool.appendChildElement(emotion, EmotionML.E_INTENSITY);
		intensity.setAttribute(EmotionML.A_VALUE, String.valueOf(random.nextFloat()));
		intensity.setAttribute(EmotionML.A_CONFIDENCE, String.valueOf(random.nextFloat()));
		String[] dimensionNames = new String[] {EmotionML.VOC_FSRE_DIMENSION_AROUSAL, 
				EmotionML.VOC_FSRE_DIMENSION_VALENCE, 
				EmotionML.VOC_FSRE_DIMENSION_UNPREDICTABILITY, 
				EmotionML.VOC_FSRE_DIMENSION_POTENCY
			};
		for (String dim : dimensionNames) {
			Element desc = XMLTool.appendChildElement(emotion, EmotionML.E_DIMENSION);
			desc.setAttribute(EmotionML.A_NAME, dim);
			desc.setAttribute(EmotionML.A_VALUE, String.valueOf(random.nextFloat()));
			desc.setAttribute(EmotionML.A_CONFIDENCE, String.valueOf(random.nextFloat()));
		}

		// Interest categories:
		String[] interestCategories = new String[] { EmotionML.VOC_SEMAINE_INTEREST_CATEGORY_BORED,
				EmotionML.VOC_SEMAINE_INTEREST_CATEGORY_NEUTRAL,
				EmotionML.VOC_SEMAINE_INTEREST_CATEGORY_INTERESTED
			};
		for (String cat : interestCategories) { // each interest category in its own emotion
			Element em = XMLTool.appendChildElement(interpretation, EmotionML.E_EMOTION, EmotionML.namespaceURI);
			em.setAttribute(EmotionML.A_CATEGORY_VOCABULARY, EmotionML.VOC_SEMAINE_INTEREST_CATEGORY_DEFINITION);
			Element category = XMLTool.appendChildElement(em, EmotionML.E_CATEGORY);
			category.setAttribute(EmotionML.A_NAME, cat);
			category.setAttribute(EmotionML.A_CONFIDENCE, String.valueOf(random.nextFloat()));
		}
		
		return doc;
	}
}
