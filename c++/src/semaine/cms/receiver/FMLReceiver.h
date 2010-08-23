/*
 *  FMLReceiver.h
 *  semaine
 *
 *  Created by Marc Schröder on 03.11.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */



#ifndef SEMAINE_CMS_RECEIVER_FMLRECEIVER_H
#define SEMAINE_CMS_RECEIVER_FMLRECEIVER_H

#include <semaine/config.h>

#include <semaine/cms/receiver/XMLReceiver.h>



using namespace cms;
using namespace semaine::cms::exceptions;
using namespace semaine::cms::message;

namespace semaine {
namespace cms {
namespace receiver {

/**
 * An abstraction of the Receiver for FML.
 * @author marc
 *
 */
class FMLReceiver : public XMLReceiver
{
public:
	/**
	 * Create a receiver that will listen for all messages in the given Topic
	 * on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: default value empty;</li>
	 * <li>jms Password: default value empty.</li>
	 * </ul>
	 * @param topic the name of the CMS Topic to listen to.
	 */
	FMLReceiver(const std::string & topicName) throw(CMSException) :
		XMLReceiver(topicName)
	{}
	
	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector,
	 * on the default JMS server.
	 * The default JMS server is determined as follows:
	 * <ul>
	 * <li>jms URL: default value <code>tcp://localhost:61616</code>;</li>
	 * <li>jms User: default value empty;</li>
	 * <li>jms Password: default value empty.</li>
	 * </ul>
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	FMLReceiver(const std::string & topicName, const std::string & messageSelector)
	throw(CMSException) :
		XMLReceiver(topicName, messageSelector)
	{}

	/**
	 * Create a receiver that will listen for all messages in the given Topic
	 * on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topic the name of the CMS Topic to listen to.
	 */
	FMLReceiver(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
		const std::string & topicName)
	throw (CMSException) :
		XMLReceiver(cmsUrl, cmsUser, cmsPassword, topicName)
	{}

	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector,
	 * on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	FMLReceiver(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
		const std::string & topicName, const std::string & messageSelector)
	throw (CMSException) :
		XMLReceiver(cmsUrl, cmsUser, cmsPassword, topicName, messageSelector)
	{}


};

} // namespace receiver
} // namespace cms
} // namespace semaine


#endif

