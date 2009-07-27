/*
 * AgentStateInfo.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 *  Copyright 2009 DFKI GmbH. All rights reserved.
 */

#ifndef SEMAINE_DATATYPES_STATEINFO_USERSTATEINFO_H
#define SEMAINE_DATATYPES_STATEINFO_USERSTATEINFO_H

#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <semaine/datatypes/xml/SemaineML.h>

namespace semaine {

namespace datatypes {

namespace stateinfo {

class UserStateInfo: public semaine::datatypes::stateinfo::StateInfo {
public:
	UserStateInfo(XERCES_CPP_NAMESPACE::DOMDocument * doc)
	throw(semaine::cms::exceptions::MessageFormatException)
	: semaine::datatypes::stateinfo::StateInfo(doc, "UserStateInfo",
			semaine::datatypes::xml::SemaineML::E_USERSTATE,
			semaine::datatypes::xml::SemaineML::namespaceURI,
			semaine::datatypes::stateinfo::StateInfo::UserState)
	{}

	UserStateInfo(std::map<std::string, std::string> const & infoItems)
	throw(semaine::cms::exceptions::SystemConfigurationException)
	: semaine::datatypes::stateinfo::StateInfo(infoItems, "UserStateInfo",
			semaine::datatypes::stateinfo::StateInfo::UserState)
	{}

	virtual ~UserStateInfo() {}
};

}

}

}

#endif /* SEMAINE_DATATYPES_STATEINFO_USERSTATEINFO_H */
