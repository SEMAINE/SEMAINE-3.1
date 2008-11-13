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

#include "EnrichedBMLPsydule.h"
#include <FL/Fl.H>
#include <crtdbg.h>
#include "IniManager.h"

IniManager inimanager;

void main()
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

	EnrichedBMLPsydule *p;
	p = new EnrichedBMLPsydule("Greta.EnrichedBMLPsydule","127.0.0.1",10000);
	if(p->Register("Greta.Whiteboard", "Greta.Data.EnrichedBMLCode")==1)
	{
		printf("started EnrichedBMLPsydule\n");
		while(true)
		{
			p->ReadMessage(p->ReceiveMessage(100));
		}
	}
	delete p;
}
