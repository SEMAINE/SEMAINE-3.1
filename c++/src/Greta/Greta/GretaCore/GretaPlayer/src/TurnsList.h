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

// TurnsList.h: interface for the TurnsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TURNSLIST_H__E35BA22F_EE55_4408_B32B_A0E2C395673C__INCLUDED_)
#define AFX_TURNSLIST_H__E35BA22F_EE55_4408_B32B_A0E2C395673C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
*
* class :TurnsList
*
*/

class TurnsList  
{
public:

	/**
	*
	* contructor 
	*
	* @param  name
	* @param  t
	*/

	TurnsList(char* name,int t);

	/**
	* this method 
	* 
	*
	* @param  name
	* @param  t
	*/

	void TurnsList::Add(char* name,int t);

	/**
	* destructor 
	*/

	virtual ~TurnsList();

	TurnsList* prev;

	TurnsList* next;
	char filename[256];
	int time;
	bool executed;
};

#endif // !defined(AFX_TURNSLIST_H__E35BA22F_EE55_4408_B32B_A0E2C395673C__INCLUDED_)
