/*
 *  EmotionML.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "EmotionML.h"

namespace semaine {
namespace datatypes {
namespace xml {

const std::string EmotionML::namespaceURI = "http://www.w3.org/2009/10/emotionml";
const std::string EmotionML::version = "1.0-WD20100729";
	
// Elements
const std::string EmotionML::E_EMOTIONML = "emotionml";
const std::string EmotionML::E_EMOTION = "emotion";
const std::string EmotionML::E_ROOT_TAGNAME = "emotionml";
const std::string EmotionML::E_CATEGORY = "category";
const std::string EmotionML::E_DIMENSION = "dimension";
const std::string EmotionML::E_APPRAISAL = "appraisal";
const std::string EmotionML::E_ACTION_TENDENCY = "action-tendency";
const std::string EmotionML::E_INTENSITY = "intensity";
const std::string EmotionML::E_INFO = "info";
const std::string EmotionML::E_REFERENCE = "reference";
const std::string EmotionML::E_TRACE = "trace";

// Attributes
// Attributes of <emotion>
const std::string EmotionML::A_CATEGORY_VOCABULARY = "category-set";
const std::string EmotionML::A_DIMENSION_VOCABULARY = "dimension-set";
const std::string EmotionML::A_APPRAISAL_VOCABULARY = "appraisal-set";
const std::string EmotionML::A_ACTION_TENDENCY_VOCABULARY = "action-tendency-set";
const std::string EmotionML::A_ID = "id";
const std::string EmotionML::A_START = "start";
const std::string EmotionML::A_END = "end";
const std::string EmotionML::A_VERSION = "version";
const std::string EmotionML::A_MODALITY = "modality";

// Attributes of <category>, <dimension>, <appraisal>, <action-tendency> and <intensity>

const std::string EmotionML::A_NAME = "name";
const std::string EmotionML::A_VALUE = "value";
const std::string EmotionML::A_CONFIDENCE = "confidence";

// Attributes of <reference>
const std::string EmotionML::A_URI = "uri";
const std::string EmotionML::A_ROLE = "role";
const std::string EmotionML::A_MEDIA_TYPE = "media-type";

// Attribute of <trace>:
const std::string EmotionML::A_FREQ = "freq";
const std::string EmotionML::A_SAMPLES = "samples";

// Values

// Values of A_ROLE:
const std::string EmotionML::V_EXPRESSED_BY = "expressedBy";
const std::string EmotionML::V_EXPERIENCED_BY = "experiencedBy";
const std::string EmotionML::V_TRIGGERED_BY = "triggeredBy";
const std::string EmotionML::V_TARGETED_AT = "targetedAt";


// Selected vocabularies

// FSRE dimensions as in http://www.w3.org/TR/2010/WD-emotionml-20100729/#fsre-dimensions
const std::string EmotionML::VOC_FSRE_DIMENSION_DEFINITION = "http://www.example.com/emotion/dimension/FSRE.xml"; // provisional
const std::string EmotionML::VOC_FSRE_DIMENSION_VALENCE = "valence";
const std::string EmotionML::VOC_FSRE_DIMENSION_POTENCY = "potency";
const std::string EmotionML::VOC_FSRE_DIMENSION_AROUSAL = "arousal";
const std::string EmotionML::VOC_FSRE_DIMENSION_UNPREDICTABILITY = "unpredictability";


// SEMAINE-specific dimension vocabulary containing only one dimension, "intensity":
const std::string EmotionML::VOC_SEMAINE_INTENSITY_DIMENSION_DEFINITON = "http://www.semaine-project.eu/emo/dimension/intensity.xml";
const std::string EmotionML::VOC_SEMAINE_INTENSITY_DIMENSION_INTENSITY = "intensity";

// SEMAINE-specific set of interest-related categories:
const std::string EmotionML::VOC_SEMAINE_INTEREST_CATEGORY_DEFINITION = "http://www.semaine-project.eu/emo/category/interest.xml";
const std::string EmotionML::VOC_SEMAINE_INTEREST_CATEGORY_BORED = "bored";
const std::string EmotionML::VOC_SEMAINE_INTEREST_CATEGORY_NEUTRAL = "neutral";
const std::string EmotionML::VOC_SEMAINE_INTEREST_CATEGORY_INTERESTED = "interested";

} // namespace xml
} // namespace datatypes
} // namespace semaine


