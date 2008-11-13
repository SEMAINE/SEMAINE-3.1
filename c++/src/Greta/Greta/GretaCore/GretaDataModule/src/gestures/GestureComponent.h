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

/// @file GestureComponent.h
/// interface for the GestureComponent class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESTURECOMPONENT_H__135D1055_939A_4F80_9423_AA2E26A42644__INCLUDED_)
#define AFX_GESTURECOMPONENT_H__135D1055_939A_4F80_9423_AA2E26A42644__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <iostream>
#include <BAPframe.h>
//#include "GestureEngineGlobals.h"

/**@#-*/
namespace GestureSpace {
/**@#+*/

	enum ComponentStatusType {abstract,concrete};
	
	/** Abstract base class for GestureComponents (elements that make up GestureComponentVector in class GesturePhase 
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class GestureComponent  
	{

	public:
		virtual void SetSide(SideType s);
		virtual void Concretize(); // xform from abstract to concrete form
		
		/** VERY IMPORTANT! Defines GestureComponent as abstract base class */
		virtual void GetAngles(BAPFrame& f) =0;
		
		virtual void Print();
		
		ComponentStatusType status;
		SideType side;
		string fileFolder;
		string filePrefix;
		float certainty; // how certain this feature should be executed (0.0 .. 1.0)
		float startTime;
		float endTime;

		GestureComponent(float start=0.0, float end=1.0, float c=1.0,SideType s=r);
		virtual ~GestureComponent();
		
	protected:
		bool output;
	};

/**@#-*/
} // END namespace GestureSpace
/**@#+*/

#endif // !defined(AFX_GESTURECOMPONENT_H__135D1055_939A_4F80_9423_AA2E26A42644__INCLUDED_)
