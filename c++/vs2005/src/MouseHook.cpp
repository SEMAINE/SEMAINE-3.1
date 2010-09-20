#include <windows.h>
#include <iostream>

#include "MouseHook.h"

#pragma data_seg("HOOK")
HHOOK thishook = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:HOOK,RWS")
#pragma comment(lib, "user32")

HINSTANCE hInst = NULL;

void (* callbackFunction)(MouseHook::MouseHookEvent) = NULL;

//DWORD *processes[40];

LRESULT CALLBACK LowLevelMouseProc(int nCode,WPARAM wParam,LPARAM lParam);


extern "C" __declspec(dllexport)
void sethook(void (* callback)(MouseHook::MouseHookEvent))
{
	//std::cout << "hook received" << std::endl;
	thishook=SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInst, 0);
	callbackFunction = callback;
}

extern "C" __declspec(dllexport)
void unhook()
{
	UnhookWindowsHookEx(thishook);	
}


LRESULT CALLBACK LowLevelMouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	//std::cout << "mouse event received" << std::endl;
	MouseHook::MouseHookEvent mhe;
	switch (wParam) {
		case WM_LBUTTONDOWN:
			mhe = MouseHook::LEFT_DOWN;
			break;
		case WM_LBUTTONUP:
			mhe = MouseHook::LEFT_UP;
			break;
		case WM_MBUTTONDOWN:
			mhe = MouseHook::MID_DOWN;
			break;
		case WM_MBUTTONUP:
			mhe = MouseHook::MID_UP;
			break;
		case WM_RBUTTONDOWN:
			mhe = MouseHook::RIGHT_DOWN;
			break;
		case WM_RBUTTONUP:
			mhe = MouseHook::RIGHT_UP;
			break;
		default:
			mhe = MouseHook::NONE;
			break;
	}
	if (callbackFunction != NULL && mhe != MouseHook::NONE) {
		(*callbackFunction)(mhe);
	}
	return (::CallNextHookEx(thishook,nCode,wParam,lParam));

}

int WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hInstance;
		DisableThreadLibraryCalls(hInstance);
		break;
	}
	return 1;
}