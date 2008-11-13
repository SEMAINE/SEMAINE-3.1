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

/**
*  Contains informations on an expressivity parameter.
*  An expressivity parameter is a floating point number that influence some physical
*  characteristic of the agent's movement.
*  @author Maurizio Mancini
*  @date 2007
*/

#pragma once
#include <string>
#include <map>


/**
*
* class :EngineParameter
*
*/

class EngineParameter
{
public:



	/**
	*
	* contructor 
	*
	*/

	EngineParameter(void);


	/**
	*
	* contructor 
	*
	* @param  new_lower
	* @param  float new_upper
	*/

	EngineParameter(float new_lower, float new_upper);


	/**
	*
	* contructor 
	*
	* @param  new_lower
	* @param  float new_center
	* @param  float new_upper
	*/

	EngineParameter(float new_lower, float new_center, float new_upper);


	/**
	*
	* destructor 
	*/

	~EngineParameter(void);


	/** 
	* Name of the expressivity parameter.
	* 
	*/
	std::string name;

	/**
	* this method 
	* 
	*
	* @param  new_lower
	* @param  float new_upper
	*/

	void SetScaling(float new_lower, float new_upper);

	/**
	* this method 
	* 
	*
	* @param  new_lower
	* @param  float new_center
	* @param  float new_upper
	*/

	void SetScaling(float new_lower, float new_center, float new_upper);

	/**
	* this method 
	* 
	*
	* @param  new_value
	*/

	void SetValue(float new_value);

	float GetValue(){ return value; };
	float *GetValueAddr(){ return &value; };
	float GetScaledValue(){ return scaled_value; };
	float *GetScaledValueAddr(){ return &scaled_value; };

private:

	/**
	* this method 
	* 
	*
	* @param  new_lower
	* @param  float new_center
	* @param  float new_upper
	*/

	void InitValue(float new_lower, float new_center, float new_upper);

	float lower, center, upper;

	/** 
	* Value of the expressivity parameter.
	*/
	float value;
	float scaled_value;


	/**
	* this method 
	* 
	*
	*/

	void RescaleValue();
};

/** 
* Search map of EngineParameter objects.
* 
*/
typedef std::map<std::string,EngineParameter> EngineParameters;
