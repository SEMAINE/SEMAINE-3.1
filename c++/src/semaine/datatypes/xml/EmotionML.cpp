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

const std::string EmotionML::version = "0.1";
const std::string EmotionML::namespaceURI = "http://www.w3.org/2005/Incubator/emotion";
	
	// Elements
const std::string EmotionML::E_EMOTION = "emotion";
const std::string EmotionML::E_ROOT_TAGNAME = "emotion";
const std::string EmotionML::E_CATEGORY = "category";
const std::string EmotionML::E_DIMENSIONS = "dimensions";
const std::string EmotionML::E_AROUSAL = "arousal";
const std::string EmotionML::E_VALENCE = "valence";
const std::string EmotionML::E_POTENCY = "potency";
const std::string EmotionML::E_POWER = "power";
const std::string EmotionML::E_INTENSITY = "intensity";
const std::string EmotionML::E_ANTICIPATION = "anticipation";
const std::string EmotionML::E_OBJECT = "object";

	// Attributes
const std::string EmotionML::A_SET = "set";
const std::string EmotionML::A_NAME = "name";
const std::string EmotionML::A_VALUE = "value";
const std::string EmotionML::A_CONFIDENCE = "confidence";
const std::string EmotionML::A_TYPE = "type";

} // namespace xml
} // namespace datatypes
} // namespace semaine


