//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// HeadGesture.h
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4786)
#include "Signal.h"
#include "HeadGesturePhase.h"
#include <vector>

using namespace MMSystemSpace;

/**@#-*/
namespace HeadSpace {
	/**@#+*/


	/**
	* class :HeadGesture
	*
	*/

	class HeadGesture : public Signal
	{
	public:

		/**
		* contructor 
		*
		*/

		HeadGesture();

		/**
		* 
		*
		* @return 
		* @param  id
		*/

		HeadGesture(std::string id);

		/**
		* 
		*
		* @return 
		* @param  id
		* @param  float start
		* @param  float duration
		* @param strokes
		*/

		HeadGesture(std::string id, float start, float duration, std::vector<float> *strokes);

		/**
		* 
		*
		* @return 
		* @param  id
		* @param  float start
		* @param  float duration
		* @param  float stroke
		*/

		HeadGesture(std::string id, float start, float duration, float stroke);

		/**
		* contructor 
		*
		* @param  HeadGesture &rhs
		*/

		HeadGesture(const HeadGesture &rhs);

		/**
		*  
		*
		* @return 
		* @param  MMSystemSpace::Signal &rhs
		*/

		HeadGesture(const MMSystemSpace::Signal &rhs);

		/**
		* 
		*
		* @return 
		* @param  MMSystemSpace::Signal &signal
		* @param  HeadGesture &gesture
		*/

		HeadGesture(const MMSystemSpace::Signal &signal,const HeadGesture &gesture);

		/**
		* destructor 
		*/

		~HeadGesture(void);

		/**
		*  
		*
		* @return 
		*/

		int Temporize();

		/**
		*  
		* 
		* @param  fps
		*/

		void AddMissingPreparationRetraction(int fps);

		/**
		*  
		* 
		*
		* @param  fps
		*/

		void AddMissingHold(int fps);

		/**
		*  
		* 
		*
		*/

		void ExpandStrokes();

		/**
		*  
		* 
		*
		* @param  &keyframes
		* @param  fps
		*/

		void CollectKeyframes(std::vector<FAPFrame> &keyframes,float fps);
		std::vector<HeadGesturePhase> phases;

		/**
		* contructor 
		*
		* @param  which
		*/

		HeadGesturePhase* GetPhase(HeadGesturePhaseType which);

		/**
		*  
		* 
		*
		* @return 
		* @param  which
		*/

		std::vector<HeadGesturePhase>::iterator GetPhaseReverse(HeadGesturePhaseType which);

		/**
		*  
		* 
		*
		* @return 
		* @param  startiter
		* @param  which
		*/

		std::vector<HeadGesturePhase>::iterator GetNextPhase(std::vector<HeadGesturePhase>::iterator startiter,HeadGesturePhaseType which);

		/**
		* contructor 
		*
		* @param  which
		* @param  scaling
		*/

		bool ScalePhases(HeadGesturePhaseType which,float scaling);

		/**
		*  
		* 
		*
		* @return 
		*/

		bool CheckTiming();
		//HeadGesture *prev,*next;
	};

	/**@#-*/
}
/**@#+*/
