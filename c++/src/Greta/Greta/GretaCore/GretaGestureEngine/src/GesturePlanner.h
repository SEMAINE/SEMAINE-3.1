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

/// @file GesturePlanner.h
/// interface for the GesturePlanner class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESTUREPLANNER_H__9BF81341_02D9_4951_9BF1_6589D428E6DC__INCLUDED_)
#define AFX_GESTUREPLANNER_H__9BF81341_02D9_4951_9BF1_6589D428E6DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)


#include "GestureEngineGlobals.h"
#include "Gesture.h"
#include "Gestuary.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <algorithm>

/**@#-*/ 
namespace GestureSpace {
	/**@#+*/ 

	/**
	* abstract notion of arm movement speed; used for Fitt's law timing
	*/
	enum ArmSpeedType {AST_VerySlow=-2,AST_Slow=-1,AST_Normal=0,AST_Fast=1,AST_VeryFast=2};

	/**
	* Handles Gesture requests, timing calculations; generates rest gestures.
	*
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu
	* @version 1.0
	*/
	class GesturePlanner  
	{
	public:

		/** Request a gesture with timing information 
		*
		* Request looks for the gesture in the Gestuary and, if the search was successful,
		* calls Schedule to fit the gesture into the animation timing plan.
		* @see #Schedule
		*/
		bool Request(string description,float start, float stroke, float end, float SPC, float TMP, float FLD, float PWR, float REP);	

		/** Request a gesture with additional repetitions (=partially repeated stroke movements) 
		*
		* Similar to Request, only that additional timing information for repetitions is passed
		* @see #Request
		*/
		bool RequestWithRepetition(string description, float start, float stroke, float end, float SPC, float TMP, float FLD, float PWR, float REP, FloatVector repetitions);

		/** Find gaps in Gesture plan and fill them with rest poses 
		*
		*@see #InsertRestLateral
		*/
		void			InsertRestKeys(bool skiprestinsertion=0);

		/** Initializes GesturePlanner and loads Gesture database */
		void			Init();

		GestureVector*  GetGestureVector() { return mpvGesture; }
		void			Print();
		GesturePlanner();
		virtual			~GesturePlanner();

	protected:
		bool output;

		/** Adjusts the arm position of a rest gesture
		*
		* The arm position of a rest gesture is calculated using the last arm position
		* of the previous gesture and the first arm position of the following gesture.<br>
		* THE POSITIONING CODE IS NOT WORKING VERY WELL AND COMPUTATIONS ARE CURRENTLY OVERRIDDDEN
		* WITHIN THE FUNCTION.<br><br>
		*/
		void AdjustRestKey( Gesture *rest, Gesture *prev, Gesture *next);

		/* Instantiate new Gesture from prototype in database and try to schedule it
		*
		* First finds expands possible loops and repetitions in the gesture, then 
		* picks an arm for a one-armed gesture using probability.
		* Furthermore checks for timing conflicts with existing gestures.
		* Finally enqueues gesture in GestureVector if no conflicts exist.
		*/
		void Schedule( Gesture* pG,float start, float stroke, float end, FloatVector repetitions);

		/** calculate absolute frame times for the gesture based on Fitt's law */
		void FindAbsoluteTimes(Gesture *pG, float start, float stroke, float end); //for all gesture (it use FAT)
		void FAT(Gesture *pG, float start, float stroke, float end); //only for one arm or symetric gesture

		/** Inserts a rest position on one arm only
		*
		* Two different time thresholds are defined:
		* If the time allocated for the rest gesture is greater than than 
		* the larger threshold, the arm is moved down to an absolute rest.<br>
		* If the time allocated is <threshold2 but >thresh1, a dynamic rest
		* gesture is inserted and its position is calculated using 
		* AdjustRestKey
		* @see #AdjustRestKey
		*/
		void InsertRestLateral( GestureVector *pvG);

		/** Fitts' law duration calculation between to gesture phases
		*
		* Calculate the time (in seconds) needed for executing a movement between two frames
		* based on Fitt's law (Kopp's simplification)
		* @param speed -Abstract notion of "how fast" the movement should be executed.
		*/
		double FittsDuration(GesturePhaseVector::iterator from_iter, GesturePhaseVector::iterator to_iter, float TMP, float PWR, enum ArmSpeedType speed);

	private:
		/** The gesture database */
		Gestuary*		mGestuary;

		/** Ptr to vector that holds all schedules gestures for both arms */
		GestureVector*	mpvGesture; 

		/** Temporary vector that holds only left arm gestures */
		GestureVector   mvLeftGesture;

		/** Temporary vector that holds only right arm gestures */
		GestureVector	mvRightGesture;

	};
	/**@#-*/ 
} // END namespace GestureSpace
/**@#+*/ 
#endif // !defined(AFX_GESTUREPLANNER_H__9BF81341_02D9_4951_9BF1_6589D428E6DC__INCLUDED_)
