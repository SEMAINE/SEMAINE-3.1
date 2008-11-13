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

/// @file GesturePhase.h
/// interface for the GesturePhase class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GesturePhase_H__C3417D76_08AD_4930_BA3B_EF19018578EE__INCLUDED_)
#define AFX_GesturePhase_H__C3417D76_08AD_4930_BA3B_EF19018578EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <iostream>
//#include <vector>
//#include "GestureEngineGlobals.h"
//#include "GestureComponent.h"
#include "HandShape.h"
#include "ArmPosition.h"
#include "WristOrientation.h"
#include "StoredComponent.h"
#include "NoiseComponent.h"
#include "TweenComponent.h"
#include "CurvedTransition.h"

/**@#-*/
namespace GestureSpace {
/**@#+*/
	
	/** A GestureComponentVector is a vector/array of pointers to GestureComponents */
	typedef std::vector<GestureComponent *> GestureComponentVector;
	
	/** What moment in the course of a gesture does the current phase correspond to? */
	enum GesturePhaseType {PHASETYPE_DEFAULT,PREPARATION,
		PRE_STROKE_HOLD,STROKE,STROKE_START,STROKE_END,
		POST_STROKE_HOLD,RETRACTION};
	
	/** A GesturePhase can be starting phase of a loop, ending phase, or neither */
	enum PhaseLoopType {LOOP_START, LOOP_END, LOOP_NONE};
	
	//class TweenComponent; // forward declaration
	
	/** A GesturePhase is an abstract definition of a gestural pose at one instant in time 	
	*
	* A sequence of GesturePhases make up a Gesture. A GesturePhase in turn is defined by
	* its GestureComponents, or, more precisely, by objects derived from the abstract base
	* class GestureComponent (mainly ArmPosition, WristOrientation, HandShape, but also others).
	* Using Concretize and GetAngles, the GesturePhase can translate its abstract state definition
	* into concrete joint angles.
	*
	* @see Gesture
	* @see GestureComponent
	* @see MotorPlanner::Init
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class GesturePhase  
	{
	public:
		
		// MEMBER VARIABLES
		bool					output;
		GesturePhaseType		type;
		
		/** relative timing of this phase in entire gesture [0..1]
		* careful: most newer code works with the absolute timing variable
		* mAbsTime, defined below 
		*/
		float					time;
		
		/** NOT USED: how important is this phase for overall gesture [0..1] */
		float					certainty;
		
		/** what side is this phase on? valid: left(l) / right(r) */
		SideType				side;
		
		/** Vector that stores all GestureComponents
		* public b/c it is accessed by other classes (I know this is not good) */
		GestureComponentVector	v;
		
		// MEMBER FUNCTIONS
		
		/** Add a "in-between" component, a kind of simple interpolator between from and to 
		*
		* used for gestures with stroke repetitions
		* @see TweenComponent
		*/
		void AddTweenComponent(GesturePhase* from, GesturePhase* to, float t);
		
		/** Add a noise component that perturbs joint angles for added realism in repetitions */
		void AddNoise();
		
		/** Output rel/abs times; mainly for debugging */
		void PrintTimes();

		void PrintTimesOnScreen();
		
		/** "Start" the definition process of this phase.
		*
		* Mainly used by GestureReader during parsing of gesture definition files.
		* Allows for subsequent calls to Add() to sequentially build up definition
		* of this phase. Once all elements have been added, Finalize() should be called.
		* @see #Add
		* @see #Finalize
		*/
		void Start();
		
		/** Finish definition process begun by Start() */
		void Finalize();
		
		
		
		/** Set GesturePhaseType (translated from string */
		void SetType(string s);
		void SetSide(SideType s);

		void Print();
		
		/** Compute intermediate values from abstract state definition that then allow to extract joint angles
		*
		* Called by MotorPlanner just before GetAngles.
		* @see MotorPlanner::Init
		* @see #GetAngles
		*/
		void Concretize();
		
		/** Compute concrete joint angles for this phase and write result to argument f
		* 
		* Called by MotorPlanner::Init
		* @see MotorPlanner::Init*/
		void GetAngles(BAPFrame &f);
		
		/** Clear the GestureComponentVector */
		void ClearVector();
		
		/** Adds: overloaded functions to define this phase by creating GestureComponents */
		void Add(string finger, string bend);
		void Add(FingerBaseType t);
		void Add(PalmType t);
		void Add(ArmX x, bool fixed=false);
		void Add(ArmY y, bool fixed=false);
		void Add(ArmZ z, bool fixed=false);
		void Add(string s);
		void Add(BendType t); 
		void Add(FingerType f, BendType b);
		void Add(SymbolClosedType t);
		void Add(SymbolOpenType t);
		void Add(SymbolType t);
		void Add(FormThumbType t);
		void Add(FormType t, bool fixed=false);
		void Add(HandShapeType t);
		void Add(ArmPositionX x, bool fixed=false);
		void Add(ArmPositionY y, bool fixed=false);
		void Add(ArmPositionZ z, bool fixed=false);
		void AddStoredPose(string file);
		void AddTransition(std::vector<std::string> params) {transition = new CurvedTransition(); transition->instantiate(params);}
		string GetStoredPose();
		void RemoveStoredPose();
		
		void AddWristMagnitude(float m);
		void AddWrist(string sFb, string sP, bool to);

		/** Get/Set...: simple inline accessors*/
		void SetAbsTime(float t) {//assert(t>=0.0f); 
									mAbsTime=t;}
		void SetRelTime(float t) {time=t;}		
		GesturePhaseType	GetType()		{return type;}
		SideType			GetSide()		{return side;}
		ArmPosition*		GetPArm()		{return currentArm;}
		HandShape*			GetPHand()		{return currentHand;}
		WristOrientation*	GetPWrist()		{return currentWrist;}
		float				GetAbsTime()	{return mAbsTime;}
		float				GetRelTime()	{return time;}
		PhaseLoopType		GetLoopType()	{return mLoopType;}
		void				SetLoopType(PhaseLoopType t)	{mLoopType=t;}
		CurvedTransition*	GetCurvedTransition()	{return transition;}
		void				SetCurvedTransition(CurvedTransition* ct)	{delete transition; transition=ct;}
		
		/** :TRICKY: overloaded copy constructor */
		GesturePhase(const GesturePhase &rhs);
		
		/** default constructor */
		GesturePhase(	GesturePhaseType t=PHASETYPE_DEFAULT, 
						float tm=0.5, 
						float c=1.0, 
						SideType s=no_side);
		virtual ~GesturePhase();

		void SetSPC(float SPC);

		void SetPWR(float PWR);

		float GetPWR();

		bool HasTransition() {return (transition!=NULL && transition->isActive() && currentArm->useIK);}
		
	protected:
		/** Pointer to the GestureComponent in the GCVector that defines the Hand or NULL*/
		HandShape*			currentHand;
		/** Pointer to the GestureComponent in the GCVector that defines the Arm or NULL*/
		ArmPosition*		currentArm;
		/** Pointer to the GestureComponent in the GCVector that defines the Wrist or NULL*/
		WristOrientation*	currentWrist;
		/** Pointer to the GestureComponent in the GCVector that defines added noise or NULL*/
		NoiseComponent*	currentNoise;
		/** defines the transition to the next phase */
		CurvedTransition * transition;
	private:
		/** absolute time in seconds that this phase has in the total animation */
		float				mAbsTime;
		/** is this phase start or end of loop, or neither? */
		PhaseLoopType		mLoopType;

		float PWR;
	};


	/** Defines sorting function by ascending _relative_ phase time for GesturePhases */
	struct AscGesturePhaseSort {
		bool operator()(GesturePhase*& X, GesturePhase*& Y) {
			return ((X->GetRelTime())<(Y->GetRelTime()));
		}
		
 	};

	/** Defines sorting function by ascending _absolute_ phase time for GesturePhases */
	struct AscAbsGesturePhaseSort{
		bool operator()(GesturePhase*& X, GesturePhase*& Y) {
			return ((X->GetAbsTime())<(Y->GetAbsTime()));
		}
		
 	};

/**@#-*/
} // END namespace GestureSpace
/**@#+*/
//include "TweenComponent.h"
#endif // !defined(AFX_GesturePhase_H__C3417D76_08AD_4930_BA3B_EF19018578EE__INCLUDED_)
