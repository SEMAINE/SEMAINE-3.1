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

// Expression.h: interface for the Expression class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
// 
// File:   Expression.h
// Author: Administrateur
//
// Created on 11 février 2008, 17:20

#include "FaceExpression.h"

#include<string>


/**
* class :Expression
*
*/

class Expression 
{
private:

	float time_min;
	float time_max;
	std::string name;
	std::string emotion;
	std::string profile;
	float intensity_min;
	float intensity_max;
	int faps [69];    
	//int *faps;

public:


	/**
	* contructor 
	*
	*/

	Expression();

	/**
	* destructor 
	*/

	virtual ~Expression();



	/**
	*  
	* 
	*
	* @return 
	*/

	float getTimeMin();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getTimeMax();

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

	std::string getEmotionName();

	/**
	*  
	* 
	*
	* @return 
	*/

	std::string getProfileName();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getIntensityMin();

	/**
	*  
	* 
	*
	* @return 
	*/

	float getIntensityMax();

	/**
	*  
	* 
	*
	* @return 
	*/

	int *getFaps();

	/**
	*  
	* 
	*
	* @return 
	* @param  fap1
	*/

	int getFapValue(int fap1);


	/**
	*  
	* 
	*
	* @param  intensity
	*/

	void rescaleIntensity(float intensity);

	/**
	*  
	* 
	*
	* @param  timemin
	*/

	void setTimeMin(float timemin);

	/**
	*  
	* 
	*
	* @param  timemax
	*/

	void setTimeMax(float timemax);

	/**
	*  
	* 
	*
	* @param  label
	*/

	void setName(std::string label);

	/**
	*  
	* 
	*
	* @param  label
	*/

	void setEmotionName(std::string label);

	/**
	*  
	* 
	*
	* @param  label
	*/

	void setProfileName(std::string label);

	/**
	*  
	* 
	*
	* @param  intmin
	*/

	void setIntensityMin(float intmin);

	/**
	*  
	* 
	*
	* @param  intmax
	*/

	void setIntensityMax(float intmax);

	/**
	*  
	* 
	*
	* @param faps1
	*/

	void  setFaps(int *faps1);

	/**
	*  
	* 
	*
	* @param  fap1
	* @param  int value1
	*/

	void setFap(int fap1, int value1);

	/**
	* contructor 
	*
	*/

	FaceExpression* getFaceExpression();

};
