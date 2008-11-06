/*
 *  EMMA.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "EMMA.h"

namespace semaine {
namespace datatypes {
namespace xml {

const std::string EMMA::version = "1.0";
const std::string EMMA::namespaceURI = "http://www.w3.org/2003/04/emma";
	
	// Elements
const std::string EMMA::E_EMMA = "emma";
const std::string EMMA::E_ROOT_TAGNAME = "emma";
const std::string EMMA::E_INTERPRETATION = "interpretation";
const std::string EMMA::E_ONEOF = "one-of";

	// Attributes
const std::string EMMA::A_START = "start";
const std::string EMMA::A_END = "end";
const std::string EMMA::A_DURATION = "duration";
const std::string EMMA::A_VERBAL = "verbal";
const std::string EMMA::A_MODE = "mode";
const std::string EMMA::A_CONFIDENCE = "confidence";

} // namespace xml
} // namespace datatypes
} // namespace semaine


