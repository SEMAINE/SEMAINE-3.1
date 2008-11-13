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

// GestureTree.cpp: implementation of the GestureTree class.
//
//////////////////////////////////////////////////////////////////////
#include "GestureTree.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GestureTree::GestureTree(char* name)

{
	FirstSon=NULL;
	Parent=NULL;
	NextBrother=NULL;
	StartTime=0;
	EndTime=0;
	Activation=1.0f;
	Requested=false;
	strcpy(Type,"");
	strcpy(Description,name);
}

int GestureTree::GetSonsNumber()
{
	GestureTree* s;
	int count=0;
	s=FirstSon;
	while(s!=NULL)
	{
		count++;
		s=s->NextBrother;
	}
	return count;
}

GestureTree* GestureTree::GetSonN(int n)
{
	GestureTree* s;
	int count=1;
	s=FirstSon;
	while((s!=NULL)&&(count<n))
	{
		count++;
		s=s->NextBrother;
	}
	if(count==n)return s;
	else return NULL;
}

GestureTree* GestureTree::GetParent()
{
	return Parent;
}

GestureTree* GestureTree::GetNextBrother()
{
	return NextBrother;
}

bool GestureTree::GetRequested()
{
	return Requested;
}

void GestureTree::AddSon(GestureTree* s)
{
	if(FirstSon==NULL)
	{
		FirstSon=s;
		s->NextBrother=NULL;
		s->Parent=this;
	}else
	{
		GestureTree* t=FirstSon;
		while(t->NextBrother!=NULL)
			t=t->NextBrother;
		t->NextBrother=s;
		s->NextBrother=NULL;
		s->Parent=this;
	}
}

double GestureTree::GetStartTime()
{
	return StartTime;
}

double GestureTree::GetEndTime()
{
	return EndTime;
}

char* GestureTree::GetDescription()
{
	return Description;
}

char* GestureTree::GetType()
{
	return Type;
}

void GestureTree::SetStartTime(double t)
{
	StartTime=t;
}

void GestureTree::SetEndTime(double t)
{
	EndTime=t;
}

void GestureTree::SetDescription(char* name)
{
	strcpy(Description,name);
}

void GestureTree::SetType(char* name)
{
	strcpy(Type,name);
}

void GestureTree::SetRequested(bool r)
{
	GestureTree* g;
	Requested=r;
	if(GetSonN(1)!=NULL)
	{
		g=GetSonN(1);
		while(g!=NULL)
		{
			g->SetRequested(r);
			g=g->GetNextBrother();
		}
	}
}

GestureTree::~GestureTree()
{

}

void GestureTree::SetActivation(float i)
{
	Activation=i;
}

float GestureTree::GetActivation()
{
	return Activation;
}
