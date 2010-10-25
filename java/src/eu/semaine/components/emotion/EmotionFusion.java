/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.emotion;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.PriorityBlockingQueue;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.sender.EmmaSender;
import eu.semaine.util.SEMAINEUtils;
import eu.semaine.util.XMLTool;

/**
 * A component combining emotion evidence from different sources into a consolidated emotion statement.
 * Both input and output are EMMA messages transporting EmotionML data.
 * @author marc, with advise on how to fuse the pieces from Hatice Gunes
 *
 */
public class EmotionFusion extends Component {

	private EmmaReceiver emmaReceiver;
	private EmmaSender emmaSender;

	private PriorityBlockingQueue<EmotionDataSet> emotionDataSetQueue;
	private Map<String, EmotionDataSet> emotionDataSets;
	
	/**
	 * The maximum time we let an EmotionDataSet collect data points before sending it onward.
	 */
	private final long maxDelay;
	/**
	 * The expected number of data points. If this is reached or exceeded, send EmotionDataSet immediately.
	 */
	private final int targetNumDataPoints;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public EmotionFusion() throws JMSException {
		super("EmotionFusion");
		emmaReceiver = new EmmaReceiver("semaine.data.state.user.emma.emotion.*.>");
		receivers.add(emmaReceiver);
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma.emotion.voice"));
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma.emotion.face"));
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma.emotion.head"));
		// TODO: Remove this once TUM have updated their code:
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma"));
		// TODO: Remove this once Imperial have updated their code:
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma.Head5DEmotion"));
		emmaSender = new EmmaSender("semaine.data.state.user.emma.emotion", getName());
		senders.add(emmaSender);
		emotionDataSetQueue = new PriorityBlockingQueue<EmotionDataSet>();
		emotionDataSets = new HashMap<String, EmotionDataSet>();
		maxDelay = Long.getLong("semaine.emotion-fusion.max-delay", 500); // ms time to fuse
		targetNumDataPoints = Integer.getInteger("semaine.emotion-fusion.num-raters", 999); // number of data points before we send on the data without looking at the time limit
	}


	@Override
	protected void react(SEMAINEMessage m) throws JMSException {
		if (!(m instanceof SEMAINEEmmaMessage)) {
			return;
		}
		SEMAINEEmmaMessage em = (SEMAINEEmmaMessage) m;
		Element interpretation = em.getTopLevelInterpretation();
		if (interpretation == null) {
			return;
		}
		List<Element> emotions = em.getEmotionElements(interpretation);
		if (emotions.isEmpty()) {
			log.debug("Ignoring EMMA without emotions from "+em.getTopicName());
			return;
		}
		log.debug("EMMA element with EmotionML received from "+em.getTopicName());
		for (Element emotion : emotions) {
			parseEmotion(emotion);
		}
	}
	
	
	@Override
	protected void act() throws JMSException {
		EmotionDataSet eds;
		List<EmotionDataSet> toBeSent = null;
		long currentTime = meta.getTime();
		while ((eds = emotionDataSetQueue.peek()) != null) {
			boolean ready = (eds.getNumDataPoints() >= targetNumDataPoints || currentTime - eds.getCreationTime() >= maxDelay);
			if (!ready) {
				break;
			}
			// eds is ready
			eds = emotionDataSetQueue.poll();
			emotionDataSets.remove(createKey(eds.getDescriptionType(), eds.getVocabularyURI(), eds.getName()));
			
			if (toBeSent == null) {
				toBeSent = new LinkedList<EmotionDataSet>();
			}
			toBeSent.add(eds);
		}
		if (toBeSent != null) {
			assert !toBeSent.isEmpty();
			Document emma = createEmotionXML(toBeSent);
			emmaSender.sendXML(emma, meta.getTime());
		}
	}
	

	
	private void parseEmotion(Element emotion) {
		assert emotion.getLocalName().equals(EmotionML.E_EMOTION);
		assert emotion.getNamespaceURI().equals(EmotionML.namespaceURI);
		
		parseDimensions(emotion, EmotionML.E_DIMENSION);
		parseDimensions(emotion, EmotionML.E_APPRAISAL);
		parseDimensions(emotion, EmotionML.E_ACTION_TENDENCY);
		parseIntensity(emotion);
		parseCategory(emotion);
	}
	
	private void parseDimensions(Element emotion, String descriptionType) {
		String vocabularyAttribute = EmotionML.getVocabularyAttributeForDescription(descriptionType);
		assert vocabularyAttribute != null;
		String vocabularyURI = emotion.getAttribute(vocabularyAttribute);
		List<Element> descriptors = XMLTool.getChildrenByLocalNameNS(emotion, descriptionType, EmotionML.namespaceURI);
		if (!isEmotionMLMeaningful(vocabularyAttribute, descriptionType, vocabularyURI, descriptors)) {
			return;
		}

		// OK, vocabulary and items are present.
		for (Element descriptor : descriptors) {
			String name = descriptor.getAttribute(EmotionML.A_NAME);
			if (name.equals("")) {
				continue;
			}
			// not yet implemented: could verify that name fits with vocabulary
			String valueString = descriptor.getAttribute(EmotionML.A_VALUE);
			float value = SEMAINEUtils.parseFloat(valueString, Float.NaN);
			if (Float.isNaN(value)) {
				continue;
			}
			value = trimToRange(value, EmotionML.V_VALUE_MIN, EmotionML.V_VALUE_MAX, EmotionML.A_VALUE);
			// Now we have a valid value.
			
			// Confidence?
			// default to 1 if not present
			String confidenceString = descriptor.getAttribute(EmotionML.A_CONFIDENCE);
			float confidence = SEMAINEUtils.parseFloat(confidenceString, 1);
			confidence = trimToRange(confidence, 0, 1, EmotionML.A_CONFIDENCE);
			// Now we have a confidence.
			enterDataPoint(descriptionType, vocabularyURI, name, value, confidence);
		}
	}
	
	private void parseIntensity(Element emotion) {
		Element intensity = XMLTool.getChildElementByLocalNameNS(emotion, EmotionML.E_INTENSITY, EmotionML.namespaceURI);
		if (intensity == null) {
			return;
		}
		String valueString = intensity.getAttribute(EmotionML.A_VALUE);
		float value = SEMAINEUtils.parseFloat(valueString, Float.NaN);
		if (Float.isNaN(value)) {
			return;
		}
		value = trimToRange(value, EmotionML.V_VALUE_MIN, EmotionML.V_VALUE_MAX, EmotionML.A_VALUE);
		// Now we have a valid value.
		
		// Confidence?
		// default to 1 if not present
		String confidenceString = intensity.getAttribute(EmotionML.A_CONFIDENCE);
		float confidence = SEMAINEUtils.parseFloat(confidenceString, 1);
		confidence = trimToRange(confidence, 0, 1, EmotionML.A_CONFIDENCE);
		// Now we have a confidence.
		enterDataPoint(EmotionML.E_INTENSITY, null, null, value, confidence);
	}
	
	private void parseCategory(Element emotion) {
		String descriptionType = EmotionML.E_CATEGORY;
		String vocabularyAttribute = EmotionML.getVocabularyAttributeForDescription(descriptionType);
		assert vocabularyAttribute != null;
		String vocabularyURI = emotion.getAttribute(vocabularyAttribute);
		List<Element> descriptors = XMLTool.getChildrenByLocalNameNS(emotion, descriptionType, EmotionML.namespaceURI);
		if (!isEmotionMLMeaningful(vocabularyAttribute, descriptionType, vocabularyURI, descriptors)) {
			return;
		}
		assert descriptors.size() > 0;
		Element category = descriptors.get(0);
		String name = category.getAttribute(EmotionML.A_NAME);
		if (name.equals("")) {
			return;
		}
		// Confidence?
		// default to 1 if not present
		String confidenceString = category.getAttribute(EmotionML.A_CONFIDENCE);
		float confidence = SEMAINEUtils.parseFloat(confidenceString, 1);
		confidence = trimToRange(confidence, 0, 1, EmotionML.A_CONFIDENCE);
		// Now we have a confidence.
		enterDataPoint(descriptionType, vocabularyURI, name, Float.NaN, confidence);
	}
	
	private boolean isEmotionMLMeaningful(String vocabularyAttribute, String descriptionType, String vocabularyURI, List<Element> descriptors) {
		if (vocabularyURI.equals("") && descriptors.isEmpty()) {
			// No info of this type
			return false;
		} else if (vocabularyURI.equals("") && !descriptors.isEmpty()) {
			log.warn("EmotionML is faulty: it contains items of type <"+descriptionType
					+"> but no '"+vocabularyAttribute+"' attribute -- ignoring!");
			return false;
		} else if (!vocabularyURI.equals("") && descriptors.isEmpty()) {
			log.warn("EmotionML looks strange: it contains an attribute '"+vocabularyAttribute
					+"' but no <"+descriptionType+"> elements -- ignoring!");
			return false;
		}
		return true;
	}
	
	private void enterDataPoint(String descriptionType, String vocabularyURI, String name, float value, float confidence) {
		String key = createKey(descriptionType, vocabularyURI, name);
		EmotionDataSet eds = emotionDataSets.get(key);
		if (eds == null) {
			eds = new EmotionDataSet(descriptionType, vocabularyURI, name, meta.getTime());
			emotionDataSets.put(key, eds);
			emotionDataSetQueue.add(eds);
		}
		eds.addDataPoint(value, confidence);
	}
	
	private String createKey(String descriptionType, String vocabularyURI, String name) {
		return descriptionType +" "+ name+" of "+vocabularyURI;
	}
	
	/**
	 * From the given emotion data sets, create an EMMA message containing EmotionML that represents the fused information.
	 * @param emotions
	 */
	private Document createEmotionXML(List<EmotionDataSet> emotions) {
		Document doc = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		doc.getDocumentElement().setPrefix("emma");
		Element interpretation = XMLTool.appendChildElement(doc.getDocumentElement(), EMMA.E_INTERPRETATION);
		// Put all EmotionDataSets using the same vocabularyURI into the same <emotion>:
		Map<String, Element> vocabularyURI2Emotion = new HashMap<String, Element>();
		for (EmotionDataSet eds : emotions) {
			String descriptionElementName = eds.getDescriptionType();
			String vocabularyAttributeName = EmotionML.getVocabularyAttributeForDescription(descriptionElementName);
			Element emotion = vocabularyURI2Emotion.get(eds.getVocabularyURI());
			if (emotion == null) {
				emotion = XMLTool.appendChildElement(interpretation, EmotionML.E_EMOTION, EmotionML.namespaceURI);
				emotion.setPrefix("emo");
				// reuse the <emotion> for every descriptionType except EmotionML.E_CATEGORY:
				if (!EmotionML.E_CATEGORY.equals(descriptionElementName)) {
					vocabularyURI2Emotion.put(eds.getVocabularyURI(), emotion);
				} // i.e., for categories create a new emotion every time
			}
			if (vocabularyAttributeName != null) {
				assert eds.getVocabularyURI() != null;
				emotion.setAttribute(vocabularyAttributeName, eds.getVocabularyURI());
			}
			Element description = XMLTool.appendChildElement(emotion, descriptionElementName);
			String name = eds.getName();
			if (name != null) {
				description.setAttribute(EmotionML.A_NAME, name);
			}
			float value = eds.getMean();
			if (!Float.isNaN(value)) {
				description.setAttribute(EmotionML.A_VALUE, String.valueOf(value));
			}
			float confidence = eds.getConfidence();
			if (!Float.isNaN(confidence)) {
				description.setAttribute(EmotionML.A_CONFIDENCE, String.valueOf(confidence));
			}
		}
		return doc;
	}
	
	private float trimToRange(float value, float min, float max, String valueName) {
		if (value < min) {
			log.warn("Value "+value+" of "+valueName+" is smaller than minimum "+min+" -- trimming to minimum value");
			return min;
		} else if (value > max) {
			log.warn("Value "+value+" of "+valueName+" is larger than maximum "+max+" -- trimming to maximum value");
			return max;
		}
		return value;
	}
	
	
	/**
	 * This class represents the set of data points describing the same emotion. All data points have been received on or after getCreationTime().
	 * Comparison between EmotionDataSets is based on their creation time, newer is bigger.
	 * @author marc
	 *
	 */
	private class EmotionDataSet implements Comparable<EmotionDataSet>
	{
		private String descriptionType;
		private String vocabularyURI;
		private String name;
		private long creationTime;
		private int numDataPoints;
		private float weightedMean;
		private float weightedSumSqDiff;
		private float sumOfWeights;
		
		public EmotionDataSet(String descriptionType, String vocabularyURI, String name, long creationTime) {
			this.descriptionType = descriptionType;
			this.vocabularyURI = vocabularyURI;
			this.name = name;
			this.creationTime = creationTime;
			numDataPoints = 0;
			weightedMean = 0;
			weightedSumSqDiff = 0;
			sumOfWeights = 0;
		}
		
		public String getDescriptionType() {
			return descriptionType;
		}
		
		public String getVocabularyURI() {
			return vocabularyURI;
		}
		
		public String getName() {
			return name;
		}
		
		public long getCreationTime() {
			return creationTime;
		}
		
		public int getNumDataPoints() {
			return numDataPoints;
		}
		
		/**
		 * Get the weighted mean of all data points added to this data set so far.
		 * @return
		 */
		public float getMean() {
			return weightedMean;
		}
		
		/**
		 * Get the variance of all data points added to this data set so far.
		 * @return
		 */
		public float getVariance() {
			return sumOfWeights == 0 ? 0 : weightedSumSqDiff / sumOfWeights;
		}
		
		public float getStandardDeviation() {
			return (float) Math.sqrt(getVariance());
		}
		
		/**
		 * Provide the confidence that the given value is correct.
		 * Where the data was computed as a weighted average of more than one value, this returns 1 minus the standard deviation across data points.
		 * Otherwise (e.g., for categories which don't have values), the confidence is the average of confidences.
		 * @return
		 */
		public float getConfidence() {
			float stddev = getStandardDeviation();
			if (Float.isNaN(stddev)) {
				// average of confidences of data points:
				return sumOfWeights / numDataPoints;
			}
			// new confidence based on standard deviation
			return 1 - trimToRange(getStandardDeviation(), 0, 1, "computed confidence");
		}
		
		/**
		 * Add one data point to this emotion data set. This is one statement about the value of the emotion,
		 * and it is weighted by the confidence with which this statement is made.
		 * @param value
		 * @param confidence
		 */
		public void addDataPoint(float value, float confidence) {
			// update mean and variance using incremental weighted computation of mean and variance,
			// see e.g. http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Weighted_incremental_algorithm
			if (confidence <= 0) {
				return;
			}
			float newSumOfWeights = sumOfWeights + confidence;
			float delta = value - weightedMean;
			float weightedDelta = delta * confidence / newSumOfWeights;
			weightedMean += weightedDelta;
			weightedSumSqDiff += sumOfWeights * delta * weightedDelta;
			sumOfWeights = newSumOfWeights;
			// Now, weightedMean is our mean, and (weightedSumSqDiff / sumOfWeights) is our variance.

			numDataPoints++;
		}

		/**
		 * One emotion data set is smaller (higher priority) than another if
		 * (a) it has reached targetNumDataPoints but the other one hasn't, or
		 * (b) if its creation time is earlier than the other's.
		 */
		@Override
		public int compareTo(EmotionDataSet o) {
			if (numDataPoints >= targetNumDataPoints && o.numDataPoints < targetNumDataPoints) {
				return -1; // higher priority
			}
			if (o.numDataPoints >= targetNumDataPoints && numDataPoints < targetNumDataPoints) {
				return 1; // lower priority
			}
			// compare based on creation time, earlier is higher priority:
			return creationTime < o.creationTime ? -1
				: creationTime > o.creationTime ? 1
				: 0;
		}
		
	}
}
