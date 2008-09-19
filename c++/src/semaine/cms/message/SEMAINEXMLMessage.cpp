/*
 *  SEMAINEXMLMessage.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SEMAINEXMLMessage.h"


namespace semaine {
namespace cms {
namespace message {

const std::string SEMAINEXMLMessage::IS_XML = "xml";


SEMAINEXMLMessage::SEMAINEXMLMessage(const Message * message)
throw(MessageFormatException,SystemConfigurationException) :
	SEMAINEMessage(message)
{
	try {
		XMLPlatformUtils::Initialize();
	} catch (const XMLException& xe) {
		char * err = XMLString::transcode(xe.getMessage());
		std::cerr << err << std::endl;
		XMLString::release(&err);
		throw SystemConfigurationException("Cannot initialise XML system");
	}

	const TextMessage * tm = dynamic_cast<const TextMessage *>(message);
	if (tm == NULL) {
		throw MessageFormatException(std::string("Expected a text message, but got a ")+typeid(*message).name());
	}
	
	try {
		bool isXML = message->getBooleanProperty(IS_XML);
		if (!isXML)
			throw MessageFormatException("expected XML message");
	} catch (const CMSException & ce) {
		ce.printStackTrace();
		throw MessageFormatException(std::string("Cannot read property '")+IS_XML+"'");
	}
	parseDocument();
}

SEMAINEXMLMessage::~SEMAINEXMLMessage()
{
	  XMLPlatformUtils::Terminate();
}

void SEMAINEXMLMessage::parseDocument()
throw(MessageFormatException,SystemConfigurationException)
{
	XercesDOMParser* parser = new XercesDOMParser();
    //parser->setValidationScheme(XercesDOMParser::Val_Always);    
	parser->setDoNamespaces(true);

    try {
		std::string msgText = getText();
		const char * msgTextC = msgText.c_str();
		MemBufInputSource* memIS = new MemBufInputSource((const XMLByte *)msgTextC, strlen(msgTextC), "test", false);
        parser->parse(*memIS);
		document = parser->getDocument();
    } catch (const XMLException& toCatch) {
        char* err = XMLString::transcode(toCatch.getMessage());
        std::cerr << err << std::endl;
        XMLString::release(&err);
        throw MessageFormatException("Cannot parse xml");
	} catch (const DOMException& toCatch) {
        char* err = XMLString::transcode(toCatch.msg);
        std::cerr << err << std::endl;
        XMLString::release(&err);
        throw MessageFormatException("Cannot parse xml");
    } catch (...) {
        throw MessageFormatException("Cannot parse xml");
    }

    delete parser;

}


} // namespace message
} // namespace cms
} // namespace semaine