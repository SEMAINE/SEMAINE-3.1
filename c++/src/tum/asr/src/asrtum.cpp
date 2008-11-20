// exceptions
#include <iostream>
#include <list>

#include <semaine/config.h>

#include <semaine/util/XMLTool.h>
#include <semaine/components/Component.h>
#include <semaine/system/ComponentRunner.h>

#include "ASR.h"

int main (int argc, char *argv[]) {
	try {
		semaine::util::XMLTool::startupXMLTools();

		char*configfile=NULL;
		if (argc > 1) configfile=argv[1];

		//std::list<semaine::components::Component *> comps;
		semaine::components::asr::ASR *asr = new semaine::components::asr::ASR(configfile);
		//comps.push_back(new semaine::components::asr::ASR());
		
		//semaine::system::ComponentRunner cr(comps);
		//cr.go();
		//cr.waitUntilCompleted();
		asr->run();

		semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
