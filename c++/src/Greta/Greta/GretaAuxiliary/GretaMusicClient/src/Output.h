#pragma once

#include <string>

#include <FL/Fl_Multiline_Output.H>

class Output : public Fl_Multiline_Output
{
public:
	Output(int x,int y,int w,int h);
	~Output(void);
	void printf(std::string);
private:
	std::string buffer;
};
