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

// C_DataContainer.h: 

//////////////////////////////////////////////////////////////////////

/*
* Created on 11 mai 2007, 17:11
*
*/


/**
* data container.
* it contains all the data
* @author Administrateur
*/

#pragma once

#include "./../structures/C_SchemaSection.h"
#include "CemGlobals.h"

#include "Expression.h"

#include <vector>

/**
* class :C_DataContainer
*
*/

class C_DataContainer  
{

private :

	//std::vector<XMLGenericTree*> blendingschemas = null;

	//int [][] faces=null;

	std::vector<Expression> *expressions;

	std::vector<C_SchemaSection> *sectionschemas;    	
	std::vector<C_SchemaSection> *divisions;
	std::vector<C_SchemaSection> *masking_divisions;

	int **facialareas;
	int **masking_facialareas;

	//fslimits[0][x]=start x;
	//fslimits[1][x]=stop x;

	int **fslimits;

public:


	/**
	* destructor 
	*/

	virtual ~C_DataContainer();

	/**
	* contructor 
	*
	*/

	C_DataContainer();        


	/**
	* gets THE division of the face in a case of superposition
	*
	* @return array of facial areas
	*/

	int  **getfacialareas();

	/**
	* gets the division of the face in a case of masking
	* @return array of facial areas
	*/

	int **getmasking_facialareas();

	//public int [][] getfaces(){return faces;}


	/**
	* gets an array of extreme values of faps
	* @return array of extreme values of faps
	*/

	int **getfs_limits();

	/**
	* gets a vector of section schemas
	* @return a vector of sectionschemas
	*/
	std::vector<C_SchemaSection> *getsectionschemas();

	//std::vector<Expression> *getexpressions();


	/**
	*  
	* 
	*
	*/

	void init();    

}//end of class
;
