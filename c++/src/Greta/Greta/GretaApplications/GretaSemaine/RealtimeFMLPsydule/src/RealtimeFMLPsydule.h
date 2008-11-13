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


// RealtimeFMLPsydule.h: interface for the RealtimeFMLPsydule class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include <time.h>
#include "Psydule.h"
#include "CentralClock.h"
#include "../../../include/common/definitions.h"
//#include "AgentState.h"
#include "FML-APML_AgentEngine.h"
//#include "listener\ListenerData.h"


class RealtimeFMLPsydule
{
public:
	RealtimeFMLPsydule();
	virtual ~RealtimeFMLPsydule();

	// Methods :
	// Initialise the application
	int RealtimeFMLPsydule::initApplication();
	// Initialise the receiver interfaces
	int RealtimeFMLPsydule::initRegistration();
    
	int RealtimeFMLPsydule::ReadMessage(Message *msg);
	std::string RealtimeFMLPsydule::CalculateFMLAnimation(std::string content);
	void RealtimeFMLPsydule::SendBML(std::string bml);

	//AgentState *agentstate;

private:
	// The psyclone module adress
	JString psyduleHost,GretaName;	
	// The psyclone module port
	int psydulePort;
	// The Psydule Input interface
	Psydule *psydule;
	// Computer central clock
	CentralClock *pc;
	FMLAPML_AgentEngine *faae;

	std::string agentname;

	//ListenerData *listenerdata;
	
};

