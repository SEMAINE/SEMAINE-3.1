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

// BAPwriter.h: interface for the BAPwriter class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#pragma warning( disable : 4786 ) 
#endif // _MSC_VER > 1000


#include "FAPframe.h"
#include <string>

/**
* class :FAPwriter
*
*/

class FAPwriter  
{
public:

	/**
	*  
	* 
	*
	* @return 
	* @param frames
	* @param str
	*/

	int WriteToString(FAPFrameVector *frames, std::string **str);

	/**
	*  
	* 
	*
	* @return 
	* @param frames
	* @param  std::string fapfilename
	* @param  fps
	*/

	int WriteToFile(FAPFrameVector *frames, std::string fapfilename,int fps);

	/**
	*  
	* 
	*
	* @return 
	* @param frames
	* @param  int fap
	*/

	std::string FAPwriter::FapFrameToString(FAPFrameVector *frames, int fap);

	/**
	* contructor 
	*
	*/

	FAPwriter();

	/**
	* destructor 
	*/

	virtual ~FAPwriter();

};
