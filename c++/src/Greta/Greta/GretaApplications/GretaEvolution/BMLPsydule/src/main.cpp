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

#include "BMLPsydule.h"
#include <FL/Fl.H>
#include <crtdbg.h>
#include "IniManager.h"
#include "DataContainer.h"

IniManager inimanager;
std::string ini_filename;
DataContainer *datacontainer;

void main(int argc, char *argv[])
{
	/*
	GlutMaster gm;
	Player *player;
	player=new Player(&gm,false,false,0);
	while(!player->player_finished)
	{
		glutCheckLoop();
	}
	AgentAnimationEngine AAE;
	//AAE.CalculateTurn("input/business.xml");
*/

	/*
	GretaEvolutionWindow *window;
	window = new GretaEvolutionWindow();

	//window->set_modal();
	
	window->show();
  
	Fl::run();

	delete window;
	*/

	/* DEBUG VERSION HEAP CORRUPTION CHECK 
   added by bjoern 2004-04-21          */
	/*
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag( tmpFlag );
	*/
	//PsydulesManager *pm;
	//pm=new PsydulesManager();

	//pm->StartPsydules();

	if(argc==2)
	{
		ini_filename=argv[1];
	} else {
		ini_filename="greta.ini";	
	}
	
	//datacontainer needs some data..
	inimanager.ReadIniFile(ini_filename);
	
	datacontainer = new DataContainer();

	//load faces, gestures etc
	//Radek!!!
	int code=datacontainer->initAll();
	//int code=datacontainer->initBMLEngine();
	if (code==0) {
		printf("Problem : out \n");
		exit(1);
	}

	BMLPsydule *p;
	Message *msg;
	p = new BMLPsydule("Greta.BMLPsydule","localhost",10000);
	if(p->Register("Greta.Whiteboard", "Greta.Data.InternalBMLCode")==1)
	{
		printf("started BMLPsydule\n");
		while(true)
		{
			msg=p->ReceiveMessage(100);
			if(msg!=0)
				p->ReadMessage(msg);
			//else
			//	break;
		}
	}
	delete p;
}
