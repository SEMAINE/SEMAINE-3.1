/*
 *  XMLSender.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "XMLSender.h"

#include <semaine/cms/message/SEMAINEXMLMessage.h>
#include <semaine/util/XMLTool.h>

using namespace semaine::cms::message;
using namespace semaine::util;

namespace semaine {
namespace cms {
namespace sender {

XMLSender::XMLSender(const std::string & topicName, const std::string & datatype, const std::string & source)
throw(CMSException, SystemConfigurationException) :
	Sender(topicName, datatype, source)
{
	setupXMLStuff();
}

XMLSender::XMLSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
	const std::string & datatype, const std::string & source, const std::string & topicName)
throw (CMSException, SystemConfigurationException) :
	Sender(cmsUrl, cmsUser, cmsPassword, topicName, datatype, source)
{
	setupXMLStuff();
}


XMLSender::~XMLSender()
{
	writer->release();
	writer = NULL;
}


void XMLSender::fillMessageProperties(Message * message, long long usertime)
throw(CMSException)
{
	Sender::fillMessageProperties(message, usertime);
	message->setBooleanProperty(SEMAINEXMLMessage::IS_XML, true);
}

void XMLSender::sendXML(const DOMDocument * document, long long usertime, const std::string & event)
throw(CMSException, SystemConfigurationException)
{
	if (isPeriodic())
		throw SystemConfigurationException("XML sender is expected to be event-based, but sender is in periodic mode.");
	if (!isConnectionStarted)
		throw SystemConfigurationException("Connection is not started!");
	MemBufFormatTarget * output = new MemBufFormatTarget();
	writer->writeNode(output, *document);
	const XMLByte * buf = output->getRawBuffer();
	//int len = output->getLen();
	std::string xmlString((const char *)buf);
	sendTextMessage(xmlString, usertime, event);
}


void XMLSender::setupXMLStuff()
throw (SystemConfigurationException)
{
	try {
		XMLCh tempStr[100];
		//XMLString::transcode("LS", tempStr, 99);
		//XMLCh * ls = XMLString::transcode("LS");
		//DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(ls);
		//XMLString::release(&ls);
		DOMImplementation * impl = XMLTool::getDOMImplementation();
		DOMImplementationLS * implLS = dynamic_cast<DOMImplementationLS *>(impl);
		if (implLS == NULL) {
			throw new SystemConfigurationException(std::string("DOM impl is not a DOMImplementationLS, but a ")+typeid(*impl).name());
		}
		writer = implLS->createDOMWriter();
		XMLString::transcode("UTF-8", tempStr, 99);
		writer->setEncoding(tempStr);
	} catch (XMLException &xe) {
		char * err = XMLString::transcode(xe.getMessage());
		std::cerr << err << std::endl;
		XMLString::release(&err);
		throw SystemConfigurationException("Cannot initialise XML system");
	} catch (DOMException &de) {
		char * err = XMLString::transcode(de.getMessage());
		std::cerr << err << std::endl;
		XMLString::release(&err);
		throw SystemConfigurationException("Cannot initialise XML system");
	}
}

} // namespace sender
} // namespace cms
} // namespace semaine
