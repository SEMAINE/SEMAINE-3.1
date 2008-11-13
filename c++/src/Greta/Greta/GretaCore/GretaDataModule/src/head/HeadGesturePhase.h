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

// HeadGesturePhase.h
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4786)

#include "FAPFrame.h"
#include <vector>
#include <string>

/**@#-*/
namespace HeadSpace {
	/**@#+*/

	/**
	* class :HeadGesturePoint
	*
	*/

	class HeadGesturePoint
	{
	public:
		int rotx,roty,rotz;
		float start,end;
	};

	enum HeadGesturePhaseType {all_lenght=0,preparation,stroke,hold,retraction,unknown_phase};

	/**
	* class :HeadGesturePhase
	*
	*/

	class HeadGesturePhase
	{
	public:

		/**
		* contructor 
		*
		*/

		HeadGesturePhase(void);

		/**
		* contructor 
		*
		* @param  HeadGesturePhase &rhs
		*/

		HeadGesturePhase(const HeadGesturePhase &rhs);

		/**
		* destructor 
		*/

		~HeadGesturePhase(void);

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

		void CollectKeyframes(std::vector<FAPFrame> &keyframes,float fps,float scalefactor,float continuity,float tension);

		/**
		*  
		* 
		*
		* @param  rotx
		* @param  roty
		* @param  rotz
		*/

		void AddPoint(int rotx,int roty,int rotz);

		/**
		* contructor 
		*
		*/

		HeadGesturePhaseType type;
		std::string phasename;
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

		HeadGesturePhaseType GetPhaseId(std::string phasename);

		/**
		*  
		* 
		* @return 
		* @param  type_id
		*/

		std::string GetPhaseName(HeadGesturePhaseType type_id);
		bool concretized;
		std::vector<HeadGesturePoint> points;

	};

	/**@#-*/
}
/**@#+*/
