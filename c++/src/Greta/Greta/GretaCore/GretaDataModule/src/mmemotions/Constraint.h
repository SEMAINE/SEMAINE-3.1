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

// Constraint.h: interface for the Constraint class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include <string>

/**
* class :Constraint
*
*/

class Constraint 
{

private:

	std::string type;
	std::string range;

	int arg1_id;
	int arg2_id;

	//1 start, 2 stop
	int arg1_type;
	int arg2_type;

	bool lessthan;
	bool morethan;
	bool equal;

	float value;

public:


	/**
	* contructor 
	*
	*/

	Constraint ();

	/**
	* destructor 
	*/

	virtual ~Constraint ();


	/**
	*  
	* 
	*
	* @param  type1
	*/

	void setConstraintType(std::string type1);

	/**
	*  
	* 
	*
	* @param  range1
	*/

	void setConstraintRange(std::string range1);


	/**
	*  
	* 
	*
	* @return 
	*/

	std::string getConstraintType();

	/**
	*  
	* 
	*
	* @return 
	*/

	std::string getConstraintRange();


	/**
	*  
	* 
	*
	* @param  id1
	* @param  type1
	*/

	void setArg1(int id1,int type1);

	/**
	*  
	* 
	*
	* @param  id1
	* @param  type1
	*/

	void setArg2(int id1,int type1);


	/**
	*  
	* 
	*
	* @return 
	*/

	int getArg1Id();

	/**
	*  
	* 
	*
	* @return 
	*/

	int getArg2Id();


	/**
	*  
	* 
	*
	* @return 
	*/

	int getArg1Type();

	/**
	*  
	* 
	*
	* @return 
	*/

	int getArg2Type();


	/**
	*  
	* 
	*
	*/

	void setLessThan();

	/**
	*  
	* 
	*
	*/

	void setMoreThan();

	/**
	*  
	* 
	*
	*/

	void setEqual();


	/**
	*  
	* 
	*
	* @return 
	*/

	bool isLessThan();

	/**
	*  
	* 
	*
	* @return 
	*/

	bool isMoreThan();

	/**
	*  
	* 
	*
	* @return 
	*/

	bool isEqual();


	/**
	*  
	* 
	*
	* @param  value1
	*/

	void setValue(float value1);

	/**
	*  
	* 
	*
	* @return 
	*/

	float getValue();

};
