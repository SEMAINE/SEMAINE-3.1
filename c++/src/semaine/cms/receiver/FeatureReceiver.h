/*
 *  FeatureReceiver.h
 *  semaine
 *
 *  Created by Marc Schröder on 18.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_RECEIVER_FEATURERECEIVER_H
#define SEMAINE_CMS_RECEIVER_FEATURERECEIVER_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/cms/receiver/Receiver.h>
#include <semaine/cms/exceptions/MessageFormatException.h>
#include <semaine/cms/message/SEMAINEFeatureMessage.h>


using namespace cms;
using namespace semaine::cms::exceptions;
using namespace semaine::cms::message;

namespace semaine {
namespace cms {
namespace receiver {

/**
 * An abstraction of the Receiver for feature vectors.
 * @author marc
 *
 */
class FeatureReceiver : public Receiver
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
	FeatureReceiver(const std::string & topicName) throw(CMSException) :
		Receiver(topicName)
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
	FeatureReceiver(const std::string & topicName, const std::string & messageSelector)
	throw(CMSException) :
		Receiver(topicName, messageSelector)
	{}

	/**
	 * Create a receiver that will listen for all messages in the given Topic
	 * on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topic the name of the CMS Topic to listen to.
	 */
	FeatureReceiver(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
		const std::string & topicName)
	throw (CMSException) :
		Receiver(cmsUrl, cmsUser, cmsPassword, topicName)
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
	FeatureReceiver(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
		const std::string & topicName, const std::string & messageSelector)
	throw (CMSException) :
		Receiver(cmsUrl, cmsUser, cmsPassword, topicName, messageSelector)
	{}


protected:
	virtual SEMAINEMessage * createSEMAINEMessage(const Message * message)
	throw(MessageFormatException)
	{
		SEMAINEFeatureMessage * fm = new SEMAINEFeatureMessage(message);
		try {
			if (featureNames.empty() && fm->hasFeatureNames()) {
				featureNames = fm->getFeatureNames();
			} else if (!featureNames.empty() && !fm->hasFeatureNames()) {
				fm->setFeatureNames(featureNames);
			}
		} catch (CMSException & e) {
			std::stringstream buf;
			buf << "Problem accessing feature names: ";
			e.printStackTrace(buf);
			throw MessageFormatException(buf.str());
		}
		return fm;
	}


private:
	std::vector<std::string> featureNames;
};

} // namespace receiver
} // namespace cms
} // namespace semaine


#endif

