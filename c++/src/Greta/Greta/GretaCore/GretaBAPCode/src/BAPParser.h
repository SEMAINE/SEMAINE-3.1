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

// BAPParser.h: interface for the BAPParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAPPARSER_H__EE87FA0F_DC12_453A_8805_88B67ABBF74A__INCLUDED_)
#define AFX_BAPPARSER_H__EE87FA0F_DC12_453A_8805_88B67ABBF74A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning( disable : 4786 ) 
#endif // _MSC_VER > 1000


#include "BAPFrame.h"
#include "BAPConverter.h"
//include "EmoteParameters.h"
//fwd declaration:
/**
* class :EmoteParameters;
*
*/

class EmoteParameters;

/**
* class :BAPParser
*
*/

class BAPParser  
{
public:

	/**
	* 
	*
	* @return 
	* @param str
	* @param pFrames
	* @param pEmoteParams
	*/

	int ParseString(string *str, BAPFrameVector *pFrames,EmoteParameters *pEmoteParams);

	/**
	* 
	*
	* @return 
	* @param filename
	* @param pFrames
	* @param pEmoteParams
	*/

	int ParseFile(string filename, 
		BAPFrameVector *pFrames,
		EmoteParameters *pEmoteParams);

	/**
	*  
	* 
	*
	* @return 
	* @param filename
	* @param pFrames
	*/

	int ParseBuffer(string filename, 
		BAPFrameVector *pFrames);

	int fps;

	/**
	* contructor 
	*
	*/

	BAPParser();

	/**
	* destructor 
	*/
	virtual ~BAPParser();

private:
	BAPConverter converter;

};

#endif // !defined(AFX_BAPPARSER_H__EE87FA0F_DC12_453A_8805_88B67ABBF74A__INCLUDED_)
