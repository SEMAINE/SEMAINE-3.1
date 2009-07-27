/*
 * AgentStateInfo.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 *  Copyright 2009 DFKI GmbH. All rights reserved.
 */

#ifndef SEMAINE_DATATYPES_STATEINFO_SYSTEMSTATEINFO_H
#define SEMAINE_DATATYPES_STATEINFO_SYSTEMSTATEINFO_H

#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <semaine/datatypes/xml/SemaineML.h>

namespace semaine {

namespace datatypes {

namespace stateinfo {

class SystemStateInfo: public semaine::datatypes::stateinfo::StateInfo {
public:
	SystemStateInfo(XERCES_CPP_NAMESPACE::DOMDocument * doc)
	throw(semaine::cms::exceptions::MessageFormatException)
	: semaine::datatypes::stateinfo::StateInfo(doc, "SystemStateInfo",
			semaine::datatypes::xml::SemaineML::E_SETUP,
			semaine::datatypes::xml::SemaineML::namespaceURI,
			semaine::datatypes::stateinfo::StateInfo::SystemState)
	{}

	SystemStateInfo(std::map<std::string, std::string> const & infoItems)
	throw(semaine::cms::exceptions::SystemConfigurationException)
	: semaine::datatypes::stateinfo::StateInfo(infoItems, "SystemStateInfo",
			semaine::datatypes::stateinfo::StateInfo::SystemState)
	{}

	virtual ~SystemStateInfo() {}
};

}

}

}

#endif /* SEMAINE_DATATYPES_STATEINFO_SYSTEMSTATEINFO_H */
