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

/// @file MotorPlanner.h
/// interface for the MotorPlanner class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOTORPLANNER_H__EE4A5782_5C04_4785_AD69_46385AB2545C__INCLUDED_)
#define AFX_MOTORPLANNER_H__EE4A5782_5C04_4785_AD69_46385AB2545C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GestureEngineGlobals.h"
#include "BAPFrame.h"
#include "AbstractInterpolator.h"
#include "Gesture.h"
#include "GestureKinematics.h"
/**@#-*/
namespace GestureSpace {
	/**@#+*/

	/**
	* Generates concrete joint angles; BAP file output.
	*
	* Given an abstract gesture plan provided by class 
	* GesturePlanner,
	* creates BAP (== joint angle) representations of key frames and
	* performs smooth angle interpolation using TCB splines.
	*
	* @see GesturePlanner 
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class MotorPlanner  
	{
	public:
		/** return a specified frame from the final animation 
		* 
		* called by BAPPlayer/PlayerWindow */
		BAPFrame* GetFrame(int number);

		BAPFrameVector* GetAnimation(void);

		/** Initializes MotorPlanner and calculates JointAngles for key frames 
		*
		* Init steps through the GestureVector, and for every GesturePhase of every Gesture,
		* calculates joint angles by calling GesturePhase::GetAngles()
		* the joint angles are stored in mpLeftArmKeys and mpRightArmKeys */
		void Init( GestureVector* vGesture, int sframe, int eframe);

		/** Generates all BAPFrames using Interpolators and stores the result in mpAnimationFrames*/
		void GenerateAnimation( );
		/** Generates all BAPFrames using Interpolators and writes the result to BAP file argFileName*/
		void WriteAnimation(string argFileName);

		/**
		*
		* @param bf
		* @param framenumber
		*/
		void AddBAPFrame(BAPFrame *bf,int framenumber);
		/**
		*
		* @return
		*/
		BAPFrame *GetLastBAPKeyFrame();
		/**
		*
		* @param returnbf
		*/

		void GetCopyLastBAPKeyFrame(BAPFrame *returnbf);
		/**
		*
		*
		* @return
		*/

		BAPFrame *GetLastBAPFrame();
		/**
		*
		* @param returnbf
		*/
		void GetCopyLastBAPFrame(BAPFrame *returnbf);

		/**
		* constructor
		*
		*/

		MotorPlanner();

		/**
		* destructor
		*
		*/
		virtual ~MotorPlanner();

		/** has Init been called? i.e. can interpolated results be queried? */
		bool				mInitialized;

	private:

		/** :TODO: Implement! Enforces Joint Rotation Limits */
		void ApplyJointLimits(BAPFrame& rFrame);

		/** Adjusts timing of wrist relative to fingers (see Lasseter, 1987, p. 39)*/
		void FollowThrough(BAPFrame* pBFrame, GesturePhase* pGFrame, SideType side, int prevFrame);

		/** Converts Arm keys into separate Shoulder, Elbow, Wrist and Finger Keys */
		bool InitSplines2(AbstractInterpolator& rInterpolator, BAPFrameVector& rArmKeys, SideType side, DOFGroup group);

		/** apply perlin noise function on torso */
		void ApplyTorsoPerlinNoise(BAPFrame &f, int fnum);

		/** Add fake frames if there a re less then 4 frames in the vector */
		void UpToFour(BAPFrameVector *frame_vector);

		void AddKeyFrames(Gesture* gesture, GesturePhase * phase, BAPFrame * pBAPf, int * prev_frame);

		/** first frame (why float?) in animation*/
		int				mStartFrame;

		/** last frame (why float?) in animation*/
		int				mEndFrame;

		/** Storage vector for key frames of left arm */
		BAPFrameVector		mpLeftArmKeys;

		/** Storage vector for key frames of right arm */
		BAPFrameVector		mpRightArmKeys;

		/** Storage vector for all animation frames (post-interpolation)*/
		BAPFrameVector		mpAnimationFrames;

		/** debug output flag*/
		bool				output;
		GestureKinematics	mGks;

	};

	/**@#-*/
} // END namespace GestureSpace
/**@#+*/

#endif // !defined(AFX_MOTORPLANNER_H__EE4A5782_5C04_4785_AD69_46385AB2545C__INCLUDED_)
