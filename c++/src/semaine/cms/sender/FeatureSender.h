/*
 *  FeatureSender.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_SENDER_FEATURESENDER_H
#define SEMAINE_CMS_SENDER_FEATURESENDER_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/cms/sender/Sender.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

using namespace cms;
using namespace semaine::cms::sender;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace sender {

class FeatureSender : public Sender
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
	 * @param the period in which we intend to send data, in milliseconds
	 * @throws CMSException
	 */
	FeatureSender(const std::string & topicName, const std::string & datatype, const std::string & source, int period)
	throw(CMSException);
	
	virtual ~FeatureSender()
	{}

	/**
	 * Create a new CMS IOBase connection with the given topic on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topicName the name of the topic to send to.
	 * @param datatype the name of the type of data being sent.
	 * @param source the name of the component sending the data.
	 * @param the period in which we intend to send data, in milliseconds
	 * @throws CMSException
	 */
	FeatureSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName, const std::string & datatype, const std::string & source, int period)
	throw (CMSException);

	/**
	 * Initiate the list of feature names to use. This method must be called
	 * exactly once, after the creation of the object and before the first
	 * send operation.
	 * @param aFeatureNames the feature names, in exactly the same order
	 * as the features that will be sent.
	 * @throws SystemConfigurationException if feature names are already set.
	 */
	void setFeatureNames(const std::vector<std::string> & aFeatureNames)
	throw (SystemConfigurationException);

	/**
	 * Returns 1, when feature names were already set and 0 otherwise. 
	 *
	 */
	bool areFeatureNamesSet();

	/**
	 * Provides access to the array of feature names. May be useful for debugging.
	 * @return the internal feature names array.
	 */
	const std::vector<std::string> & getFeatureNames()
	{
		return featureNames;
	}
	
	/**
	 * Send a vector of float features. Before sending features,
	 * {@link #setFeatureNames() must be called, and the
	 * names must correspond to the features passed in the argument.
	 * @param features the features to send.
	 * @param usertime the time in "user" space that these features
	 * refer to, in milliseconds since 1970.
	 * @throws CMSException
	 * @throws SystemConfigurationException if the feature names have not been set,
	 * or if the number of features is not the same as the number of feature names
	 */
	void sendFeatureVector(const std::vector<float> & features, long long usertime)
	throw(CMSException, SystemConfigurationException);

protected:
	bool sendBinary;
	std::vector<std::string> featureNames;

	void sendBinaryFeatureVector(const std::vector<float> & features, long long usertime)
	throw(CMSException, SystemConfigurationException)
	{
		throw SystemConfigurationException("not yet implemented");
	}
	
	void sendTextFeatureVector(const std::vector<float> & features, long long usertime)
	throw(CMSException);

};

} // namespace sender
} // namespace cms
} // namespace semaine



#endif

