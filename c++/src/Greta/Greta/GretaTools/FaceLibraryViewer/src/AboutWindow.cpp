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

// AboutWindow.cpp: implementation of the AboutWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "AboutWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void selectedbutton(Fl_Widget*w, void* v)
{
	// printf("scelto %d\n",((AgentScoreWindow*)v)->classes->value());
	((AboutWindow*)v)->hide();
	delete ((AboutWindow*)v);
}

AboutWindow::AboutWindow():Fl_Window(100,200,300,200,"agent score 0.1")
{
	//char *val;

	//strcpy(val, "Implemented by Maurizio Mancini and Elisabetta Bevacqua\nPlease, contact us for each problem\nm.mancini@iut.univ-paris8.fr e.bevacqua@iut.univ-paris8.fr");
	Info=new Fl_Multiline_Output(25,30, 250, 120, "");
	Info->box(FL_NO_BOX);
//	Info->s
	Info->value("Implemented by\nMaurizio Mancini & Elisabetta Bevacqua\n\nPlease, contact us for any problem:\nm.mancini@iut.univ-paris8.fr\ne.bevacqua@iut.univ-paris8.fr");
	button=new Fl_Button(120,150,50,20,"Close");
	button->callback((Fl_Callback *)selectedbutton,this);
}

AboutWindow::~AboutWindow()
{
	delete button;
}
