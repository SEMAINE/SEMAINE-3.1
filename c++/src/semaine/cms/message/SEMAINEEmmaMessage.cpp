/*
 * SEMAINEEmmaMessage.cpp
 *
 *  Created on: 23.08.2010
 *      Author: marc
 */

#include "SEMAINEEmmaMessage.h"
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/EmotionML.h>
#include <semaine/datatypes/xml/SemaineML.h>

using namespace cms;
using namespace semaine::cms::exceptions;
using namespace XERCES_CPP_NAMESPACE;
using namespace semaine::datatypes::xml;

namespace semaine {

namespace cms {

namespace message {

SEMAINEEmmaMessage::SEMAINEEmmaMessage(const Message * message)
throw(SystemConfigurationException, MessageFormatException)
: SEMAINEXMLMessage(message)
{
}


DOMElement * SEMAINEEmmaMessage::getOneOf()
{
	return XMLTool::getChildElementByLocalNameNS(document->getDocumentElement(), EMMA::E_ONEOF, EMMA::namespaceURI);
}

DOMElement * SEMAINEEmmaMessage::getSequence()
{
	return XMLTool::getChildElementByLocalNameNS(document->getDocumentElement(), EMMA::E_SEQUENCE, EMMA::namespaceURI);
}

DOMElement * SEMAINEEmmaMessage::getTopLevelInterpretation()
{
	return XMLTool::getChildElementByLocalNameNS(document->getDocumentElement(), EMMA::E_INTERPRETATION, EMMA::namespaceURI);
}


std::list<DOMElement *> * SEMAINEEmmaMessage::getEmotionElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getCategoryElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, EmotionML::E_CATEGORY, EmotionML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getGenderElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, "gender", SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getFeatureElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_FEATURE, SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getBehaviourElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_BEHAVIOUR, SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getTextElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_TEXT, SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getSpeakingElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_SPEAKING, SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getPitchElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_PITCH, SemaineML::namespaceURI);
}





}

}

}
