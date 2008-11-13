#include ".\output.h"

Output::Output(int x,int y,int w,int h): Fl_Multiline_Output(x,y,w,h)
{
	buffer="";
}

Output::~Output(void)
{
}

void Output::printf(std::string s)
{
	buffer=buffer+s;
	this->value(buffer.c_str());
}