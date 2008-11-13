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

/// @file GestureTree.h
/// interface for the GestureTree class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESTURETREE_H__11687800_B78F_4A51_BEF5_230420CC7266__INCLUDED_)
#define AFX_GESTURETREE_H__11687800_B78F_4A51_BEF5_230420CC7266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

/** 
* class :GestureTree
* Tree structure used for scheduling gestures in TurnPlanner.
* @see TurnPlanner
* @author Maurizio Mancini
*/

class GestureTree  
{
public:

	/**
	* contructor 
	*
	* @param  name
	*/

	GestureTree(char* name);

	/**
	* destructor 
	*/

	virtual ~GestureTree();

	/**
	*  
	* 
	*
	* @return 
	*/

	int GetSonsNumber();

	/**
	*
	* contructor 
	*
	* @param  n
	*/

	GestureTree* GetSonN(int n);

	/**
	* contructor 
	*
	*/

	GestureTree* GetParent();

	/**
	* contructor 
	*
	*/

	GestureTree* GetNextBrother();

	/**
	* contructor 
	*
	* @param  s
	*/


	/**
	*  
	* 
	*
	* @param  s
	*/

	void AddSon(GestureTree* s);

	/**
	*  
	* 
	*
	* @return 
	*/

	double GetStartTime();

	/**
	*  
	* 
	*
	* @return 
	*/

	double GetEndTime();

	/**
	*  
	* 
	*
	* @return 
	*/

	char* GetDescription();

	/**
	*  
	* 
	*
	* @return 
	*/

	char* GetType();

	/**
	*  
	* 
	*
	* @return 
	*/

	bool GetRequested();

	/**
	*  
	* 
	*
	* @param  t
	*/

	void SetStartTime(double t);

	/**
	*  
	* 
	*
	* @param  t
	*/

	void SetEndTime(double t);

	/**
	*  
	* 
	*
	*/

	void SetDescription(char*);

	/**
	*  
	* 
	*
	* @param  name
	*/

	void SetType(char* name);

	/**
	*  
	* 
	*
	* @param  r
	*/

	void SetRequested(bool r);


	/**
	*  
	* 
	*
	* @return 
	*/

	float GetActivation(void);

	/**
	*  
	* 
	*
	* @param  i
	*/

	void SetActivation(float i);

private:

	/**
	* 
	*
	*/
	GestureTree* FirstSon;

	/**
	*
	*
	*/
	GestureTree* Parent;

	/**
	*
	*
	*/
	GestureTree* NextBrother;

	char Type[20];
	char Description[250];
	double StartTime,EndTime;
	bool Requested;
	float Activation;
};

#endif // !defined(AFX_GESTURETREE_H__11687800_B78F_4A51_BEF5_230420CC7266__INCLUDED_)
