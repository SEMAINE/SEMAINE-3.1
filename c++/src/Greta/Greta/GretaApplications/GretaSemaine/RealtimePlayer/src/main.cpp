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

#include "RealtimePlayerWindow.h"
#include "IniManager.h"
#include "RandomGen.h"
#include "FL/Fl.H"
#include "FileNames.h"
#include "FramesManager.h"
#include "CentralClock.h"
#include "CommandsManager.h"

#include <list>
#include <semaine/config.h>
#include <semaine/util/XMLTool.h>
#include <semaine/components/Component.h>
#include <semaine/system/ComponentRunner.h>
#include "semaineplayer.h"

FILE *agent_log;
FileNames filenames;
IniManager inimanager;
RandomGen *randomgen;
std::string ini_filename;
 

/*int WINAPI WinMain(
     HINSTANCE hInstance,
     HINSTANCE hPrevInstance,
     LPSTR     lpszCmdParm,
     int       nCmdShow
)*/
void main(int argc, char ** argv)
{
	try {

	if (argc==2)
	{
		ini_filename=argv[1];
	} else {
		ini_filename="greta.ini";
	}
	inimanager.ReadIniFile(ini_filename);
	randomgen=new RandomGen();
	CentralClock *pc=new CentralClock();
	FramesManager *fm=new FramesManager(pc);
	fm->start();
	CommandsManager *cm=new CommandsManager(pc);
	RealtimePlayerWindow rpw(fm, cm, pc);
	
	semaine::util::XMLTool::startupXMLTools();

	std::list<semaine::components::Component *> comps;
	comps.push_back(new semaine::components::player::semaineplayer(fm));
	semaine::system::ComponentRunner cr(comps);
	cr.go();

	rpw.show();
	Fl::run();
	semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
