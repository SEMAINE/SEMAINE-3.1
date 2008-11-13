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


// Exception.h: interface for the Exception class.
//
//////////////////////////////////////////////////////////////////////
// created : 21-05-08  by : Radek  (niewiadomski@iut.univ-paris8.fr)

//modifications:
//

#pragma once
#include <string>

/**
*
* class :Exception
*
*/

class Exception 
{

public:


	/**
	*
	* contructor 
	*
	*/

	Exception();

	/**
	* this method 
	* 
	*
	* @return 
	* @param  message
	*/

	Exception(std::string message);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  message
	* @param  int code
	*/

	Exception(std::string message, int code);

	/**
	*
	* destructor 
	*/

	virtual ~Exception();

private:

	std::string message;
	int code;


	/**
	* this method 
	* 
	*
	* @return 
	*/

	std::string getMessage();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	int getCode();

};


