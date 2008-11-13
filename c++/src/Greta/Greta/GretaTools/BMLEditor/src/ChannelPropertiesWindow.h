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

// ChannelPropertiesWindow.h: interface for the ChannelPropertiesWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ChannelPropertiesWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_ChannelPropertiesWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Choice.H>
#include <fl/fl_counter.h>
#include <fl/fl_value_output.h>
#include <vector>

class ChannelPropertiesWindow : public Fl_Window  
{
public:

	ChannelPropertiesWindow();
	virtual ~ChannelPropertiesWindow();
	int handle(int e);

private:

	void draw();
	
};

#endif // !defined(AFX_ChannelPropertiesWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
