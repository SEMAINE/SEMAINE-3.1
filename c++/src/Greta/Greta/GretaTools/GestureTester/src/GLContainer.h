#pragma once

#include <string>

#include <FL/Fl_Window.H>

class GLContainer : public Fl_Window  
{
public:
	GLContainer(void);
	~GLContainer(void);
	void *glwindow;
	void Play(std::string s);
	void draw();
};
