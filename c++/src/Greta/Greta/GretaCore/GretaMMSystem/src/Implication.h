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

/**@#-*/
namespace MMSystemSpace {
/**@#+*/

/**
 *  An implication is a rule contained in a BehaviorSet. With rules you can
 *  constraint the presence or absence of signals. For example one signal
 *  could be produced if another signal is produced and vice-versa.
 *  
 *  @author Maurizio Mancini manmau@yahoo.com
 *  @date 2007
 */
class Implication
{
public:
	/** 
    * Class contructor.
    * 
    */
	Implication(void);
	/** 
    * Class destructor.
    * 
    */
	~Implication(void);
	/** 
    * Contains the signals that constitute the first part of the
	* implication. Their presence in the multimodal signal is used
	* to constraint the presence or absence of other signals.
    * 
    */
	SignalVector ifpresent;
	/** 
    * These are the signals that must be present in the multimodal
	* signal if the signals in the Implication#ifpresent vector are present.
    * 
    */
	SignalVector thenpresent;
	/** 
    * These are the signals that must NOT be present in the multimodal
	* signal if the signals in the Implication#ifpresent vector are present.
    * 
    */
	SignalVector thennotpresent;
};

/** 
* A list of Implication objects.
* 
*/
typedef std::list<Implication> ImplicationList;

/**@#-*/
}
/**@#+*/