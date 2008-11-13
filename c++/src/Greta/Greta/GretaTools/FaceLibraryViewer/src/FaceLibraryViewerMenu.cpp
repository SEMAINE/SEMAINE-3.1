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

// FaceLibraryViewerMenu.cpp: implementation of the FaceLibraryViewerMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceLibraryViewerMenu.h"
#include "FileMenu.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FaceLibraryViewerMenu::FaceLibraryViewerMenu(int x,int y,int w,int h):Fl_Menu_Bar(x,y,w,h,"")
{
	/*
	Fl_Menu_Item popup[] = {
	{"&alpha", 0, 0, 0, 0},
	{"&beta", 0, 0, 0, 0},
	{"gamma", 0, 0, 0, FL_MENU_DIVIDER},
	{"&strange", 0, 0, 0, 0},
	{"&charm", 0, 0, 0, 0},
	{"&truth", 0, 0, 0, 0},
	{"b&eauty", 0, 0, 0, 0},
	{"sub&menu", 0, 0, 0, FL_SUBMENU},
	{"one", 0, 0, 0, 0},
	{"two", 0, 0, 0, 0},
	{"three", 0, 0, 0, 0},
	{0},
	{"inactive", 0, 0, 0, FL_MENU_INACTIVE|FL_MENU_DIVIDER},
	{"invisible",0, 0, 0, FL_MENU_INVISIBLE},
	{"check", 0, 0, 0, FL_MENU_TOGGLE|FL_MENU_VALUE},
	{"box", 0, 0, 0, FL_MENU_TOGGLE},
	{0}};
	*/

	
}

FaceLibraryViewerMenu::~FaceLibraryViewerMenu()
{

}


//DEL int FaceLibraryViewerMenu::handle(int event)
//DEL {
//DEL  
//DEL 	return Fl_Menu_Bar::handle(event);
//DEL }
