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

// SyncroPsydule.cpp: implementation of the SyncroPsydule class.
//
//////////////////////////////////////////////////////////////////////

#include "SyncroPsydule.h"
#include "SDL_thread.h"
#include "SDL_net.h"

//extern FileNames filenames;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool syncropsydulesecondthreadalive=true;

int syncropsydulesecondthread(void *caller)
{
	SyncroPsydule* sp;
	Message *msg;
	JString content;

	sp=((SyncroPsydule*)caller);

/*	printf("*synchropsydule: second thread started*\n");

	while(syncropsydulesecondthreadalive)
	{
		SDL_mutexP((SDL_mutex *)sp->mutex);
		msg=sp->ReceiveMessage(10);

		if(msg==NULL) return(0);

		content=msg->getContent();
		if(strcmp(msg->type.c_str(),"Greta.Data.Time")==0)
		{
			if(strcmp(content.c_str(),"")!=0)
				sp->time=atoi(content.c_str());	
			return(1);
		}
		else
			sp->postMessage(msg->from, msg->type, msg, "", "");
		SDL_mutexV((SDL_mutex *)sp->mutex);
		SDL_Delay(30);
	}
	printf("*second thread exiting*\n");*/
	return 1;
}


SyncroPsydule::SyncroPsydule(JString name, JString host, int port) : Psydule(name, host, port)
{
	this->name=name;
	this->port=port;

	mutex=(SDL_mutex *)SDL_CreateMutex();

	if(InitSP()==0)
		exit(0);

	SDL_CreateThread(syncropsydulesecondthread,this);

}

SyncroPsydule::~SyncroPsydule()
{
}

int SyncroPsydule::InitSP()
{
	if(SDL_Init(SDL_INIT_EVENTTHREAD)==-1)
	{
		printf("SDL_Init: %s\n", SDL_GetError());
		return 0;
	}
	return 1;
}