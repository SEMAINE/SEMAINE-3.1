/*
 *  FeatureSender.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "FeatureSender.h"

#include <sstream>

namespace semaine {
namespace cms {
namespace sender {

FeatureSender::FeatureSender(const std::string & topicName, const std::string & datatype, const std::string & source, int period)
throw(CMSException) :
	Sender(topicName, datatype, source),
	sendBinary(false)
{
	this->period = period;
}

FeatureSender::FeatureSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
	const std::string & datatype, const std::string & source, const std::string & topicName, int period)
throw (CMSException) :
	Sender(cmsUrl, cmsUser, cmsPassword, topicName, datatype, source),
	sendBinary(false)
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

void FeatureSender::sendFeatureVector(const std::vector<float> & features, long long usertime)
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
		sendBinaryFeatureVector(features, usertime);
	else
		sendTextFeatureVector(features, usertime);
}


void FeatureSender::sendTextFeatureVector(const std::vector<float> & features, long long usertime)
throw(CMSException)
{
	std::stringstream buf;
	for (int i=0; i<features.size(); i++) {
		buf << features[i] << " " << featureNames[i] << std::endl;
	}
	sendTextMessage(buf.str(), usertime);
}




} // namespace sender
} // namespace cms
} // namespace semaine
