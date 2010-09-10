/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.datatypes.xml;

/**
 * This class provides string constants that match
 * the Second Public Working Draft of EmotionML,
 * http://www.w3.org/TR/2010/WD-emotionml-20100729/
 * @author marc
 *
 */
public class EmotionML 
{
	public static final String namespaceURI = "http://www.w3.org/2009/10/emotionml";
	public static final String version = "1.0-WD20100729";

	// Elements
	public static final String E_EMOTIONML = "emotionml";
	public static final String E_EMOTION = "emotion";
	public static final String E_ROOT_TAGNAME = E_EMOTIONML;
	public static final String E_CATEGORY = "category";
	public static final String E_DIMENSION = "dimension";
	public static final String E_APPRAISAL = "appraisal";
	public static final String E_ACTION_TENDENCY = "action-tendency";
	public static final String E_INTENSITY = "intensity";
	public static final String E_INFO = "info";
	public static final String E_REFERENCE = "reference";
	public static final String E_TRACE = "trace";
	
	// Attributes
	// Attributes of <emotion>
	public static final String A_CATEGORY_VOCABULARY = "category-set";
	public static final String A_DIMENSION_VOCABULARY = "dimension-set";
	public static final String A_APPRAISAL_VOCABULARY = "appraisal-set";
	public static final String A_ACTION_TENDENCY_VOCABULARY = "action-tendency-set";
	public static final String A_ID = "id";
	public static final String A_START = "start";
	public static final String A_END = "end";
	public static final String A_VERSION = "version";
	public static final String A_MODALITY = "modality";
	
	// Attributes of <category>, <dimension>, <appraisal>, <action-tendency> and <intensity>
	public static final String A_NAME = "name";
	public static final String A_VALUE = "value";
	public static final String A_CONFIDENCE = "confidence";

	// Attributes of <reference>
	public static final String A_URI = "uri";
	public static final String A_ROLE = "role";
	public static final String A_MEDIA_TYPE = "media-type";

	// Attribute of <trace>:
	public static final String A_FREQ = "freq";
	public static final String A_SAMPLES = "samples";
	
	// Values
	
	public static final float V_VALUE_MIN = 0f;
	public static final float V_VALUE_MAX = 1f;
	
	// Values of A_ROLE:
	public static final String V_EXPRESSED_BY = "expressedBy";
	public static final String V_EXPERIENCED_BY = "experiencedBy";
	public static final String V_TRIGGERED_BY = "triggeredBy";
	public static final String V_TARGETED_AT = "targetedAt";
	
	
	// Selected vocabularies
	
	// FSRE dimensions as in http://www.w3.org/TR/2010/WD-emotionml-20100729/#fsre-dimensions
	public static final String VOC_FSRE_DIMENSION_DEFINITION = "http://www.example.com/emotion/dimension/FSRE.xml"; // provisional
	public static final String VOC_FSRE_DIMENSION_VALENCE = "valence";
	public static final String VOC_FSRE_DIMENSION_POTENCY = "potency";
	public static final String VOC_FSRE_DIMENSION_AROUSAL = "arousal";
	public static final String VOC_FSRE_DIMENSION_UNPREDICTABILITY = "unpredictability";
	
	
	// SEMAINE-specific dimension vocabulary containing only one dimension, "intensity":
	public static final String VOC_SEMAINE_INTENSITY_DIMENSION_DEFINITON = "http://www.semaine-project.eu/emo/dimension/intensity.xml";
	public static final String VOC_SEMAINE_INTENSITY_DIMENSION_INTENSITY = "intensity";
	
	// SEMAINE-specific set of interest-related categories:
	public static final String VOC_SEMAINE_INTEREST_CATEGORY_DEFINITION = "http://www.semaine-project.eu/emo/category/interest.xml";
	public static final String VOC_SEMAINE_INTEREST_CATEGORY_BORED = "bored";
	public static final String VOC_SEMAINE_INTEREST_CATEGORY_NEUTRAL = "neutral";
	public static final String VOC_SEMAINE_INTEREST_CATEGORY_INTERESTED = "interested";
	
	// SEMAINE-specific interest dimension:
	public static final String VOC_SEMAINE_INTEREST_DIMENSION_DEFINITION = "http://www.semaine-project.eu/emo/dimension/interest.xml";
	public static final String VOC_SEMAINE_INTEREST_DIMENSION_INTEREST = "interest";
	
	// SEMAINE-specific communicative-function dimensions:
	public static final String VOC_SEMAINE_COMMFUNC_DIMENSION_DEFINITION = "http://www.semaine-project.eu/emo/dimension/communicative-functions.xml";
	public static final String VOC_SEMAINE_COMMFUNC_DIMENSION_AGREEMENT = "agreement";
	public static final String VOC_SEMAINE_COMMFUNC_DIMENSION_ACCEPTANCE = "acceptance";
	public static final String VOC_SEMAINE_COMMFUNC_DIMENSION_BELIEF = "belief";
	public static final String VOC_SEMAINE_COMMFUNC_DIMENSION_LIKING = "liking";
	public static final String VOC_SEMAINE_COMMFUNC_DIMENSION_UNDERSTANDING = "understanding";
	public static final String VOC_SEMAINE_COMMFUNC_DIMENSION_INTEREST = "interest";
	
	// SEMAINE-specific listener-meanings dimensions:
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_DEFINITION = "http://www.semaine-project.eu/emo/dimension/listener-meanings.xml";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_ANGER = "anger";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_SADNESS = "sadness";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_AMUSEMENT = "amusement";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_HAPPINESS = "happiness";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_CONTEMPT = "contempt";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_ANTICIPATION = "anticipation";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_SOLIDARITY = "solidarity";
	public static final String VOC_SEMAINE_LISTMEAN_DIMENSION_ANTAGONISM = "antagonism";


	// SEMAINE-specific four emotion quadrants
	public static final String VOC_SEMAINE_QUADRANTS_CATEGORY_DEFINITION = "http://www.semaine-project.eu/emo/category/four-quadrants.xml";
	public static final String VOC_SEMAINE_QUADRANTS_CATEGORY_POSITIVE_ACTIVE = "positive-active";
	public static final String VOC_SEMAINE_QUADRANTS_CATEGORY_NEGATIVE_ACTIVE = "negative-active";
	public static final String VOC_SEMAINE_QUADRANTS_CATEGORY_NEGATIVE_PASSIVE ="negative-passive";
	public static final String VOC_SEMAINE_QUADRANTS_CATEGORY_POSITIVE_PASSIVE = "positive-passive";
	
	
	
	/**
	 * Map from emotion description tag names to vocabulary attributes.
	 * @param description one of {@link #E_CATEGORY}, {@link #E_DIMENSION}, {@link #E_APPRAISAL}, {@link #E_ACTION_TENDENCY}
	 * @return one of {@link #A_CATEGORY_VOCABULARY}, {@link #A_DIMENSION_VOCABULARY}, {@link #A_APPRAISAL_VOCABULARY}, {@link #A_ACTION_TENDENCY_VOCABULARY}
	 * or null if description is none of the known values.
	 */
	public static String getVocabularyAttributeForDescription(String description) {
		if (E_CATEGORY.equals(description)) {
			return A_CATEGORY_VOCABULARY;
		}
		if (E_DIMENSION.equals(description)) {
			return A_DIMENSION_VOCABULARY;
		}
		if (E_APPRAISAL.equals(description)) {
			return A_APPRAISAL_VOCABULARY;
		}
		if (E_ACTION_TENDENCY.equals(description)) {
			return A_ACTION_TENDENCY_VOCABULARY;
		}
		return null;
	}
	
	/**
	 * Map from vocabulary attributes to the corresponding description tag names.
	 * @param attribute one of {@link #A_CATEGORY_VOCABULARY}, {@link #A_DIMENSION_VOCABULARY}, {@link #A_APPRAISAL_VOCABULARY}, {@link #A_ACTION_TENDENCY_VOCABULARY}
	 * @return one of {@link #E_CATEGORY}, {@link #E_DIMENSION}, {@link #E_APPRAISAL}, {@link #E_ACTION_TENDENCY} or null if attribute is none of the known values.
	 */
	public static String getDescriptionTagnameForVocabularyAttribute(String attribute) {
		if (A_CATEGORY_VOCABULARY.equals(attribute)) {
			return E_CATEGORY;
		}
		if (A_DIMENSION_VOCABULARY.equals(attribute)) {
			return E_DIMENSION;
		}
		if (A_APPRAISAL_VOCABULARY.equals(attribute)) {
			return E_APPRAISAL;
		}
		if (A_ACTION_TENDENCY_VOCABULARY.equals(attribute)) {
			return E_ACTION_TENDENCY;
		}
		return null;
	}
	
	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float semaineArousal2FSREArousal(float semaineArousal) {
		return from11to01(semaineArousal);
	}
	
	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float fsreArousal2SemaineArousal(float fsreArousal) {
		return from01to11(fsreArousal);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float semaineValence2FSREValence(float semaineValence) {
		return from11to01(semaineValence);
	}
	
	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float fsreValence2SemaineValence(float fsreValence) {
		return from01to11(fsreValence);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float semainePower2FSREPotency(float semainePower) {
		return from11to01(semainePower);
	}
	
	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float fsrePotency2SemainePower(float fsrePotency) {
		return from01to11(fsrePotency);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float semaineExpectation2FSREUnpredictability(float semaineExpectation) {
		return 1 - from11to01(semaineExpectation); // opposite polarity
	}
	
	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML. 
	 */
	public static float fsreUnpredictability2SemaineExpectation(float fsreUnpredictability) {
		return from01to11(fsreUnpredictability);
	}

	/**
	 * Convert a value from the value range [-1;1] to the value range [0;1].
	 * @param valueIn11
	 * @return the value mapped to the target interval.
	 */
	private static float from11to01(float valueIn11) {
		float valueIn01 = (valueIn11 + 1f) * 0.5f;
		// and force into target interval
		return Math.max(0f, Math.min(1f, valueIn01));
	}
	
	/**
	 * Convert a value from the value range [0;1] to the value range [-1;1].
	 * @param valueIn01
	 * @return the value mapped to the target interval.
	 */
	private static float from01to11(float valueIn01) {
		float valueIn11 = valueIn01 * 2f - 1f;
		// and force into target interval
		return Math.max(-1f, Math.min(1f, valueIn11));
	}
	
	
	


}
