/*
 *  SemaineML.h
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_DATATYPES_XML_SEMAINEML_H
#define SEMAINE_DATATYPES_XML_SEMAINEML_H

#include <semaine/config.h>

namespace semaine {
namespace datatypes {
namespace xml {

class SemaineML
{
public:
	static const std::string version;
	static const std::string namespaceURI;
	
	// Elements
	static const std::string E_USERSTATE;
	static const std::string E_AGENTSTATE;
	static const std::string E_DIALOGSTATE;
	static const std::string E_FEATURE;
	static const std::string E_EVENT;
	static const std::string E_BEHAVIOUR;
	static const std::string E_TEXT;
	static const std::string E_EMOTIONALLY_CONCORDANT_WITH_USER;
	static const std::string E_DIALOG_ACT;
	static const std::string E_DIALOG_HISTORY;
	static const std::string E_SPEAKER;
	static const std::string E_LISTENER;
	static const std::string E_TOPIC;

	// Attributes
	static const std::string A_WHO;
	static const std::string A_NAME;
	static const std::string A_SPEAKER;
	static const std::string A_TOPIC;
	static const std::string A_VALUE;
	static const std::string A_TIME;
	static const std::string A_INTENSITY;
};

} // namespace xml
} // namespace datatypes
} // namespace semaine

#endif

