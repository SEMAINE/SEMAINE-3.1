/*
 *  FML.h
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_DATATYPES_XML_FML_H
#define SEMAINE_DATATYPES_XML_FML_H

#include <semaine/config.h>

namespace semaine {
namespace datatypes {
namespace xml {

class FML
{
public:
	static const std::string version;
	static const std::string namespaceURI;
	
	// Elements
	static const std::string E_FML;
	static const std::string E_ROOT_TAGNAME;
	static const std::string E_PERFORMATIVE;
	static const std::string E_THEME;
	static const std::string E_RHEME;
	static const std::string E_WORLD;
	static const std::string E_EMOTION;

	// Attributes
	static const std::string A_ID;
	static const std::string A_START;
	static const std::string A_END;
	static const std::string A_TYPE;
};

} // namespace xml
} // namespace datatypes
} // namespace semaine

#endif

