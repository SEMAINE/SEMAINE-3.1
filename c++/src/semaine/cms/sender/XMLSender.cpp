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
using namespace XERCES_CPP_NAMESPACE;

namespace semaine {
namespace cms {
namespace sender {

XMLSender::XMLSender(const std::string & topicName, const std::string & datatype, const std::string & source)
throw(CMSException, SystemConfigurationException) :
	Sender(topicName, datatype, source)
{
}

XMLSender::XMLSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
	const std::string & datatype, const std::string & source, const std::string & topicName)
throw (CMSException, SystemConfigurationException) :
	Sender(cmsUrl, cmsUser, cmsPassword, topicName, datatype, source)
{
}


XMLSender::~XMLSender()
{
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
	const std::string xmlString = XMLTool::dom2string(document);
	sendTextMessage(xmlString, usertime, event);
}


} // namespace sender
} // namespace cms
} // namespace semaine
