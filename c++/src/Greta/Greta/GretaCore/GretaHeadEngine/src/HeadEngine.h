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

#pragma once
#pragma warning(disable:4786)

#include <string>
#include <map>
#include <vector>
#include "head/HeadGesture.h"
#include "XMLGenericParser.h"
#include "DataViewerWindow.h"
#include "TCBInterpolator.h"
#include "Signal.h"
#include "FAPFrame.h"

/**@#-*/
namespace HeadSpace {
	/**@#+*/

	/**
	*
	* class :HeadEngine
	*
	*/

	class HeadEngine
	{
	public:

		/**
		*
		* contructor 
		*
		*/

		HeadEngine(void);

		/**
		*
		* destructor 
		*/

		~HeadEngine(void);
		//int LoadGestuary(std::string gestuaryfilename);

		/**
		*  
		* 
		*
		* @return 
		* @param  reference
		* @param  float start
		* @param  float duration
		* @param  float stroke
		*/

		int ScheduleHeadGesture(std::string reference, float start, float duration, float stroke);

		/**
		*  
		* 
		*
		* @return 
		* @param  reference
		* @param  float start
		* @param  float duration
		* @param strokes
		*/

		int ScheduleHeadGesture(std::string reference, float start, float duration, std::vector<float> *strokes);

		/**
		*  
		* 
		*
		* @return 
		* @param  tg
		*/

		int ScheduleHeadGesture(MMSystemSpace::Signal tg);

		/**
		*  
		* 
		*
		* @return 
		* @param  s
		* @param  gestureid
		*/

		int ScheduleHeadGesture(MMSystemSpace::Signal s,std::string gestureid);

		/**
		*  
		* 
		*
		* @param  start
		* @param  float end
		* @param  float fps
		*/

		void RenderAnimation(float start, float end, float fps);

		/**
		*  
		* 
		*
		*/

		void Cleanup();

		/**
		*  
		* 
		*
		* @return 
		*/

		std::vector<FAPFrame> *GetAnimation();
		std::map<std::string,HeadGesture> gestuary;
		std::vector<HeadGesture> gestures;
	private:
		std::vector<FAPFrame> animation;
		std::vector<FAPFrame> keyframes;
		DataViewerWindow *dw;
		TCBInterpolator interpolator[3];
		float fps;
		float animationlength;

		/**
		*  
		* 
		*
		* @param  tg
		*/

		void AddGesture(HeadGesture tg);

		/**
		*  
		* 
		*
		* @param  duration
		*/

		void CreateEmptyAnimation(float duration);

		/**
		*  
		* 
		*
		* @param  which
		*/

		void VisualizeBAP(int which);

		/**
		*  
		* 
		*
		*/

		void TemporizeGestures();

		/**
		*  
		* 
		*
		*/

		void AddMissingPreparationRetraction();

		/**
		*  
		* 
		*
		*/

		void AddMissingHold();

		/**
		*  
		* 
		*
		*/

		void CollectKeyframes();

		/**
		*  
		* 
		*
		*/

		void InsertKeyframes();

		/**
		*  
		* 
		*
		*/

		void InterpolateKeyframes();
	};

	/**@#-*/
}
/**@#+*/
