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

// WoZWindow.cpp: implementation of the WoZWindow class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "WoZWindow.h"
#include "FL/Fl.H"

using namespace cmlabs;

extern IniManager inimanager;

void selectedMimicryValue(Fl_Widget* w, WoZWindow* v)
{
	char value[255];
	sprintf(value,"%.2f",v->mimic_level->value());
	v->mimic_level_output->value(value);
}


WoZWindow::WoZWindow():Fl_Window(100,100,330,330,"Start Window")
{
	JString host;
	int port;
	char value[256];
	
	inimanager.ReadIniFile("greta.ini");
	
	mimic_level=new Fl_Slider(40,300,150,20,"mimicry level");
	mimic_level->type(FL_HOR_NICE_SLIDER);
	mimic_level->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	mimic_level->bounds(0,1);
	mimic_level->step(0.1);
	mimic_level->value(0.5);
	mimic_level->callback((Fl_Callback *)selectedMimicryValue,this);
	mimic_level_output=new Fl_Output(192,300,40,20);
	sprintf(value,"%.2f",mimic_level->value());
	mimic_level_output->value(value);

	sendMimicryLevel=new Fl_Button(235,300,50,20,"send");

	sendPoppy=new Fl_Button(40,20,100,100,"POPPY");

	sendPrudence=new Fl_Button(190,20,100,100,"PRUDENCE");

	sendObadiah=new Fl_Button(40,160,100,100,"OBADIAH");

	sendSpyke=new Fl_Button(190,160,100,100,"SPYKE");


	host=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	port=inimanager.GetValueInt("PSYCLONE_PORT");

	psydule=new Psydule("Greta.WoZWindow",host,port);

	Poppy=new AgentState(LoadAgentState("listener/Poppy.xml"));
	Prudence=new AgentState(LoadAgentState("listener/Prudence.xml"));
	Obadiah=new AgentState(LoadAgentState("listener/Obadiah.xml"));
	Spyke=new AgentState(LoadAgentState("listener/Spyke.xml"));


	if(psydule->Register("Greta.Whiteboard","")!=0)
		printf("registered to Psyclone\n");
	else
	{
		printf("could not register to Psyclone\n");
		psydule=0;
	}
	
}

WoZWindow::~WoZWindow()
{
}

void WoZWindow::draw()
{
	Fl_Window::draw();
}

int WoZWindow::handle(int e)
{
	int i;
	if(Fl::event_button()==FL_LEFT_MOUSE)
	{
		if(e==FL_PUSH)
		{
			if(Fl::event_inside(sendPoppy))
			{
				SendAgentState("Poppy");
			}
			if(Fl::event_inside(sendPrudence))
			{
				SendAgentState("Prudence");
			}
			if(Fl::event_inside(sendObadiah))
			{
				SendAgentState("Obadiah");
			}
			if(Fl::event_inside(sendSpyke))
			{
				SendAgentState("Spyke");
			}
			if(Fl::event_inside(sendMimicryLevel))
			{
				SendMimicryLevel();
			}
		}
	}

	return Fl_Window::handle(e);
}

std::string WoZWindow::LoadAgentState(std::string filepath)
{
	std::string state;
	std::string line;
	if(filepath=="")
	{
		printf("WoZ: error parsing %s\n",filepath.c_str());
		return "";
	}
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			state=state+line+"\n";
		}
		inputfile.close();
	}
	else 
		return "";
	return state;
}

void WoZWindow::SendAgentState(std::string SAL)
{
	int i;
	char value[255];
	std::string opname;
	std::string emotion;
	std::string name="";
	AgentState *agentstate;

	if(SAL=="Poppy")
	{
		name="POPPY";
		emotion="joy";
		agentstate=Poppy;
	}
	if(SAL=="Prudence")
	{
		name="PRUDENCE";
		emotion="neutral";
		agentstate=Prudence;
	}
	if(SAL=="Obadiah")
	{
		name="OBADIAH";
		emotion="sadness";
		agentstate=Obadiah;
	}
	if(SAL=="Spyke")
	{
		name="SPYKE";
		emotion="anger";
		agentstate=Spyke;
	}

	name="agent_identity.agent.name=" + name;
	emotion="emotion_state.emotion.name=" + emotion;

	opname="mimicry_level.mimicry.value";
	sprintf(value,"%.2f",mimic_level->value());
	agentstate->internalstate.SetAttrValue(opname, (std::string)value);

	std::string msg=agentstate->internalstate.PrintXML();
	psydule->PostString(msg, "Greta.Whiteboard", "Greta.Data.AgentState");
	psydule->PostString(name, "Greta.Whiteboard", "Greta.Data.UpdateAgentState");
	psydule->PostString(emotion, "Greta.Whiteboard", "Greta.Data.UpdateAgentState");
}

void WoZWindow::SendMimicryLevel()
{
	char value[256];

	sprintf(value,"%.2f",mimic_level->value());
	std::string mimicrylevel="mimicry_level.mimicry.value=" + (std::string)value;

	psydule->PostString(mimicrylevel, "Greta.Whiteboard", "Greta.Data.UpdateAgentState");
}

