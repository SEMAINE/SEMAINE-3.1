/*
 *  SSML.h
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_DATATYPES_XML_SSML_H
#define SEMAINE_DATATYPES_XML_SSML_H

#include <semaine/config.h>

namespace semaine {
namespace datatypes {
namespace xml {

class SSML
{
public:
	static const std::string version;
	static const std::string namespaceURI;
	
	// Elements
	static const std::string E_SSML;
	static const std::string E_ROOT_TAGNAME;
	static const std::string E_MARK;

	// Attributes
	static const std::string A_NAME;
};

} // namespace xml
} // namespace datatypes
} // namespace semaine

#endif

