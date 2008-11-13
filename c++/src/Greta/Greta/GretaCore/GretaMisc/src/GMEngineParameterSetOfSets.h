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
#include <iostream>
#include <utility>
#include <string>
#include <map>
#include "EngineParameter.h"
#include "GMEngineParameterSet.h"


/**
 *  A set of behavior EngineParameterSetOfSets.
 *  Behavior EngineParameterSetOfSets are the channels involved in the communication between humans.
 *  In the present system we use five EngineParameterSetOfSets (excluding speech): face, head, gaze, gestures, body.
 *  This class is an extension of the stl#map class.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class GMEngineParameterSetOfSets
{
public:
	/** 
    * Class contructor.
    * 
    */
	GMEngineParameterSetOfSets();
	/** 
    * Class destructor.
    * 
    */
	~GMEngineParameterSetOfSets();
	/** 
    * It returns the pointer to the EngineParameterSet specified in the parameter.
    * 
    */
	GMEngineParameterSet * GetEngineParameterSet(std::string name);
	/** 
    * It returns the value of the attribute specified in the parameter.
	* @param name Is the parameter name in the form: EngineParameterSetname.parameter.attribute
    * 
    */
	float GetAttrValue(std::string name);
	/*
	* It's add a parameter to the map of parameter
	*/
	int addParameter(std::string name,GMEngineParameterSet param);
	/** 
    * It loads the definition of the EngineParameterSetOfSets from a file.
	* The format of this file is defined in @link profiles.xsd
    * 
    */
	int LoadFromFile(std::string filepath);

	int LoadFromBuffer(std::string buffer);


	// Attribute :
	// The map of parameter :
	std::map<std::string,GMEngineParameterSet> listOfParameters;

};

/** 
* Name definition, the name Baseline is just a synonym of EngineParameterSetOfSets.
* 
*/
typedef GMEngineParameterSetOfSets GMBaseline;

/** 
* Name definition, the name Dynamicline is just a synonym of EngineParameterSetOfSets.
* 
*/
typedef GMEngineParameterSetOfSets GMDynamicline;
