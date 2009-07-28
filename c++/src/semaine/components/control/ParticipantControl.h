/*
 * ParticipantControl.h
 *
 *  Created on: 28.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_COMPONENTS_CONTROL_PARTICIPANTCONTROL_H
#define SEMAINE_COMPONENTS_CONTROL_PARTICIPANTCONTROL_H

#include <semaine/components/Component.h>
#include <semaine/cms/sender/StateSender.h>
#include <semaine/cms/receiver/StateReceiver.h>
#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <cms/CMSException.h>

namespace semaine {

namespace components {

namespace control {

class ParticipantControl: public semaine::components::Component {
public:
	ParticipantControl() throw(CMSException);
	virtual ~ParticipantControl();
	void sendContextInfo(const std::string & userPresent, const std::string & character);
protected:
	void react(SEMAINEMessage * message) throw (std::exception);
	void customStartIO() throw(std::exception);
private:
	semaine::cms::sender::StateSender * contextSender;
	semaine::cms::receiver::StateReceiver * contextReceiver;
};

}

}

}

#endif /* SEMAINE_COMPONENTS_CONTROL_PARTICIPANTCONTROL_H */
