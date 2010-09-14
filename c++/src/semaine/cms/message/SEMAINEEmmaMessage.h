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

	/**
	 * Get the emotion elements that are children of the given interpretation element.
	 * @param interpretation an interpretation element
	 * @return a list of emotion elements, or an empty list if there are no emotion children below interpretation.
	 */
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getEmotionElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);

	/**
	 * Get a list of any non-verbal elements which are direct children of the given interpretation.
	 * @param interpretation
	 * @return a list of non-verbal elements of various kinds, or an empty list if there are no such elements.
	 * @see #getBMLElements(Element)
	 * @see #getGenderElements(Element)
	 * @see #getSpeakingElements(Element)
	 * @see #getVocalizationElements(Element)
	 * @see #getPitchElements(Element)
	 * @see #getFacePresentElements(Element)
	 */
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getNonverbalElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);

	/**
	 * Get the BML (behaviour markup language) elements that are children of the given interpretation element.
	 * @param interpretation
	 * @return a list of BML elements, or an empty list if there are no such children below interpretation.
	 */
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getBMLElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);

	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getGenderElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getSpeakingElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getPitchElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getVocalizationElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);
	std::list<XERCES_CPP_NAMESPACE::DOMElement *> * getFacePresentElements(XERCES_CPP_NAMESPACE::DOMElement * interpretation);

};

}

}

}

#endif /* SEMAINE_CMS_MESSAGE_SEMAINESTATEMESSAGE_H */
