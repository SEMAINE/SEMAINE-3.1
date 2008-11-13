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

// FaceLibraryViewerGlutWindow.cpp: implementation of the FaceLibraryViewerGlutWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceLibraryViewerGlutWindow.h"
#include <FL/glut.H>
#include <FL/Fl.H>
#include <GL/glu.h>	// Header File For The GLu32 Library
#include "IniManager.h"


extern IniManager inimanager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FaceLibraryViewerGlutWindow::FaceLibraryViewerGlutWindow(int x, int y, int w, int h, const char *label):Fl_Gl_Window(x,y,w,h,label)
{
	mode(FL_DOUBLE|FL_RGB|FL_DEPTH);
	width=w;
	height=h;

	light_ambient_0[0] = 0.2;
	light_ambient_0[1] = 0.2;
	light_ambient_0[2] = 0.2;
	light_ambient_0[3] = 1.0;//{0.2, 0.2, 0.2, 1.0}; //{0.15, 0.15, 0.15, 1.0};  //0.4
	light_diffuse_0[0] = 0.7;
	light_diffuse_0[1] = 0.7;
	light_diffuse_0[2] = 0.7;
	light_diffuse_0[3] = 1.0;//{0.90, 0.80, 0.80, 1.0};//{0.70, 0.60, 0.60, 1.0};//{0.80, 0.80, 0.80, 1.0};
	light_specular_0[0] = 1.0;
	light_specular_0[1] = 1.0;
	light_specular_0[2] = 1.0;
	light_specular_0[3] = 1.0;
	light_position_0[0] = -20.0;
	light_position_0[1] = 60.1;
	light_position_0[2] = 50.2;
	light_position_0[3] = 1.0;

	camerapanx=0;
	camerapany=-92;
	camerapanz=-53;
	camerarotx=0;
	cameraroty=0;
	camerarotz=0;

	agent=0;

}

FaceLibraryViewerGlutWindow::~FaceLibraryViewerGlutWindow()
{

}

void FaceLibraryViewerGlutWindow::draw()
{

	if(agent==0)
	{
		inimanager.SetValueInt("PLAYER_ONLYFACE",1);
		agent=new AgentAspect(inimanager.GetValueString("CHARACTER_SPEAKER"),inimanager.GetValueString("CHARACTER_SPEAKER"));
		agent->AssignFile("empty");

		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular_0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);
	}
glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(35.0f,(GLfloat)this->w()/(GLfloat)this->h(),1.0, 1500.0);


	if (!valid())
	{
		glViewport(0,0,this->w(),this->h());

		glEnable (GL_LIGHTING);
		glEnable (GL_LIGHT0);

		glEnable(GL_NORMALIZE);

		glEnable(GL_DEPTH_TEST);
	}

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	//printf("camera: pan %f %f %f  rot %f %f %f\r",camerapan.x,camerapan.y,camerapan.z,camerarot.x,camerarot.y,camerarot.z);

	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();

	glTranslatef(camerapanx,0,camerapanz);

	glRotatef(camerarotx,1,0,0);	
	glRotatef(cameraroty,0,1,0);

	glTranslatef(-camerapanx,0,-camerapanz);
		
	glTranslatef(camerapanx,camerapany,camerapanz);

	//glTranslatef(camerapanx,camerapany,camerapanz);

	agent->draw();

}


int FaceLibraryViewerGlutWindow::handle(int event)
{
	int deltax,deltay;
	deltax=0;
	deltay=0;
	if(event==FL_PUSH)
	{
		mousepressedx=Fl::event_x();
		mousepressedy=Fl::event_y();
		return 1;
	}
	if(event==FL_DRAG)
	{
		deltax=(mousepressedx-Fl::event_x())*0.7f;
		deltay=(mousepressedy-Fl::event_y())/2.0f;
		mousepressedx=Fl::event_x();
		mousepressedy=Fl::event_y(); 
	}
	if((event==FL_DRAG)&&(Fl::event_button()==FL_RIGHT_MOUSE))
	{
		camerapanz=camerapanz+deltay*cos(cameraroty/360.0f*6.28f);
		return 1;
	}
	if((event==FL_DRAG)&&(Fl::event_button()==FL_LEFT_MOUSE))
	{
		cameraroty=cameraroty-deltax;
		camerarotx=camerarotx-deltay;
	}
	if((event==FL_DRAG)&&(Fl::event_button()==FL_MIDDLE_MOUSE))
	{
		camerapanx=camerapanx-deltax*cos(cameraroty/360.0f*6.28f);
		//camerapanz=camerapanz+deltax*sin(cameraroty/360.0f*6.28f);
		camerapany=camerapany+deltay;
	}
	return Fl_Gl_Window::handle(event);
}

