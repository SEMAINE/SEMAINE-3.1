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

/// @file GestureFileNames.h
/// interface for the GestureFileNames class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESTUREFILENAMES_H__50867F22_4FA8_11D6_9F3E_A8BC5A38F170__INCLUDED_)
#define AFX_GESTUREFILENAMES_H__50867F22_4FA8_11D6_9F3E_A8BC5A38F170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <stdio.h>
#include "GestureEngineGlobals.h"
#include <windows.h>


/** Routines to read the contents of the gesture definition directory.
 * Used by the GestureReader.
 * Windows-specific implementation!
 * @see TurnPlanner
 * @author Maurizio Mancini
 */
class GestureFileNames  
{
public:
	HANDLE filehandle;
	WIN32_FIND_DATA filedata;
	GestureFileNames();
	virtual ~GestureFileNames();

	char *GestureFileNames::GetFileName();
	bool GestureFileNames::Next();
	void GestureFileNames::Close();
};

#endif // !defined(AFX_GESTUREFILENAMES_H__50867F22_4FA8_11D6_9F3E_A8BC5A38F170__INCLUDED_)
