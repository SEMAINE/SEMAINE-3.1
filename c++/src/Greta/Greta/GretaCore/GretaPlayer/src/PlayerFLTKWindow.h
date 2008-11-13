//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// PlayerFLTKWindow.h: interface for the PlayerFLTKWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PlayerFLTKWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_PlayerFLTKWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

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
#include "PlayerFLTKMenu.h"
#include "PlayerFLTKGLWindow.h"
#include "PlayerFLTKGLWindowView.h"
#include "IniManager.h"


#include <windows.h>
#include "dibapi.h"


/**
* this method 
* 
*
* @param avifilename
* @param  int xdim
* @param  int ydim
*/

extern "C" void AviInit(char *avifilename, int xdim, int ydim);

/**
* this method 
* 
*
* @return 
* @param  hDib
*/

extern "C" int AddFrame(HANDLE hDib);

/**
* this method 
* 
*
* @return 
* @param strFileWAV
*/

extern "C" int AddWAV(char *strFileWAV);

/**
* this method 
* 
*
* @return 
*/

extern "C" void AviClose(void);

/**
*
* class :PlayerFLTKWindow
*
*/

class PlayerFLTKWindow : public Fl_Window  
{
public:

	/**
	* this method 
	* 
	*
	* @return 
	* @param  event
	*/

	int handle(int event);

	/**
	*
	* contructor 
	*
	*/

	PlayerFLTKWindow();

	/**
	* this method 
	* 
	*
	* @return 
	* @param  filename
	*/

	PlayerFLTKWindow(std::string filename);


	/**
	*
	* destructor 
	*/

	virtual ~PlayerFLTKWindow();
	PlayerFLTKMenu *menu;
	PlayerFLTKGLWindow *glw;
	PlayerFLTKGLWindowView *glw2;

	/**
	* this method 
	* 
	*
	* @param  q
	*/

	void SetQuiet(bool q);

	/**
	* this method 
	* 
	*
	* @param  filebasename
	*/

	void LoadFile(std::string filebasename);

	/**
	* this method 
	* 
	*
	*/

	void draw();


	/**
	* this method 
	* 
	*
	*/

	void PlayerFLTKWindow::Capture();

	/**
	* this method 
	* 
	*
	*/

	void PlayerFLTKWindow::EndCapture();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	HANDLE PlayerFLTKWindow::DIBCaptureRegion();

	bool capturing;

private:

	/**
	* this method 
	* 
	*
	*/

	void Initialization();
	int position_y;
	int position_x;
	int height;
	int width;
	Fl_Button *button_play;
	Fl_Button *button_stop;
	Fl_Button *button_frameback;
	Fl_Button *button_framenext;
	Fl_Button *button_rewind;
	Fl_Check_Button *button_hair;
	Fl_Check_Button *button_skin;
	Fl_Check_Button *button_ifapu;
	Fl_Check_Button *button_eyesteeths;
	Fl_Check_Button *button_skeleton;
	Fl_Box *facebox;
	Fl_Box *bodybox;
	Fl_Button *button_startcapture;
	Fl_Button *button_endcapture;
	Fl_Button *button_refreshfile;
	Fl_Output *framenumber;
	Fl_Output *framerate;
	Fl_Button *button_savecamera[3];
	Fl_Button *button_loadcamera[3];
	bool quietversion;
	bool isfullscreen;
	bool goingback;
	bool goingnext;


	//HANDLE PlayerFLTKWindow::DIBCaptureRegion();
};

#endif // !defined(AFX_PlayerFLTKWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
