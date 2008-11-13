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

// BCRules.h: interface for the BCRules class.
//
//////////////////////////////////////////////////////////////////////


#pragma once


#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include <map>


/** \brief generation of Rules
*
*
*
*/


class BCRules
{
public:
	/** 
	* constructor
	*
	*/
	BCRules();
	/** 
	* destructor 
	*
	*/
	virtual ~BCRules();

	/** 
	*
	*
	*/
	void BCRules::PrintRule();

	std::string name;

	std::map<std::string, std::string> mapParameters;
};

class Sig
{
public:
	/** 
	* constructor
	*
	*/
	Sig(){}
	/** 
	* destructor 
	*
	*/
	virtual ~Sig(){}

	std::string name;
	std::string modality;
	int id;
	float alternative_percentage;
};

class BCComplexRules
{
public:
	/** 
	* constructor
	*
	*/
	BCComplexRules();
	/** 
	* destructor 
	*
	*/
	virtual ~BCComplexRules();

	/** 
	*
	*
	*/
	void BCComplexRules::PrintRule();

	std::string name;

	float response_probability;
	int response_priority;

	float reactive_probability;
	float signals_probability;

	//std::map<std::string, std::string> usersignalsVec;
	std::vector<Sig> usersignalsVec;
	std::vector<Sig> signalVec; //mimicry or specific signals
};

