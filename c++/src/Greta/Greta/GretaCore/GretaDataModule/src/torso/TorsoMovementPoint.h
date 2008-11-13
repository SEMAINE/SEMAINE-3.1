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

// TorsoMOvementPoint.h
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4786)

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "BAPFrame.h"
//#include "BAPconverter.h"

/**@#-*/
namespace TorsoSpace {
	/**@#+*/

	enum TorsoHorizontalRotationType {h_right=0,h_center,h_left};
	enum TorsoVerticalRotationType {v_backward=0,v_center,v_forward};
	enum TorsoSagittalRotationType {s_right=0,s_center,s_left};
	enum TorsoSpineRotationType {r_right=0,r_center,r_left};

	/**
	* class :TorsoMovementPoint
	*
	*/

	class TorsoMovementPoint
	{
	public:

		/**
		* contructor 
		*
		*/

		TorsoMovementPoint(void);

		/**
		* contructor 
		*
		* @param  TorsoMovementPoint &rhs
		*/

		TorsoMovementPoint(const TorsoMovementPoint &rhs);

		/**
		* destructor 
		*/

		~TorsoMovementPoint(void);

		TorsoHorizontalRotationType Hposition;
		TorsoVerticalRotationType Vposition;
		TorsoSagittalRotationType Sposition;

		TorsoSpineRotationType SpineRotation;


		/**
		*  
		* 
		*
		* @return 
		* @param  name
		*/

		TorsoHorizontalRotationType GetHpositionId(std::string name);

		/**
		*  
		* 
		*
		* @return 
		* @param  name
		*/

		TorsoVerticalRotationType GetVpositionId(std::string name);

		/**
		*  
		* 
		*
		* @return 
		* @param  name
		*/

		TorsoSagittalRotationType GetSpositionId(std::string name);


		/**
		*  
		* 
		*
		* @return 
		* @param  name
		*/

		TorsoSpineRotationType GetSpineRotationId(std::string name);


		/**
		*  
		* 
		*
		* @return 
		* @param  t
		*/

		int LoadPose(int t);

		/**
		*  
		* 
		*
		* @return 
		* @param  t
		* @param  float scaling
		*/

		int LoadPose(int t, float scaling);

		/**
		*  
		* 
		*
		* @return 
		* @param  filename
		*/

		int LoadPose(std::string filename);

		/**
		*  
		* 
		*
		* @return 
		* @param  filename
		* @param  scaling
		*/

		int LoadPose(std::string filename,float scaling);

		BAPFrame *baps;

		float time;

		bool compensateshoulders;

		bool compensatehead;

		bool spatial_fixed;
	};

	/**@#-*/
}
/**@#+*/
