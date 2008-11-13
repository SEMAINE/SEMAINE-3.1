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

/// @file WristAngleSkeleton.h
/// interface for the WristAngleSkeleton class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRISTANGLESKELETON_H__6FA44ABE_FBAF_45ED_BA12_C18E593D6A00__INCLUDED_)
#define AFX_WRISTANGLESKELETON_H__6FA44ABE_FBAF_45ED_BA12_C18E593D6A00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Wm4Matrix3.h"
#include "Wm4Quaternion.h"
#include "Skeleton.h"
//#include "Joint.h"
//#include "BAPframe.h"
#include "WristTypes.h"
#include "JointLimitWrapper.h"
//#include "GestureEngineGlobals.h"

/**@#-*/

using namespace Wm4;

namespace GestureSpace {
/**@#+*/
	
	/** Skeleton to calculate Wrist Orientation Angles 
	*  
	*  Arms-only skeleton used to calculate wrist orientations
	*  with inverse matrices.
	*  Root pointer inherited from base class Skeleton
	*
	* @see Skeleton
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class WristAngleSkeleton : public Skeleton  
	{
	public:

		/** Computes Wrist angles - NEEDS shoulder and elbow angles already present in BAPframe f! 
		 * @param f the BAPframe where calculated wrist angles will be written to.
		 * @param palm goal palm orientation 
		 * @param fb goal finger base orientation
		 * @param side which arm (when both_sides or no_side is passed, no angles can be loaded for the arm and computation will be skipped) 
		 * @param wrist_magnitude a scaling factor to control how far from the neutral wrist position (0.0) to the goal position (1.0) the wrist should be bent
		*/
		void GetWristAngles(BAPFrame &f, 
							PalmType palmFrom, 
							FingerBaseType fbFrom,
							PalmType palm, 
							FingerBaseType fb, 
							SideType side, 
							float slerp=1.0);
		
		WristAngleSkeleton();
		virtual ~WristAngleSkeleton();

		/** Pointer to right shoulder joint */
		Joint* rShoulder;
		/** Pointer to left shoulder joint */
		Joint* lShoulder;
		/** Pointer to right elbow joint */
		Joint* rElbow;
		/** Pointer to left elbow joint */
		Joint* lElbow;
		/** Pointer to right wrist joint */
		Joint* rWrist;
		/** Pointer to left wristjoint; */
		Joint* lWrist;

	protected:
		bool output;
		JointLimitWrapper *jointLimits;
		alg3::mat4 CreateAlignMatrix(PalmType palm, FingerBaseType fb);

		void SlerpQuat2(Quaternionf *quat1,Quaternionf *quat2,float slerp, Quaternionf *result);
	};
/**@#-*/
} // end namespace GestureSpace
/**@#+*/
#endif // !defined(AFX_WRISTANGLESKELETON_H__6FA44ABE_FBAF_45ED_BA12_C18E593D6A00__INCLUDED_)
