#include <activemq/library/ActiveMQCPP.h>

// exceptions
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <list>
#include <queue>

// Whoever defines these, decaf::lang::Thread doesn't like them.
#undef MIN_PRIORITY
#undef MAX_PRIORITY

#include <semaine/config.h>

#include <semaine/util/XMLTool.h>
#include <semaine/components/Component.h>
#include <semaine/system/ComponentRunner.h>
#include <semaine/cms/sender/StateSender.h>
#include <semaine/datatypes/stateinfo/StateInfo.h>
#include <semaine/datatypes/stateinfo/UserStateInfo.h>


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
	semaine::cms::sender::StateSender * stateSender;
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
	stateSender = new semaine::cms::sender::StateSender("semaine.data.state.user.behaviour",
		semaine::datatypes::stateinfo::StateInfo::UserState, getName());
	senders.push_back(stateSender);
	waitingTime = 10; // call act() every 10 ms
}

ButtonTracker::~ButtonTracker()
{
	delete stateSender;
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

	std::map<std::string, std::string> info;
	info["buttonEvent"] = activity;
	info["buttonEventTime"] = usertimeString;

	semaine::datatypes::stateinfo::UserStateInfo si(info);

	stateSender->sendStateInfo(&si, usertime);
}

} // namespace ButtonTracker

int main () {
	try {
		activemq::library::ActiveMQCPP::initializeLibrary();
		semaine::util::XMLTool::startupXMLTools();

		std::list<semaine::components::Component *> comps;
		
		comps.push_back(new ButtonTracker::ButtonTracker());
		
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
