/*
 *  SEMAINEMessageAvailableListener.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_SEMAINEMESSAGEAVAILABLELISTENER_H
#define SEMAINE_CMS_SEMAINEMESSAGEAVAILABLELISTENER_H

namespace semaine {
namespace cms {
namespace receiver {
class Receiver;
}}}

namespace semaine {
namespace cms {

class SEMAINEMessageAvailableListener
{
    public:
	/**
	 * This method is called when a SEMAINE message is received by a receiver,
	 * from the receiver's thread. The method is expected to cause the listener
	 * to call (in this or another thread) the Receiver's {@link Receiver#getMessage()}
	 * method. The receiver will block until getMessage() has been called.
	 * 
	 * The listener can decide whether to get the message immediately or
	 * to put the receiver in a queue until the listener is ready to process.
	 * For slow components, the second option avoids piling up messages;
	 * instead, they may be discarded by the JMS server if their time to live
	 * is exceeded.
	 * @param receiver the receiver that has a message available.
	 */
	virtual void messageAvailableFrom(semaine::cms::receiver::Receiver * const receiver) = 0;
};







} // namespace cms
} // namespace semaine

#endif