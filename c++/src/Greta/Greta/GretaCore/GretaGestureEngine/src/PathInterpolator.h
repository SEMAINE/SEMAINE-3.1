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

// PathInterpolator.h: interface for the PathInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHINTERPOLATOR_H__E18EE98D_4E4A_4E31_A19A_0A5A6EF0E247__INCLUDED_)
#define AFX_PATHINTERPOLATOR_H__E18EE98D_4E4A_4E31_A19A_0A5A6EF0E247__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include "AbstractInterpolator.h"
#include "GestureKinematics.h"
#include "WildMagic/Wm4Vector3.h"
#include "WildMagic/Wm4TCBSpline3.h"
#include "WildMagic/Wm4TCBSpline2.h"

/**@#-*/
using namespace Wm4 ;
namespace GestureSpace {
/**@#+*/
	
	/** Arm-position interpolator through TCB-Spline path control of wrist location in space. 
	 * Uses IKAN inverse kinematics. 
	 * Can only interpoate DOFGroup ARM_DOFs.
	 * @see DOFGroup
	 */
	class PathInterpolator : public AbstractInterpolator  
	{
	public:
		PathInterpolator(enum SideType s=r);
		virtual ~PathInterpolator();
		
		/** Given frame number n and DOF group t, retrieve interpolated angle values for all DOFs in t and write them to f.
		 @param f BAPFrame where interpolated angles are written to
		 @param n frame number 
		 @param t Degree of freedom group for which values should be calculated/loaded
		*/
		void GetFrame(BAPFrame &f, int n, enum DOFGroup t);
			
		/** Initialize TCB splines and calculate all inbetween frames for given SplineType (has to be ARM_DOFs)*/
		void Init(BAPFrameVector v, enum DOFGroup t=ARM_DOFs);
	private:
		
		TCBSpline3f* mpTCB3_Path;	/**< Ptr to TCB Spline for wrist positions */
		TCBSpline2f* mpTCB2_Swivel;	/**< Ptr to TCB Spline for elbow swivel angles */
		TCBSpline2f* mpTCB2_Time;	/**< Ptr to TCB Spline for time mapping. NOT USED? */
		GestureKinematics mGK;		/**< Class with fwd/inv. kinematics routines */
	};

/**@#-*/
} // END namespace GestureSpace
/**@#+*/

#endif // !defined(AFX_PATHINTERPOLATOR_H__E18EE98D_4E4A_4E31_A19A_0A5A6EF0E247__INCLUDED_)
