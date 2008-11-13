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

/// @file WristOrientation.h
/// interface for the WristOrientation class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRISTORIENTATION_H__00D83C16_549A_4BE7_94EB_8CE5349E1DDD__INCLUDED_)
#define AFX_WRISTORIENTATION_H__00D83C16_549A_4BE7_94EB_8CE5349E1DDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <iostream>
//#include <vector>

#include "GestureEngineGlobals.h"
#include "GestureComponent.h"
#include "WristTypes.h"
#include "WristAngleSkeleton.h"
/**@#-*/
namespace GestureSpace {
/**@#+*/
	
	
	
	/** Defines the wrist orientation of a GesturePhase; derived from GestureComponent
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class WristOrientation : public GestureComponent
	{
	public:
		
		/** GetAngles uses a WristSkeleton to calculate local wrist angles given a global goal orientation
		*
		* @see WristSkeleton
		*/
		void GetAngles(BAPFrame &f);
		
		/** Palm and FingerBase together define an unambiguous orientation in 3space */
		void SetPalm(PalmType t) {palm=t;};
		void SetFromPalm(PalmType t) {fromPalm=t;};
		PalmType GetPalm() {return palm;};
		PalmType GetFromPalm() {return fromPalm;};

		/** Palm and FingerBase together define an unambiguous orientation in 3space */
		void SetFingerBase(FingerBaseType t) {fingerbase=t;};
		void SetFromFingerBase(FingerBaseType t) {fromFingerbase=t;};
		FingerBaseType GetFingerBase() {return fingerbase;};
		FingerBaseType GetFromFingerBase() {return fromFingerbase;};

		void SetInterpolating(bool b) {interpolating=b;}

		/** Set how far wrist should be bent from neutral position.
		 * @see wrist_magnitude
		 */
		void SetWristMagnitude(float m) {slerp=m;};
		float GetWristMagnitude() {return slerp;};

		WristOrientation();
		
		virtual ~WristOrientation();
		
		/** Obsolete. */
		void Concretize();
		
		void Print();
		
	protected:

		/** Palm normal vector (vector normal to the palm plane pointing inwards).
		 * if intperpolating, this specifies the "to" pose 
		 */
		PalmType			palm;
		
		/** Finger base vector (points _in_ palm plane from wrist base to first finger knuckles.
		 * if intperpolating, this specifies the "to" pose.
		*/
		FingerBaseType		fingerbase;

		/** "Canonical" goal position of one palm and fingerbase type, or interpolating between two orientations? */
		bool interpolating;

		/** How far the wrist should move from neutral position towards goal orientation (0..1) */
		float				slerp;

		/** If interpolating: Palm normal vector of interpolation "from" pose */
		PalmType			fromPalm;
		
		/** If interpolating: FingerBase normal vector of interpolation "from" pose */
		FingerBaseType		fromFingerbase;

		/** skeleton representation to calculate wrist angles */
		static WristAngleSkeleton* mWASkeleton;

	
	};
	
/**@#-*/
} // end namespace GestureSpace
/**@#+*/

#endif // !defined(AFX_WRISTORIENTATION_H__00D83C16_549A_4BE7_94EB_8CE5349E1DDD__INCLUDED_)
