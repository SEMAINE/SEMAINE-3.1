/*
 *  XMLSender.h
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_SENDER_XMLSENDER_H
#define SEMAINE_CMS_SENDER_XMLSENDER_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/cms/event.h>

#include <semaine/cms/sender/Sender.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>

#include <semaine/util/XMLTool.h>


using namespace cms;
using namespace semaine::cms::sender;
using namespace semaine::cms::exceptions;

namespace semaine {
namespace cms {
namespace sender {

class XMLSender : public Sender
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
	 * @throws CMSException
	 */
	XMLSender(const std::string & topicName, const std::string & datatype, const std::string & source)
	throw(CMSException, SystemConfigurationException);

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
	XMLSender(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName, const std::string & datatype, const std::string & source)
	throw (CMSException, SystemConfigurationException);

	virtual ~XMLSender();

	/**
	 * Send an XML message via this sender, for event-based messages.
	 * This will send a message to the registered topic
	 * with the following message properties:
	 * <ul>
	 *   <li><code>datatype</code> is a String property containing the value produced by {@link #getDatatype()};</li>
	 *   <li><code>source</code> is a String property containing the value produced by {@link #getSource()};</li>
	 *   <li><code>usertime</code> is a long property containing the value of parameter <code>usertime</code>;</li>
	 *   <li><code>event</code> is a String property containing
	 *   the String representation of the event parameter to this method.</li>
	 * </ul>
	 * Furthermore, if {@link #getTimeToLive()} returns a non-zero value, the message will
	 * contain a header field <code>JMSExpiration</code> containing the time when the message
	 * will expire.
	 * @param text the message text.
	 * @param usertime the "user" time at which this message is being sent,
	 * in milliseconds since system startup.
	 * @param event the type of event represented by this message.
	 * @param contentID a unique identifier for the message's content.
	 * If this is not the empty string, it will cause the addition of the String property <code>content-id</code> in the message.
	 * @param contentCreationTime the time when the content in this message was created.
	 * If this is not negative, it will cause the addition of the Long property <code>content-creation-time</code> in the message.
	 * @param contentType an optional content type for the message's content;
	 * The value may be one of SEMAINEMessage::CONTENT_TYPE_UTTERANCE, SEMAINEMessage::CONTENT_TYPE_LISTENERVOCALISATION,
	 * SEMAINEMessage::CONTENT_TYPE_VISUALONLY, or any other string.
	 * If this is not the empty string, it will cause the addition of the String property <code>content-type</code> in the message.
	 * @throws SystemConfigurationException if the connection is not started or the sender is in periodic mode.
	 */
	void sendXML(const XERCES_CPP_NAMESPACE::DOMDocument * document, long long usertime, const std::string & event = SEMAINE_CMS_EVENT_SINGLE, const std::string & contentID = "",
			long long contentCreationTime = -1, const std::string & contentType = "")
	throw(CMSException, SystemConfigurationException);




protected:
	virtual void fillMessageProperties(Message * message, long long usertime)
	throw(CMSException);

};

} // namespace sender
} // namespace cms
} // namespace semaine



#endif

