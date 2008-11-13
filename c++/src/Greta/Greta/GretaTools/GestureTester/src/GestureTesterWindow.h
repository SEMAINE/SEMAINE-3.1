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

// GestureTesterWindow.h: interface for the GestureTesterWindow class.
//
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.h>
#include <fl/fl_counter.h>
#include <fl/fl_value_output.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Choice.h>
#include <FL/Fl_Round_Button.h>
#include <vector>
#include "gestures/Gesture.h"
#include "GLContainer.h"
#include "ExpressivityWindow.h"

using namespace GestureSpace;

class GestureTesterWindow : public Fl_Window  
{
public:

	Fl_Button *testgesture;
	Fl_Button *quit;
	
	Fl_Choice *gesturename;
	Fl_Choice *expressionname;
	Fl_Input *expressionfilename;

	GLContainer *agentwindow;

	ExpressivityWindow *expressivitywindow;

	GestureTesterWindow();
	virtual ~GestureTesterWindow();
	int handle(int e);

	void draw();

	void ShowActualFrame();

	void Test();

	Gesture *gesture;

	int actualframe;

	BAPFrame *lastbf;
};
