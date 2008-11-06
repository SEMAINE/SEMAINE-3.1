/*
 *  FML.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "FML.h"

namespace semaine {
namespace datatypes {
namespace xml {

const std::string FML::version = "0.1";
const std::string FML::namespaceURI = "http://www.mindmakers.org/fml";
	
	// Elements
const std::string FML::E_FML = "fml";
const std::string FML::E_ROOT_TAGNAME = "fml";
const std::string FML::E_PERFORMATIVE = "performative";
const std::string FML::E_THEME = "theme";
const std::string FML::E_RHEME = "rheme";
const std::string FML::E_WORLD = "world";
const std::string FML::E_EMOTION = "emotion";

	// Attributes
const std::string FML::A_ID = "id";
const std::string FML::A_START = "start";
const std::string FML::A_END = "end";
const std::string FML::A_TYPE = "type";

} // namespace xml
} // namespace datatypes
} // namespace semaine


