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

// BAPAction.h: interface for the BAPAction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAPACTION_H__65540171_089D_40F5_B56E_47D919D29150__INCLUDED_)
#define AFX_BAPACTION_H__65540171_089D_40F5_B56E_47D919D29150__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BAPFrame.h"


/**
* class :BAPAction
*
*/

class BAPAction  
{
public:

	/**
	* contructor 
	*
	* @param  name
	* @param  string type
	* @param  string actor
	* @param v
	*/

	BAPAction(string name, string type, string actor, BAPFrameVector *v);

	/**
	*
	* @param actorname
	*/

	void SetActor(char *actorname);

	/**
	* 
	*
	* @param type
	*/

	void SetActionType(char *type);

	/**
	* 
	*
	* @param name
	*/

	void SetActionName(char *name);

	/**
	* 
	*
	* @return 
	*/

	BAPFrameVector * GetAnimationPtr();

	/**
	* 
	*
	* @return 
	*/

	const char * GetActor();

	/**
	* 
	*
	* @return 
	*/

	const char * GetActionType();

	/**
	* 
	*
	* @return 
	*/

	const char * GetActionName();


	/**
	* contructor 
	*
	*/

	BAPAction();

	/**
	* destructor 
	*/

	virtual ~BAPAction();
private:
	string m_name; //action name
	string m_type; //action type
	string m_actor; 
	BAPFrameVector * m_BAPFrames;
};

#endif // !defined(AFX_BAPACTION_H__65540171_089D_40F5_B56E_47D919D29150__INCLUDED_)
