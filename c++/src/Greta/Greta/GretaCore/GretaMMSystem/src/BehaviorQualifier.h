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

#include <string>
#include "Modulation.h"

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  Contains informations on a BehaviorQualifier.
 *  A Modulation corresponds to the set of modulations (mathematical operations)
 *  that have to be performed over the parameters of
 *  the Baseline when a certain CommunicativeAct is encountered. So this object contains a search map
 *  of operations.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class BehaviorQualifier
{
public:
	/** 
    * Class contructor.
    * 
    */
	BehaviorQualifier(void);
	/** 
    * Class destructor.
    * 
    */
	~BehaviorQualifier(void);
	/** 
    * Name of the CommunicativeAct associated to this Modulation.
    * 
    */
	std::string name;
	/** 
    * This is the search map of the modulations contained in the Modulation.
	* The map is made by lists of modulations, that is, each map element is a list
	* of modulations. The operations associated to the same attribute of the Baseline
	* are linked in a list and only the head of the list is kept in the search map.
    * 
    */
	ModulationMap modulations;
	/** 
    * This method adds a modulation to the search map of operations.
    * @param Modulation mod the modulation to add
    */
	int AddModulation(Modulation mod);
};

/** 
* List of Modulation objects.
* 
*/
typedef std::list<BehaviorQualifier> BehaviorQualifierList;

/** 
* Search map of Modulation objects.
* 
*/
typedef std::map<std::string,BehaviorQualifier> BehaviorQualifierMap;

/**@#-*/
}
/**@#+*/