/*
 *  SEMAINEFeatureMessage.h
 *  semaine
 *
 *  Created by Marc Schröder on 17.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_MESSAGE_SEMAINEFEATUREMESSAGE_H
#define SEMAINE_CMS_MESSAGE_SEMAINEFEATUREMESSAGE_H

#include <semaine/config.h>

#include <semaine/cms/message/SEMAINEMessage.h>
#include <semaine/cms/exceptions/MessageFormatException.h>

#include <cms/Message.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/CMSException.h>

using namespace cms;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace message {

class SEMAINEFeatureMessage : public SEMAINEMessage
{
public:
	SEMAINEFeatureMessage(const Message * message) throw(MessageFormatException);
	virtual ~SEMAINEFeatureMessage();

	/**
	 * Get the feature vector from the message.
	 * @return the feature vector contained in the message.
	 * @throws CMSException
	 * @throws MessageFormatException
	 */
	std::vector<float> getFeatureVector()
	throw(CMSException, MessageFormatException);

	/**
	 * Get the list of feature names if available.
	 * @return an array of strings containing the names of features if available, 
	 * or null if the feature names are not available.
	 * @throws JMSException
	 */
	std::vector<std::string> getFeatureNames()
	throw(CMSException);


protected:
	std::vector<float> features;
	std::vector<std::string> featureNames;


	void readFromBytesMessage(const BytesMessage * bm)
	throw(CMSException, MessageFormatException);


	void readFromTextMessage(const TextMessage * tm)
	throw(CMSException);




};

} // namespace message
} // namespace cms
} // namespace semaine


#endif

