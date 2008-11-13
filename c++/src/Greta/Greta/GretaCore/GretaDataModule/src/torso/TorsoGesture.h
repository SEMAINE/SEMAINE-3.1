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

// TorsoGesture.h
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4786)

#include <vector>
#include "TorsoGesturePhase.h"
#include "Signal.h"


/**@#-*/
namespace TorsoSpace {
	/**@#+*/


	/**
	* class :TorsoGesture
	*
	*/

	class TorsoGesture : public MMSystemSpace::Signal
	{
	public:

		/**
		* contructor 
		*
		*/

		TorsoGesture();

		/**
		*  
		* 
		*
		* @return 
		* @param  id
		*/

		TorsoGesture(std::string id);

		/**
		*  
		* 
		*
		* @return 
		* @param  id
		* @param  float start
		* @param  float duration
		* @param strokes
		*/

		TorsoGesture(std::string id, float start, float duration, std::vector<float> *strokes);

		/**
		*  
		* 
		*
		* @return 
		* @param  id
		* @param  float start
		* @param  float duration
		* @param  float stroke
		*/

		TorsoGesture(std::string id, float start, float duration, float stroke);

		/**
		* contructor 
		*
		* @param  TorsoGesture &rhs
		*/

		TorsoGesture(const TorsoGesture &rhs);

		/**
		*  
		* 
		*
		* @return 
		* @param  MMSystemSpace::Signal &rhs
		*/

		TorsoGesture(const MMSystemSpace::Signal &rhs);

		/**
		*  
		* 
		*
		* @return 
		* @param  MMSystemSpace::Signal &signal
		* @param  TorsoGesture &gesture
		*/

		TorsoGesture(const MMSystemSpace::Signal &signal,const TorsoGesture &gesture);

		/**
		* destructor 
		*/

		~TorsoGesture(void);

		/**
		*  
		* 
		*
		* @return 
		*/

		int Temporize();

		/**
		*  
		* 
		*
		* @param  restpose
		* @param  fps
		*/

		void AddMissingPreparationRetraction(TorsoMovementPoint restpose,int fps);

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
		* @param  &keyframes
		* @param  fps
		*/

		void CollectKeyframes(std::vector<BAPFrame> &keyframes,float fps);
		std::vector<TorsoGesturePhase> phases;

		/**
		* contructor 
		*
		* @param  which
		*/

		TorsoGesturePhase* GetPhase(TorsoGesturePhaseType which);

		/**
		* contructor 
		*
		* @param  which
		* @param  scaling
		*/

		bool ScalePhases(TorsoGesturePhaseType which,float scaling);

		/**
		*  
		*
		* @return 
		*/

		bool CheckTiming();
		//TorsoGesture *prev,*next;
	};

	/**@#-*/
}
/**@#+*/
