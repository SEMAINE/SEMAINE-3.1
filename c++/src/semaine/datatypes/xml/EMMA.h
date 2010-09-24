/*
 *  EMMA.h
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_DATATYPES_XML_EMMA_H
#define SEMAINE_DATATYPES_XML_EMMA_H

#include <semaine/config.h>

namespace semaine {
namespace datatypes {
namespace xml {

class EMMA
{
public:
	static const std::string version;
	static const std::string namespaceURI;
	
	// Elements
	static const std::string E_EMMA;
	static const std::string E_ROOT_TAGNAME;
	static const std::string E_INTERPRETATION;
	static const std::string E_ONEOF;
	static const std::string E_SEQUENCE;
	static const std::string E_GROUP;

	// Attributes
	static const std::string A_START;
	static const std::string A_OFFSET_TO_START;
	static const std::string A_END;
	static const std::string A_DURATION;
	static const std::string A_VERBAL;
	static const std::string A_MODE;
	static const std::string A_CONFIDENCE;
	static const std::string A_TOKENS;
};

} // namespace xml
} // namespace datatypes
} // namespace semaine

#endif

