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

// ListenerData.h: interface for the ListenerData class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include "IniManager.h"
#include "AdaptRules.h"
#include "BCRules.h"
#include "XMLGenericTree.h"


/**
* class :ListenerData
*
*/

class ListenerData  
{

private:

	/**
	*  
	* 
	*
	* @return 
	* @param  filepath
	*/

	int ListenerData::LoadInputAdaptRules(std::string filepath);

	/**
	*  
	* 
	*
	* @return 
	* @param  children
	* @param oper
	*/

	int ListenerData::LoadOperation(std::list<XMLGenericTree*>::iterator children, Operations *oper);

	/**
	*  
	* 
	*
	* @return 
	* @param  filepath
	*/

	int ListenerData::LoadBackchannelRules(std::string filepath);
	int ListenerData::LoadBackchannelComplexRules(std::string filepath);
	void ListenerData::GenerateCombinationRules();
	bool ListenerData::isRulePresent(BCComplexRules *rule);

	/**
	*  
	* 
	*
	* @return 
	* @param  filepath
	*/

	int ListenerData::LoadAgentState(std::string filepath);

	/**
	*  
	* 
	*
	* @return 
	* @param  s
	*/

	std::string ListenerData::ToUpper(std::string s);

public:


	/**
	* contructor 
	*
	*/

	ListenerData();

	/**
	* destructor 
	*/

	virtual ~ListenerData();


	/**
	*  
	* 
	*
	*/
	void ListenerData::LoadAll(std::string data_repertory);



	std::map<std::string, BCRules> mapBackchannelRules;
	std::map<std::string, BCComplexRules> mapBackchannelComplexRules;
	std::map<std::string, AdaptRules> mapAdaptRules;
	std::string agentstate;
};
