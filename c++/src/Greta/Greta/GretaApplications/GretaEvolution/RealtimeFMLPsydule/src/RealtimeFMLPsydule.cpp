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


// RealtimeFMLPsydule.cpp: implementation of the RealtimeFMLPsydule class.
//
//////////////////////////////////////////////////////////////////////



#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include "RealtimeFMLPsydule.h"
#include "RandomGen.h"
#include "FileNames.h"
#include "DataContainer.h"
#include "Selector.h"

extern DataContainer *datacontainer;
extern IniManager inimanager;
extern FileNames filenames;

RandomGen *rando;

//timeat
int DELAY = 1000;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RealtimeFMLPsydule::RealtimeFMLPsydule()
{
	pc=new CentralClock();
	initApplication();
	initRegistration();

	agentname="";

	while(true)
	{
		Sleep(5);
		Message *msg_last;
		Message msg_fml, msg_clock, msg_agent;
		msg_last=psydule->ReceiveMessage(10);
		ReadMessage(msg_last);
		/*
		msg_fml=NULL;
		msg_agent=NULL;
		msg_clock=NULL;

		while(msg_last!=NULL)
		{
			if(strcmp(msg_last->type.c_str(),GretaName+PSYCLONE_FML_MESSAGE_HEADER)==0)
			{
				msg_fml=*msg_last;
			}
			if(strcmp(msg_last->type.c_str(),GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
			{
				msg_clock=*msg_last;
			}
			if(strcmp(msg_last->type.c_str(),GretaName+PSYCLONE_UPDATEAGENTSTATE_MESSAGE_HEADER)==0)
			{
				msg_agent=*msg_last;
			}
			msg_last=psydule->ReceiveMessage(10);
		}
		ReadMessage(&msg_fml);
		ReadMessage(&msg_clock);
		ReadMessage(&msg_agent);
		*/
	}
}

RealtimeFMLPsydule::~RealtimeFMLPsydule()
{

}

// Load the application parameter from the inimanager
int RealtimeFMLPsydule::initApplication()
{	
	// Register Psyclone module adress
	psyduleHost=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	// Register Psyclone module port
	psydulePort=inimanager.GetValueInt("PSYCLONE_PORT");
	// Register Greta_Module_Name
	GretaName=(JString)inimanager.GetValueString("GRETA_NAME").c_str();
	
	// Creation of the Psyclone inteface
	psydule=new Psydule(GretaName+REALTIMEFMLPSYDULE_MODULE_NAME,psyduleHost,psydulePort);

	return 1;
}

int RealtimeFMLPsydule::initRegistration()
{	
	// registered packet list
	std::list<JString> registeredGretaDatatypes;
	// register to Greta Clock messages.
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER);
	// register to FML messages.
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_FML_MESSAGE_HEADER);
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_UPDATEAGENTSTATE_MESSAGE_HEADER);

	
	// registration GretaWiteboard
	if(psydule->Register(GretaName+PSYCLONE_GRETA_WHITEBOARD,registeredGretaDatatypes)!=0)
	{
		std::cout << GretaName+REALTIMEFMLPSYDULE_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_OK << std::endl;		
	}
	else
	{
		std::cout << GretaName+REALTIMEFMLPSYDULE_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_FAILED<< std::endl;
		psydule=0;
		return 0;
	}
	std::cout << "Registration succesful" << std::endl;

	return 1;
}

int RealtimeFMLPsydule::ReadMessage(Message *msg)
{
	JString content;
	
	if(msg==NULL) return(0);

	content=msg->getContent();
	
	if(strcmp(content.c_str(),"")!=0)
	{
		std::string app=(std::string)content;
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
		{
			int time;
			time=atoi(content.c_str());
			pc->SetTime(time);
		}
		else
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_FML_MESSAGE_HEADER)==0)
		{
			std::string bml;
			std::string time;
			int starttime=0;
			std::string fml=(std::string)content;
			bml=CalculateFMLAnimation((std::string)content);
			/*time= bml.substr(bml.find("start")+7, bml.length());
			time=time.substr(0,time.find_first_of('"'));
			starttime=(int)(atof(time.c_str())*500);

			printf("%d > %d\n", starttime, pc->GetTime()+1000);
			if(starttime>pc->GetTime()+1000)*/
				SendBML(bml);
		}	
		else
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_UPDATEAGENTSTATE_MESSAGE_HEADER)==0)
		{
			if(app.substr(0,app.find_first_of("="))=="agent_identity.agent.name")
			{
				agentname=app.substr(app.find_first_of("=")+1);
			}
		}
		return 1;
	}
	else
		return 0;
}

std::string RealtimeFMLPsydule::CalculateFMLAnimation(std::string content)
{
	std::string bml;
	faae=new FMLAPML_AgentEngine();
	std::string baseline;
	if(agentname!="")
		baseline="mmsystem/" + agentname +".xml";
	else
	{
		//printf("NO SAL CHARACTER DEFINED!!!\n");
		int pos=(int)(inimanager.GetValueString("BASELINE_DEFAULT").find("mmsystem"));
		if (pos!=std::string::npos)
			baseline=inimanager.GetValueString("BASELINE_DEFAULT").substr(pos,inimanager.GetValueString("BASELINE_DEFAULT").length());
		else
			baseline="mmsystem/baseline_default.xml";
	}
	bml=faae->Execute("", baseline.c_str(), (char*)content.c_str(), 1);
	delete faae;
	//printf("%s\n", bml.c_str());
	return(bml);
}

void RealtimeFMLPsydule::SendBML(std::string bml)
{
	//printf("BML\n%s\n\n", bml.c_str());
	psydule->PostString(bml,GretaName+PSYCLONE_GRETA_WHITEBOARD,GretaName+PSYCLONE_BML_MESSAGE_HEADER);
}
