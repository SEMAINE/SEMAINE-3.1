/*
 * AgentStateInfo.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 *  Copyright 2009 DFKI GmbH. All rights reserved.
 */

#ifndef SEMAINE_DATATYPES_STATEINFO_CONTEXTSTATEINFO_H
#define SEMAINE_DATATYPES_STATEINFO_CONTEXTSTATEINFO_H

#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <semaine/datatypes/xml/SemaineML.h>

namespace semaine {

namespace datatypes {

namespace stateinfo {

class ContextStateInfo: public semaine::datatypes::stateinfo::StateInfo {
public:
	ContextStateInfo(XERCES_CPP_NAMESPACE::DOMDocument * doc)
	throw(semaine::cms::exceptions::MessageFormatException)
	: semaine::datatypes::stateinfo::StateInfo(doc, "ContextStateInfo",
			semaine::datatypes::xml::SemaineML::E_CONTEXT,
			semaine::datatypes::xml::SemaineML::namespaceURI,
			semaine::datatypes::stateinfo::StateInfo::ContextState)
	{}

	ContextStateInfo(std::map<std::string, std::string> const & infoItems)
	throw(semaine::cms::exceptions::SystemConfigurationException)
	: semaine::datatypes::stateinfo::StateInfo(infoItems, "ContextStateInfo",
			semaine::datatypes::stateinfo::StateInfo::ContextState)
	{}

	virtual ~ContextStateInfo() {}
};

}

}

}

#endif /* SEMAINE_DATATYPES_STATEINFO_CONTEXTSTATEINFO_H */
