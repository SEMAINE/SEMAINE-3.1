// exceptions
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <list>
#include <queue>

#include <semaine/config.h>

#include <semaine/util/XMLTool.h>
#include <semaine/components/Component.h>
#include <semaine/system/ComponentRunner.h>
#include <semaine/cms/sender/EmmaSender.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/SemaineML.h>

#include "MouseHook.h"


#pragma comment(lib, "MouseHook")
#pragma comment(lib, "user32")

extern "C" void sethook(void (* callback)(MouseHook::MouseHookEvent));
extern "C" void unhook();


std::queue<MouseHook::MouseHookEvent> mouseClicks;

void __cdecl registerMouseClick(MouseHook::MouseHookEvent mhe) {
	//std::cout << "Mouse click received: " << mhe << std::endl;
	mouseClicks.push(mhe);
}


DWORD CALLBACK MouseShovelThread(LPVOID pVoid)
{
	sethook(&registerMouseClick);
	MSG msg;
	while (0 < GetMessage(&msg, NULL, 0, 0))
		DispatchMessage(&msg);

	unhook();

	//std::cout << "quitting now..." << std::endl;
	return 0;
}






using namespace semaine::util;
using namespace semaine::datatypes::xml;

namespace ButtonTracker {

class ButtonTracker : public semaine::components::Component
{
public:
	ButtonTracker() throw (cms::CMSException);
	virtual ~ButtonTracker();

protected:
	virtual void act() throw (cms::CMSException);
	virtual void customStartIO() throw(std::exception);
	virtual void customStopIO() throw(std::exception);

private:
	semaine::cms::sender::EmmaSender * emmaSender;
	DWORD threadId;
	bool listenToLeftButton;
	bool listenToMidButton;
	bool listenToRightButton;
	void sendMouseEvent(MouseHook::MouseHookEvent mhe) throw(cms::CMSException);

};



ButtonTracker::ButtonTracker() throw(cms::CMSException) :
	Component("ButtonTracker"),
		listenToLeftButton(true),
		listenToMidButton(true),
		listenToRightButton(false)
{
	emmaSender = new semaine::cms::sender::EmmaSender("semaine.data.context.button", getName());
	senders.push_back(emmaSender);
	waitingTime = 10; // call act() every 10 ms
}

ButtonTracker::~ButtonTracker()
{
	delete emmaSender;
}

void ButtonTracker::customStartIO() throw(std::exception)
{
	CloseHandle(CreateThread(NULL, 0, MouseShovelThread, NULL, 0, &threadId));
}

void ButtonTracker::customStopIO() throw(std::exception)
{
	PostThreadMessage(threadId, WM_QUIT, 0, 0);
}

void ButtonTracker::act() throw(cms::CMSException)
{
	while (!mouseClicks.empty()) {
		MouseHook::MouseHookEvent mhe = mouseClicks.front();
		mouseClicks.pop();
		sendMouseEvent(mhe);
	}
}

void ButtonTracker::sendMouseEvent(MouseHook::MouseHookEvent mhe) {
	std::string activity = "";
	switch (mhe) {
		case MouseHook::LEFT_DOWN:
			if (!listenToLeftButton) {
				return;
			}
			activity = "left-down";
			break;
		case MouseHook::LEFT_UP:
			if (!listenToLeftButton) {
				return;
			}
			activity = "left-up";
			break;
		case MouseHook::MID_DOWN:
			if (!listenToMidButton) {
				return;
			}
			activity = "mid-down";
			break;
		case MouseHook::MID_UP:
			if (!listenToMidButton) {
				return;
			}
			activity = "mid-up";
			break;
		case MouseHook::RIGHT_DOWN:
			if (!listenToRightButton) {
				return;
			}
			activity = "right-down";
			break;
		case MouseHook::RIGHT_UP:
			if (!listenToRightButton) {
				return;
			}
			activity = "right-up";
			break;
		default:
			return;
	}

	long long usertime = meta.getTime();
	std::stringstream buf;
	buf << usertime;
	std::string usertimeString = buf.str();


	// Create and fill a simple EMMA document
	XERCES_CPP_NAMESPACE::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
	XERCES_CPP_NAMESPACE::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
	XERCES_CPP_NAMESPACE::DOMElement * behaviour = XMLTool::appendChildElement(interpretation, SemaineML::E_BEHAVIOUR, SemaineML::namespaceURI);
	XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, usertimeString);
	XMLTool::setAttribute(behaviour, SemaineML::A_NAME, activity);
	emmaSender->sendXML(document, usertime);
}

} // namespace ButtonTracker

int main () {
	try {
		semaine::util::XMLTool::startupXMLTools();

		std::list<semaine::components::Component *> comps;
		
		comps.push_back(new ButtonTracker::ButtonTracker());
		
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
