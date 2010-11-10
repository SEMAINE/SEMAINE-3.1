/*
 *  SEMAINEXMLMessage.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SEMAINEXMLMessage.h"

using namespace XERCES_CPP_NAMESPACE;

namespace semaine {
namespace cms {
namespace message {

const std::string SEMAINEXMLMessage::IS_XML = "xml";


SEMAINEXMLMessage::SEMAINEXMLMessage(const Message * message)
throw(semaine::cms::exceptions::MessageFormatException,semaine::cms::exceptions::SystemConfigurationException) :
	SEMAINEMessage(message), document(NULL)
{
	const TextMessage * tm = dynamic_cast<const TextMessage *>(message);
	if (tm == NULL) {
		throw semaine::cms::exceptions::MessageFormatException(std::string("Expected a text message, but got a ")+typeid(*message).name());
	}
	
	try {
		bool isXML = message->getBooleanProperty(IS_XML);
		if (!isXML)
			throw semaine::cms::exceptions::MessageFormatException("expected XML message");
	} catch (const CMSException & ce) {
		ce.printStackTrace();
		throw semaine::cms::exceptions::MessageFormatException(std::string("Cannot read property '")+IS_XML+"'");
	}
	parseDocument();
}

SEMAINEXMLMessage::~SEMAINEXMLMessage()
{
	if (document != NULL) {
		document->release();
		document = NULL;
	}
}

void SEMAINEXMLMessage::parseDocument()
throw(semaine::cms::exceptions::MessageFormatException,semaine::cms::exceptions::SystemConfigurationException)
{

    try {
		std::string msgText = getText();
	    document = XMLTool::parse(msgText);
    } catch (const XMLException& toCatch) {
        char* err = XMLString::transcode(toCatch.getMessage());
        std::cerr << err << std::endl;
        XMLString::release(&err);
        throw semaine::cms::exceptions::MessageFormatException("Cannot parse xml");
	} catch (const DOMException& toCatch) {
        char* err = XMLString::transcode(toCatch.msg);
        std::cerr << err << std::endl;
        XMLString::release(&err);
        throw semaine::cms::exceptions::MessageFormatException("Cannot parse xml");
    } catch (...) {
        throw semaine::cms::exceptions::MessageFormatException("Cannot parse xml");
    }


}


} // namespace message
} // namespace cms
} // namespace semaine
