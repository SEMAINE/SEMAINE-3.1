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

/// @file GestureList.h
/// interface for the GestureList class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESTURELIST_H__7A46F691_A25D_4640_94C0_C3CBC5C880E5__INCLUDED_)
#define AFX_GESTURELIST_H__7A46F691_A25D_4640_94C0_C3CBC5C880E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include "GestureTree.h"

/** 
* class :GestureList
* Linked list structure used for scheduling gestures in TurnPlanner.
* @see TurnPlanner
* @author Maurizio Mancini
*/

class GestureList  
{
public:

	/**
	* contructor 
	*
	*/

	GestureList();

	/**
	* contructor 
	*
	* @param  i
	*/

	GestureList(GestureTree* i);

	/**
	* contructor 
	*
	* @param  i
	* @param  l
	*/

	GestureList(GestureTree* i,GestureList* l);

	/**
	* destructor 
	*/

	virtual ~GestureList();
	GestureTree* item;

	/**
	* next element
	*
	*/

	GestureList* next;

	/**
	* prevoius element
	*
	*/

	GestureList* prev;
};

#endif // !defined(AFX_GESTURELIST_H__7A46F691_A25D_4640_94C0_C3CBC5C880E5__INCLUDED_)
