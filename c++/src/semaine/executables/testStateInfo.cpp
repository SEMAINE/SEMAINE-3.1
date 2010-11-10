// exceptions
#include <iostream>
#include <list>
#include <map>

#include <semaine/datatypes/stateinfo/AgentStateInfo.h>
#include <semaine/datatypes/stateinfo/UserStateInfo.h>
#include <semaine/datatypes/stateinfo/DialogStateInfo.h>
#include <semaine/datatypes/stateinfo/ContextStateInfo.h>
#include <semaine/datatypes/stateinfo/SystemStateInfo.h>


#include <xercesc/dom/DOM.hpp>

#include <semaine/util/XMLTool.h>

#include <xqilla/xqilla-dom3.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>

#include <activemq/library/ActiveMQCPP.h>

using namespace XERCES_CPP_NAMESPACE;
using namespace std;


int main (int argc, char *argv[]) {
	try {

		activemq::library::ActiveMQCPP::initializeLibrary();
		semaine::util::XMLTool::startupXMLTools();
/*
		// From info map to document:
		std::map<std::string, std::string> someInfo;
		someInfo["needToSpeak"] = "45";
		someInfo["listenerRefusal"] = "0.0123";
		someInfo["emotion-quadrant"] = "negativeActive";
		semaine::datatypes::stateinfo::AgentStateInfo asi(someInfo);

		std::cout << "Document: " << std::endl;
		std::cout << semaine::util::XMLTool::dom2string(asi.getDocument());
*/

		// From document to info map:
		std::string filename = "delme.xml";
		std::cout << "Now reading in document from " << filename << std::endl;
		XERCES_CPP_NAMESPACE::DOMDocument * doc = semaine::util::XMLTool::parseFile(filename);
		std::cout << "Parsed file: " << std::endl << semaine::util::XMLTool::dom2string(doc) << std::endl;

		semaine::datatypes::stateinfo::AgentStateInfo asi2(doc);
		std::string infos[] = {"needToSpeak", "listenerRefusal", "emotion-quadrant"};
		int n = 3;
		for (int i=0; i<n; i++) {
			std::string inf = infos[i];
			const std::string val = asi2.getInfo(inf);
			std::cout << "For info '" << inf << "', value is '" << val << "'" << std::endl;
		}

		semaine::util::XMLTool::shutdownXMLTools();
	    activemq::library::ActiveMQCPP::shutdownLibrary();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (XQillaException & xqe) {
		std::cerr << "XQillaException: " << UTF8(xqe.getString()) << std::endl;
	}
}
