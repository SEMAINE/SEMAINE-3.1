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
#include <semaine/datatypes/xml/BML.h>


using namespace cms;
using namespace XERCES_CPP_NAMESPACE;
using namespace semaine::datatypes::xml;

namespace semaine {

namespace cms {

namespace message {

SEMAINEEmmaMessage::SEMAINEEmmaMessage(const Message * message)
throw(semaine::cms::exceptions::SystemConfigurationException, semaine::cms::exceptions::MessageFormatException)
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

std::list<DOMElement *> * SEMAINEEmmaMessage::getNonverbalElements(DOMElement * interpretation)
{
	std::list<DOMElement *> * nonverbals = new std::list<DOMElement *>();
	nonverbals->splice(nonverbals->end(), * getGenderElements(interpretation));
	nonverbals->splice(nonverbals->end(), * getSpeakingElements(interpretation));
	nonverbals->splice(nonverbals->end(), * getPitchElements(interpretation));
	nonverbals->splice(nonverbals->end(), * getVocalizationElements(interpretation));
	nonverbals->splice(nonverbals->end(), * getFacePresentElements(interpretation));
	nonverbals->splice(nonverbals->end(), * getBMLElements(interpretation));
	return nonverbals;
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getBMLElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, BML::E_BML, BML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getGenderElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, "gender", SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getSpeakingElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_SPEAKING, SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getPitchElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_PITCH, SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getVocalizationElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_VOCALIZATION, SemaineML::namespaceURI);
}

std::list<DOMElement *> * SEMAINEEmmaMessage::getFacePresentElements(DOMElement * interpretation)
{
	return XMLTool::getChildrenByLocalNameNS(interpretation, SemaineML::E_FACEPRESENT, SemaineML::namespaceURI);
}




}

}

}
