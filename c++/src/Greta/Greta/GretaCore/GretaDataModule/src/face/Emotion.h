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

// Emotion.h: interface for the Emotion class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

/**
* class :Emotion
*
*/

class Emotion 
{
private:

	std::string name;

	std::string type;

	float evaluation;

	float intensity;

public:


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
	* @param  name1
	*/

	void setName (std::string name1);

	/**
	*  
	* 
	*
	* @return 
	*/

	float getEvaluation();

	/**
	*  
	* 
	*
	* @param  evaluation1
	*/

	void setEvaluation (float evaluation1);


	/**
	*  
	* 
	*
	* @return 
	*/

	float getIntensity();


	/**
	*  
	* 
	*
	* @param  intensity1
	*/

	void setIntensity (float intensity1);


	/**
	* contructor 
	*
	*/

	Emotion();

	/**
	*  
	* 
	*
	* @return 
	* @param  name1
	* @param  float evaluation1
	*/

	Emotion(std::string name1, float evaluation1);

	/**
	*  
	* 
	*
	* @return 
	* @param  name1
	*/

	Emotion(std::string name1);


	/**
	* destructor 
	*/

	virtual ~Emotion();


	/**
	*  
	* 
	*
	* @param  type1
	*/

	void setType(std::string type1);	

	/**
	*  
	* 
	*
	* @return 
	*/

	std::string getType();	
};


