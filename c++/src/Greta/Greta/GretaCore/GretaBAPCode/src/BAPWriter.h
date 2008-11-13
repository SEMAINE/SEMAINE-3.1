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

// BAPWriter.h: interface for the BAPWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAPWRITER_H__EAD9A4EA_E87A_426E_BB2E_FB9CFFF0BD13__INCLUDED_)
#define AFX_BAPWRITER_H__EAD9A4EA_E87A_426E_BB2E_FB9CFFF0BD13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning( disable : 4786 ) 
#endif // _MSC_VER > 1000


#include "BAPFrame.h"


/**
* class :BAPWriter
*
*/

class BAPWriter  
{
public:

	/**
	* 
	*
	* @return 
	* @param frames
	* @param  str
	*/

	int WriteToString(BAPFrameVector *frames, string ** str);

	/**
	*  
	* 
	*
	* @return 
	* @param frames
	* @param  string filename
	*/

	int WriteToFile(BAPFrameVector *frames, string filename);

	/**
	* contructor 
	*
	*/

	BAPWriter();

	/**
	* destructor 
	*/

	virtual ~BAPWriter();

};

#endif // !defined(AFX_BAPWRITER_H__EAD9A4EA_E87A_426E_BB2E_FB9CFFF0BD13__INCLUDED_)
