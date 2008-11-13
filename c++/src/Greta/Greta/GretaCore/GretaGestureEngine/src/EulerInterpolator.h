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

// EulerInterpolator.h: interface for the EulerInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EULERINTERPOLATOR_H__22CD14AA_3EAD_4514_B50B_B14C8152967C__INCLUDED_)
#define AFX_EULERINTERPOLATOR_H__22CD14AA_3EAD_4514_B50B_B14C8152967C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include "AbstractInterpolator.h"
#include "WildMagic/Wm4Vector2.h"
#include "WildMagic/Wm4Vector3.h"
#include "WildMagic/Wm4TCBSpline2.h"
#include "WildMagic/Wm4TCBSpline3.h"

/**@#-*/
/**@#-*/
using namespace Wm4 ;
namespace GestureSpace {
/**@#+*/
	class EulerInterpolator : public AbstractInterpolator  
	{
	public:

/**
*
* cosntructor
*
*/
		EulerInterpolator(enum SideType s=r);

/**
*
* destructor
*
*/
		virtual ~EulerInterpolator();

/**
*
* @param f
* @param n
* @param t
*
*/
		void GetFrame(BAPFrame &f, int n, enum DOFGroup t);
		
		/** 
		* Initialize TCB splines and calculate all inbetween frames for given DOFGroup 
		* @param v
		* @param t=FINGER_DOFs
		*/
		void Init(BAPFrameVector v, enum DOFGroup t=FINGER_DOFs);
	
	private:
		/** Ptr to TCB Spline for Shoulder keys */
		TCBSpline3f* mpTCB3_Shoulder;
		
		/** Ptr to TCB Spline for Elbow keys */
		TCBSpline2f* mpTCB2_ElbowFlex;
		
		/** Ptr to TCB Spline for Wrist keys */
		TCBSpline3f* mpTCB3_WristOrient;
		
		/** Ptrs to TCB Splines for first joint of fingers keys */
		TCBSpline3f* mpTCB3_Finger1[5];
		
		/** Ptrs to TCB Splines for 2nd+3rd joints of fingers keys */
		TCBSpline2f* mpTCB2_Finger23[5];
	};
/**@#-*/
} // END namespace GestureSpace
/**@#+*/
#endif // !defined(AFX_EULERINTERPOLATOR_H__22CD14AA_3EAD_4514_B50B_B14C8152967C__INCLUDED_)
