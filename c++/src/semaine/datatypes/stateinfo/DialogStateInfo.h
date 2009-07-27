/*
 * AgentStateInfo.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 *  Copyright 2009 DFKI GmbH. All rights reserved.
 */

#ifndef SEMAINE_DATATYPES_STATEINFO_DIALOGSTATEINFO_H
#define SEMAINE_DATATYPES_STATEINFO_DIALOGSTATEINFO_H

#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <semaine/datatypes/xml/SemaineML.h>

namespace semaine {

namespace datatypes {

namespace stateinfo {

class DialogStateInfo: public semaine::datatypes::stateinfo::StateInfo {
public:
	DialogStateInfo(XERCES_CPP_NAMESPACE::DOMDocument * doc)
	throw(semaine::cms::exceptions::MessageFormatException)
	: semaine::datatypes::stateinfo::StateInfo(doc, "DialogStateInfo",
			semaine::datatypes::xml::SemaineML::E_DIALOGSTATE,
			semaine::datatypes::xml::SemaineML::namespaceURI,
			semaine::datatypes::stateinfo::StateInfo::DialogState)
	{}

	DialogStateInfo(std::map<std::string, std::string> const & infoItems)
	throw(semaine::cms::exceptions::SystemConfigurationException)
	: semaine::datatypes::stateinfo::StateInfo(infoItems, "DialogStateInfo",
			semaine::datatypes::stateinfo::StateInfo::DialogState)
	{}

	virtual ~DialogStateInfo() {}
};

}

}

}

#endif /* SEMAINE_DATATYPES_STATEINFO_DIALOGSTATEINFO_H */
