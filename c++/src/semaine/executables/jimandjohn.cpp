// exceptions
#include <iostream>
#include <list>

#include <semaine/config.h>

#include <semaine/util/XMLTool.h>

#include <decaf/lang/System.h>
#include <decaf/lang/Thread.h>

#include <semaine/cms/CMSLogger.h>
#include <semaine/cms/receiver/Receiver.h>
#include <semaine/cms/sender/Sender.h>
#include <semaine/components/Component.h>
#include <semaine/components/dummy/GenericTestComponent.h>
#include <semaine/components/dummy/DummyFeatureExtractor.h>
#include <semaine/components/dummy/DummyFML2BML.h>
#include <semaine/system/ComponentRunner.h>



int main () {
	try {
		semaine::util::XMLTool::startupXMLTools();

		std::list<semaine::components::Component *> comps;
		comps.push_back(new semaine::components::dummy::GenericTestComponent("Jim", "semaine.data.chat.oneway", "semaine.data.chat.theotherway"));
		comps.push_back(new semaine::components::dummy::GenericTestComponent("John", "semaine.data.chat.theotherway", "semaine.data.chat.oneway"));
		semaine::system::ComponentRunner cr(comps);
		cr.go();

		semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
