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

/// @file ArmPosition.h
/// Interface for the ArmPosition class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARMPOSITION_H__E630F452_1209_40DF_80E9_53B3BB956889__INCLUDED_)
#define AFX_ARMPOSITION_H__E630F452_1209_40DF_80E9_53B3BB956889__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <iostream>
//#include <sstream>
//#include <vector>

#include "GestureEngineGlobals.h"
#include "ArmCoordinates.h"
#include "GestureComponent.h"
#include "GestureKinematics.h"


/**@#-*/
namespace GestureSpace {
	/**@#+*/



	/** Defines position of arm (end position of wrist in space) ; derived from GestureComponent
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class ArmPosition : public GestureComponent
	{
	public:
		void			Concretize();
		void			GetAngles(BAPFrame &f);
		alg3::vec3		GetCoordinates();
		void			SetXYZ(ArmPositionX _x, ArmPositionY _y, ArmPositionZ _z);
		ArmPosition(ArmPositionX _x, ArmPositionY _y, ArmPositionZ _z);

		/** TRICKY: overloaded copy constructor for expressivity! */
		ArmPosition (const ArmPosition &rhs);

		void			Print();
		ArmPosition();
		virtual			~ArmPosition();
		ArmPositionX	x;
		ArmPositionY	y;
		ArmPositionZ	z;		
		ArmX			AbstractX;
		ArmY			AbstractY;
		ArmZ			AbstractZ;
		float			scaleX,scaleY,scaleZ;
		float			fixedX,fixedY,fixedZ;
		bool			useIK; //use forward or inverse kinematics for arm positioning
		float			expressivitySPC;
		void ComputeScale();
	private:
		GestureKinematics IK;
	};
	/**@#-*/
} //end namespace GestureSpace
/**@#+*/
#endif // !defined(AFX_ARMPOSITION_H__E630F452_1209_40DF_80E9_53B3BB956889__INCLUDED_)
