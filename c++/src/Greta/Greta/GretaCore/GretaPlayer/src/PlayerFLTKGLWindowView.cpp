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

// PlayerFLTKGLWindowView.cpp: implementation of the PlayerFLTKGLWindowView class.
//
//////////////////////////////////////////////////////////////////////

#include "PlayerFLTKGLWindowView.h"
#include <FL/glut.H>
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <GL/glext.h>
#include <FL/Fl.H>
#include "IniManager.h"
#include "FL/Fl_draw.H"
#include <crtdbg.h>

extern IniManager inimanager;

void gretaplayerviewwindowidle(void* v)
{
    ((PlayerFLTKGLWindowView*)v)->redraw(); 
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PlayerFLTKGLWindowView::PlayerFLTKGLWindowView(int x, int y, int w, int h, const char *label):Fl_Gl_Window(x,y,w,h,label)
{


	mode(FL_DOUBLE|FL_RGB|FL_DEPTH);
	width=w;
	height=h;

	camerapanx=0;
	camerapany=-100;
	camerapanz=0;
	camerarotx=0;
	cameraroty=180;
	camerarotz=0;

//	agent=0;
//	listener=0;

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
	light_position_0[3] = 1.0; // lower the light for hilight effect on eyes

	filetoload="";

	Fl::add_idle(gretaplayerviewwindowidle,this);

	outputframenumber=0;
	outputframerate=0;

	capturing=false;

	previoustime=0;

	frameaccumulator=0;

	world=0;
}

PlayerFLTKGLWindowView::~PlayerFLTKGLWindowView()
{
	Fl::remove_idle(gretaplayerviewwindowidle);
	delete world;
	world=0;
//	delete agent;
//	agent=0;
}

void PlayerFLTKGLWindowView::RemoveIdle()
{
	Fl::remove_idle(gretaplayerviewwindowidle);
}

void PlayerFLTKGLWindowView::draw()
{
	
	if (world==0)
	{
		world= new World();
		//world->LoadWorld("environments/environment_for_gretaplayer.xml");
	}

	/*
	if(agent==0)
	{
		agent=new AgentAspect(inimanager.GetValueString("CHARACTER_SPEAKER"),inimanager.GetValueString("CHARACTER_SPEAKER"));
		
		//agent->AssignFile("empty");

		agent->EnableAudio(true);

		if(inimanager.GetValueInt("PLAYER_SHOWLISTENER")==1)
		{
			listener=new AgentAspect(inimanager.GetValueString("CHARACTER_LISTENER"),inimanager.GetValueString("CHARACTER_LISTENER"));
			//listener->AssignFile("empty");
			listener->EnableAudio(true);
			listener->MoveAgent(50,0,0);
			listener->RotateAgent(0,-90,0);
			listener->SetHairColor(0.6,0.5,0);
			//listener->SwitchHair();

			agent->RotateAgent(0,90,0);
		}

		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular_0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);
		
	}
	*/

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular_0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);
	
/*
	if(filetoload!="")
	{
		agent->AssignFile((char*)filetoload.c_str());
		filetoload="";
	}
*/
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(100.0f,(GLfloat)this->w()/(GLfloat)this->h(),1.0, 150000.0);


	if (!valid())
	{
		glViewport(0,0,this->w(),this->h());

		glEnable (GL_LIGHTING);
		glEnable (GL_LIGHT0);

		glEnable(GL_NORMALIZE);

		glEnable(GL_DEPTH_TEST);
	}

//	glClearColor(0.8,0.8,0.8,1.0);
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_COLOR_MATERIAL);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
	
	//printf("camera: pan %f %f %f  rot %f %f %f\r",camerapanx,camerapany,camerapanz,camerarotx,cameraroty,camerarotz);

	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();

	glTranslatef(camerapanx,0,camerapanz);

	glRotatef(camerarotx,1,0,0);	
	glRotatef(cameraroty,0,1,0);

	glTranslatef(-camerapanx,0,-camerapanz);
		
	glTranslatef(camerapanx,camerapany,camerapanz);

	//glTranslatef(camerapanx,camerapany,camerapanz);
	if(world!=0)
	{
		world->draw();
	}

//	agent->draw();
//	if(listener!=0)
//		listener->draw();
/*
	if(outputframenumber!=0)
	{
		if(agent->GetBaseFileName()=="empty")
			outputframenumber->value("no file");else
		{
			sprintf(usefulstring,"%d/%d (%.2f)",agent->GetCurrentFrame()+1,
				agent->GetTotalNumberOfFrames(),agent->GetCurrentFrame()*(1.0f/agent->GetFapFps()));
			outputframenumber->value(usefulstring);
		}
	}
*/	
	if(outputframerate!=0)
	{
		if(frameaccumulator==14)
		{
			actualtime=((float)clock());
			sprintf(usefulstring,"%.1f fps",(15000.0f/(actualtime-previoustime)));
			previoustime=actualtime;
			outputframerate->value(usefulstring);
			frameaccumulator=0;
		}
		else
			frameaccumulator++;
	}
/*	
	if(capturing==true)
	{
		if(strcmp(agent->GetStatus(),"ready")==0)
			this->EndCapture();
		else
			avirecorder.writeFrame();
	}
	//printf("agent is %s\n",agent->GetStatus());
*/	
}

int PlayerFLTKGLWindowView::handle(int event)
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
	if(((Fl::event_key()==FL_Alt_L)&&(event==FL_DRAG))||((event==FL_DRAG)&&(Fl::event_button()==FL_MIDDLE_MOUSE)))
	{
		int mod=((int)cameraroty) % 360;
		if( ((mod>=-90)&&(mod<90)) ||
			((mod>=270)&&(mod<360))||
			((mod>=-360)&&(mod<-270)))
			camerapanx=camerapanx-deltax*cos(cameraroty/360.0f*6.28f);
		else
			camerapanx=camerapanx+deltax*cos(cameraroty/360.0f*6.28f);

		//camerapanz=camerapanz+deltax*sin(cameraroty/360.0f*6.28f);
		camerapany=camerapany+deltay;
	}
	return Fl_Gl_Window::handle(event);
}



void PlayerFLTKGLWindowView::SetOutputControl(Fl_Output*framenumber,Fl_Output*framerate)
{
	this->outputframenumber=framenumber;
	this->outputframerate=framerate;
}

void PlayerFLTKGLWindowView::StartCapture()
{/*
	if(capturing)
		return;
	if((agent!=0)&&(strcmp(agent->GetStatus(),"ready")==0))
	{
		if(avirecorder.open((char*)(agent->GetBaseFileName()+".avi").c_str(),0,h(),0,w(),25))
		{
			agent->StopTalking();
			agent->WRITEAVI=true;
			agent->SetCurrentFrame(0);
			agent->StartTalking();
			if(listener!=0)
			{
				listener->StopTalking();
				listener->WRITEAVI=true;
				listener->SetCurrentFrame(0);
				listener->StartTalking();
			}
			capturing=true;
			printf("started capturing\n");
		}
		else
			printf("problem initializing avi file.\n");
	}
*/
}


void PlayerFLTKGLWindowView::EndCapture()
{
	/*
	if(agent!=0)
	{
		if(strcmp(agent->GetStatus(),"ready")==0)
			if(capturing==true)
			{
				capturing=false;
				agent->WRITEAVI=false;
				if(listener!=0)
					listener->WRITEAVI=false;
				avirecorder.close();
				printf("avifile closed\n");

				printf("adding audio\n");
				if(avirecorder.add_wav((agent->GetBaseFileName()+".wav").c_str()))
					printf("done\n");
				else
					printf("problem with wav file!\n");


				
			}
	}
	*/
}