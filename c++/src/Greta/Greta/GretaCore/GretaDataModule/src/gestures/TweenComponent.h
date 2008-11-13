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

/// @file TweenComponent.h
/// interface for the TweenComponent class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TWEENCOMPONENT_H__92D00FDC_2746_4939_8996_FA6F1223FD29__INCLUDED_)
#define AFX_TWEENCOMPONENT_H__92D00FDC_2746_4939_8996_FA6F1223FD29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "GestureEngineGlobals.h"

#include "GestureComponent.h"
//#include "BAPframe.h"
//#include "baptypes.h"
//include "GesturePhase.h"
/**@#-*/

namespace GestureSpace {
/**@#+*/

	class GesturePhase;
	/** Defines a component that performs a simple interpolation between two frames; derived from GestureComponent
	*
	* Used in Gestures that have repetitions (=additional beats that are partial strokes).
	* The border frames of the interpolation first need to be defined with SetPhases(),
	* GetAngles() then interpolates angles at point mT in interval [0,1]<br>
	* :TODO: use a more sophisticated interpolation using quaternions
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class TweenComponent : public GestureComponent  
	{
	public:
		void SetT(float t);
		void GetAngles(BAPFrame &f);
		void SetPhases(GesturePhase* from, GesturePhase* to);
		TweenComponent();
		virtual ~TweenComponent();
		
	private:
		float mT;
		GesturePhase* mFromFrame;
		GesturePhase* mToFrame;
	};

/**@#-*/
} //end namespace GestureSpace 
/**@#+*/

#endif // !defined(AFX_TWEENCOMPONENT_H__92D00FDC_2746_4939_8996_FA6F1223FD29__INCLUDED_)
