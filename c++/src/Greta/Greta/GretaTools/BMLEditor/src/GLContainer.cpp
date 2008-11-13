#include ".\glcontainer.h"
#include "PlayerFLTKGLWindow.h"
#include "IniManager.h"

extern IniManager inimanager;


void GLContainerWindowclosed(void* v)
{
	((GLContainer*)v)->hide();
	//delete ((GLContainer*)v);
}

GLContainer::GLContainer(int x,int y,int w,int h, int loadlistener):Fl_Window(x,y,w,h,"feedback window")
{
	this->size_range(w,h,800,900,1,1,0);

	listener=loadlistener;
	inimanager.SetValueString("CHARACTER_SPEAKER","greta");
	inimanager.SetValueInt("PLAYER_ONLYFACE",0);
	if(listener==1)
		inimanager.SetValueInt("PLAYER_SHOWLISTENER",1);
	else
		inimanager.SetValueInt("PLAYER_SHOWLISTENER",0);
	glwindow=new PlayerFLTKGLWindow(5,5,w-10,h-10,"test");
	this->callback((Fl_Callback *)GLContainerWindowclosed,this);
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