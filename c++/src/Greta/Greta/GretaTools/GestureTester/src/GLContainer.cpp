#include ".\glcontainer.h"
#include "PlayerFLTKGLWindow.h"
#include "IniManager.h"

extern IniManager inimanager;

void GLContainerWindowclosed(void* v)
{
}

GLContainer::GLContainer(void):Fl_Window(450,100,320,200,"feedback window")
{
	inimanager.SetValueString("CHARACTER_SPEAKER","greta");
	inimanager.SetValueInt("PLAYER_ONLYFACE",0);
	inimanager.SetValueInt("PLAYER_SHOWLISTENER",0);
	glwindow=new PlayerFLTKGLWindow(5,5,w()-10,h()-10,"test");
	this->callback((Fl_Callback *)GLContainerWindowclosed,this);
	this->size_range(320,200,1280,1024,1,1,0);
}

GLContainer::~GLContainer(void)
{	
	((PlayerFLTKGLWindow*)glwindow)->RemoveIdle();
	delete ((PlayerFLTKGLWindow*)glwindow);
}

void GLContainer::Play(std::string s)
{
	((PlayerFLTKGLWindow*)glwindow)->agent->EnableAudio(false);
	((PlayerFLTKGLWindow*)glwindow)->agent->AssignFile((char*)s.c_str());
	((PlayerFLTKGLWindow*)glwindow)->agent->StartTalking();
}

void GLContainer::draw()
{
	((PlayerFLTKGLWindow*)glwindow)->size(w()-10,h()-10);
	Fl_Window::draw();
}