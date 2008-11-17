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
#include <semaine/system/ComponentRunner.h>
#include <semaine/util/XMLTool.h>

using namespace XERCES_CPP_NAMESPACE;
using namespace semaine::util;

void testCMSLogger()
{
	semaine::cms::CMSLogger * log = semaine::cms::CMSLogger::getLog("cmsmain");
	log->info("Hello world");
}

void testReceiver()
{
		semaine::cms::receiver::Receiver * r = new semaine::cms::receiver::Receiver("testtopic");
		r->startConnection();
		std::cout << "Connected to " << r->getCMSUrl() << std::endl;
		std::cout << "Reading messages from topic " << r->getTopicName() << std::endl;
		while (true) {
			SEMAINEMessage * m = r->receive();
			std::cout << "usertime: " << m->getUsertime() << " -- message: " << m->getText() << std::endl;
			delete m;
		}
}

void testSender()
{
    semaine::cms::CMSLogger * log = semaine::cms::CMSLogger::getLog("cmsmain");
	log->info("will try to send something");
	semaine::cms::sender::Sender * s = new semaine::cms::sender::Sender("testtopic", "test data", "command line");
	s->startConnection();
	std::cout << "Connected to " << s->getCMSUrl() << std::endl;
	std::cout << "Sending messages to topic " << s->getTopicName() << std::endl;
	char line[1024];
	while (true) {
		std::cin.getline(line,1024);
		try {
			s->sendTextMessage(std::string(line), decaf::lang::System::currentTimeMillis());
		} catch (CMSException & e) {
		    log->error("cannot send message", &e);
		}
	}
}


void testXML()
{
	std::string myXML = std::string("<semaine:text xmlns:semaine=\"http://www.semaine-project.eu\">this is what the user said</semaine:text>");
	
	DOMDocument * document = XMLTool::parse(myXML);
	DOMElement * root = document->getDocumentElement();
	char * tagName = XMLString::transcode(root->getTagName());
	char * nameSpace = XMLString::transcode(root->getNamespaceURI());
	char * textContent = XMLString::transcode(root->getTextContent());
	std::cout << "XML root tag name: " << tagName << ", namespace: " << nameSpace << std::endl;
	std::cout << "text content: " << textContent << std::endl;
	XMLString::release(&tagName);
	XMLString::release(&nameSpace);
	XMLString::release(&textContent);

	std::cout << "serialising:" << std::endl;
	std::cout << XMLTool::dom2string(document) << std::endl;
	
	document->release();
}


int main () {
	try {
		semaine::util::XMLTool::startupXMLTools();
		testXML();
		testCMSLogger();
		testReceiver();
		testSender();
		semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
