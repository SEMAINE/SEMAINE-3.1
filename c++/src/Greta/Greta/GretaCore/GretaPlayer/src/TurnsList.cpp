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

// TurnsList.cpp: implementation of the TurnsList class.
//
//////////////////////////////////////////////////////////////////////

#include "TurnsList.h"
#include <stdio.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TurnsList::TurnsList(char* name,int t)
{
	strcpy(filename,name);
	time=t;
	executed=false;
	prev=NULL;
	next=NULL;
}

void TurnsList::Add(char* name,int t)
{
	TurnsList* element;
	if(next==NULL)
	{
		element=new TurnsList(name,t);
		next=element;
		element->prev=this;
	}
	else
		next->Add(name,t);
}

TurnsList::~TurnsList()
{

}
