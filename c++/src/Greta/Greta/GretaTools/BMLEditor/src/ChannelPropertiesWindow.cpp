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

// ChannelPropertiesWindow.cpp: implementation of the ChannelPropertiesWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "ChannelPropertiesWindow.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/fl_widget.h>
#include <FL/fl_draw.H>

ChannelPropertiesWindow::ChannelPropertiesWindow():Fl_Window(10,20,700,500,"channel properties")
{

}

ChannelPropertiesWindow::~ChannelPropertiesWindow()
{

}

int ChannelPropertiesWindow::handle(int e)
{
	return Fl_Window::handle(e);
}

void ChannelPropertiesWindow::draw()
{
	Fl_Window::draw();
}
