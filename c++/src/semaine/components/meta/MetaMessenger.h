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

#include <semaine/config.h>

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
	static const std::string SYSTEM_READY_TIME;

	MetaMessenger(const std::string & componentName) throw(CMSException);
	
	void reportState(const std::string & state) throw(CMSException);
	
	virtual void onMessage(const Message * m);

	bool isSystemReady();
	
	/**
	 * Get the current time in common, normalised time space.
	 * Processes should use only this method for determining the time.
	 * The value is counting milliseconds since some arbitrary point
	 * in time; therefore, the absolute time is not informative,
	 * but differences of time values are. 
	 * @return
	 */
	long long getTime();


private:
	const std::string componentName;
	MessageProducer * producer;
	MessageConsumer * consumer;
	bool systemReady;
	long long timeDelta;

	void setSystemReady(bool ready);	
	void setTime(long long systemTime);

};


} // namespace meta
} // namespace components
} // namespace semaine



#endif

