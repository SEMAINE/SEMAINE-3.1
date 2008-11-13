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

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  Contains informations on a Modulation.
 *  A Modulation is a mathemetical function applied on some numerical values and/or
 *  attributes of the communicative modalities and its result is stored in an attribute
 *  of a communicative modality. The terms of the Modulation (called operands) can
 *  be 1, 2 or 3 depending on the @ref Operator.
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class Modulation
{
public:
	/** 
    * Class contructor.
    * 
    */
	Modulation(void);
	/** 
    * Class destructor.
    * 
    */
	~Modulation(void);
	/** 
    * The attribute in which the result of the Modulation is stored.
	* The format of the string is modality.attribute. For example
	* to refer to the Spatial value of the face modality: face.SPC.
	* Allowed modalities and attributes are listed in the file @link global.xsd.
    * 
    */
	std::string destination;
	/** 
    * The operator that will be used in the Modulation.
	* Allowed operators are listed in the file @link global.xsd.
    * 
    */
	std::string Operator;
	/** 
    * The operands of the Modulation. 
	* Allowed operators are listed in the file @link global.xsd.
    * 
    */
	std::string Operand1,Operand2,Operand3;
};

/** 
* List of Modulation objects.
* 
*/
typedef std::list<Modulation> ModulationList;

/** 
* Search map of ModulationList objects.
* 
*/

typedef std::map<std::string,ModulationList> ModulationMap;

/**@#-*/
}
/**@#+*/