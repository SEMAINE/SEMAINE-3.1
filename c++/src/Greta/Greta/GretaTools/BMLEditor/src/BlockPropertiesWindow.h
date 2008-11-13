//Copyright 1999-2005 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// BlockPropertiesWindow.h: interface for the BlockPropertiesWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BlockPropertiesWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_BlockPropertiesWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.H>
#include <fl/fl_counter.h>
#include <fl/fl_value_output.h>
#include <vector>

class BlockPropertiesWindow : public Fl_Window  
{
public:

	BlockPropertiesWindow(void *window,void* block);
	virtual ~BlockPropertiesWindow();
	void BlockPropertiesWindow::LoadClassInstance();
	int BlockPropertiesWindow::SetStartEnd(std::string start,std::string end);
	float BlockPropertiesWindow::FindTMValue(std::string timemarker);
	int handle(int e);
	bool finished;
	void draw();

	Fl_Button *okbutton;
	Fl_Button *cancelbutton;
	Fl_Button *deletebutton;
	Fl_Input *name;
	Fl_Choice *classname;
	Fl_Choice *instancename;
	Fl_Input *start;
	Fl_Input *end;
	Fl_Choice *starttm;
	Fl_Choice *endtm;

	Fl_Slider *SPC;
	Fl_Slider *TMP;
	Fl_Slider *FLD;
	Fl_Slider *PWR;
	Fl_Slider *REP;
	
	void *w;
	void *b;
	std::string message;

private:
	
	void Text(char *s,int x,int y);
	
};

#endif // !defined(AFX_BlockPropertiesWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
