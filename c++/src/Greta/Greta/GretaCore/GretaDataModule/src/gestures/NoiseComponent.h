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

/// @file NoiseComponent.h
/// interface for the NoiseComponent class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOISECOMPONENT_H__07B55563_063E_4E81_8572_62135DA360E5__INCLUDED_)
#define AFX_NOISECOMPONENT_H__07B55563_063E_4E81_8572_62135DA360E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <time.h>
//#include <stdlib.h>

//#include "GestureEngineGlobals.h"
#include "GestureComponent.h"

/**@#-*/
namespace GestureSpace {
/**@#+*/
	
	/** Defines a noise component that perturbs joint angles of a GesturePhase; derived from GestureComponent
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class NoiseComponent : public GestureComponent  
	{
	public:
		/** :TRICKY: Changes angles of the existing set (Mask=true) angles in BAPframe f
		*
		* Make sure to call this function only AFTER the angles that should be 
		* perturbed already exist in BAPframe f.<br>GesturePhase::GetAngles makes sure the 
		* calls to the components' GetAngle functions are executed in order
		*
		* @see GesturePhase::GetAngles
		*/
		void GetAngles(BAPFrame &f);

		NoiseComponent();
		virtual ~NoiseComponent();

	private:
		float GetRandomFloat();
	};

/**@#-*/
} // end namespace GestureSpace
/**@#+*/
#endif // !defined(AFX_NOISECOMPONENT_H__07B55563_063E_4E81_8572_62135DA360E5__INCLUDED_)
