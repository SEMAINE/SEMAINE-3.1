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

#define NUMBER_OF_FAPS 69

/*! \brief single FAP (Face Animation Parameter)
*
* this class defines an object that stores the value and the mask of a single FAP
* @see FAPFrame
* @author Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com
*/

class FAP
{
public:
	/*! \brief class constructor
	*
	*/
	FAP(void);
	/*! \brief class destructor
	*
	*/
	~FAP(void);
	/*! \brief the FAP mask
	*
	* in the MPEG4 standard each FAP has a mask which can be 0 or 1 which tells if the FAP
	* is active or not; if it is active then the value of the FAP has to be used
	* otherwise it has to be ignored
	*/
	bool active;
	/*! \brief the FAP value
	*
	*/
	int value;
};

