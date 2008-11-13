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

#pragma once

#include <vector>
/**
 *  @author Christopher Peters
 *  @date 2005
 */

class PersonPerceptSTSS
{
  public:

    /**
	* Percept constructor
	*/
    PersonPerceptSTSS(float body, float head, float eyes, float tS);

	/**
	* Update the percept
	*/
    void update();

	/**
	* The eye angle degree
	*/
    float eyeAngleDeg;
	/**
	* The head angle degree
	*/
    float headAngleDeg;
	/**
	* The body angle degree
	*/
    float bodyAngleDeg;

	/**
	* The eye attention level
	*/
    float attentionLevelEyes;
	/**
	* The eye angle degree
	*/
    bool eyesVisible;
	/**
	* The head attention level
	*/
    float attentionLevelHead;
	/**
	* The head angle degree
	*/
    bool headVisible;
	/**
	* The eye attention level
	*/
    float attentionLevelBody;
	/**
	* The eye angle degree
	*/
    bool bodyVisible;

    float attentionLevelMetric;
    bool MAMactivated;

    float ILcache;  //cache of the IL integrated between this percept and the previous one

    float distance;

    float timeStamp;

};