/*
 *  GenericTestComponent.h
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */
 
#ifndef SEMAINE_COMPONENTS_DUMMY_GENERICTESTCOMPONENTS_H
#define SEMAINE_COMPONENTS_DUMMY_GENERICTESTCOMPONENTS_H

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
#include <semaine/cms/message/SEMAINEMessage.h>

using namespace cms;
using namespace semaine::components;
using namespace semaine::cms::message;

namespace semaine {
namespace components {
namespace dummy {

class GenericTestComponent : public Component
{
public:
	GenericTestComponent(const std::string & name, const std::string & receiveTopic, const std::string & sendTopic)
	throw(CMSException);
	~GenericTestComponent();

protected:
	virtual void act() throw(CMSException);
	virtual void react(SEMAINEMessage * message) throw(CMSException);

private:
	int boredTime;
	long long lastMessageTime;

};


} // namespace dummy
} // namespace components
} // namespace semaine



#endif

