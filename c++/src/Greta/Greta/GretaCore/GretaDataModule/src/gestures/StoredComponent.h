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

/// @file StoredComponent.h
/// interface for the StoredComponent class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOREDCOMPONENT_H__0BEA29C3_0124_4325_9CBE_CBFF4857685C__INCLUDED_)
#define AFX_STOREDCOMPONENT_H__0BEA29C3_0124_4325_9CBE_CBFF4857685C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "GestureEngineGlobals.h"

#include "GestureComponent.h"

/**@#-*/
namespace GestureSpace {
/**@#+*/

	/** Defines a component that simply reads angles from a pose definition file; derived from GestureComponent
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class StoredComponent : public GestureComponent  
	{
	public:
		void SetFilename(string s);
		string GetFilename()		{ return fileName; }
		void GetAngles(BAPFrame& f);
		StoredComponent();
		virtual ~StoredComponent();

	protected:
		string fileName;
	};

/**@#-*/
}
/**@#+*/

#endif // !defined(AFX_STOREDCOMPONENT_H__0BEA29C3_0124_4325_9CBE_CBFF4857685C__INCLUDED_)
