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

// This main show an use example of the Greta Behavior Planner Modules
#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include <stdio.h>

#include "DataContainer.h"
#include "IniManager.h"
#include "GBPEngine.h"
#include "GBPDefinition.h"

DataContainer *datacontainer;
IniManager inimanager;
std::string ini_filename;

void main()
{
	
	ini_filename="greta.ini";	
	inimanager.ReadIniFile(ini_filename);
	datacontainer = new DataContainer();
	//load faces, gestures etc
	
	
	//Radek!!!
	int code=datacontainer->initAll();
	//int code=datacontainer->initFMLEngine();
	if (code==0) {
		printf("Problem : out \n");
		exit(1);
	}		
	
	// Creation du Greta Behavior Planner Engine
	GBPEngine gbpEngine = GBPEngine(GBP_INI_FILE);
	// Execution of the Greta Behavior Planner Engine
	gbpEngine.run();
	// creation of the gbpTranscoder
	while(1)
	{
		Sleep(5);
	}
	getch();
}


