/*
 *  BytesSender.h
 *  semaine
 *
 *  Created by Marc Schröder on 12.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_SENDER_BYTESSENDER_H
#define SEMAINE_CMS_SENDER_BYTESSENDER_H


#include <semaine/config.h>

#include <semaine/cms/sender/Sender.h>

using namespace semaine::cms::sender;


namespace semaine {
namespace cms {
namespace sender {

class BytesSender : public Sender
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
	BytesSender(const std::string & topicName, const std::string & datatype, const std::string & source)
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
	BytesSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName, const std::string & datatype, const std::string & source)
	throw (CMSException, SystemConfigurationException);

	virtual ~BytesSender();
	
	void sendBytesMessage(const unsigned char * bytes, std::size_t numBytes, long long usertime, const std::string & event = SEMAINE_CMS_EVENT_SINGLE)
	throw(CMSException, SystemConfigurationException);
	


};

} // namespace sender
} // namespace cms
} // namespace semaine

#endif

