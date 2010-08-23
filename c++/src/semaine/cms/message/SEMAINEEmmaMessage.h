/*
 * SEMAINEEmmaMessage.h
 *
 *  Created on: 23.08.2010
 *      Author: marc
 */

#ifndef SEMAINE_CMS_MESSAGE_SEMAINEEMMAMESSAGE_H
#define SEMAINE_CMS_MESSAGE_SEMAINEEMMAMESSAGE_H

#include <semaine/cms/message/SEMAINEXMLMessage.h>
#include <cms/Message.h>
#include <semaine/util/XMLTool.h>
#include <semaine/cms/exceptions/MessageFormatException.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

using namespace cms;

namespace semaine {

namespace cms {

namespace message {

class SEMAINEEmmaMessage: public semaine::cms::message::SEMAINEXMLMessage {
public:
	SEMAINEEmmaMessage(const Message * message)
	throw(semaine::cms::exceptions::MessageFormatException, semaine::cms::exceptions::SystemConfigurationException);
	virtual ~SEMAINEEmmaMessage() {};

	XERCES_CPP_NAMESPACE::DOMElement * getOneOf();
	XERCES_CPP_NAMESPACE::DOMElement * getSequence();
	XERCES_CPP_NAMESPACE::DOMElement * getTopLevelInterpretation();

	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getEmotionElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getCategoryElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getGenderElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getFeatureElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getBehaviourElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getTextElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getSpeakingElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getPitchElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);

};

}

}

}

#endif /* SEMAINE_CMS_MESSAGE_SEMAINESTATEMESSAGE_H */
