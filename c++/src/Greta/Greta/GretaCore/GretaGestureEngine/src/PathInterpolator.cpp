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

// PathInterpolator.cpp: implementation of the PathInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#include "PathInterpolator.h"
#include "GestureKinematics.h"

//turn of possible loss of data through conversion warning
#pragma warning( disable : 4244 ) 

using namespace GestureSpace;
using namespace Wm4;

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif
#ifndef DTOR
#define DTOR(x) ((x)*M_PI/180.0)
#endif
#ifndef RTOD
#define RTOD(x) ((x)*180.0/M_PI)
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//ARC-LENGTH-PARAMETRIZATION SUPPORT:
// length-from-time and time-from-length
// virtual float GetLength (float fT0, float fT1) const;
// virtual float GetTime (float fLength, int iIterations = 32,
// float fTolerance = 1e-06f) const;

PathInterpolator::PathInterpolator(enum SideType s)
{
	side=s;
}

PathInterpolator::~PathInterpolator()
{

}

/** Given a vector of pointers to BAPFrames, creates TCB splines */ 
void PathInterpolator::Init(BAPFrameVector v, enum DOFGroup t) 
{
	assert(t==ARM_DOFs);

	int size = v.size();
	int i;

	//if(size<4)
		//printf("GestureEngine: warning, interpolation failed (less than 4 points)\n");


	///////////////////// create spline for wristposition //////////////////////////////
	Vector3f* sPts = new Vector3f[size]; 
	Vector2f* sSwivels = new Vector2f[size];

	float* sT = new float[size];
	float* sC = new float[size];
	float* sB = new float[size];
	float* sTime = new float[size];
	float swivel;

	for (i=0; i<size; i++) {
		alg3::vec3 pos= mGK.FkSolve(side,*v[i], &swivel);
		sPts[i]=Vector3f(pos[0],pos[1],pos[2]);

		//////////////////////////////////////////////////////////////////////////
		//IK again to print test output:
		//apply inverse kinematics to extract joint angles
		//float flexion;
		//alg3::vec3 shoulder_angles = 
		//	mGK.IkSolveByPosition(alg3::vec3(pos[0],pos[1],pos[2]),&flexion,side);
		//printf("IK   : sx %2.2f, sy %2.2f, sz %2.2f, ex: %2.2f, ey 0.0\n\n",
		//		shoulder_angles[0],shoulder_angles[1],shoulder_angles[2],flexion);
		///////////////////////////////////////////////////////////////////////////


		// set tension, continuity, bias, time:
		// sT[i]    = 0.0; sC[i]	 = 0.0;  sB[i]	 = 0.0;
		sT[i]=v[i]->TCBParam[Tension];
		sC[i]=v[i]->TCBParam[Continuity];
		sB[i]=v[i]->TCBParam[Bias];
		sTime[i] = (float(v[i]->GetFrameNumber()));
		sSwivels[i]=Vector2f(swivel,0.0);
		//printf("Swivel angle: %2.2f (side %d frame %2f)\n",RTOD(swivel),side,sTime[i]);
	}
	mpTCB3_Path= new TCBSpline3f( (size-1),sTime,sPts,sT,sC,sB );
	mpTCB2_Swivel=new TCBSpline2f((size-1),sTime,sSwivels,sT,sC,sB);
	//mpTCB2_Time=new TCBSpline2(  (size-1),s
}


void PathInterpolator::GetFrame(BAPFrame &f, int n, enum DOFGroup t)
{
	assert(t==ARM_DOFs);

	int debugint=-1;
	f.SetFrameNumber(n);

	//get interpolated wrist position
	Vector3f	pos   = mpTCB3_Path->GetPosition(float(n));
	Vector2f swivel= mpTCB2_Swivel->GetPosition(float(n));

	float flexion;
	
	//set swivel angle
	mGK.SetSwivel(swivel[0],side);

	//apply inverse kinematics to extract joint angles
	alg3::vec3 shoulder_angles = 
		mGK.IkSolveByPosition(alg3::vec3(pos[0],pos[1],pos[2]),&flexion,side);

	//write angles to BAP frame
	if(side==r) {
		f.SetBAP(string("r_shoulder.rotateX"),degrees,shoulder_angles[0]);
		f.SetBAP(string("r_shoulder.rotateY"),degrees,shoulder_angles[1]);
		f.SetBAP(string("r_shoulder.rotateZ"),degrees,shoulder_angles[2]);
		f.SetBAP(string("r_elbow.rotateX"), degrees, flexion);
		f.SetBAP(string("r_elbow.rotateY"),degrees, 0.f);
	} else {
		f.SetBAP(string("l_shoulder.rotateX"),degrees,shoulder_angles[0]);
		f.SetBAP(string("l_shoulder.rotateY"),degrees,shoulder_angles[1]);
		f.SetBAP(string("l_shoulder.rotateZ"),degrees,shoulder_angles[2]);
		f.SetBAP(string("l_elbow.rotateX"), degrees, flexion);
		f.SetBAP(string("l_elbow.rotateY"),degrees, 0.f);

	}
}
