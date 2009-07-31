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

int main () {
	try {
		semaine::util::XMLTool::startupXMLTools();

		std::list<semaine::components::Component *> comps;

		comps.push_back(new semaine::components::control::ParticipantControl());

		semaine::system::ComponentRunner cr(comps);
		cr.go();
		cr.waitUntilCompleted();

		semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
