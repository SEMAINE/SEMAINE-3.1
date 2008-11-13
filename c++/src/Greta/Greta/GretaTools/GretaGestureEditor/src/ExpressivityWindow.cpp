#include ".\expressivitywindow.h"

#include <FL/fl_draw.H>

void ExpressivityWindowclosed(void* v)
{
}

ExpressivityWindow::ExpressivityWindow(void):Fl_Window(700,500,200,180,"expressivity")
{

	SPC=new Fl_Slider(20,10,15,150,"SPC");
	SPC->align(FL_ALIGN_BOTTOM);
	SPC->labelfont(FL_HELVETICA);
	SPC->labelsize(11);
	SPC->type(FL_VERT_NICE_SLIDER);
	SPC->bounds(1,-1);
	SPC->step(0.01);
	SPC->value(0.0);

	TMP=new Fl_Slider(50,10,15,150,"TMP");
	TMP->align(FL_ALIGN_BOTTOM);
	TMP->labelfont(FL_HELVETICA);
	TMP->labelsize(11);
	TMP->type(FL_VERT_NICE_SLIDER);
	TMP->bounds(1,-1);
	TMP->step(0.01);
	TMP->value(0.0);

	FLD=new Fl_Slider(80,10,15,150,"FLD");
	FLD->align(FL_ALIGN_BOTTOM);
	FLD->labelfont(FL_HELVETICA);
	FLD->labelsize(11);
	FLD->type(FL_VERT_NICE_SLIDER);
	FLD->bounds(1,-1);
	FLD->step(0.01);
	FLD->value(0.0);

	PWR=new Fl_Slider(110,10,15,150,"PWR");
	PWR->align(FL_ALIGN_BOTTOM);
	PWR->labelfont(FL_HELVETICA);
	PWR->labelsize(11);
	PWR->type(FL_VERT_NICE_SLIDER);
	PWR->bounds(1,-1);
	PWR->step(0.01);
	PWR->value(0.0);

	REP=new Fl_Slider(145,10,15,150,"REP");
	REP->align(FL_ALIGN_BOTTOM);
	REP->labelfont(FL_HELVETICA);
	REP->labelsize(11);
	REP->type(FL_VERT_NICE_SLIDER);
	REP->bounds(1,-1);
	REP->step(1.0);
	REP->value(-1.0);

	this->callback((Fl_Callback *)ExpressivityWindowclosed,this);
}

ExpressivityWindow::~ExpressivityWindow(void)
{
}

void ExpressivityWindow::draw()
{
	Fl_Window::draw();
	fl_font(FL_HELVETICA,11);
	fl_color(0,0,0);
	fl_draw("2 rep.",165,20);
	fl_draw("1 rep.",165,90);
	fl_draw("no rep.",165,160);
}