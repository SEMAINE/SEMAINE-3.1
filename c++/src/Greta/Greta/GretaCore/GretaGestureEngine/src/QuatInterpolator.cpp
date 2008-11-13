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

// QuatInterpolator.cpp: implementation of the QuatInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#include "QuatInterpolator.h"

//turn of possible loss of data through conversion warning
#pragma warning( disable : 4244 ) 

using namespace GestureSpace;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QuatInterpolator::QuatInterpolator(enum SideType s)
{
	side=s;
	mQWrist.clear();
	mQElbow.clear();
	mQShoulder.clear();
	mQWristOffset=0;
	mQElbowOffset=0;
	mQShoulderOffset=0;
}

QuatInterpolator::~QuatInterpolator()
{

}

void QuatInterpolator::Init(BAPFrameVector v, enum DOFGroup st) {
	
	if (st==WRIST_DOFs) {
		QuatInit(v,WRIST_SPLINE);
	}
	if (st==ARM_DOFs) {
		QuatInit(v,ELBOW_SPLINE);
		QuatInit(v,SHOULDER_SPLINE);
	}
	if (st==FINGER_DOFs) {
		assert(st!=FINGER_SPLINE);
	}

}
void QuatInterpolator::QuatInit(BAPFrameVector v, enum SplineType st)
{
	
	// reserve space in storage vector
	if (st==WRIST_SPLINE) {
		mQWrist.reserve(v.back()->GetFrameNumber()-v.front()->GetFrameNumber()+2);
		mQWristOffset=v.front()->GetFrameNumber();
	} else if (st==ELBOW_SPLINE) {
		mQElbow.reserve(v.back()->GetFrameNumber()-v.front()->GetFrameNumber()+2);
		mQElbowOffset=v.front()->GetFrameNumber();
	} else if (st==SHOULDER_SPLINE) {
		mQShoulder.reserve(v.back()->GetFrameNumber()-v.front()->GetFrameNumber()+2);
		mQShoulderOffset=v.front()->GetFrameNumber();
	
	}

	
	int size = v.size();
	int i;
	float rx,ry,rz;

	QuatVector vQ;
	QuatVector vA,vB; 
	Matrix3f rm;
	
	// Construct Quaternion vector q_0..q_(size-1)
	for (i=0; i<size; i++) {
		if (st==WRIST_SPLINE) {
			
			rx=float(v[i]->GetBAPAngle(wrist_pivot,side,radians));
			ry=float(v[i]->GetBAPAngle(wrist_twisting,side,radians)); //FORGOT SIDE HERE!
			rz=float(v[i]->GetBAPAngle(wrist_flexion,side,radians));  //AND HERE!
			rm.FromEulerAnglesZYX(rz,ry,rx);
		} else if (st==ELBOW_SPLINE) {
			rx=float(v[i]->GetBAPAngle(elbow_flexion,side,radians));
			ry=float(v[i]->GetBAPAngle(elbow_twisting,side,radians));
			rz=0.0;
			rm.FromEulerAnglesZYX(rz,ry,rx);
		} else if (st==SHOULDER_SPLINE) {
			rx=float(v[i]->GetBAPAngle(shoulder_flexion,side,radians));
			ry=float(v[i]->GetBAPAngle(shoulder_abduct,side,radians));
			rz=float(v[i]->GetBAPAngle(shoulder_twisting,side,radians));
			rm.FromEulerAnglesZYX(rz,ry,rx);
		}
		Quaternionf Q;
		Q.FromRotationMatrix(rm);
		vQ.push_back(Q);
		
	}
	
	// Construct intermediate quaternions a_0 .. a_(size-1)
	// using TCB Spline Interpolation
	// Formulas can be found in: Eberly, Key frame interpolation via splines and quaternions
	
	QuatVector::iterator qThis;
	QuatVector::iterator qPrev=vQ.begin();
	QuatVector::iterator qNext;
	i=0;
	float tension,continuity,bias;
	for(qThis=vQ.begin();qThis!=vQ.end();qThis++) {
		
		qNext=qThis+1;
		if(qNext==vQ.end())
			qNext--;
		
		
		tension=v[i]->GetTCBParam(Tension);
		continuity=v[i]->GetTCBParam(Continuity);
		bias=v[i]->GetTCBParam(Bias);

		float frac1=((1.0-tension)*(1.0-continuity)*(1.0-bias))/(2.0);
		float frac2=((1.0-tension)*(1.0+continuity)*(1.0+bias))/(2.0);
		float frac3=((1.0-tension)*(1.0+continuity)*(1.0-bias))/(2.0);
		float frac4=((1.0-tension)*(1.0-continuity)*(1.0+bias))/(2.0);
		Quaternionf term1=(qThis->Inverse()*(*qNext)).Log();
		Quaternionf term2=(qPrev->Inverse()*(*qThis)).Log();
		Quaternionf T0= (frac1*term1)+(frac2*term2);
		Quaternionf T1= (frac3*term1)+(frac4*term2);
		Quaternionf A=(*qThis)*((T0-term1)*0.5).Exp();
		Quaternionf B=(*qThis)*((term2-T1)*0.5).Exp();
		vA.push_back(A);
		vB.push_back(B);
		/* Quaternion term1= (qThis->Inverse()*(*qNext)).Log();
		Quaternion term2= (qThis->Inverse()*(*qPrev)).Log();
		Quaternion term3= ((-0.25)*(term1+term2)).Exp();
		Quaternion A = (*qThis)*term3;
		vA.push_back(A);*/
		

		if (qThis!=vQ.begin()) qPrev++;
		i++;
	}
	
	// now go through and compute interpolated quaterions S_n(t),
	// convert back into euler angles and store them in solution vector
	float t,solx,soly,solz;
	Quaternionf S,intA,intB;
	Matrix3f mr;
	for (i=0; i<(size-1); i++) {
		int j0,j1,j;
		j0=v[i]->GetFrameNumber();
		j1=v[i+1]->GetFrameNumber();

		// Store Keyframe Quaternions: 
		S=vQ[i];
		S.ToRotationMatrix(mr);
		mr.ToEulerAnglesZYX(solz,soly,solx); //result in radians
		StoreQSolution(j0,solx,soly,solz, st);
		
		// Store Interpolated Quaternions:
		for(j=(j0+1);j<j1;j++) {
			
			//LINEAR TIME INTERPOLATION! INSERT TIMING FUNCTION HERE
			t=float(j-j0)/float(j1-j0);
			
			//OLDEST: S=S.Squad(t,vQ[i], vA[i], vA[i+1], vQ[i+1]);
			//S=S.Squad(t,vQ[i],vA[i],vB[i+1],vQ[i+1]);
			
			//SQUAD interpolation: doesn't work yet
			//S.Intermediate(vQ[i],vQ[i+2],vQ[i+2],intA,intB);
			//S=Squad(t,vQ[i],intA,intB,vQ[i+1]);
			//cout<<"QuatInterp - Slerp - j0:"<<j0<<" j:"<<j<<" j1:"<<j1<<";  t="<<t<<endl;
			SlerpQuat2(&vQ[i],&vQ[i+1],t,&S);
			

			S.ToRotationMatrix(mr);
			mr.ToEulerAnglesZYX(solz,soly,solx);
			StoreQSolution(j,solx,soly,solz, st);
		}
	}
	
	// Store Last Quaternion:
	S=vQ[size-1];
	S.ToRotationMatrix(mr);
	mr.ToEulerAnglesZYX(solz,soly,solx);
	StoreQSolution(v.back()->GetFrameNumber(),solx,soly,solz, st);
}

void QuatInterpolator::GetFrame(BAPFrame &f, int n, enum DOFGroup t)
{
	int debugint=-1;
	f.SetFrameNumber(n);
	
	/////////////////////// get arm spline values ////////////////////////////////
	if (t==ARM_DOFs) {

		///////////////////// get shoulder spline values ////////////////////////
		f.SetBAP(shoulder_flexion,side,GetQSolution(n,SHOULDER_SPLINE).X());
		f.SetBAP(shoulder_abduct,side,GetQSolution(n,SHOULDER_SPLINE).Y());
		f.SetBAP(shoulder_twisting,side,GetQSolution(n,SHOULDER_SPLINE).Z());
		///////////////////// get elbow spline values ////////////////////////////////
		f.SetBAP(elbow_flexion,side,GetQSolution(n,ELBOW_SPLINE).X());
		f.SetBAP(elbow_twisting,side,GetQSolution(n,ELBOW_SPLINE).Y());
	}
	
	
	///////////////////// get wrist spline values ////////////////////////////////
	if (t==WRIST_DOFs) {
		// USING QUATERNION INTERPOLATION:
		f.SetBAP(wrist_twisting,side,GetQSolution(n,WRIST_SPLINE).Y());
		f.SetBAP(wrist_flexion,side,GetQSolution(n,WRIST_SPLINE).Z());
		f.SetBAP(wrist_pivot,side,GetQSolution(n,WRIST_SPLINE).X());
	}
	
	
	///////////////////// get finger spline values ////////////////////////////////
	if (t==FINGER_DOFs) {
		assert(0==1);
		// Quaternion interpolation not YET defined for fingers!
	}
}


void QuatInterpolator::StoreQSolution(int frame, float x, float y, float z, enum SplineType t)
{
	if (t==WRIST_SPLINE) {	
		mQWrist[frame-mQWristOffset]=Vector3f(x*100000.0,y*100000.0,z*100000.0);
		if (output) cout<< "D QUATERNION: frame= "<<frame<<", XYZ=("<<x*RTOD<<", "
				<<y*RTOD<<", "<<z*RTOD<<")"<<endl;
	}
	else if (t==ELBOW_SPLINE) {
		mQElbow[frame-mQElbowOffset]=Vector3f(x*100000.0,y*100000.0,z*100000.0);
		if (output) cout<< "D QUATERNION: frame= "<<frame<<", XYZ=("<<x*RTOD<<", "
				<<y*RTOD<<", "<<z*RTOD<<")"<<endl;
	
	}
	else if (t==SHOULDER_SPLINE) {
		mQShoulder[frame-mQShoulderOffset]=Vector3f(x*100000.0,y*100000.0,z*100000.0);
		if (output) cout<< "D QUATERNION: frame= "<<frame<<", XYZ=("<<x*RTOD<<", "
				<<y*RTOD<<", "<<z*RTOD<<")"<<endl;
	}
	else assert(0==1);
}

Vector3f QuatInterpolator::GetQSolution(int frame, enum SplineType t)
{
	if (t==WRIST_SPLINE) {	
		return mQWrist[frame-mQWristOffset];
	}
	else if (t==ELBOW_SPLINE)  {	
		return mQElbow[frame-mQElbowOffset];
	}
	else if (t==SHOULDER_SPLINE) {	
		return mQShoulder[frame-mQShoulderOffset];
	}
	else{
		assert(0==1); // ERROR!
		return Vector3f(0,0,0);
	}// NEVER REACHED, JUST TO SILENCE WARNING C4715
}


///////////////////////////////////////////////////////////////////////////////
// Function:	SlerpQuat2
// Purpose:		Spherical Linear Interpolation Between two Quaternions
// Arguments:	Two Quaternions, blend factor, result quaternion
///////////////////////////////////////////////////////////////////////////////
#define DELTA	0.001		// DIFFERENCE AT WHICH TO LERP INSTEAD OF SLERP
void QuatInterpolator::SlerpQuat2(Quaternionf *quat1,Quaternionf *quat2,float slerp, Quaternionf *result)
{
/// Local Variables ///////////////////////////////////////////////////////////
	Quaternionf quat1b;
	double omega,cosom,sinom,scale0,scale1;
///////////////////////////////////////////////////////////////////////////////
	// USE THE DOT PRODUCT TO GET THE COSINE OF THE ANGLE BETWEEN THE
	// QUATERNIONS
	cosom = quat1->X() * quat2->X() + 
			quat1->Y() * quat2->Y() + 
			quat1->Z() * quat2->Z() +
			quat1->W() * quat2->W(); 

	// MAKE SURE WE ARE TRAVELING ALONG THE SHORTER PATH
	if (cosom < 0.0)
	{
		// IF WE ARE NOT, REVERSE ONE OF THE QUATERNIONS
		cosom = -cosom;
		quat1b.X() = - quat1->X();
		quat1b.Y() = - quat1->Y();
		quat1b.Z() = - quat1->Z();
		quat1b.W() = - quat1->W();
	} else {
		quat1b.X() = quat1->X();
		quat1b.Y() = quat1->Y();
		quat1b.Z() = quat1->Z();
		quat1b.W() = quat1->W();
	}

	
	if ((1.0 - cosom) > DELTA) {
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - double(slerp)) * omega) / sinom;
		scale1 = sin(double(slerp) * omega) / sinom;
	} else {
		scale0 = 1.0 - slerp;
		scale1 = slerp;
	}

	result->X() = (float)(scale0 * quat1b.X() + scale1 * quat2->X());
	result->Y() = (float)(scale0 * quat1b.Y() + scale1 * quat2->Y());
	result->Z() = (float)(scale0 * quat1b.Z() + scale1 * quat2->Z());
	result->W() = (float)(scale0 * quat1b.W() + scale1 * quat2->W());
}
// SlerpQuat2  /////////////////////////////////////////////////////////////////