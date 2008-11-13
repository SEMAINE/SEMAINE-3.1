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

// PlayerFLTKGLWindow.cpp: implementation of the PlayerFLTKGLWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "PlayerFLTKGLWindow.h"
#include <FL/glut.H>
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <GL/glext.h>
#include <FL/Fl.H>
#include "FL/Fl_draw.H"
#include <crtdbg.h>

extern IniManager inimanager;

void gretaplayeridle(void* v)
{
	//windows specific function. if you remove this the cpu will be loaded 100%
	//in linux you should replace this with other functions (nanosleep?)
	Sleep(5);
	((PlayerFLTKGLWindow*)v)->redraw();
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PlayerFLTKGLWindow::PlayerFLTKGLWindow(int x, int y, int w, int h, const char *label, bool autoupdate, bool realtime):Fl_Gl_Window(x,y,w,h,label)
{
	this->realtime=realtime;
	filetoload="";
	Initialize(x,y,w,h,autoupdate);
}
PlayerFLTKGLWindow::PlayerFLTKGLWindow(int x, int y, int w, int h, const char *label,std::string filename, bool autoupdate, bool realtime):Fl_Gl_Window(x,y,w,h,label)
{
	this->realtime=realtime;
	filetoload=filename;
	Initialize(x,y,w,h,autoupdate);
}

void PlayerFLTKGLWindow::Initialize(int x, int y, int w, int h, bool autoupdate)
{
	mode(FL_DOUBLE|FL_RGB|FL_DEPTH);
	width=w;
	height=h;

	if(!inimanager.GetValueInt("PLAYER_ONLYFACE"))
	{
		camerapanx=0;
		camerapany=-53.000000;
		camerapanz=-262.591888;
		camerarotx=7.000000;
		cameraroty=0.000000;
		camerarotz=0;
	}
	else
	{
		camerapanx=0;
		camerapany=-93.000000;
		camerapanz=-73.000000;
		camerarotx=-2.000000;
		cameraroty=0;
		camerarotz=0;
	}
	if(inimanager.GetValueInt("PLAYER_SHOWLISTENER")==1)
	{
		camerapanx=-15;
		camerapany=-87;
		camerapanz=-83;
		camerarotx=0;
		cameraroty=0;
		camerarotz=0;
	}

	agent=0;
	listener=0;

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

	if(autoupdate==true)
		Fl::add_idle(gretaplayeridle,this);

	outputframenumber=0;
	outputframerate=0;

	capturing=false;

	previoustime=0;

	frameaccumulator=0;

	glwview=0;
	
	world=new World();

	listenerinsubwindow=(bool)inimanager.GetValueFloat("PLAYER_LISTENERSUBWINDOW");
	//listenerinsubwindow=0;
	camerasettings.ReadIniFile("camera.ini");
}

PlayerFLTKGLWindow::~PlayerFLTKGLWindow()
{
	Fl::remove_idle(gretaplayeridle);
	delete world;
	world=0;
	delete agent;
	agent=0;
}

void PlayerFLTKGLWindow::MoveCamera(float px,float py,float pz,float rx,float ry,float rz)
{
	camerapanx=px;
	camerapany=py;
	camerapanz=pz;
	camerarotx=rx;
	cameraroty=ry;
	camerarotz=rz;
}


void PlayerFLTKGLWindow::RemoveIdle()
{
	Fl::remove_idle(gretaplayeridle);
}

void PlayerFLTKGLWindow::draw()
{
	if(this->visible()!=true)
		return;

	if(agent==0)
	{
		agent=new AgentAspect("speaker",inimanager.GetValueString("CHARACTER_SPEAKER"),realtime);

		agent->AssignFile("empty");

		agent->EnableAudio(true);

		agent->LoadBAPFrame(1,0);

		if(inimanager.GetValueInt("PLAYER_SHOWLISTENER")==1)
		{
			listener=new AgentAspect("listener",inimanager.GetValueString("CHARACTER_LISTENER"));
			//listener->AssignFile("empty");
			listener->EnableAudio(true);
			listener->MoveAgent(50,0,0);
			listener->RotateAgent(0,-90,0);
			listener->SetHairColor(0.6f,0.3f,0.1f);
			//listener->SwitchHair();

			agent->RotateAgent(0,90,0);
		}

		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular_0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);
		
		
	}

	if(filetoload!="")
	{
		agent->AssignFile((char*)filetoload.c_str());
		//agent->StartTalking();
		filetoload="";
	}

	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//se non si prevedono subwindow:
	if(!listenerinsubwindow)
		glViewport(0,0,this->w(),this->h());
	else
	//in previsione di avere una SUBWINDOW:
		glViewport(0,0,this->w(),150);
	//ricordando che 0,0 è in basso a sinistra

	//se non c'è la subwindow:
	if(!listenerinsubwindow)
		gluPerspective(25.0f,(GLfloat)this->w()/(GLfloat)this->h(),1.0, 150000.0);
	else
	//se c'è la SUBWINDOW:
		gluPerspective(25.0f,(GLfloat)this->w()/150.0f,1.0, 150000.0);


	if (!valid())
	{
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

	if(world!=0)
	{
		world->draw();
	}

	agent->draw();

	
	ViewCamera();

	if(listener!=0)
		listener->draw();

	if(listenerinsubwindow)
	{
		//SUBWINDOW

		// supponendo di voler fare una subwindow in alto a sinistra
		//definire la viewport, il punto 0,0 è in basso a sinistra

		glViewport(0,150,250,this->h()-150);
		
		//da qui in poi si ripetono le operazioni per il drawing

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//la prospettiva va fatta in base alla viewport
		//siccome la viewport era 150*100 queste dimensioni appaiono anche qui sotto:
		gluPerspective(25.0f,250.0f/(this->h()-150.0f),1.0, 150000.0);

		glMatrixMode(GL_MODELVIEW);
	
		glLoadIdentity();
			
		glTranslatef(0.2,0,-80);

		glRotatef(0,1,0,0);	
		glRotatef(-90,0,1,0);

		glTranslatef(-0.2,0,80);
			
		glTranslatef(0.2,-92,-80);
		//ridisegna l'agente

		if(agent->WRITEAVI==true)
			if(agent->GetCurrentFrame()>0)
				agent->SetCurrentFrame(agent->GetCurrentFrame()-1);
		agent->draw();

		if(listener!=0)
		{
			//definire la viewport, il punto 0,0 è in basso a sinistra

			glViewport(250,150,250,this->h()-150);
			
			//da qui in poi si ripetono le operazioni per il drawing

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			//la prospettiva va fatta in base alla viewport
			//siccome la viewport era 150*100 queste dimensioni appaiono anche qui sotto:
			gluPerspective(25.0f,250.0f/(this->h()-150.0f),1.0, 150000.0);

			glMatrixMode(GL_MODELVIEW);
		
			glLoadIdentity();
				
			glTranslatef(0,0,-32);

			glRotatef(0,1,0,0);	
			glRotatef(90,0,1,0);

			glTranslatef(0,0,32);
				
			glTranslatef(0,-92,-32);

			if(listener->WRITEAVI==true)
				if(listener->GetCurrentFrame()>0)
					listener->SetCurrentFrame(listener->GetCurrentFrame()-1);

			listener->draw();
		}
		
		//fine SUBWINDOW
	}

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

	if(capturing==true)
	{
		if(strcmp(agent->GetStatus(),"ready")==0)
			this->EndCapture();
		else
			avirecorder.writeFrame();
	}
	//printf("agent is %s\n",agent->GetStatus());

}

int PlayerFLTKGLWindow::handle(int event)
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



void PlayerFLTKGLWindow::SetOutputControl(Fl_Output*framenumber,Fl_Output*framerate)
{
	this->outputframenumber=framenumber;
	this->outputframerate=framerate;
}

void PlayerFLTKGLWindow::StartCapture()
{
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

}


void PlayerFLTKGLWindow::EndCapture()
{
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
}

void PlayerFLTKGLWindow::ViewCamera()
{
	point3d pos;

	if(glwview==0)
		return;

	pos=agent->GetPosition();

	glwview->camerapanx=0;
	glwview->camerapany=-100;
	glwview->camerapanz=0;
	glwview->camerarotx=((agent->FAPs->frames[agent->GetCurrentFrame()].FAPs[48].value+agent->FAPs->frames[agent->GetCurrentFrame()].FAPs[25].value)/(100000*6.28))*360;
	glwview->cameraroty=180+(-(agent->FAPs->frames[agent->GetCurrentFrame()].FAPs[49].value+agent->FAPs->frames[agent->GetCurrentFrame()].FAPs[23].value)/(100000*6.28))*360;
	glwview->camerarotz=(agent->FAPs->frames[agent->GetCurrentFrame()].FAPs[50].value/(100000*6.28))*360;
}

void PlayerFLTKGLWindow::SetCameraView(int i)
{
	char number[3];
	float px,py,pz,rx,ry,rz;
	sprintf(number,"%d",i+1);
	std::string parameter;
	parameter=std::string(number)+"_POS_X";
	px=camerasettings.GetValueFloat(parameter);
	parameter=std::string(number)+"_POS_Y";
	py=camerasettings.GetValueFloat(parameter);
	parameter=std::string(number)+"_POS_Z";
	pz=camerasettings.GetValueFloat(parameter);
	parameter=std::string(number)+"_ROT_X";
	rx=camerasettings.GetValueFloat(parameter);
	parameter=std::string(number)+"_ROT_Y";
	ry=camerasettings.GetValueFloat(parameter);
	parameter=std::string(number)+"_ROT_Z";
	rz=camerasettings.GetValueFloat(parameter);
	this->MoveCamera(px,py,pz,rx,ry,rz);
}

void PlayerFLTKGLWindow::SaveCameraSettings(int i)
{
	char number[3];
	sprintf(number,"%d",i+1);
	camerasettings.SetValueFloat(std::string(number)+"_POS_X",camerapanx);
	camerasettings.SetValueFloat(std::string(number)+"_POS_Y",camerapany);
	camerasettings.SetValueFloat(std::string(number)+"_POS_Z",camerapanz);
	camerasettings.SetValueFloat(std::string(number)+"_ROT_X",camerarotx);
	camerasettings.SetValueFloat(std::string(number)+"_ROT_Y",cameraroty);
	camerasettings.SetValueFloat(std::string(number)+"_ROT_Z",camerarotz);
	camerasettings.WriteIniFile("camera.ini");
}