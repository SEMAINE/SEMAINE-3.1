/*
 *  FeatureSender.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_SENDER_XMLSENDER_H
#define SEMAINE_CMS_SENDER_XMLSENDER_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/cms/event.h>

#include <semaine/cms/sender/Sender.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
  #include <iostream>
#else
  #include <iostream.h>
#endif

XERCES_CPP_NAMESPACE_USE



using namespace cms;
using namespace semaine::cms::sender;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace sender {

class XMLSender : public Sender
{
public:
	/**
	 * Create a new Sender to the given topic on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: default value empty;</li>
	 * <li>jms Password: default value empty.</li>
	 * </ul>
	 * Datatype and source will be sent with every message.
	 * @param topic the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @throws CMSException
	 */
	XMLSender(const std::string & topicName, const std::string & datatype, const std::string & source)
	throw(CMSException, SystemConfigurationException);
	
	/**
	 * Create a new CMS IOBase connection with the given topic on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topicName the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @throws CMSException
	 */
	XMLSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName, const std::string & datatype, const std::string & source)
	throw (CMSException, SystemConfigurationException);

	virtual ~XMLSender();
	
	void sendXML(const DOMDocument * document, long long usertime, const std::string & event = SEMAINE_CMS_EVENT_SINGLE)
	throw(CMSException, SystemConfigurationException);
	
	


protected:
	virtual void fillMessageProperties(Message * message, long long usertime)
	throw(CMSException);

private:
	DOMWriter * writer;

	void setupXMLStuff() throw (SystemConfigurationException);
};

} // namespace sender
} // namespace cms
} // namespace semaine



#endif

