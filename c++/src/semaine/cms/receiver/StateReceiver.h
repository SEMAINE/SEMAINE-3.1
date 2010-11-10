/*
 * StateReceiver.h
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_CMS_RECEIVER_STATERECEIVER_H
#define SEMAINE_CMS_RECEIVER_STATERECEIVER_H

#include "XMLReceiver.h"

#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <semaine/cms/message/SEMAINEStateMessage.h>
#include <map>

namespace semaine {

namespace cms {

namespace receiver {

class StateReceiver: public semaine::cms::receiver::XMLReceiver {
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
	 * @param stateInfoType the type of state info represented by SEMAINEStateMessage objects created by {@link #createSEMAINEMessage(Message)}.
	 * This determines the type of {@link StateInfo} object returned by {@link SEMAINEStateMessage#getState()}.
	 */
	StateReceiver(const std::string & topicName, semaine::datatypes::stateinfo::StateInfo::Type datatype) throw(CMSException) :
		XMLReceiver(topicName), stateInfoType(datatype)
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
	 * @param stateInfoType the type of state info represented by SEMAINEStateMessage objects created by {@link #createSEMAINEMessage(Message)}.
	 * This determines the type of {@link StateInfo} object returned by {@link SEMAINEStateMessage#getState()}.
	 */
	StateReceiver(const std::string & topicName, const std::string & messageSelector, semaine::datatypes::stateinfo::StateInfo::Type datatype)
	throw(CMSException) :
		XMLReceiver(topicName, messageSelector), stateInfoType(datatype)
	{}


	/**
	 * Create a receiver that will listen for all messages in the given Topic
	 * on the given JMS server.
	 * @param cmsUrl the url where to contact the JMS server
	 * @param cmsUser the username to use (can be an empty string)
	 * @param cmsPassword the password to use (can be an empty string)
	 * @param topic the name of the CMS Topic to listen to.
	 * @param stateInfoType the type of state info represented by SEMAINEStateMessage objects created by {@link #createSEMAINEMessage(Message)}.
	 * This determines the type of {@link StateInfo} object returned by {@link SEMAINEStateMessage#getState()}.
	 */
	StateReceiver(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
		const std::string & topicName, semaine::datatypes::stateinfo::StateInfo::Type datatype)
	throw (CMSException) :
		XMLReceiver(cmsUrl, cmsUser, cmsPassword, topicName), stateInfoType(datatype)
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
	 * @param stateInfoType the type of state info represented by SEMAINEStateMessage objects created by {@link #createSEMAINEMessage(Message)}.
	 * This determines the type of {@link StateInfo} object returned by {@link SEMAINEStateMessage#getState()}.
	 */
	StateReceiver(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword,
		const std::string & topicName, const std::string & messageSelector, semaine::datatypes::stateinfo::StateInfo::Type datatype)
	throw (CMSException) :
		XMLReceiver(cmsUrl, cmsUser, cmsPassword, topicName, messageSelector), stateInfoType(datatype)
	{}


	virtual ~StateReceiver() {}

	/**
	 * Provide the current best guess for the given info item.
	 * @param infoItem
	 * @return
	 */
	std::string getCurrentBestGuess(const std::string & infoItem);
	virtual void onMessage(const Message * m);


protected:
	virtual semaine::cms::message::SEMAINEMessage * createSEMAINEMessage(const Message * message)
	throw(semaine::cms::exceptions::MessageFormatException)
	{
		return new semaine::cms::message::SEMAINEStateMessage(message, stateInfoType);
	}

private:
	semaine::datatypes::stateinfo::StateInfo::Type stateInfoType;
	std::map<std::string, std::string> currentBestGuess;
	void updateInformationState(semaine::cms::message::SEMAINEMessage * m);

};

}

}

}

#endif /* SEMAINE_CMS_RECEIVER_STATERECEIVER_H */
