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
	 * Create a receiver that will listen for all messages in the given Topic.
	 * @param topic the name of the JMS Topic to listen to.
	 */
	FeatureReceiver(const std::string & topicName) throw(CMSException) :
		Receiver(topicName)
	{}
	
	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector. 
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	FeatureReceiver(const std::string & topicName, const std::string & messageSelector)
	throw(CMSException) :
		Receiver(topicName, messageSelector)
	{}


protected:
	virtual SEMAINEMessage * createSEMAINEMessage(const Message * message)
	throw(MessageFormatException)
	{
		return new SEMAINEFeatureMessage(message);
	}



};

} // namespace receiver
} // namespace cms
} // namespace semaine


#endif

