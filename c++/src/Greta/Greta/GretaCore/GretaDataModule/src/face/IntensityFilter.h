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

// IntensityFilter.h: interface for the IntensityFilter class.
//
//////////////////////////////////////////////////////////////////////
// created on 12 février 2008:  by : Radek  (niewiadomski@iut.univ-paris8.fr)

//modifications:
//


#pragma once

#include "XMLGenericParser.h"
#include "FaceExpression.h"
#include "Emotion.h"

#include<string>


/**
* class :IntensityFilter
*
*/

class IntensityFilter 
{

private: 

	XMLGenericParser *xmlparser;
	std::list<Emotion*> emotionvector;

public:


	/**
	* contructor 
	*
	*/

	IntensityFilter();

	/**
	*  
	* 
	*
	* @return 
	* @param  filename
	*/

	int init(std::string filename);

	/**
	* destructor 
	*/

	virtual ~IntensityFilter();

	/**
	*  
	* 
	*
	* @return 
	* @param fe1
	* @param  float intensity
	* @param  float probability
	*/

	FaceExpression* modify(FaceExpression *fe1, float intensity, float probability);

	/**
	*  
	* 
	*
	*/

	void printList();

	/**
	*  
	* 
	*
	* @return 
	* @param  label
	*/

	Emotion* findEmotion(std::string label);
};


