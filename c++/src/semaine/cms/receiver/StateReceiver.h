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
	 * Create a receiver that will listen for all messages in the given Topic.
	 * @param topic the name of the JMS Topic to listen to.
	 */
	StateReceiver(const std::string & topicName, semaine::datatypes::stateinfo::StateInfo::Type datatype) throw(CMSException) :
		XMLReceiver(topicName), stateInfoType(datatype)
	{}

	/**
	 * Create a receiver that will listen only to the messages in the given Topic
	 * that will pass the given messageSelector.
	 * @param topicName the name of the JMS Topic to listen to.
	 * @param messageSelector a message selector expression, see e.g. http://java.sun.com/javaee/5/docs/api/javax/jms/Message.html
	 * for the detailed description.
	 */
	StateReceiver(const std::string & topicName, const std::string & messageSelector, semaine::datatypes::stateinfo::StateInfo::Type datatype)
	throw(CMSException) :
		XMLReceiver(topicName, messageSelector), stateInfoType(datatype)
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
	throw(MessageFormatException)
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
