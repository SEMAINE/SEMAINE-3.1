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
#include<stdio.h>

#include "ClockPsyduleWindow.h"
//#include "BML_AgentEngine.h"
//#include "PlayerFLTKWindow.h"
#include "FL/Fl.H"
#include "ClockPsydule.h"
#include "IniManager.h"
#include "SDL_thread.h"
#include "SDL_net.h"

extern IniManager inimanager;



void clockpsyduleidle(void* v)
{
	Sleep(10);
	if(((ClockPsyduleWindow*)v)->clp!=0)
	{
		if((((ClockPsyduleWindow*)v)->clp->pc.GetTime()-((ClockPsyduleWindow*)v)->clp->sentat)>2000)
		((ClockPsyduleWindow*)v)->clp->SendTime();
		((ClockPsyduleWindow*)v)->UpdateTime(((ClockPsyduleWindow*)v)->clp->pc.GetTime());
	}
	else
	{
		((ClockPsyduleWindow*)v)->clockoutput->value("error");
	}
	((ClockPsyduleWindow*)v)->redraw();
}


ClockPsyduleWindow::ClockPsyduleWindow(std::string argv):Fl_Double_Window(100,100,220,100,"Clock Psydule")
{
 
	JString host,GretaName;
	int port;
	if (argv!="")
	{
		ini_filename=argv;
	} else {
        ini_filename="greta.ini";
	}
	inimanager.ReadIniFile(ini_filename);
	host=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	port=inimanager.GetValueInt("PSYCLONE_PORT");
	GretaName=(JString)inimanager.GetValueString("GRETA_NAME").c_str();

	clp=new ClockPsydule(GretaName+".ClockPsydule",host,port,GretaName);
	
	if(clp->Register(GretaName+PSYCLONE_GRETA_WHITEBOARD,"")!=0)
	{
	
	}
	else
		clp=0;

	clockoutput=new Fl_Output(100,20,60,20,"Clock sent at (ms):");
	clockoutput->labelfont(FL_HELVETICA);
	clockoutput->labelsize(10);
	clockoutput->textfont(FL_HELVETICA);
	clockoutput->textsize(10);
	reset=new Fl_Button(100,50,30,15,"reset");
	reset->labelfont(FL_HELVETICA);
	reset->labelsize(10);

	Fl::add_idle(clockpsyduleidle,this);

	this->show();
}

ClockPsyduleWindow::~ClockPsyduleWindow()
{
	delete clp;
}

void ClockPsyduleWindow::draw()
{
	Fl_Window::draw();
}

int ClockPsyduleWindow::handle(int e)
{
	if(Fl::event_inside(reset))
	{
		if(Fl::event_button()==FL_LEFT_MOUSE)
		{
			if(e==FL_PUSH)
			{
				clp->Reset();
			}
		}
	}
	return Fl_Window::handle(e);
}

void ClockPsyduleWindow::UpdateTime(int t)
{
	char timestr[100];
	sprintf(timestr,"%d",t);
	this->clockoutput->value(timestr);
}
