/*
 *  FeatureSender.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "FeatureSender.h"
#include <semaine/cms/event.h>

#include <sstream>

namespace semaine {
namespace cms {
namespace sender {

FeatureSender::FeatureSender(const std::string & topicName, const std::string & datatype, const std::string & source, int period)
throw(CMSException) :
	Sender(topicName, datatype, source)
{
	this->period = period;
}

FeatureSender::FeatureSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
	const std::string & datatype, const std::string & source, const std::string & topicName, int period)
throw (CMSException) :
	Sender(cmsUrl, cmsUser, cmsPassword, topicName, datatype, source)
{
	this->period = period;
}

void FeatureSender::setFeatureNames(const std::vector<std::string> & aFeatureNames)
throw (SystemConfigurationException)
{
	if (!this->featureNames.empty())
		throw SystemConfigurationException("feature names are already set, can only set once");
	featureNames = aFeatureNames;
}

bool FeatureSender::areFeatureNamesSet()
{
	return (!featureNames.empty());
}

void FeatureSender::sendFeatureVector(const std::vector<float> & features, long long usertime,
		bool sendBinary, const std::string & contentID,
		long long contentCreationTime, const std::string & contentType)
throw(CMSException, SystemConfigurationException)
{
	if (featureNames.empty())
		throw SystemConfigurationException("you must set feature names before sending feature vectors");
	if (featureNames.size() != features.size())  {
		std::ostringstream oss;
		oss << "vectors are not of equal length: names=" << featureNames.size() << ", features=" << features.size();
		throw SystemConfigurationException(oss.str());
	}
	if (sendBinary)
		sendBinaryFeatureVector(features, usertime, contentID, contentCreationTime, contentType);
	else
		sendTextFeatureVector(features, usertime, contentID, contentCreationTime, contentType);
}


void FeatureSender::sendTextFeatureVector(const std::vector<float> & features, long long usertime,
		const std::string & contentID, long long contentCreationTime, const std::string & contentType)
throw(CMSException)
{
	std::stringstream buf;
	for (size_t i=0; i<features.size(); i++) {
		buf << features[i] << " " << featureNames[i] << std::endl;
	}
	sendTextMessage(buf.str(), usertime, contentID, contentCreationTime, contentType);
}

void FeatureSender::sendBinaryFeatureVector(const std::vector<float> & features, long long usertime,
		const std::string & contentID, long long contentCreationTime, const std::string & contentType)
throw(CMSException)
{
	if (!isConnectionStarted)
		throw SystemConfigurationException("Connection is not started!");
    if (exception != NULL) {
    	exception->printStackTrace();
    	throw SystemConfigurationException("Exception Listener has received an exception, will not try to send");
    }
	BytesMessage * message = session->createBytesMessage();
	message->writeInt((int)features.size());
	for (size_t i=0, len=features.size(); i<len; i++) {
		message->writeFloat(features[i]);
	}
	fillMessageProperties(message, usertime, contentID, contentCreationTime, contentType);
	if (isPeriodic())
		message->setIntProperty(SEMAINEMessage::PERIOD, getPeriod());
	else // event-based
		message->setStringProperty(SEMAINEMessage::EVENT, SEMAINE_CMS_EVENT_SINGLE);
	producer->send(message);
	delete message;
}



} // namespace sender
} // namespace cms
} // namespace semaine
