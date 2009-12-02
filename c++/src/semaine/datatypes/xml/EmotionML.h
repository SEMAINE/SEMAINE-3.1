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

class EmotionML
{
public:
	static const std::string version;
	static const std::string namespaceURI;
	
	// Elements
	static const std::string E_EMOTION;
	static const std::string E_ROOT_TAGNAME;
	static const std::string E_CATEGORY;
	static const std::string E_DIMENSIONS;
	static const std::string E_AROUSAL;
	static const std::string E_VALENCE;
	static const std::string E_POTENCY;
  static const std::string E_POWER;
  static const std::string E_INTENSITY;
  static const std::string E_ANTICIPATION;
	static const std::string E_OBJECT;

	// Attributes
	static const std::string A_SET;
	static const std::string A_NAME;
	static const std::string A_VALUE;
	static const std::string A_CONFIDENCE;
	static const std::string A_TYPE;
};

} // namespace xml
} // namespace datatypes
} // namespace semaine

#endif

