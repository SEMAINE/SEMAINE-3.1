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

// AttentionInteresetModel.h: interface for the AttentionInteresetModel class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "MemoryRecord.h"

/**
*
* class :AttentionInterestModel
*
*/


class AttentionInterestModel
{

public:

/**
*
* contructor 
*
*/

	AttentionInterestModel(void);
	
/**
*
* destructor 
*/

	~AttentionInterestModel(void);

/**
 *
 *
* @param  bodyDegrees
* @param  float headDegrees
* @param  float eyeDegrees
* @param  float currentTime
*/

	void addRecord(float bodyDegrees, float headDegrees, float eyeDegrees, float currentTime);

/**
 *
 *
* @return 
* @param  timeStamp
*/

	MemoryRecord* queryRecord(float timeStamp);

/**
 *
 *
* @return 
* @param  listNumber
*/

	MemoryRecord* queryRecord(int listNumber);

/**
 *
 *
* @return 
* @param  timeStamp
*/

	float getAttentionLevel(float timeStamp);

/**
 *
 *
* @return 
* @param  timeInterval
* @param  float currentTime
*/

	float getInterestLevelBack(float timeInterval, float currentTime);

/**
 *
 *
* @return 
* @param  timeInterval1
* @param  float timeInterval2
*/

	float getInterestLevelInterval(float timeInterval1, float timeInterval2);

};
