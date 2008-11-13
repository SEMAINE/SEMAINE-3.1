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

// PlayerPsyduleWindow.cpp: implementation of the PlayerPsyduleWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "PlayerPsyduleWindow.h"
#include "BML_AgentEngine.h"
#include "PlayerFLTKWindow.h"
#include "AgentAspect.h"
#include "SDL_timer.h"
#include <string>

#include "PlayerPsydule.h"

extern IniManager inimanager;

void playerpsyduleidle(void* v)
{
	PlayerPsyduleWindow *caller=(PlayerPsyduleWindow*)v;
	AgentAspect *agent=caller->glwindow->agent;

	SDL_Delay(1);
	
	if((strcmp(agent->GetStatus(),"talking")!=0)&&(caller->istalking==true))
	{
		((PlayerPsydule*)caller->pp)->postMessage("Greta.Whiteboard","Greta.PlayerPsydule.Status.Stopped","","","");
		caller->istalking=false;
	}

	if((strcmp(agent->GetStatus(),"talking")==0)&&(caller->istalking==false))
	{
		((PlayerPsydule*)caller->pp)->postMessage("Greta.Whiteboard","Greta.PlayerPsydule.Status.Started","","","");
		caller->istalking=true;
	}

	Message* msg;
	msg=((PlayerPsydule*)(caller->pp))->ReceiveMessage(10);
	if(msg!=0)
	{
		JString type=msg->type;
		if(type.equals("Greta.Data.FAPdata"))
		{
			//Fl::remove_idle(playerpsyduleidle);

			//printf("agent state is %s\n",agent->GetStatus());

			if((strcmp(agent->GetStatus(),"talking")!=0)&&(strcmp(agent->GetStatus(),"starting")!=0))
			{
				agent->AssignFAPBuffer((char*)msg->getContent().c_str());
				//agent->AssignFile("output/aux");
			}
			else
				printf("could not load FAP buffer, player is busy\n");

		}
		if(type.equals("Greta.Data.BAPdata"))
		{
			if((strcmp(agent->GetStatus(),"talking")!=0)
				&&(strcmp(agent->GetStatus(),"starting")!=0))
			{
				agent->AssignBAPBuffer(msg->getContent());
			}
			else
				printf("could not load BAP buffer, player is busy\n");
		}
		if(type.equals("Greta.Data.Audio"))
		{
			if(((PlayerPsydule*)caller->pp)->WriteBinaryFile("output/buffer.wav", msg)!=0)
				sprintf((char*)agent->wav_filename.c_str(),"output/buffer.wav");
		}
		if(type.equals("Greta.PlayerPsydule.Command.Start"))
		{
			if(strcmp(agent->GetStatus(),"talking")!=0)
			{
				agent->StartTalking();
			}
			else
				printf("could not start talking, player is busy\n");
		}
		if(type.equals("Greta.PlayerPsydule.Command.AssignFile"))
		{
			agent->AssignFile((char*)(msg->content).c_str());
		}
	}
}

PlayerPsyduleWindow::PlayerPsyduleWindow():Fl_Window(100,100,320,256,"greta player psydule 0.1")
{
	this->size_range(320,256,1280,1024,1,1,0);

	inimanager.ReadIniFile("greta.ini");
	glwindow=new PlayerFLTKGLWindow(10,10,300,216,"");

	//bbbbb=new Fl_Button(0,230,30,15,"receive");
	//bbbbb->callback((Fl_Callback *)playerpsyduleidle,this);

	isfullscreen=false;

	pp = new PlayerPsydule("Greta.PlayerPsydule","127.0.0.1",10000);
	if(((PlayerPsydule*)pp)->PlayerRegister()==1)
	{
		printf("started PlayerPsydule\n");
		istalking=false;
		Fl::add_idle(playerpsyduleidle,this);
		this->show();
	}
	else
	{
		printf("quitting...\n");
		this->hide();
	}

	
}

PlayerPsyduleWindow::~PlayerPsyduleWindow()
{
	delete pp;
}

int PlayerPsyduleWindow::handle(int e)
{
	if(Fl::event_key()==FL_Enter)
	{
		if(isfullscreen==false)
		{
			winw=this->w();
			winh=this->h();
			this->fullscreen();
			glwindow->fullscreen();
			isfullscreen=true;
		}
	}
	if(Fl::event_key()==FL_BackSpace)
	{
		if(isfullscreen)
		{
			this->fullscreen_off(10,20,winw,winh);
			this->size_range(375,250,1280,1024,1,1,0);
			glwindow->fullscreen_off(10,10,winw-20,winh-20);
			isfullscreen=false;
		}
	}
	
	glwindow->size(this->w()-20,this->h()-20);

	return Fl_Window::handle(e);
}
