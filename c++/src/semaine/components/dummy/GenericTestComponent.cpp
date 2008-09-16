/*
 *  GenericTestComponent.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "GenericTestComponent.h"

#include <decaf/lang/System.h>
#include <decaf/lang/Thread.h>

using namespace semaine::cms::receiver;
using namespace semaine::cms::sender;

namespace semaine {
namespace components {
namespace dummy {

GenericTestComponent::GenericTestComponent(const std::string & name, const std::string & receiveTopic, const std::string & sendTopic)
	throw(CMSException) :
	Component(name),
	boredTime(3000), // ms
	lastMessageTime(0)
{
	receivers.push_back(new Receiver(receiveTopic));
	senders.push_back(new Sender(sendTopic, sendTopic, "GenericTestComponent"));
}


GenericTestComponent::~GenericTestComponent()
{

}


void GenericTestComponent::act() throw(CMSException)
{
	long long time = decaf::lang::System::currentTimeMillis();
	if (time - lastMessageTime > boredTime) {
		senders.front()->sendTextMessage("I'm bored", time);
		lastMessageTime = time;
	}
}


void GenericTestComponent::react(SEMAINEMessage * message) throw(CMSException)
{
	std::cout << "Received message from '" << message->getSource() << "' of type '" << message->getDatatype()
		<< "'" << std::endl;
	std::string text = message->getText();
	std::cout << text << std::endl;
	decaf::lang::Thread::sleep(1000);
	long long time = decaf::lang::System::currentTimeMillis();
	senders.front()->sendTextMessage("Hello there", time);
	lastMessageTime = time;
}


} // namespace dummy
} // namespace components
} // namespace semaine
