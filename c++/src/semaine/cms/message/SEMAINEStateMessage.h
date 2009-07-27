/*
 * SEMAINEStateMessage.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_CMS_MESSAGE_SEMAINESTATEMESSAGE_H
#define SEMAINE_CMS_MESSAGE_SEMAINESTATEMESSAGE_H

#include <semaine/cms/message/SEMAINEXMLMessage.h>
#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <cms/Message.h>
#include <semaine/util/XMLTool.h>
#include <semaine/cms/exceptions/MessageFormatException.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

using namespace cms;

namespace semaine {

namespace cms {

namespace message {

class SEMAINEStateMessage: public semaine::cms::message::SEMAINEXMLMessage {
public:
	SEMAINEStateMessage(const Message * message, semaine::datatypes::stateinfo::StateInfo::Type stateInfoType)
	throw(semaine::cms::exceptions::MessageFormatException, semaine::cms::exceptions::SystemConfigurationException);
	virtual ~SEMAINEStateMessage();
	semaine::datatypes::stateinfo::StateInfo * getState() { return state; };

protected:
	semaine::cms::CMSLogger * log;
	semaine::datatypes::stateinfo::StateInfo * state;
	semaine::datatypes::stateinfo::StateInfo::Type stateInfoType;
	semaine::datatypes::stateinfo::StateInfo * createState(XERCES_CPP_NAMESPACE::DOMDocument * doc)
	throw(semaine::cms::exceptions::MessageFormatException);
};

}

}

}

#endif /* SEMAINE_CMS_MESSAGE_SEMAINESTATEMESSAGE_H */
