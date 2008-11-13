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

// AboutWindow.h: interface for the AboutWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABOUTWINDOW_H__9087F899_9069_47C3_B9EF_9F27FEBDF39F__INCLUDED_)
#define AFX_ABOUTWINDOW_H__9087F899_9069_47C3_B9EF_9F27FEBDF39F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Multiline_Output.H>

class AboutWindow : public Fl_Window  
{
public:
	AboutWindow();
	virtual ~AboutWindow();
	Fl_Button *button;
	Fl_Multiline_Output *Info;
};

#endif // !defined(AFX_ABOUTWINDOW_H__9087F899_9069_47C3_B9EF_9F27FEBDF39F__INCLUDED_)
