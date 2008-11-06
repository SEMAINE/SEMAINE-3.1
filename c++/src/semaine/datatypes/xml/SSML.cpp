/*
 *  SSML.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SSML.h"

namespace semaine {
namespace datatypes {
namespace xml {

const std::string SSML::version = "1.0";
const std::string SSML::namespaceURI = "http://www.w3.org/2001/10/synthesis";
	
	// Elements
const std::string SSML::E_SSML = "speak";
const std::string SSML::E_ROOT_TAGNAME = "speak";
const std::string SSML::E_MARK = "mark";

	// Attributes
const std::string SSML::A_NAME = "name";

} // namespace xml
} // namespace datatypes
} // namespace semaine


