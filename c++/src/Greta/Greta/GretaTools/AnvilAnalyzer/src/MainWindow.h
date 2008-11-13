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

// MainWindow.h: interface for the MainWindow class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "AnvilManager.h"
#include "AnvilManager2.h"
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Round_Button.H>
#include <string>

class MainWindow : public Fl_Window  
{
public:

	MainWindow();
	virtual ~MainWindow();
	int handle(int e);

private:
	void draw();
	Fl_Round_Button *converterMau;
	Fl_Round_Button *converterEli;
	Fl_Button *loadbutton;
	Fl_Button *savebmlbutton;
	std::string anvilfilename;
	AnvilManager *anvilmanager;
	AnvilManager2 *anvilmanager2;
};
