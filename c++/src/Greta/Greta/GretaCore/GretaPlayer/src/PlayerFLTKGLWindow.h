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

// PlayerFLTKGLWindow.h: interface for the PlayerFLTKGLWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PlayerFLTKGLWindow_H__97B0F480_18A4_11D9_9C92_F2F8893FEB7F__INCLUDED_)
#define AFX_PlayerFLTKGLWindow_H__97B0F480_18A4_11D9_9C92_F2F8893FEB7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Output.H>
#include "DGLAVI.h"
#include "environment/World.h"
#include "PlayerFLTKGLWindowView.h"

#include "IniManager.h"


#include "AgentAspect.h"


/**
*
* class :PlayerFLTKGLWindow
*
*/

class PlayerFLTKGLWindow : public Fl_Gl_Window  
{
public:
	int height;
	int width;
	int rot;

	/**
	* this method 
	* 
	*
	* @return 
	*/

	void draw(void);


	/**
	* this method 
	* 
	*
	* @return 
	* @param  event
	*/

	int handle(int event);


	/**
	* this method 
	* 
	*
	*/

	void StartCapture();


	/**
	* this method 
	* 
	*
	*/

	void EndCapture();


	/**
	* this method 
	* 
	*
	*/

	void RemoveIdle();


	/**
	* this method 
	* 
	*
	*/

	void ViewCamera();


	/**
	* this method 
	* 
	*
	* @param  px
	* @param  py
	* @param  pz
	* @param  rx
	* @param  ry
	* @param  rz
	*/

	void MoveCamera(float px,float py,float pz,float rx,float ry,float rz);

	bool capturing;

	GL2AVI::CAVIRecord avirecorder;


	/**
	*
	* contructor 
	*
	* @param  x
	* @param  int y
	* @param  int w
	* @param  int h
	* @param label
	* @param  bool autoupdate=true
	* @param  bool realtime=false
	*/

	PlayerFLTKGLWindow(int x, int y, int w, int h, const char *label, bool autoupdate=true, bool realtime=false);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  x
	* @param  int y
	* @param  int w
	* @param  int h
	* @param label
	* @param  std::string filename
	* @param  bool autoupdate=true
	* @param  bool realtime=false
	*/

	PlayerFLTKGLWindow(int x, int y, int w, int h, const char *label, std::string filename, bool autoupdate=true, bool realtime=false);

	/**
	*
	* destructor 
	*/

	virtual ~PlayerFLTKGLWindow();
	AgentAspect * agent;
	AgentAspect * listener;
	float camerapanx;
	float camerapany;
	float camerapanz;
	float camerarotx;
	float cameraroty;
	float camerarotz;
	int mousepressedx,mousepressedy;


	GLfloat light_ambient_0[4];
	GLfloat light_diffuse_0[4];
	GLfloat light_specular_0[4];
	GLfloat light_position_0[4];

	std::string filetoload;

	Fl_Output *outputframenumber;
	Fl_Output *outputframerate;

	stringstream usefulstream;

	float previoustime,actualtime;


	/**
	* this method 
	* 
	*
	*/
	void SetOutputControl(Fl_Output*framenumber, Fl_Output*framerate);

	char usefulstring[20];

	int frameaccumulator;

	World *world;

	PlayerFLTKGLWindowView *glwview;

	bool listenerinsubwindow;


	/**
	* this method 
	* 
	*
	* @param  i
	*/

	void SetCameraView(int i);


	/**
	* this method 
	* 
	*
	* @param  i
	*/

	void SaveCameraSettings(int i);

private:

	bool realtime;


	/**
	* this method 
	* 
	*
	* @param  x
	* @param  int y
	* @param  int w
	* @param  int h
	* @param  bool autoupdate=true
	*/

	void Initialize(int x, int y, int w, int h, bool autoupdate=true);

	IniManager camerasettings;

};

#endif // !defined(AFX_PlayerFLTKGLWindow_H__97B0F480_18A4_11D9_9C92_F2F8893FEB7F__INCLUDED_)
