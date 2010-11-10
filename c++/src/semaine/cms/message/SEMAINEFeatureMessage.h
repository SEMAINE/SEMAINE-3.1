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

#include <sstream>

using namespace cms;

namespace semaine {
namespace cms {
namespace message {

class SEMAINEFeatureMessage : public SEMAINEMessage
{
public:
	SEMAINEFeatureMessage(const Message * message) throw(semaine::cms::exceptions::MessageFormatException);
	virtual ~SEMAINEFeatureMessage();

	/**
	 * Get the feature vector from the message.
	 * @return the feature vector contained in the message.
	 * @throws CMSException
	 * @throws MessageFormatException
	 */
	std::vector<float> getFeatureVector()
	throw(CMSException, semaine::cms::exceptions::MessageFormatException);

	/**
	 * Get the list of feature names if available.
	 * @return an array of strings containing the names of features if available,
	 * or an empty vector if the feature names are not available.
	 * @throws JMSException
	 */
	std::vector<std::string> getFeatureNames()
	throw(CMSException);

	/**
	 * Check if the feature message has a list of feature names set.
	 */
	bool hasFeatureNames() { return !featureNames.empty(); }

	/**
	 * Set the feature names corresponding to the features.
	 * @param names a vector of feature names, which must correspond
	 * in number and in their order to the features in this message.
	 * @throws MessageFormatException if the number of feature names
	 * does not match the number of features saved in the message
	 *
	 */
	void setFeatureNames(std::vector<std::string> const & names)
	throw (semaine::cms::exceptions::MessageFormatException)
	{
		if (names.size() != features.size())
		{
			std::stringstream buf;
			buf << "There are " << features.size() << " features but " << names.size() << " feature names";
			throw semaine::cms::exceptions::MessageFormatException(buf.str());
		}
		featureNames = names;
	}

protected:
	std::vector<float> features;
	std::vector<std::string> featureNames;


	void readFromBytesMessage(const BytesMessage * bm)
	throw(CMSException, semaine::cms::exceptions::MessageFormatException);


	void readFromTextMessage(const TextMessage * tm)
	throw(CMSException);




};

} // namespace message
} // namespace cms
} // namespace semaine


#endif

