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

// PoseEditorFLTKWindow.h: interface for the PoseEditorFLTKWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PoseEditorFLTKWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_PoseEditorFLTKWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Choice.H>
#include <fl/fl_counter.h>
#include <fl/fl_value_output.h>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.H>
#include "PlayerFLTKGLWindow.h"

#include <windows.h>
#include "dibapi.h"

extern "C" void AviInit(char *avifilename, int xdim, int ydim);
extern "C" int AddFrame(HANDLE hDib);
extern "C" int AddWAV(char *strFileWAV);
extern "C" void AviClose(void);

class PoseEditorFLTKWindow : public Fl_Window  
{
public:
	int handle(int event);
	PoseEditorFLTKWindow();
	virtual ~PoseEditorFLTKWindow();
//	PlayerFLTKMenu *menu;
	PlayerFLTKGLWindow *glutw;
	void draw();

	Fl_Slider *rotatex;
	Fl_Slider *rotatey;
	Fl_Slider *rotatez;

	Fl_Input *numrotatex;
	Fl_Input *numrotatey;
	Fl_Input *numrotatez;

	std::string jointnames_right[60];
	std::string jointnames_left[60];
	std::string jointnames_spine[60];

	Fl_Choice *jointname_right;
	Fl_Choice *jointname_left;
	Fl_Choice *jointname_spine;

	bool modifyingright,modifyingleft,modifyingspine;

	Load();

	Save();

private:
	int position_y;
	int position_x;
	int height;
	int width;
	
	Fl_Box *facebox;

	Fl_Button *load;
	Fl_Button *save;

	bool quietversion;

	//HANDLE PoseEditorFLTKWindow::DIBCaptureRegion();
};

#endif // !defined(AFX_PoseEditorFLTKWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
