/*
 * StateSender.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_CMS_SENDER_STATESENDER_H
#define SEMAINE_CMS_SENDER_STATESENDER_H

#include <semaine/cms/sender/XMLSender.h>

#include <semaine/datatypes/stateinfo/StateInfo.h>

namespace semaine {

namespace cms {

namespace sender {

class StateSender: public semaine::cms::sender::XMLSender {
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
	StateSender(const std::string & topicName, semaine::datatypes::stateinfo::StateInfo::Type datatype, const std::string & source)
	throw(CMSException, SystemConfigurationException)
	: XMLSender(topicName, semaine::datatypes::stateinfo::StateInfo::TypeNames[datatype], source)
	{}

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
	StateSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName, semaine::datatypes::stateinfo::StateInfo::Type datatype, const std::string & source)
	throw (CMSException, SystemConfigurationException)
	: XMLSender(cmsUrl, cmsUser, cmsPassword, topicName, semaine::datatypes::stateinfo::StateInfo::TypeNames[datatype], source)
	{}


	virtual ~StateSender() {}

	void sendStateInfo(semaine::datatypes::stateinfo::StateInfo * state, long long usertime)
	throw(CMSException, SystemConfigurationException);
};

}

}

}

#endif /* SEMAINE_CMS_SENDER_STATESENDER_H */
