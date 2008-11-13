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

// BodyVertexGroup.cpp: implementation of the BodyVertexGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "BodyVertexGroup.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BodyVertexGroup::BodyVertexGroup()
{
	vertexlist=NULL;
	size=0;
	changed=false;
}

void BodyVertexGroup::AddVertex(int v)
{
	MyBodyVertexList* aux;

	if(vertexlist==NULL)
	{
		vertexlist=new MyBodyVertexList;
		vertexlist->next=NULL;
		vertexlist->vind=v;
		size++;
	}
	else
	{
		aux=vertexlist;
		while(aux->next!=NULL)
			aux=(MyBodyVertexList*)aux->next;
		aux->next=new MyBodyVertexList;
		((MyBodyVertexList*)aux->next)->next=NULL;
		((MyBodyVertexList*)aux->next)->vind=v;
		size++;
	}
}

void BodyVertexGroup::StoreArray()
{
	MyBodyVertexList* aux;
	aux=vertexlist;
	vertexarray=new int[size];
	for(int i=0; i<size; i++)
	{
		vertexarray[i]=aux->vind;
		aux=(MyBodyVertexList*)aux->next;
	}
}

BodyVertexGroup::~BodyVertexGroup()
{

}
