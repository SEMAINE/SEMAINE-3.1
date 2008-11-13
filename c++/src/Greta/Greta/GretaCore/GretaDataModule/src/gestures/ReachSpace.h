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

/// @file ReachSpace.h
/// interface for the ReachSpace class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include <baptypes.h>
//#include "GestureEngineGlobals.h"
#include "ArmCoordinates.h"
#include "algebra3.h"	// Added by ClassView


#define GS_NUMPOINTS_X 5
#define GS_NUMPOINTS_Y 7
#define GS_NUMPOINTS_Z 3

namespace GestureSpace {
	
	/** A representation of the reach space in front of the gesturer cf. McNeill 1992. */
	class ReachSpace
	{
	public:
		/** Calculate coordinates of the center of a reach space sector w.r.t. HumanoidRoot */
		alg3::vec3 GetCoordinates(int sectorX, int sectorY, int sectorZ, SideType side=r);
		
		/** scale ReachSpace representation along horizontal, vertical and depth dimensions */
		void ScaleSpace(float horiz, float vert, float front);
		
		ReachSpace();
		virtual ~ReachSpace();
	private:
		alg3::vec4 points[GS_NUMPOINTS_X][GS_NUMPOINTS_Y][GS_NUMPOINTS_Z];
		alg3::mat4 xform;
		alg3::vec4 origin; // the point from which all movement radiates out / center of mass?
	protected:
		void InitializePoints();
	};

}