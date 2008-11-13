#pragma once

#include <FL/Fl_Window.H>
#include <FL/Fl_Slider.h>

class ExpressivityWindow : public Fl_Window  
{
public:
	ExpressivityWindow(void);
	~ExpressivityWindow(void);
	void draw();

	Fl_Slider *SPC;
	Fl_Slider *TMP;
	Fl_Slider *FLD;
	Fl_Slider *PWR;
	Fl_Slider *REP;
};
