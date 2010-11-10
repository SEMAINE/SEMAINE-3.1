// exceptions
#include <iostream>
#include <list>
#include <exception>

#include <semaine/config.h>

#include <semaine/util/XMLTool.h>
#include <semaine/components/Component.h>
#include <semaine/system/ComponentRunner.h>
#include <cms/CMSException.h>
#include <semaine/components/control/ParticipantControl.h>

#include <activemq/library/ActiveMQCPP.h>

int main () {
	try {
		activemq::library::ActiveMQCPP::initializeLibrary();
		semaine::util::XMLTool::startupXMLTools();

		std::list<semaine::components::Component *> comps;

		comps.push_back(new semaine::components::control::ParticipantControl());

		semaine::system::ComponentRunner cr(comps);
		cr.go();
		cr.waitUntilCompleted();

		semaine::util::XMLTool::shutdownXMLTools();
	    activemq::library::ActiveMQCPP::shutdownLibrary();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
