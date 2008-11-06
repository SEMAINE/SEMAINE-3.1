/*
 *  SemaineML.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 06.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SemaineML.h"

namespace semaine {
namespace datatypes {
namespace xml {

const std::string SemaineML::version = "0.0.1";
const std::string SemaineML::namespaceURI = "http://www.semaine-project.eu/semaineml";
	
	// Elements
const std::string SemaineML::E_USERSTATE = "user-state";
const std::string SemaineML::E_AGENTSTATE = "agent-state";
const std::string SemaineML::E_DIALOGSTATE = "dialog-state";
const std::string SemaineML::E_FEATURE = "feature";
const std::string SemaineML::E_EVENT = "event";
const std::string SemaineML::E_BEHAVIOUR = "behaviour";
const std::string SemaineML::E_TEXT = "text";
const std::string SemaineML::E_EMOTIONALLY_CONCORDANT_WITH_USER = "emotionally-concordant-with-user";
const std::string SemaineML::E_DIALOG_ACT = "dialog-act";
const std::string SemaineML::E_DIALOG_HISTORY = "dialog-history";
const std::string SemaineML::E_SPEAKER = "speaker";
const std::string SemaineML::E_LISTENER = "listener";
const std::string SemaineML::E_TOPIC = "topic";

	// Attributes
const std::string SemaineML::A_WHO = "who";
const std::string SemaineML::A_NAME = "name";
const std::string SemaineML::A_SPEAKER = "speaker";
const std::string SemaineML::A_TOPIC = "topic";
const std::string SemaineML::A_VALUE = "value";
const std::string SemaineML::A_TIME = "time";
const std::string SemaineML::A_INTENSITY = "intensity";

} // namespace xml
} // namespace datatypes
} // namespace semaine


