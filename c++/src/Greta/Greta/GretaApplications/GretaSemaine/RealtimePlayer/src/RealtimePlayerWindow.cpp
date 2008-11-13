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

// ClockPsyduleWindow.cpp: implementation of the ClockPsyduleWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "RealtimePlayerWindow.h"
//#include "RandomGen.h"
//#include "Timer.h"

#include <windows.h>

//#include "SDL_thread.h"
//#include "SDL_net.h"
//#include "BML_AgentEngine.h"
//#include "PlayerFLTKWindow.h"

extern IniManager inimanager;

//extern	"C" int play_wave(int,char*);

void realtimeplayeridle(void* v)
{
	RealtimePlayerWindow* player=((RealtimePlayerWindow*)v);

	char buffer[100];

	Sleep(10);
//	SDL_mutex *mutex=(SDL_mutex *)player->mutex;

//	SDL_mutexP(mutex);

	
	BAPFrame bf(&player->fm->converter);
	std::string command;

	if(player->fm->GetFAPFrame(player->ff)==true)
	{
		player->glwindow->agent->LoadFAPFrame(&player->ff);
		//send frame to the agent's visualization
		//sprintf(buffer,"loaded fap frame into the agent face (%d)\n",ff.framenumber);
		//player->output->insert(buffer);
	}
	if(player->fm->GetBAPFrame(bf)==true)
	{
		player->glwindow->agent->LoadBAPFrame(-1,&bf);
		//send frame to the agent's visualization
		
		//sprintf(buffer,"loaded bap frame into the agent body (%d)\n",bf.GetFrameNumber());
		//player->output->insert(buffer);
	}
			
	if(player->cm->GetCommand(command)==true)
	{
		if(command.substr(0,command.find_first_of(" "))=="EMPTYBUFFER")
		{
			player->fm->EmptyBuffer();
		}
		/*if(command.substr(0,command.find_first_of(" "))=="DELETEDIFF")
		{
			std::string label;
			label=command.substr(command.find_first_of(" ")+1);
			label=label.substr(0,label.find_first_of(" "));
			player->fm->DeleteDiff(label);
		}*/
		if(command.substr(0,command.find_first_of(" "))=="PLAYWAVAT")
		{
			std::string filename;
			char mci_command[128];
			char mci_return[128];
			
			//RandomGen *randomgen;
			//float temp= randomgen->GetRand01();

			filename=inimanager.Program_Path+"output/rtplayer.wav";
			sprintf(mci_command, "play %s from 0",filename.c_str()); 
			mciSendString(mci_command, mci_return, 128, NULL);

		}
	}
	player->redraw();

//	SDL_mutexV(mutex);
}

int realtimeplayerreceiver(void *caller)
{
	RealtimePlayerWindow* player=((RealtimePlayerWindow*)caller);
	
	return 1;
}


RealtimePlayerWindow::RealtimePlayerWindow(FramesManager *fm, CommandsManager *cm, CentralClock *pc):Fl_Double_Window(100,100,640,480,"Greta Realtime Player"),
fm(fm), cm(cm), pc(pc)
{

	//JString host,GretaName;
	//int port;

//	mutex=(SDL_mutex *)SDL_CreateMutex();

	output=new Fl_Multiline_Output(10,450,620,20);
	output->labelfont(FL_HELVETICA);
	output->labelsize(10);
	output->textfont(FL_HELVETICA);
	output->textsize(10);

	timeoutput=new Fl_Output(40,10,60,15,"time:");
	timeoutput->labelfont(FL_HELVETICA);
	timeoutput->labelsize(10);
	timeoutput->textfont(FL_HELVETICA);
	timeoutput->textsize(10);

	fapbufferoutput=new Fl_Output(180,10,60,15,"FAP buffer:");
	fapbufferoutput->labelfont(FL_HELVETICA);
	fapbufferoutput->labelsize(10);
	fapbufferoutput->textfont(FL_HELVETICA);
	fapbufferoutput->textsize(10);

	bapbufferoutput=new Fl_Output(320,10,60,15,"BAP buffer:");
	bapbufferoutput->labelfont(FL_HELVETICA);
	bapbufferoutput->labelsize(10);
	bapbufferoutput->textfont(FL_HELVETICA);
	bapbufferoutput->textsize(10);

	//host=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	//port=inimanager.GetValueInt("PSYCLONE_PORT");	
	//GretaName=(JString)inimanager.GetValueString("GRETA_NAME").c_str();

	Fl::add_idle(realtimeplayeridle,this);

	glwindow=new PlayerFLTKGLWindow(10,35,620,410,"",false,true);

	//cm=new CommandsManager(&(this->pc));
	
}

RealtimePlayerWindow::~RealtimePlayerWindow()
{
	//delete psydule;
}

void RealtimePlayerWindow::draw()
{
	char buffer[100];
//	SDL_mutexP((SDL_mutex *)mutex);
	sprintf(buffer,"%d",pc->GetTime());
	timeoutput->value(buffer);
	sprintf(buffer,"%d",fm->GetFAPSize());
	fapbufferoutput->value(buffer);
	sprintf(buffer,"%d",fm->GetBAPSize());
	bapbufferoutput->value(buffer);
	glwindow->redraw();
	Fl_Window::draw();
//	SDL_mutexV((SDL_mutex *)mutex);
}

int RealtimePlayerWindow::handle(int e)
{
	if(Fl::event_button()==FL_LEFT_MOUSE)
	{
		if(e==FL_RELEASE)
		{
			this->fullscreen();
			//glwindow->width=400;
			glwindow->resize(10,35,this->w()-20, this->h()-40);
			output->replace(10,this->h()-60,"");
			glwindow->redraw();
		}
	}
	if(Fl::event_button()==FL_RIGHT_MOUSE)
	{
		if(e==FL_RELEASE)
		{
			this->fullscreen_off(100,100, 640,480);
			output->replace(10,450,"");
			glwindow->resize(10,35,620,410);
			glwindow->redraw();
		}
	}


	return Fl_Window::handle(e);
}
