/*
 *  BMLSender.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_SENDER_BMLSENDER_H
#define SEMAINE_CMS_SENDER_BMLSENDER_H

#include <semaine/config.h>

#include <semaine/cms/sender/XMLSender.h>

using namespace cms;
using namespace semaine::cms::sender;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace sender {

class BMLSender : public XMLSender
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
	 * @param source the name of the component sending the data.
	 * @throws CMSException
	 */
	BMLSender(const std::string & topicName, const std::string & source)
	throw(CMSException, SystemConfigurationException) :
		XMLSender(topicName, "BML", source)
	{
	}
	
	/**
	 * Create a new CMS IOBase connection with the given topic on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topicName the name of the topic to send to.
	 * @param source the name of the component sending the data.
	 * @throws CMSException
	 */
	BMLSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName, const std::string & source)
	throw (CMSException, SystemConfigurationException) :
		XMLSender(cmsUrl, cmsUser, cmsPassword, topicName, "BML", source)
	{
	}

};

} // namespace sender
} // namespace cms
} // namespace semaine



#endif

