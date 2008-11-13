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

// TorsoGesturePhase.h
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4786)

#include "TorsoMovementPoint.h"
#include <vector>

/**@#-*/
namespace TorsoSpace {
	/**@#+*/

	enum TorsoGesturePhaseType {all_lenght=0,preparation,stroke,hold,retraction,unknown_phase};
	enum TorsoTrajectoryType {linear=0,curved,circular};
	enum TorsoStanceType {closed=0,opened};

	/**
	* class :TorsoGesturePhase
	*
	*/

	class TorsoGesturePhase
	{
	public:

		/**
		* contructor 
		*
		*/

		TorsoGesturePhase(void);

		/**
		* contructor 
		*
		* @param  TorsoGesturePhase &rhs
		*/

		TorsoGesturePhase(const TorsoGesturePhase &rhs);

		/**
		* destructor 
		*/

		~TorsoGesturePhase(void);

		/**
		*  
		* 
		*
		* @param  begin
		* @param  end
		*/

		void Temporize(float begin,float end);

		/**
		*  
		* 
		*
		* @param  &keyframes
		* @param  fps
		* @param  scalefactor
		* @param  continuity
		* @param  tension
		*/

		void CollectKeyframes(std::vector<BAPFrame> &keyframes,float fps,float scalefactor,float continuity,float tension);
		TorsoTrajectoryType trajectory;

		/**
		* contructor 
		*
		*/

		TorsoGesturePhaseType type;
		TorsoStanceType stance;

		float Tmin,Tmax,Tdefault,Tduration;
		int Tfixed;
		float begin,end;

		/**
		*  
		* 
		*
		* @return 
		* @param  phasename
		*/

		TorsoGesturePhaseType GetPhaseId(std::string phasename);

		/**
		*  
		* 
		*
		* @return 
		* @param  type_id
		*/

		std::string GetPhaseName(TorsoGesturePhaseType type_id);

		/**
		*  
		* 
		*
		* @return 
		*/

		std::string GetPhaseName();

		/**
		*  
		* 
		*
		* @return 
		* @param  trajectoryname
		*/

		TorsoTrajectoryType GetTrajectoryId(std::string trajectoryname);
		std::vector<TorsoMovementPoint> points;
		bool concretized;

	};

	/**@#-*/
}
/**@#+*/
