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
#include <list>
#include <map>
#include "Signal.h"
#include "MultimodalSignal.h"
#include "Implication.h"

/**@#-*/
namespace MMSystemSpace {
/**@#+*/


/**
 * 
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class BehaviorSet
{
public:
	/** 
    * Class contructor.
    * 
    */
	BehaviorSet(void);
	/** 
    * Class destructor.
    * 
    */
	~BehaviorSet(void);

	void ApplyCore(SignalVector n);

	void ApplyImplications(ImplicationList n);

	/**
	* The name of the behavior set.
	*/
	std::string name;
	/**
	* The vertor of base signal.
	*/
	std::vector<Signal> basesimplesignals;
	/**
	* The vertor of multimodals signal.
	*/
	std::vector<MultimodalSignal> multimodalsignals;

	void PrintMMSignals(FILE *outfile);
	
};

/** 
* List of Signal objects.
* 
*/
typedef std::map<std::string,BehaviorSet> BehaviorSetMap;

/**@#-*/
}
/**@#+*/