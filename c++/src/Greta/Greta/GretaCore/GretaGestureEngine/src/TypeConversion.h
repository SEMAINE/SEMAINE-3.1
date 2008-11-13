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

/// @file TypeConversion.h
/// interface for the TypeConversion class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TYPECONVERSION_H__0350147E_6105_480E_AD38_C5C1BFB4EFA7__INCLUDED_)
#define AFX_TYPECONVERSION_H__0350147E_6105_480E_AD38_C5C1BFB4EFA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include <stdlib.h>
#include <string>


/**
* class :TypeConversion
*
* Conversions between simple types. Needed because of an unresolved 
* "ambiguous call to overloaded function" error when using atof()
* directly inside class skeleton. 
*/

class TypeConversion  
{
public:

	/**
	*  
	* 
	*
	* @return 
	* @param  s
	*/

	std::string ToLower(std::string s);

	/**
	*  
	* 
	*
	* @return 
	* @param  s
	*/

	std::string ToUpper(std::string s);


	/**
	*  
	* 
	*
	* @return 
	*/

	/** like atof(): string to double/float conversion */

	/**
	*  
	* 
	*
	* @return 
	* @param c
	*/

	double Atof(const char *c);

	/**
	*
	* contructor 
	*
	*/

	TypeConversion();

	/**
	*
	* destructor 
	*/

	virtual ~TypeConversion();

};

#endif // !defined(AFX_TYPECONVERSION_H__0350147E_6105_480E_AD38_C5C1BFB4EFA7__INCLUDED_)
