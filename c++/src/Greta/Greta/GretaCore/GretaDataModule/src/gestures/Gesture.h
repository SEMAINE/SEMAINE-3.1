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

/// @file Gesture.h
/// interface for the Gesture class.
///
/////////////////////////////////////////////////////////////////////

#pragma once

#include "GestureEngineGlobals.h"
#include "GesturePhase.h"

/**@#-*/
namespace GestureSpace {
	/**@#+*/

	/** A GesturePhaseVector is a vector/array of pointers to GesturePhases 
	* @see GesturePhase
	*/
	typedef std::vector<GesturePhase *> GesturePhaseVector;

	/** A FloatVector is a vector/array of floats */
	typedef std::vector<float> FloatVector;

	/** A Gesture holds a complete definition of a gesture. 
	*
	* The information is mainly stored in a GesturePhaseVector and some
	* gesture-global variables. 
	*
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class Gesture  
	{
	public:
		/** Set absolute start, stroke and end times for this gesture */
		void SetTimeframe(float start, float stroke, float finish);

		/** add stroke repetition(=additional repetition) times */
		void AddRepetitions(FloatVector v);

		/** Expand repetitions into additional GesturePhases */
		void ExpandRepetitions();

		/** Expand loop into additional GesturePhases */
		void ExpandLoop();
		/** Look for loop points int the current GesturePhaseVector and set iterators */
		void FindLoopInfo();

		/** :TRICKY: Overloaded copy constructor
		*
		* When a new gesture instance of a prototype gesture is created,
		* The GesturePhases need to be copied so that their values can be changed
		* without modifying the prototype gesture. This constructor assures
		* that this is performed correctly.*/
		Gesture(const Gesture &rhs);

		/** Scale absolute phase times to fill alloted timeframe and assure correct timing of stroke */
		void ScalePhaseTimes();

		/** Print some info */
		void Print();

		void PrintOnScreen();

		/** Less-than operator used for sorting gestures by starting time */
		bool operator<(const Gesture &rhs);

		/** All Get/Set.. methods are simple inline accessors */
		void		SetClass(string c)		{mClass=c;}
		string		GetClass()				{return mClass;}
		void		SetInstance(std::string i)	{mInstance=i;}
		string		GetInstance()			{return mInstance;}
		void		SetHasLoop(bool b)		{mHasLoop=b;}
		bool		GetHasLoop()			{return mHasLoop;}
		void		SetHasRepetitions(bool b) {mHasRepetitions=b;}
		bool		GetHasRepetitions()		{return mHasRepetitions;}
		void		SetLoopCycles(int n)	{mLoopCycles=n;}
		int			GetLoopCycles()			{return mLoopCycles;}
		float		GetStartTime()			{return mStart;}
		void		SetStartTime(float s)	{mStart=s;}
		float		GetFinishTime()			{return mFinish;}
		void		SetFinishTime(float f)	{mFinish=f;
		mDuration=mFinish-mStart;}
		float		GetStrokeTime()			{return mStroke;}
		void		SetStrokeTime(float b)	{mStroke=b;}

		float		GetDuration()			{return mDuration;}
		void		SetDuration(float d)	{mDuration=d;}
		float		GetDefDuration()		{return mDefDuration;}
		void		SetDefDuration(float d) {mDefDuration=d;}
		SideType	GetSide()				{return side;}
		void		SetSide(SideType s);	//expanded to change children,too.	{side=s;}
		void		SetDescription();
		string		GetDescription()		{return mDescription;}

		bool		HasStroke(GesturePhaseVector::iterator &iter);
		bool		HasPostStrokeHold(GesturePhaseVector::iterator &iter);
		void		SetExpressivitySPC(float SPC);
		void		SetExpressivityTMP(float TMP);
		void		SetExpressivityFLD(float FLD);
		void		SetExpressivityPWR(float PWR);
		void		SetExpressivityREP(float REP);

		float		GetExpressivityTMP();
		float		GetExpressivityFLD();
		float		GetExpressivityPWR();

		void		Save(string fileName);
		void		Load(string fileName);

		Gesture();
		virtual ~Gesture();

		/** The GesturePhaseVector that stores all phases */
		GesturePhaseVector phases;
		/** The GesturePhaseVector that stores the left phases when the gesture is assymetric.
		Then, the GesturePhaseVector phases is used for the right side*/
		GesturePhaseVector phasesAssym;

		/** switch the GesturePhaseVectors phases and phasesAssym */
		void		SwitchPhasesVectors();

	private:

		/** Scale relative phase times 
		* 
		* scales relative timing of frames while assuring exact timing of stroke_end frame ???*/
		void ScaleRelTimes();

		/** iterator that points to the starting frame of a loop, if it exists */
		GesturePhaseVector::iterator mStartLoop;

		/** iterator that points to the last frame of a loop, if it exists */
		GesturePhaseVector::iterator mEndLoop;

		/** Which arm is this gesture performed on? left (l), right (r) or both */
		SideType	side;

		/** Actual duration in seconds */
		float		mDuration;

		/** Default duration in seconds from Gesture definition file */
		float		mDefDuration;	

		/** How many cycles the loop (if there is one) has */
		int			mLoopCycles;

		/** Does the gesture have a loop? */
		bool		mHasLoop;

		/** Does the gesture have repetitions (of stroke) */
		bool		mHasRepetitions;

		/** Class description of gesture (ADJECTIVAL, etc.) */
		string		mClass;
		/** Instance description of gesture (SMALL, etc.) */
		string		mInstance;
		/** Complete description built from class and instance (eg. ADJECTIVAL=SMALL) */
		string		mDescription;

		/** absolute stroke time in secondss */
		float		mStroke;		
		/** absolute start time in seconds */
		float		mStart;			
		/** absolute finish time in secs */
		float		mFinish;		

		/** array of additional repetition times, empty by default
		*
		* used when mHasRepetitions=true */
		FloatVector mRepetitions;

		/** debug output flag */
		bool		output;			// output flag

		float expressivitySPC,expressivityTMP,expressivityFLD,expressivityPWR,expressivityREP;

		/** this private methode is used by save() to print one of the two vectors of phases in the ofstream */
		void savePhases(ofstream *outputfile, GesturePhaseVector *phasesVector);

	};

	/** A GestureVector is a vector/array of pointers to Gesture objects*/
	typedef std::vector<Gesture *> GestureVector;


	/** Structure that defines sorting operator for Gesture pointers used in GesturePlanner*/
	struct AscGestureSort {
		bool operator()(Gesture*& X, Gesture*& Y) {
			return ((*X)<(*Y));
		}
	};

	/**@#-*/
} // END namespace GestureSpace
/**@#+*/

