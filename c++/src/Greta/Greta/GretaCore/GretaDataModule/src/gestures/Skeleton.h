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

/// @file Skeleton.h
/// interface for the Skeleton class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKELETON_H__5483A3D9_3C9A_46A8_BE98_65A018DABD0C__INCLUDED_)
#define AFX_SKELETON_H__5483A3D9_3C9A_46A8_BE98_65A018DABD0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

//#include "Wm4Matrix3.h"
#include "Joint.h"
//#include "BAPframe.h"
//include "WristOrientation.h"

/**@#-*/
namespace GestureSpace {
/**@#+*/
	/** Skeleton base class; instances are WristAngleSkeleton and DisplaySkeleton 
	*
	* Only defines the existence of a Root joint and and the output flag - not very useful
	* @see WristAngleSkeleton
	* @see DisplaySkeleton
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class Skeleton  
	{
	public:
		//void GetWristAngles(BAPframe &f, PalmType palm, FingerBaseType fb, SideType side);
		Skeleton();
		virtual ~Skeleton();
		Joint*	GetRoot() {return Root;}
	protected:
		Joint*	Root;
		bool	output;
	};
/**@#-*/
}
/**@#+*/
#endif // !defined(AFX_SKELETON_H__5483A3D9_3C9A_46A8_BE98_65A018DABD0C__INCLUDED_)
