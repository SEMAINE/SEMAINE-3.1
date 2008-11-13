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

// BAPAction.cpp: implementation of the BAPAction class.
//
//////////////////////////////////////////////////////////////////////

//include "stdafx.h"
#include "BAPAction.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BAPAction::BAPAction()
{
	m_BAPFrames=NULL;
}

BAPAction::BAPAction(string name, string type, string actor, BAPFrameVector *v)
{
	m_name=name;
	m_type=type;
	m_actor=actor;
	//check if this is correct:
	m_BAPFrames = v;
}

BAPAction::~BAPAction()
{
	delete m_BAPFrames;
}

const char * BAPAction::GetActionName()
{
	return m_name.c_str();
}

const char * BAPAction::GetActionType()
{
	return m_type.c_str();
}

const char * BAPAction::GetActor()
{
	return m_actor.c_str();
}


BAPFrameVector * BAPAction::GetAnimationPtr()
{
	return m_BAPFrames;
}


void BAPAction::SetActionName(char *name)
{
	m_name=string(name);
}


void BAPAction::SetActionType(char *type)
{
	m_type=string(type);
}


void BAPAction::SetActor(char *actorname)
{
	m_actor=string(actorname);
}
