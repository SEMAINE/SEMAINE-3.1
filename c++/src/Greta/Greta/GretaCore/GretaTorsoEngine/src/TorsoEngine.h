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
#include "torso\TorsoGesture.h"
#include "XMLGenericParser.h"
#include "BAPFrame.h"
#include "DataViewerWindow.h"
#include "TCBInterpolator.h"
#include "Signal.h"

/**@#-*/
namespace TorsoSpace {
	/**@#+*/

	/**
	*
	* class :TorsoEngine
	*
	*/

	class TorsoEngine
	{
	public:

		/**
		*
		* contructor 
		*
		*/

		TorsoEngine(void);

		/**
		*
		* destructor 
		*/

		~TorsoEngine(void);

		//see torsogestuary in datamodule
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

		int ScheduleTorsoGesture(std::string reference, float start, float duration, float stroke);

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

		int ScheduleTorsoGesture(std::string reference, float start, float duration, std::vector<float> *strokes);

		/**
		*  
		* 
		*
		* @return 
		* @param  tg
		*/

		int ScheduleTorsoGesture(MMSystemSpace::Signal tg);

		/**
		*  
		* 
		*
		* @return 
		* @param  s
		* @param  gestureid
		*/

		int ScheduleTorsoGesture(MMSystemSpace::Signal s,std::string gestureid);

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
		* @return 
		*/

		void Cleanup(void);

		/**
		*  
		* 
		*
		* @return 
		*/

		std::vector<BAPFrame> *GetAnimation();
		std::map<std::string,TorsoGesture> gestuary;
		std::vector<TorsoGesture> gestures;
	private:
		std::vector<BAPFrame> animation;
		std::vector<BAPFrame> keyframes;
		DataViewerWindow *dw;
		TCBInterpolator interpolator[72];
		float fps;
		float animationlength;

		/**
		*  
		* 
		*
		* @param  tg
		*/

		void AddGesture(TorsoGesture tg);

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

		TorsoMovementPoint restposition;
	};

	/**@#-*/
}
/**@#+*/
