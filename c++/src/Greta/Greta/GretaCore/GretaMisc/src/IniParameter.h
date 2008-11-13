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
// IniParameter.h: interface for the IniParameter class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

/*! \brief contains the name and value of a single IniParameter
*
*  the IniParameter has been read from an ini file by the class IniManager
* @see IniManager
*/
class IniParameter  
{
public:
	/*! \brief IniParameter's name
	*
	*/
	std::string name;
	/*! \brief IniParameter's value
	*
	*/
	std::string value;
	/*! \brief class contructor
	*
	*/
	IniParameter();
	/*! \brief class destructor
	*
	*/
	virtual ~IniParameter();

};
