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

#include <xercesc/framework/MemBufFormatTarget.hpp>

using namespace XERCES_CPP_NAMESPACE;

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
	
	XercesDOMParser* parser = new XercesDOMParser();
    //parser->setValidationScheme(XercesDOMParser::Val_Always);    
	parser->setDoNamespaces(true);
	const char * msgTextC = myXML.c_str();
	MemBufInputSource* memIS = new MemBufInputSource((const XMLByte *)msgTextC, strlen(msgTextC), "test", false);
    parser->parse(*memIS);
	DOMDocument * document = parser->getDocument();
	DOMElement * root = document->getDocumentElement();
	char * tagName = XMLString::transcode(root->getTagName());
	char * nameSpace = XMLString::transcode(root->getNamespaceURI());
	char * textContent = XMLString::transcode(root->getTextContent());
	std::cout << "XML root tag name: " << tagName << ", namespace: " << nameSpace << std::endl;
	std::cout << "text content: " << textContent << std::endl;
	XMLString::release(&tagName);
	XMLString::release(&nameSpace);
	XMLString::release(&textContent);

	std::cout << "serialising using a DOMWriter:" << std::endl;
	XMLCh tempStr[100];
	XMLString::transcode("LS", tempStr, 99);
	DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
	DOMImplementationLS * implLS = dynamic_cast<DOMImplementationLS *>(impl);
	if (implLS == NULL) {
		throw new SystemConfigurationException(std::string("DOM impl is not a DOMImplementationLS, but a ")+typeid(*impl).name());
	}
	DOMWriter * writer = implLS->createDOMWriter();
	XMLString::transcode("UTF-8", tempStr, 99);
	writer->setEncoding(tempStr);
	MemBufFormatTarget * output = new MemBufFormatTarget();
	writer->writeNode(output, *document);
	const XMLByte * buf = output->getRawBuffer();
	//int len = output->getLen();
	std::string xmlString((const char *)buf);
	std::cout << xmlString << std::endl;
	
	writer->release();
	document->release();
}


int main () {
	try {
		semaine::util::XMLTool::startupXMLTools();
		testCMSLogger();
		testReceiver();
		testSender();
		testXML();
		semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
