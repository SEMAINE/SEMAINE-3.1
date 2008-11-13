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

// GestureKinematics.h: interface for the GestureKinematics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GESTUREKINEMATICS_H__EFE9BF49_8650_43D5_8949_914057637052__INCLUDED_)
#define AFX_GESTUREKINEMATICS_H__EFE9BF49_8650_43D5_8949_914057637052__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#include "ReachSpace.h"	// Added by ClassView
#include "Wm4Matrix3.h"
#include "srs.h"
//#include "myvec.h"

#include "BAPFrame.h"
//#include "baptypes.h"

namespace GestureSpace {

/** Forward and Inverse Kinematics routines for the arms of GestureEngine. */	
class GestureKinematics
{
public:
	
	/** Returns scalar distance between two keypoints. */
	double FindDistance(BAPFrame &fFrom, BAPFrame &fTo, enum SideType side);
	
	/** Defines a swivel angle to use for inverse kinematics. USED? */
	void SetSwivel(float value, enum SideType s);
	
	/** Forward Kinematics: returns position of wrist w.r.t.Humanoid Root joint. */
	alg3::vec3	FkSolve( enum SideType side, BAPFrame &f, float *swivel );
	
	/** Inverse Kinematics: given reachspace sectors X,Y,Z, writes shoulder+elbow angles into BAPFrame f. */
	void		IkSolveBySector(int sectorX, int sectorY, int sectorZ, SideType side, BAPFrame &f,
				float scaleX = 1.0f, float scaleY=1.0f, float scaleZ=1.0f);

	alg3::vec3	GetCoordinates(int sectorX, int sectorY, int sectorZ, SideType side,
				float scaleX = 1.0f, float scaleY=1.0f, float scaleZ=1.0f);

	void		IkSolveByPositionAndWriteInBAP(alg3::vec3 Goal, SideType side, BAPFrame &f);
	
	/** Inverse Kinematics: given goal position w.r.t. Humanoid Root., calculates elbow+shoulder angles. 
		Elbow flexion variable is overwritte. Explicitly returns shoulder rotations in X,Y,Z euler angles
		in our rotation scheme (rot order XYZ / ZYX) */
	alg3::vec3	IkSolveByPosition(alg3::vec3 goal, float *flexion, SideType side=r);
	
	GestureKinematics();
	virtual ~GestureKinematics();
	
private:
	
	/** compute swivel angle based one elbow, hand position rel. to shoulder */
	float ComputeSwivelAngle(alg3::vec3 &elbow, alg3::vec3 &hand);
	
	/** compute angle from cosine, sine terms */
	double ComputeAngle(double c, double s);
	
	/** representation of the reach space for spatial expressivity */
	ReachSpace gSpace;
	
	alg3::vec3  r_shoulder; /**< right shoulder coords (w.r.t humanoidroot?) */
	alg3::vec3  r_s2e;		/**< right shoulder to elbow translation */
	alg3::vec3  r_e2w;		/**< right elbow to wrist translation */
	alg3::vec3  l_shoulder; /**< left shoulder coords (w.r.t. humanoidroot?) */
	alg3::vec3  l_s2e;		/**< left shoulder to elbow translation */
	alg3::vec3  l_e2w;		/**< left elbow to wrist translation */
	
	float r_swivel;			/**< right swivel angle for IK calculation */
	float l_swivel;			/**< left swivel angle for IK calculation */
};

};
#endif // !defined(AFX_GESTUREKINEMATICS_H__EFE9BF49_8650_43D5_8949_914057637052__INCLUDED_)
