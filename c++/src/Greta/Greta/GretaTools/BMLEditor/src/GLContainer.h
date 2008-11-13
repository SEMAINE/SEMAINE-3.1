#pragma once

#include <string>

#include <FL/Fl_Window.H>

class GLContainer : public Fl_Window  
{
public:
	GLContainer(int x,int y,int w,int h, int loadlistener);
	~GLContainer(void);
	void *glwindow;
	void Play(std::string s);
	void draw();
	int listener;
};
