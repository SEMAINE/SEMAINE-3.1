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

// WristAngleSkeleton.cpp: implementation of the WristAngleSkeleton class.
//
//////////////////////////////////////////////////////////////////////
#include "WristAngleSkeleton.h"
#include "IniManager.h"

extern IniManager inimanager;

/**@#-*/
using namespace GestureSpace;
extern FILE* data_log;
/**@#+*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WristAngleSkeleton::WristAngleSkeleton()
{
	output=false;
	Root= new Joint;
	Root->type=HumanoidRoot;
	Root->Translate(0.0000,0.8240,0.0277);
	
	lShoulder= new Joint;
	lShoulder->type=l_shoulder;//was: r_shoulder
	lShoulder->Translate(0.2029, 0.6136, -0.0664);
	lShoulder->SetParent(Root);
	
	rShoulder= new Joint;
	rShoulder->type=r_shoulder;
	rShoulder->Translate(-0.1907, 0.6167, -0.0602);
	rShoulder->SetParent(Root);
	
	lElbow= new Joint;
	lElbow->type=l_elbow;
	lElbow->Translate(-0.0015, -0.3019, -0.0295);
	lElbow->SetParent(lShoulder);


	rElbow= new Joint;
	rElbow->type=r_elbow;
	rElbow->Translate(-0.0042, -0.3019, -0.0295);
	rElbow->SetParent(rShoulder);


	lWrist= new Joint;
	lWrist->type=l_wrist;
	lWrist->Translate(-0.003, -0.2694, 0.0099);
	lWrist->SetParent(lElbow);

	rWrist= new Joint;
	rWrist->type=r_wrist;
	rWrist->Translate(-0.001,  -0.2694, 0.0099);
	rWrist->SetParent(rElbow);

	Root->SetWorldMatrix();
	jointLimits=new JointLimitWrapper();

}

WristAngleSkeleton::~WristAngleSkeleton()
{
	delete rWrist;
	delete lWrist;
	delete rElbow;
	delete lElbow;
	delete rShoulder;
	delete lShoulder;
}

//////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

void WristAngleSkeleton::GetWristAngles(BAPFrame &f, PalmType palmFrom, 
							FingerBaseType fbFrom,
							PalmType palm, 
							FingerBaseType fb, 
							SideType side, 
							float slerp)
{
	// DEFAULT CASE: SET TO 0 AND RETURN:
	if ((fb == FBDefault) && (palm == PalmDefault)) {
		f.SetBAP(wrist_flexion,side,0);
		f.SetBAP(wrist_pivot,side,0);
		f.SetBAP(wrist_twisting,side,0);
		return;
	}

	//MAURIZIO 5-7-2007:
	//when the wrist has only the "from" position the following two
	//variables are set to none. in this case we should set them
	//to the same value of the "from" position
	if (fb == FBNone)
		fb=fbFrom;
	if (palm == PalmNone)
		palm=palmFrom;

	// Load arm angles from BAPframe
	double sx, sy, sz;
	double ex,ey;
	if (side == r) {
		sx = f.GetBAPAngle("r_shoulder.rotateX",degrees);
		sy = f.GetBAPAngle("r_shoulder.rotateY",degrees);
		sz = f.GetBAPAngle("r_shoulder.rotateZ",degrees);
		ex = f.GetBAPAngle("r_elbow.rotateX",degrees);
		ey = f.GetBAPAngle("r_elbow.rotateY",degrees);
		rShoulder->RotateAbsXYZ(sx, sy, sz); 
		rElbow->RotateAbsXYZ(ex,ey,0);
	
	} else if (side == l) {
		sx = f.GetBAPAngle("l_shoulder.rotateX",degrees);
		sy = f.GetBAPAngle("l_shoulder.rotateY",degrees)*(-1.0);
		sz = f.GetBAPAngle("l_shoulder.rotateZ",degrees)*(-1.0);
		ex = f.GetBAPAngle("l_elbow.rotateX",degrees);
		ey = f.GetBAPAngle("l_elbow.rotateY",degrees)*(-1.0);
		lShoulder->RotateAbsXYZ(sx, sy, sz);
		lElbow->RotateAbsXYZ(ex,ey,0);
	} else {
		if (output) 
			std::cout << "! get_wrist_angles: no valid side specified ("<<side<<")!\n";
		return;
	}
	
	// Create alignment matrices in world space
	mat4 m_fromAlign=CreateAlignMatrix(palmFrom,fbFrom);
	mat4 m_align=CreateAlignMatrix(palm,fb);

	// FINAL ROTATION MATRIX IS INVERSE OF WRIST WORLD MATRIX *
	// INVERSE OF ALIGNMENT MATRIX
	// M_wrist<-align = M_wrist<-world * M_world<-align
	mat4 M_world_to_wrist, M_align_to_world, M_fromAlign_to_world, mT,mF;
	
	if(side==r)
		M_world_to_wrist=rWrist->wm.inverse();
	else 
		M_world_to_wrist=lWrist->wm.inverse();

	// set translation components to 0 and scale to 1
	M_world_to_wrist[0][3]=
		M_world_to_wrist[1][3]=
		M_world_to_wrist[2][3]=0.f;
		M_world_to_wrist[3][3]=1.f;
	
	M_align_to_world=m_align.inverse();
	M_fromAlign_to_world=m_fromAlign.inverse();

	mT=M_world_to_wrist*M_align_to_world;
	mF=M_world_to_wrist*M_fromAlign_to_world;
	

	// CREATE MGC_TYPE MATRIX AND CONVERT BACK TO EULER ANGLES
	Wm4::Matrix3f mgcTo(mT[0][0],mT[0][1],mT[0][2],
		mT[1][0],mT[1][1],mT[1][2],
		mT[2][0],mT[2][1],mT[2][2]);
	
	Wm4::Matrix3f mgcFrom(mF[0][0],mF[0][1],mF[0][2],
		mF[1][0],mF[1][1],mF[1][2],
		mF[2][0],mF[2][1],mF[2][2]);

	// SLERP BETWEEN from and to alignments:
	//printf("GetWristAngles: Using wrist magnitude %2.2f\n",slerp);
	
	Wm4::Quaternionf qTo,qFrom,qSlerp;
	qFrom.FromRotationMatrix(mgcFrom);
	qTo.FromRotationMatrix(mgcTo);
	SlerpQuat2(&qFrom,&qTo,slerp,&qSlerp);
	qSlerp.ToRotationMatrix(mgcTo);
	
	// convert matrix back to euler angles
	float x,y,z;
	mgcTo.ToEulerAnglesZYX(z,y,x);
	if (output) cout << ". Found Euler-angles: ("<<x*RTOD<<", "<<y*RTOD<<", "<<z*RTOD<<")"<<endl;
	

	// APPLY JOINT LIMITS
	// and write solution back to bap frame
	if (side == r) {

		if (inimanager.GetValueInt("ENGINE_USE_JOINT_LIMITS")==1) {
			jointLimits->CheckRWristXYZ(x,y,z);
		}
		f.SetBAP("r_wrist.rotateX",radians,x);
		f.SetBAP("r_wrist.rotateY",radians,y);
		f.SetBAP("r_wrist.rotateZ",radians,z);
	} else if (side == l) {
		y=-y;
		z=-z;
		if (inimanager.GetValueInt("ENGINE_USE_JOINT_LIMITS")==1) {
			jointLimits->CheckLWristXYZ(x,y,z);
		}
		f.SetBAP("l_wrist.rotateX",radians,x);
		f.SetBAP("l_wrist.rotateY",radians,y);
		f.SetBAP("l_wrist.rotateZ",radians,z);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	SlerpQuat2
// Purpose:		Spherical Linear Interpolation Between two Quaternions
// Arguments:	Two Quaternions, blend factor, result quaternion
///////////////////////////////////////////////////////////////////////////////
#define DELTA	0.001		// DIFFERENCE AT WHICH TO LERP INSTEAD OF SLERP
void WristAngleSkeleton::SlerpQuat2(Quaternionf *quat1,Quaternionf *quat2,float slerp, Quaternionf *result)
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

alg3::mat4 WristAngleSkeleton::CreateAlignMatrix(PalmType palm, FingerBaseType fb)
{
	// create 4x4 alignment matrix:
	mat4	m_align = identity3D();


	// GROUP 1:
	if ((fb == FBDown) && (palm == PalmInwards)) {
		m_align = identity3D();
	} 
	else if ((fb == FBDown) && (palm == PalmOutwards)) {
		m_align = mat4( vec4(-1,0,0,0),
						vec4(0,1,0,0),
						vec4(0,0,-1,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBUp) && (palm == PalmInwards)) {
		m_align = mat4( vec4(1,0,0,0),
						vec4(0,-1,0,0),
						vec4(0,0,-1,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBUp) && (palm == PalmOutwards)) {
		m_align = mat4( vec4(-1,0,0,0),
						vec4(0,-1,0,0),
						vec4(0,0,1,0),
						vec4(0,0,0,1));
	}
	
	// GROUP 2: X, Z, Y
	else if ((fb == FBAway) && (palm == PalmInwards)) {
		m_align = mat4( vec4(1,0,0,0),
						vec4(0,0,-1,0),
						vec4(0,1,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBTowards) && (palm == PalmOutwards)) {
		m_align = mat4( vec4(-1,0,0,0),
						vec4(0,0,1,0),
						vec4(0,1,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBAway) && (palm == PalmOutwards)) {
		m_align = mat4( vec4(-1,0,0,0),
						vec4(0,0,-1,0),
						vec4(0,-1,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBTowards) && (palm == PalmInwards)) {
		m_align = mat4( vec4(1,0,0,0),
						vec4(0,0,1,0),
						vec4(0,-1,0,0),
						vec4(0,0,0,1));
	}
	
	// GROUP 3: Y, X, Z
	else if ((fb == FBOutwards) && (palm == PalmUp)) {
		m_align = mat4( vec4(0,1,0,0),
						vec4(1,0,0,0),
						vec4(0,0,-1,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBOutwards) && (palm == PalmDown)) {
		m_align = mat4( vec4(0,-1,0,0),
						vec4(1,0,0,0),
						vec4(0,0,1,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBInwards) && (palm == PalmUp)) {
		m_align = mat4( vec4(0,1,0,0),
						vec4(-1,0,0,0),
						vec4(0,0,1,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBInwards) && (palm == PalmDown)) {
		m_align = mat4( vec4(0,-1,0,0),
						vec4(-1,0,0,0),
						vec4(0,0,-1,0),
						vec4(0,0,0,1));
	}
	// GROUP 4: Y Z X
	else if ((fb == FBTowards) && (palm == PalmUp)) {
		m_align = mat4( vec4(0,1,0,0),
						vec4(0,0,1,0),
						vec4(1,0,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBAway) && (palm == PalmDown)) {
		m_align = mat4( vec4(0,-1,0,0),
						vec4(0,0,-1,0),
						vec4(1,0,0,0),
						vec4(0,0,0,1));
		
	}
	else if ((fb == FBAway) && (palm == PalmUp)) {
		m_align = mat4( vec4(0,.99999f,0,0),
						vec4(0,0,-1.f,0),
						vec4(-1.f,0,0,0),
						vec4(0,0,0,1.f));
		// :KLUDGE: FBAway&&PlamUp matrix hardcoded from maya:
		/*m_align=mat4(vec4(0.152919,0.988239,0,0),
			vec4(-0.115239,0.017832,-0.993178,0),
			vec4(-0.981497,0.151876,0.11661,0),
			vec4(0, 82.4, 2.77, 1)); */

		
	}
	else if ((fb == FBTowards) && (palm == PalmDown)) {
		m_align = mat4( vec4(0,-1,0,0),
						vec4(0,0,1,0),
						vec4(-1,0,0,0),
						vec4(0,0,0,1));
	}
	
	// GROUP 5: Z X Y
	else if ((fb == FBOutwards) && (palm == PalmAway)) {
		m_align = mat4( vec4(0,0,1,0),
						vec4(1,0,0,0),
						vec4(0,1,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBInwards) && (palm == PalmTowards)) {
		m_align = mat4( vec4(0,0,-1,0),
						vec4(-1,0,0,0),
						vec4(0,1,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBOutwards) && (palm == PalmTowards)) {
		m_align = mat4( vec4(0,0,-1,0),
						vec4(1,0,0,0),
						vec4(0,-1,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBInwards) && (palm == PalmAway)) {
		m_align = mat4( vec4(0,0,1,0),
						vec4(-1,0,0,0),
						vec4(0,-1,0,0),
						vec4(0,0,0,1));
	}

	// GROUP 6: Z Y X
	else if ((fb == FBDown) && (palm == PalmTowards)) {
		m_align = mat4( vec4(0,0,-1,0),
						vec4(0,1,0,0),
						vec4(1,0,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBUp) && (palm == PalmAway)) {
		m_align = mat4( vec4(0,0,1,0),
						vec4(0,-1,0,0),
						vec4(1,0,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBDown) && (palm == PalmAway)) {
		m_align = mat4( vec4(0,0,1,0),
						vec4(0,1,0,0),
						vec4(-1,0,0,0),
						vec4(0,0,0,1));
	}
	else if ((fb == FBUp) && (palm == PalmTowards)) {
		m_align = mat4( vec4(0,0,-1,0),
						vec4(0,-1,0,0),
						vec4(-1,0,0,0),
						vec4(0,0,0,-1));
	}
	return m_align;

	//else return;

}