/*
 *  MetaMessenger.h
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_COMPONENTS_META_METAMESSENGER_H
#define SEMAINE_COMPONENTS_META_METAMESSENGER_H

#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/Mutex.h>

#include <cms/MessageListener.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>

#include <semaine/cms/IOBase.h>

using namespace cms;
using namespace semaine::cms;

namespace semaine {
namespace components {
namespace meta {

class MetaMessenger : public IOBase, public MessageListener, public decaf::util::concurrent::Mutex
{
public:
	static const std::string COMPONENT_NAME;
	static const std::string COMPONENT_STATE;
	static const std::string SYSTEM_READY;

	MetaMessenger(const std::string & componentName) throw(CMSException);
	
	void reportState(const std::string & state) throw(CMSException);
	
	virtual void onMessage(const Message * m);

	bool isSystemReady();


private:
	const std::string componentName;
	MessageProducer * producer;
	MessageConsumer * consumer;
	bool systemReady;

	void setSystemReady(bool ready);	


};


} // namespace meta
} // namespace components
} // namespace semaine



#endif

