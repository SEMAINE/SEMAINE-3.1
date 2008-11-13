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

// ClockPsyduleWindow.h: interface for the ClockPsyduleWindow class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>

#include "ClockPsydule.h"
#include "../../../include/common/definitions.h"

class ClockPsyduleWindow : public Fl_Double_Window  
{
public:
	ClockPsyduleWindow(std::string argv);
	std::string ini_filename;
	virtual ~ClockPsyduleWindow();
	int handle(int e);
	void draw();
	void UpdateTime(int t);
	Fl_Output *clockoutput;
	Fl_Button *reset;
	ClockPsydule *clp;
};
