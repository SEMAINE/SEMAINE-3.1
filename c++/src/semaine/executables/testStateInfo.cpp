// exceptions
#include <iostream>
#include <list>
#include <map>

#include <semaine/datatypes/stateinfo/AgentStateInfo.h>
#include <semaine/datatypes/stateinfo/UserStateInfo.h>
#include <semaine/datatypes/stateinfo/DialogStateInfo.h>
#include <semaine/datatypes/stateinfo/ContextStateInfo.h>
#include <semaine/datatypes/stateinfo/SystemStateInfo.h>

int main () {
	try {
		semaine::util::XMLTool::startupXMLTools();

		std::map<std::string, std::string> someInfo;
		someInfo["needToSpeak"] = "45";
		someInfo["listenerRefusal"] = "0.0123";
		someInfo["emotion-quadrant"] = "negativeActive";
		semaine::datatypes::stateinfo::AgentStateInfo asi(someInfo);

		std::cout << "Document: " << std::endl;
		std::cout << semaine::util::XMLTool::dom2string(asi.getDocument());

		std::string filename = "delme.xml";
		std::cout << "Now reading in document from " << filename << std::endl;
		XERCES_CPP_NAMESPACE::DOMDocument * doc = semaine::util::XMLTool::parseFile(filename);
		semaine::datatypes::stateinfo::AgentStateInfo asi2(doc);
		std::string infos[] = {"needToSpeak", "listenerRefusal"/*, "emotion-quadrant"*/};
		int n = 2;
		for (int i=0; i<n; i++) {
			std::string inf = infos[i];
			const std::string val = asi2.getInfo(inf);
			std::cout << "For info '" << inf << "', value is '" << val << "'" << std::endl;
		}


		semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	//} catch (semaine::cms::exceptions::SEMAINEException & se) {
	//	std::cerr << se.what();
	//} catch (std::exception & e) {
	//	std::cerr << e.what();
	}
}
