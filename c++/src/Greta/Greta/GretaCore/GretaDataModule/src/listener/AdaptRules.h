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

// AdaptRules.h: interface for the AdaptRules class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include "InputData.h"


/*! \brief generation of AdaptRules
	*
	*
	*
*/


class Operations
{
public:
	/** 
    * Class contructor.
    * 
    */
	Operations(void);
	/** 
    * Class destructor.
    * 
    */
	~Operations(void);

	void Operations::ComputeOperation(InputData *data);

	std::string parameter;
	std::string op;
	std::string value;
};

class Implications
{
public:
	/** 
    * Class contructor.
    * 
    */
	Implications(void);
	/** 
    * Class destructor.
    * 
    */
	~Implications(void);

	bool Implications::EvaluateImplication(InputData *data);

	std::vector<Operations> vecCondiction;
	std::vector<Operations> vecConsequence;
};

class AdaptRules
{
public:
	AdaptRules();
	virtual ~AdaptRules();
	void AdaptRules::PrintRule();

	std::string name;

	std::vector<Implications> vecImplication;
	std::vector<Operations> vecModify;
};

