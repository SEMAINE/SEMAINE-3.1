/*
 *  EmotionML.h
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_DATATYPES_XML_EMOTIONML_H
#define SEMAINE_DATATYPES_XML_EMOTIONML_H

#include <semaine/config.h>

namespace semaine {
namespace datatypes {
namespace xml {
/**
 * This class provides string constants that match
 * the Second Public Working Draft of EmotionML,
 * http://www.w3.org/TR/2010/WD-emotionml-20100729/
 * @author marc
 *
 */
class EmotionML
{
public:
	static const std::string namespaceURI;
	static const std::string version;
	
	// Elements
	static const std::string E_EMOTIONML;
	static const std::string E_EMOTION;
	static const std::string E_ROOT_TAGNAME;
	static const std::string E_CATEGORY;
	static const std::string E_DIMENSION;
	static const std::string E_APPRAISAL;
	static const std::string E_ACTION_TENDENCY;
	static const std::string E_INTENSITY;
	static const std::string E_INFO;
	static const std::string E_REFERENCE;
	static const std::string E_TRACE;

	// Attributes
	// Attributes of <emotion>
	static const std::string A_CATEGORY_VOCABULARY;
	static const std::string A_DIMENSION_VOCABULARY;
	static const std::string A_APPRAISAL_VOCABULARY;
	static const std::string A_ACTION_TENDENCY_VOCABULARY;
	static const std::string A_ID;
	static const std::string A_START;
	static const std::string A_END;
	static const std::string A_VERSION;
	static const std::string A_MODALITY;

	// Attributes of <category>, <dimension>, <appraisal>, <action-tendency> and <intensity>
	static const std::string A_NAME;
	static const std::string A_VALUE;
	static const std::string A_CONFIDENCE;

	// Attributes of <reference>
	static const std::string A_URI;
	static const std::string A_ROLE;
	static const std::string A_MEDIA_TYPE;

	// Attributes of <trace>
	static const std::string A_FREQ;
	static const std::string A_SAMPLES;

	// Values

	// Values of A_ROLE:
	static const std::string V_EXPRESSED_BY;
	static const std::string V_EXPERIENCED_BY;
	static const std::string V_TRIGGERED_BY;
	static const std::string V_TARGETED_AT;


	// Selected vocabularies

	// FSRE dimensions as in http://www.w3.org/TR/2010/WD-emotionml-20100729/#fsre-dimensions
	static const std::string VOC_FSRE_DIMENSION_DEFINITION;
	static const std::string VOC_FSRE_DIMENSION_VALENCE;
	static const std::string VOC_FSRE_DIMENSION_POTENCY;
	static const std::string VOC_FSRE_DIMENSION_AROUSAL;
	static const std::string VOC_FSRE_DIMENSION_UNPREDICTABILITY;


	// SEMAINE-specific dimension vocabulary containing only one dimension, "intensity":
	static const std::string VOC_SEMAINE_INTENSITY_DIMENSION_DEFINITON;
	static const std::string VOC_SEMAINE_INTENSITY_DIMENSION_INTENSITY;

	// SEMAINE-specific set of interest-related categories:
	static const std::string VOC_SEMAINE_INTEREST_CATEGORY_DEFINITION;
	static const std::string VOC_SEMAINE_INTEREST_CATEGORY_BORED;
	static const std::string VOC_SEMAINE_INTEREST_CATEGORY_NEUTRAL;
	static const std::string VOC_SEMAINE_INTEREST_CATEGORY_INTERESTED;


	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float semaineArousal2FSREArousal(float semaineArousal) {
		return from11to01(semaineArousal);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float fsreArousal2SemaineArousal(float fsreArousal) {
		return from01to11(fsreArousal);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float semaineValence2FSREValence(float semaineValence) {
		return from11to01(semaineValence);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float fsreValence2SemaineValence(float fsreValence) {
		return from01to11(fsreValence);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float semainePower2FSREPotency(float semainePower) {
		return from11to01(semainePower);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float fsrePotency2SemainePower(float fsrePotency) {
		return from01to11(fsrePotency);
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float semaineExpectation2FSREUnpredictability(float semaineExpectation) {
		return 1 - from11to01(semaineExpectation); // opposite polarity
	}

	/**
	 * Convenience function to convert between the values as used in SEMAINE
	 * and the FSRE scales used in EmotionML.
	 */
	inline static float fsreUnpredictability2SemaineExpectation(float fsreUnpredictability) {
		return from01to11(fsreUnpredictability);
	}

private:
	/**
	 * Convert a value from the value range [-1;1] to the value range [0;1].
	 * @param valueIn11
	 * @return the value mapped to the target interval.
	 */
	inline static float from11to01(float valueIn11) {
		float valueIn01 = (valueIn11 + 1.f) * 0.5f;
		// and force into target interval
		return _max(0.f, _min(1.f, valueIn01));
	}

	/**
	 * Convert a value from the value range [0;1] to the value range [-1;1].
	 * @param valueIn01
	 * @return the value mapped to the target interval.
	 */
	inline static float from01to11(float valueIn01) {
		float valueIn11 = valueIn01 * 2.f - 1.f;
		// and force into target interval
		return _max(-1.f, _min(1.f, valueIn11));
	}

	inline static float _max(float a, float b) {
		return (a > b) ? a : b;
	}

	inline static float _min(float a, float b) {
		return (a > b) ? b : a;
	}
};

} // namespace xml
} // namespace datatypes
} // namespace semaine

#endif

