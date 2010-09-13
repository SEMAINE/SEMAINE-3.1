/*
 *  BML.h
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_DATATYPES_XML_BML_H
#define SEMAINE_DATATYPES_XML_BML_H

#include <semaine/config.h>

namespace semaine {
namespace datatypes {
namespace xml {

class BML
{
public:
	static const std::string version;
	static const std::string namespaceURI;
	
	// Elements
	static const std::string E_BML;
	static const std::string E_ROOT_TAGNAME;
	static const std::string E_SPEECH;
	static const std::string E_TEXT;
	static const std::string E_SYNC;
	static const std::string E_DESCRIPTION;
	static const std::string E_HEAD;
	static const std::string E_GAZE;
	static const std::string E_GESTURE;
	static const std::string E_LOCOMOTION;
	static const std::string E_FACE;
	static const std::string E_LIPS;

	// Attributes
	static const std::string A_ID;
	static const std::string A_START;
	static const std::string A_END;
	static const std::string A_TYPE;

	// Values
	// Values of <head type="...">:
	static const std::string V_NOD;
	static const std::string V_SHAKE;
	static const std::string V_TILTRIGHT;
	static const std::string V_TILTLEFT;
	static const std::string V_APPROACH;
	static const std::string V_RETRACT;

};

} // namespace xml
} // namespace datatypes
} // namespace semaine

#endif

