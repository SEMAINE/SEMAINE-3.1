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

// ConstraintSignal.h: interface for the ConstraintSignal class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <string>

/**
* class :ConstraintSignal
*
*/

class ConstraintSignal 
{

private:

	int id;
	std::string name;

	float repetivity;
	float probability_start;
	float probability_end;
	float occurence;
	float max_duration;
	float min_duration;

public:


	/**
	* contructor 
	*
	*/

	ConstraintSignal();

	/**
	* destructor 
	*/

	virtual ~ConstraintSignal();


	/**
	* contructor 
	*
	*/

	ConstraintSignal clone();


	/**
	*  
	* 
	*
	* @return 
	*/

	std::string getName();

	/**
	*  
	* 
	*
	* @return 
	*/

	int getId();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getRepetivity();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getProbability_start();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getProbability_end();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getOccurence();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getMax_duration();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getMin_duration();


	/**
	*  
	* 
	*
	* @param  name1
	*/

	void setName(std::string name1);

	/**
	*  
	* 
	*
	* @param  id1
	*/

	void setId(int id1);

	/**
	*  
	* 
	*
	* @param  repetivity1
	*/

	void setRepetivity(float repetivity1);

	/**
	*  
	* 
	*
	* @param  probability_start1
	*/

	void setProbability_start(float probability_start1);

	/**
	*  
	* 
	*
	* @param  probablility_end1
	*/

	void setProbability_end(float probablility_end1);

	/**
	*  
	* 
	*
	* @param  occurence1
	*/

	void setOccurence(float occurence1);

	/**
	*  
	* 
	*
	* @param  max_duration1
	*/

	void setMax_duration(float max_duration1);

	/**
	*  
	* 
	*
	* @param  min_duration1
	*/

	void setMin_duration(float min_duration1);

};
