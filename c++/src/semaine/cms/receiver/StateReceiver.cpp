/*
 * StateReceiver.cpp
 *
 *  Created on: 27.07.2009
 *      Author: marc
 */

#include "StateReceiver.h"

namespace semaine {

namespace cms {

namespace receiver {


void StateReceiver::onMessage(const Message * m)
{
	if (m != NULL) {
		try {
			message = createSEMAINEMessage(m);
			updateInformationState(message);
			// Notify component that new message has arrived
			if (listener != NULL) {
				listener->messageAvailableFrom(this);
				// Block until message has been picked up
				synchronized (&mutex) {
					while (notPickedUp()) {
						try {
							mutex.wait();
						} catch (lang::Exception & ie) {}
					}
				}
			}
		} catch (MessageFormatException & mfe) {
			log->warn("Problem creating SEMAINE message: ", &mfe);
		}
	}
}



void StateReceiver::updateInformationState(SEMAINEMessage *m)
{
	SEMAINEStateMessage * as = dynamic_cast<SEMAINEStateMessage *>(m);
	assert(as != NULL);
	std::map<std::string, std::string> info = as->getState()->getInfos();
	for (std::map<std::string, std::string>::iterator it = info.begin(); it != info.end(); ++it) {
		currentBestGuess[it->first] = currentBestGuess[it->second];
	}
}




}

}

}
