//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// C_Assymetry.h: 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../utils/CemGlobals.h"
#include "Emotion.h"

/**
* class :C_Assymetry
*
*/

class C_Assymetry 
{

	double onthelist[12][CEM_MAX_FAP];
	int directions[CEM_MAX_FAP];

	//unknown
	//1 left
	//2 right

	//first dimension emotions
	//second dimension fap

public:


	/**
	* contructor 
	*
	*/

	C_Assymetry() ;	

	/**
	* destructor 
	*/

	virtual ~C_Assymetry() ;    

	/**
	* adds assymetry to the array of fap values for expression descibed by id
	* @param id expression id
	* @return new array of faps
	*/

	void update(int **expression, Emotion *e);

};


