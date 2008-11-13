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

// 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "ERLParser.h"
#include "ERLTree.h"
#include "APMLParser.h"
#include "APMLTree.h"

/**
*
* class :ERL_AgentEngine
*
*/

class ERL_AgentEngine
{
public:

	/**
	* contructor 
	*
	*/

	ERL_AgentEngine(void);

	/**
	* destructor 
	*/

	~ERL_AgentEngine(void);


	/**
	* this method 
	* 
	*
	* @return 
	* @param  ERLfilename
	*/

	int Compute(std::string ERLfilename);

	std::string APMLfilename,libraryfilename,agentdefinitionfilename,ERLfilename;

	ERLTree *ERLtree;


};
