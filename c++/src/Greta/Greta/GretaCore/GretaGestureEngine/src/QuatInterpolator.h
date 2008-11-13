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

/** @file QuatInterpolator.h 
 * interface for the QuatInterpolator class. */
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUATINTERPOLATOR_H__8DD6E1C3_63DF_4E81_B870_69BF684EF4FA__INCLUDED_)
#define AFX_QUATINTERPOLATOR_H__8DD6E1C3_63DF_4E81_B870_69BF684EF4FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include "AbstractInterpolator.h"

#include "WildMagic/Wm4Vector2.h"
#include "WildMagic/Wm4Vector3.h"
#include "WildMagic/Wm4TCBSpline2.h"
#include "WildMagic/Wm4TCBSpline3.h"
#include "WildMagic/Wm4Quaternion.h"
#include "WildMagic/Wm4Vector3.h"

/**@#-*/
using namespace Wm4 ;
namespace GestureSpace {
/**@#+*/

	/** A QuatVector is a vector/array of Quaternions (from Mgc Library) */
	typedef vector<Quaternionf> QuatVector;

	/** Quaternion interpolator using quat slerp for orientation interpolation. 
	 * Can only interpoate DOFGroups for arm and wrist.
	 * @see DOFGroup
	 * @see AbstractInterpolator
	 */
	class QuatInterpolator : public AbstractInterpolator  
	{
	public:
		void GetFrame(BAPFrame &f, int n, enum DOFGroup t=ARM_DOFs);
		void Init(BAPFrameVector v, enum DOFGroup st=ARM_DOFs);
		QuatInterpolator(enum SideType s = r);
		virtual ~QuatInterpolator();
	private:
		/** Initialization for one quaternion vector at a time */
		void QuatInit(BAPFrameVector v, enum SplineType st);
		
		/** Lookup solution angle values of quaternion interpolation for a given frame */
		Vector3f GetQSolution(int frame, SplineType t);
		
		/** Store solution angle values of quaternion interpolation for a given frame */
		void StoreQSolution(int frame,float x,float y, float z, SplineType t);

		/** Quaternion spherical linear interpolation (SLERP) */
		void SlerpQuat2(Quaternionf *quat1,Quaternionf *quat2, float slerp, Quaternionf *result);

		/** Vector that holds Euler-X,Y,Z results of quaternion interpolation for Wrist */
		vector<Vector3f> mQWrist;
		
		/** Vector that holds Euler-X,Y,Z results of quaternion interpolation for Elbow */
		vector<Vector3f> mQElbow;
		
		/** Vector that holds Euler-X,Y,Z results of quaternion interpolation for Shoulder */
		vector<Vector3f> mQShoulder;
		
		/** array offset constant for animations not starting at frame 1 */
		int mQWristOffset;
		
		/** array offset constant for animations not starting at frame 1 */
		int mQElbowOffset;
		
		/** array offset constant for animations not starting at frame 1 */
		int mQShoulderOffset;
	};
/**@#-*/
} // END namespace GestureSpace
/**@#+*/
#endif // !defined(AFX_QUATINTERPOLATOR_H__8DD6E1C3_63DF_4E81_B870_69BF684EF4FA__INCLUDED_)
