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

// PlayerFLTKWindow.cpp: implementation of the PlayerFLTKWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "PlayerFLTKWindow.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/fl_widget.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Draw.h>

#include "IniManager.h"

extern IniManager inimanager;

	static LPRECT rRegion;
	static    HDIB     hDib; 
	static RECT captRegion; 

void selectedmenu(Fl_Widget* w, void* v)
{

	if(strcmp(((PlayerFLTKWindow*)v)->menu->text(),"about")==0)
	{
	}
	if(strcmp(((PlayerFLTKWindow*)v)->menu->text(),"quit")==0)
	{
		((PlayerFLTKWindow*)v)->hide();
		((PlayerFLTKWindow*)v)->glw->RemoveIdle();
	}
	if(strcmp(((PlayerFLTKWindow*)v)->menu->text(),"load BAP-FAP file")==0)
	{
		Fl_File_Chooser *chooser;
		chooser= new Fl_File_Chooser((inimanager.Program_Path+"output").c_str(),"BAP-FAP files (*.*ap)",FL_SINGLE,"select a BAP or FAP file");
		chooser->show();
		while (chooser->shown())
			Fl::wait();
		std::string s;
		s="";
		if(chooser->value()!=0)
			s=chooser->value();
		if(s!="")
		{
			s=s.substr(0,s.find_last_of("."));
			((PlayerFLTKWindow*)v)->glw->agent->AssignFile((char*)s.c_str());
			if(((PlayerFLTKWindow*)v)->glw->listener!=0)
				((PlayerFLTKWindow*)v)->glw->listener->AssignFile((char*)(s+"_listener").c_str());
		}
		delete chooser;
	}
	if(strcmp(((PlayerFLTKWindow*)v)->menu->text(),"refresh file")==0)
	{
		((PlayerFLTKWindow*)v)->glw->agent->StopTalking();
		((PlayerFLTKWindow*)v)->glw->agent->SetCurrentFrame(0);
		((PlayerFLTKWindow*)v)->glw->agent->ReloadFile();
	}
	if(strcmp(((PlayerFLTKWindow*)v)->menu->text(),"load environment")==0)
	{
		Fl_File_Chooser *chooser;
		chooser= new Fl_File_Chooser((inimanager.Program_Path+"environments").c_str(),"environment files (*.xml)",FL_SINGLE,"select an environment file");
		chooser->show();
		while (chooser->shown())
			Fl::wait();
		std::string s;
		s="";
		if(chooser->value()!=0)
			s=chooser->value();
		if(s!="")
		{
			((PlayerFLTKWindow*)v)->glw->world->LoadWorld(s);
			//((PlayerFLTKWindow*)v)->glw2->world->LoadWorld(s);
		}
		delete chooser;
	}
	if(strcmp(((PlayerFLTKWindow*)v)->menu->text(),"show faces")==0)
	{
		if (((PlayerFLTKWindow*)v)->glw->listenerinsubwindow==1)
			((PlayerFLTKWindow*)v)->glw->listenerinsubwindow=0;
		else
			((PlayerFLTKWindow*)v)->glw->listenerinsubwindow=1;
	}
}

void selectedplay(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->StartTalking();
	if(((PlayerFLTKWindow*)v)->glw->listener!=0)
		((PlayerFLTKWindow*)v)->glw->listener->StartTalking();
}

void selectedstop(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->StopTalking();
	if(((PlayerFLTKWindow*)v)->glw->listener!=0)
		((PlayerFLTKWindow*)v)->glw->listener->StopTalking();
	
}

void selectedframeback(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->StopTalking();
	((PlayerFLTKWindow*)v)->glw->agent->OneFrameDown();
	if(((PlayerFLTKWindow*)v)->glw->listener!=0)
	{
		((PlayerFLTKWindow*)v)->glw->listener->StopTalking();
		((PlayerFLTKWindow*)v)->glw->listener->OneFrameDown();
	}
}

void selectedframenext(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->StopTalking();
	((PlayerFLTKWindow*)v)->glw->agent->OneFrameUp();
	if(((PlayerFLTKWindow*)v)->glw->listener!=0)
	{
		((PlayerFLTKWindow*)v)->glw->listener->StopTalking();
		((PlayerFLTKWindow*)v)->glw->listener->OneFrameUp();
	}
}

void selectedrewind(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->StopTalking();
	((PlayerFLTKWindow*)v)->glw->agent->SetCurrentFrame(0);
	if(((PlayerFLTKWindow*)v)->glw->listener!=0)
	{
		((PlayerFLTKWindow*)v)->glw->listener->StopTalking();
		((PlayerFLTKWindow*)v)->glw->listener->SetCurrentFrame(0);
	}
}

void selectedhair(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->SwitchHair();
}

void selectedskin(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->SwitchSkin();
}

void selectedifapu(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->SwitchIFAPU();
}

void selectedeyes(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->SwitchEyes();
}

void selectedskeleton(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->agent->SwitchSkeleton();
}

void selectedstartcapture(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->StartCapture();
	/*
	if(!((PlayerFLTKWindow*)v)->capturing)
		((PlayerFLTKWindow*)v)->Capture();
	else
		((PlayerFLTKWindow*)v)->EndCapture();
		*/
}
void selectedendcapture(Fl_Widget* w, void* v)
{
	((PlayerFLTKWindow*)v)->glw->EndCapture();
	/*
	if(!((PlayerFLTKWindow*)v)->capturing)
		((PlayerFLTKWindow*)v)->Capture();
	else
		((PlayerFLTKWindow*)v)->EndCapture();
		*/
}

void gretaplayerwindowclosed(void* v)
{
	((PlayerFLTKWindow*)v)->hide();
	((PlayerFLTKWindow*)v)->glw->RemoveIdle();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




PlayerFLTKWindow::PlayerFLTKWindow(std::string filename):Fl_Window(10,20,inimanager.GetValueInt("PLAYER_WIN_WIDTH")+200,inimanager.GetValueInt("PLAYER_WIN_HEIGHT")+70,"Greta player")
{
	glw=new PlayerFLTKGLWindow(200,35,490,430,"test",filename);
	Initialization();
}

PlayerFLTKWindow::PlayerFLTKWindow():Fl_Window(10,20,inimanager.GetValueInt("PLAYER_WIN_WIDTH")+200,inimanager.GetValueInt("PLAYER_WIN_HEIGHT")+70,"Greta player")
{
	glw=new PlayerFLTKGLWindow(200,35,490,430,"test");
	Initialization();
}

void PlayerFLTKWindow::Initialization()
{
	int i;

	this->color(fl_rgb_color(204,204,204));
	this->size_range(375,250,1280,1024,1,1,0);
	//channels=new ChannelsImage(5,35,100,500-45,"");
	//glw2=new PlayerFLTKGLWindowView(20,250,150,120,"vision");
	//glw->glwview=glw2;
	
	menu=new PlayerFLTKMenu(0,0,this->w(),30);

	//disattivate : changed to "1" because of the bug
	menu->add("player/load environment",0,(Fl_Callback *)selectedmenu,this,1);
	//disattivate : changed to "1" because of the bug
	menu->add("player/show faces",0,(Fl_Callback *)selectedmenu,this,1);
	menu->add("player/quit",0,(Fl_Callback *)selectedmenu,this,0);
	menu->add("agent/load BAP-FAP file",0,(Fl_Callback *)selectedmenu,this,0);
	menu->add("agent/refresh file",0,(Fl_Callback *)selectedmenu,this,0);
	menu->add("?/about",0,(Fl_Callback *)selectedmenu,this,0);
	//this->end();

	button_rewind=new Fl_Button(200,468,30,30,"@|<");
	button_rewind->callback((Fl_Callback *)selectedrewind,this);
	button_frameback=new Fl_Button(235,468,30,30,"@<<");
	//button_frameback->callback((Fl_Callback *)selectedframeback,this);
	button_stop=new Fl_Button(270,468,30,30,"@square");
	button_stop->callback((Fl_Callback *)selectedstop,this);
	button_play=new Fl_Button(305,468,30,30,"@>");
	button_play->callback((Fl_Callback *)selectedplay,this);
	button_framenext=new Fl_Button(340,468,30,30,"@>>");
	//button_framenext->callback((Fl_Callback *)selectedframenext,this);
	/*
	button1=new Fl_Button(110,450,100,40);
	button1image=new Fl_JPEG_Image("button1.jpg");
	button1->label("ciao!");
	button1->image(button1image);
	button2=new Fl_Button(215,450,100,40);
	button2->label("bottone");
	this->end();
	*/


	facebox=new Fl_Box(FL_BORDER_BOX,5,35,190,100,"rendering");
	facebox->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT|FL_ALIGN_TOP);

	button_hair=new Fl_Check_Button(170,35,20,20,"show hair");
	button_hair->align(FL_ALIGN_LEFT);
	button_hair->value(1);
	button_hair->callback((Fl_Callback *)selectedhair,this);

	button_skin=new Fl_Check_Button(170,60,20,20,"show head skin");
	button_skin->align(FL_ALIGN_LEFT);
	button_skin->value(1);
	button_skin->callback((Fl_Callback *)selectedskin,this);

	button_eyesteeths=new Fl_Check_Button(170,85,20,20,"show eyes/teeths/tongue");
	button_eyesteeths->align(FL_ALIGN_LEFT);
	button_eyesteeths->value(1);
	button_eyesteeths->callback((Fl_Callback *)selectedeyes,this);

	button_skeleton=new Fl_Check_Button(170,110,20,20,"show body skin");
	button_skeleton->align(FL_ALIGN_LEFT);
	button_skeleton->value(1);
	button_skeleton->callback((Fl_Callback *)selectedskeleton,this);

	//button_ifapu=new Fl_Check_Button(170,115,20,20,"ifapu");
	//button_ifapu->align(FL_ALIGN_LEFT);
	//button_ifapu->value(1);
	//button_ifapu->callback((Fl_Callback *)selectedifapu,this);

	button_startcapture=new Fl_Button(375,468,60,30,"capture");
	button_startcapture->callback((Fl_Callback *)selectedstartcapture,this);

	//button_endcapture=new Fl_Button(75,145,60,30,"end capture");
	//button_endcapture->callback((Fl_Callback *)selectedendcapture,this);

	framenumber=new Fl_Output(90,150,100,30,"frame n.");
	framenumber->textfont(FL_HELVETICA);
	framenumber->textsize(12);
	framenumber->align(FL_ALIGN_LEFT);

	framerate=new Fl_Output(90,185,100,30,"frame rate");
	framerate->textfont(FL_HELVETICA);
	framerate->textsize(14);
	framerate->align(FL_ALIGN_LEFT);
	//framenumber->label("frame n.");

	for(i=0;i<3;i++)
	{
		char *number;
		number=(char*)malloc(sizeof(char*)*3);
		sprintf(number,"%d",i+1);
		button_savecamera[i]=new Fl_Button(90+25*i,400,20,20,number);
	}

	for(i=0;i<3;i++)
	{
		char *number;
		number=(char*)malloc(sizeof(char*)*3);
		sprintf(number,"%d",i+1);
		button_loadcamera[i]=new Fl_Button(90+25*i,425,20,20,number);
	}

	glw->SetOutputControl(framenumber,framerate);

	this->callback((Fl_Callback *)gretaplayerwindowclosed,this);

	position_x=0;
	position_y=0;
	width=700;
	height=400;
	
	quietversion=false;

	capturing=false;

	isfullscreen=false;

	goingback=false;

	goingnext=false;


}

PlayerFLTKWindow::~PlayerFLTKWindow()
{
	delete glw;
}

//DEL void PlayerFLTKWindow::draw()
//DEL {
//DEL 	channels->resize(5,5,width-10,height-10);
//DEL }

//DEL void PlayerFLTKWindow::resize(int x, int y, int w, int h)
//DEL {
//DEL 	position_x=x;
//DEL 	position_y=y;
//DEL 	width=w;
//DEL 	height=h;
//DEL 	channels->resize(5,5,width-10,height-10);
//DEL }

int PlayerFLTKWindow::handle(int event)
{
	int i;
	for(i=0;i<3;i++)
	{
		if((event==FL_PUSH)&&(Fl::event_button()==FL_LEFT_MOUSE))
		{
			if(Fl::event_inside(button_loadcamera[i]))
			{
				glw->SetCameraView(i);
				return 0;
			}
		}
	}
	
	for(i=0;i<3;i++)
	{
		if((event==FL_PUSH)&&(Fl::event_button()==FL_LEFT_MOUSE))
		{
			if(Fl::event_inside(button_savecamera[i]))
			{
				glw->SaveCameraSettings(i);
				return 0;
			}
		}
	}

	if((event==FL_PUSH)&&(Fl::event_button()==FL_LEFT_MOUSE))
	{
		if(Fl::event_inside(button_frameback))
		{
			goingback=true;
			return 1;
		}
	}
	if((event==FL_RELEASE)&&(Fl::event_button()==FL_LEFT_MOUSE))
	{
		if(Fl::event_inside(button_frameback))
		{
			goingback=false;
			return 1;
		}
	}
	if((event==FL_PUSH)&&(Fl::event_button()==FL_LEFT_MOUSE))
	{
		if(Fl::event_inside(button_framenext))
		{
			goingnext=true;
			return 1;
		}
	}
	if((event==FL_RELEASE)&&(Fl::event_button()==FL_LEFT_MOUSE))
	{
		if(Fl::event_inside(button_framenext))
		{
			goingnext=false;
			return 1;
		}
	}
	if(Fl::event_key()==FL_Enter)
	{
		this->fullscreen();
		glw->fullscreen();
		isfullscreen=true;
	}
	if(Fl::event_key()==FL_BackSpace)
	{
		this->fullscreen_off(10,20,inimanager.GetValueInt("PLAYER_WIN_WIDTH"),inimanager.GetValueInt("PLAYER_WIN_HEIGHT"));
		this->size_range(375,250,1280,1024,1,1,0);
		glw->fullscreen_off(200,35,inimanager.GetValueInt("PLAYER_WIN_WIDTH")-200,inimanager.GetValueInt("PLAYER_WIN_HEIGHT")-70);
		isfullscreen=false;
	}


	return Fl_Window::handle(event);
}

void PlayerFLTKWindow::draw()
{
	if(goingback==true)
	{
		glw->agent->StopTalking();
		glw->agent->OneFrameDown();
		if(glw->listener!=0)
		{
			glw->listener->StopTalking();
			glw->listener->OneFrameDown();
		}
	}
	if(goingnext==true)
	{
		glw->agent->StopTalking();
		glw->agent->OneFrameUp();
		if(glw->listener!=0)
		{
			glw->listener->StopTalking();
			glw->listener->OneFrameUp();
		}
	}
	if(isfullscreen==false)
	{
		menu->size(this->w(),30);
		glw->size(this->w()-200,this->h()-70);
		glw->invalidate();
		button_rewind->position(200,this->h()-32);
		button_frameback->position(235,this->h()-32);
		button_stop->position(270,this->h()-32);
		button_play->position(305,this->h()-32);
		button_framenext->position(340,this->h()-32);
		button_startcapture->position(375,this->h()-32);
		framenumber->position(465,this->h()-32);
	}

	Fl_Window::draw();

	fl_font(FL_HELVETICA,11);
	fl_color(0,0,0);
	fl_draw("save camera",22,415);
	fl_draw("recall camera",22,440);
	
	if(capturing)
	{
		if(strcmp(glw->agent->GetStatus(),"ready")==0)
			this->EndCapture();
		if(strcmp(glw->agent->GetStatus(),"talking")==0)
		{
			//HANDLE dibh;

			//dibh=DIBCaptureRegion();

			//AddFrame(dibh);
			
		}
	}

}

void PlayerFLTKWindow::SetQuiet(bool q)
{
	quietversion=q;
	if(q==true)
		menu->hide();
}

void PlayerFLTKWindow::LoadFile(std::string filebasename)
{
	glw->filetoload=filebasename;
}


HANDLE PlayerFLTKWindow::DIBCaptureRegion()
{

	captRegion.left = x();
	captRegion.top = y();
	captRegion.right = x()+w();
	captRegion.bottom = y()+h();
	rRegion = &captRegion;

	hDib = CopyScreenToDIB(rRegion);

	return hDib;
}

void PlayerFLTKWindow::Capture()
{
	if(strcmp(glw->agent->GetStatus(),"ready")==0)
	{
		AviInit("output.avi",w(),h());
		glw->agent->StopTalking();
		glw->agent->WRITEAVI=true;
		glw->agent->SetCurrentFrame(0);
		glw->agent->StartTalking();
		capturing=true;
		printf("started capturing\n");
	}
}

void PlayerFLTKWindow::EndCapture()
{

	if(glw->agent!=0)
	{
		if(strcmp(glw->agent->GetStatus(),"ready")==0)
			if(capturing==true)
			{
				printf("1\n");
				//printf("wave filename is %s\n",glw->agent->GetWavFileName());			
				capturing=false;
				printf("ended capturing\n");
				glw->agent->WRITEAVI=false;
				glw->agent->StopTalking();
				glw->agent->SetCurrentFrame(0);
				printf("2\n");
				AddWAV(glw->agent->GetWavFileName());
				printf("3\n");
				AviClose();
			}
	}
}
