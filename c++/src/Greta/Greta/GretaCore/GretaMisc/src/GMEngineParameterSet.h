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


/**
 *  Contains informations on an EngineParameterSet.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class GMEngineParameterSet
{
public:
	/** 
    * Class contructor.
    * 
    */
	GMEngineParameterSet();
	/** 
    * Class destructor.
    * 
    */
	~GMEngineParameterSet();
	
	/**
	* The method to add new parameters
	*/
	int addParameter(std::string name, EngineParameter parameter);

	/**
	* The method to get a parameter
	*/
	EngineParameter getParameter(std::string name);

	/** 
    * The name of the EngineParameterSet.
    * 
    */
	std::string name;
	/** 
    * A search map for the parameters of the EngineParameterSet.
    * 
    */
	EngineParameters mp;
	/** 
    * It returns the address in memory of the parameter specified in the name.
	* @param name the name of the attribute to search for.
	* The format of the name is: EngineParameterSetname.parameter.attribute
    * 
    */
	float *GetAttrAddress(std::string name);

};
