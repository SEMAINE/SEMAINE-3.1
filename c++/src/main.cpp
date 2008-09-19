// exceptions
#include <iostream>
#include <list>

#include <semaine/config.h>

#include <decaf/lang/System.h>
#include <decaf/lang/Thread.h>

#include <semaine/cms/CMSLogger.h>
#include <semaine/cms/receiver/Receiver.h>
#include <semaine/cms/sender/Sender.h>
#include <semaine/components/Component.h>
#include <semaine/components/dummy/GenericTestComponent.h>
#include <semaine/components/dummy/DummyFeatureExtractor.h>
#include <semaine/system/ComponentRunner.h>

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


void genericTestComponents()
{
	std::list<semaine::components::Component *> comps;
	comps.push_back(new semaine::components::dummy::GenericTestComponent("Jim", "semaine.data.chat.oneway", "semaine.data.chat.theotherway"));
	comps.push_back(new semaine::components::dummy::GenericTestComponent("John", "semaine.data.chat.theotherway", "semaine.data.chat.oneway"));
	semaine::system::ComponentRunner cr(comps);
	cr.go();
	
}


void dummyFeatureExtractor()
{
	std::list<semaine::components::Component *> comps;
	comps.push_back(new semaine::components::dummy::DummyFeatureExtractor());
	semaine::system::ComponentRunner cr(comps);
	cr.go();

}


int main () {
	try {
		//testCMSLogger();
		//testReceiver();
		//testSender();
		genericTestComponents();
		//dummyFeatureExtractor();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
