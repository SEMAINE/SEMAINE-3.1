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

// GestureKinematics.cpp: implementation of the GestureKinematics class.
//
//////////////////////////////////////////////////////////////////////

//#include <cassert>
//#include "Wm4Matrix3.h"
#include "ArmCoordinates.h"
#include "GestureKinematics.h"


#include "IniManager.h"
extern IniManager inimanager;

extern FILE* data_log;

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif
#ifndef DTOR
#define DTOR(x) ((x)*M_PI/180.0)
#endif
#ifndef RTOD
#define RTOD(x) ((x)*180.0/M_PI)
#endif


using namespace GestureSpace;
using namespace Wm4;
using namespace alg3;
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GestureKinematics::GestureKinematics()
{
	r_shoulder[0]=-19.07f;//x
	r_shoulder[1]=61.67f;//y
	r_shoulder[2]=-6.02f;//z
	r_s2e[0]=-0.420;
	r_s2e[1]=-30.190;
	r_s2e[2]=-2.950;
	r_e2w[0]=-0.100;
	r_e2w[1]=-26.940;
	r_e2w[2]=0.990;
	r_swivel=float(DTOR(-20.0));
	
	l_shoulder[0]=20.29f;
	l_shoulder[1]=61.63f;
	l_shoulder[2]=-6.64f;
	l_s2e[0]=-0.150;
	l_s2e[1]=-30.190;
	l_s2e[2]=-2.950;
	l_e2w[0]=-0.300;
	l_e2w[1]=-26.940;
	l_e2w[2]=0.990;
	l_swivel=float(DTOR(20.0));

}

GestureKinematics::~GestureKinematics()
{

}



void GestureKinematics::IkSolveBySector(int sectorX, int sectorY, int sectorZ, SideType side, BAPFrame &f, float scaleX, float scaleY, float scaleZ) {
	alg3::vec3 vGoal= GetCoordinates(sectorX,sectorY,sectorZ,side,scaleX,scaleY,scaleZ);
	IkSolveByPositionAndWriteInBAP(vGoal, side, f);
}

alg3::vec3 GestureKinematics::GetCoordinates(int sectorX, int sectorY, int sectorZ, SideType side, float scaleX, float scaleY, float scaleZ){
#ifdef WITH_EXPRESSIVITY
#ifdef OUTPUT
	fprintf(data_log,"EXPRESSIVITY: scaling gesture space by factors X:%2.2f, Y:%2.2f, Z:%2.2f\n",scaleX,scaleY,scaleZ);
#endif
	//adjust these parameters to change the size of the gesture space.
	gSpace.ScaleSpace(scaleX,scaleY,scaleZ);	
#endif
	return gSpace.GetCoordinates(sectorX,sectorY,sectorZ,side);
}

void GestureKinematics::IkSolveByPositionAndWriteInBAP(alg3::vec3 Goal, SideType side, BAPFrame &f){
	alg3::vec3 shoulderRot;
	float flexion;
	// Call to IK calculation
	shoulderRot= IkSolveByPosition(Goal, &flexion,side);

	// Write results back to BAPFrame
	if (side==r || side==both_sides || side==assym){
		f.SetBAP("r_elbow.rotateX",degrees, flexion);
		f.SetBAP("r_elbow.rotateY",degrees, 0.0f);
		f.SetBAP("r_shoulder.rotateX",degrees,shoulderRot[0]);
		f.SetBAP("r_shoulder.rotateY",degrees,shoulderRot[1]);
		f.SetBAP("r_shoulder.rotateZ",degrees,shoulderRot[2]);
	}
	if (side==l) {
		f.SetBAP("l_elbow.rotateX",degrees, flexion);
		f.SetBAP("l_elbow.rotateY",degrees, 0.0f);
		f.SetBAP("l_shoulder.rotateX",degrees,shoulderRot[0]);
		f.SetBAP("l_shoulder.rotateY",degrees,shoulderRot[1]);
		f.SetBAP("l_shoulder.rotateZ",degrees,shoulderRot[2]);
	}
}

//
alg3::vec3 GestureKinematics::IkSolveByPosition(alg3::vec3 goal, float *flexion, SideType side)
{
	
	alg3::vec3 shoulder,s2e,e2w;//shoulder position vector, using model coord system
	alg3::vec3 result;
	float swivel;

	if(side==r || side==no_side || side==both_sides || side==assym) {
		shoulder=vec3(r_shoulder);
		s2e=vec3(r_s2e);
		e2w=vec3(r_e2w);
		swivel=r_swivel;

	} else if(side==l) {
		shoulder=vec3(l_shoulder);
		s2e=vec3(l_s2e);
		e2w=vec3(l_e2w);
		swivel=l_swivel;

	}
	
	// goal position vector; in shoulder's coordinate system
	vec3 rel_goal=goal-shoulder;

	//swivel reduction when y < some threshold.
	float max=float(s2e.length()+e2w.length());
	float thr=0.7f*max; //threshold
	float reduc_frac=0.5f; //reduction fraction
	float lerp;
	if(rel_goal[1]<0 && (-rel_goal[1])>thr) {
		lerp= ((-rel_goal[1])-thr) / (max-thr);
		fprintf(data_log,"swivel reduction from %2.2f to ",swivel);
		swivel=(1.f-(reduc_frac*lerp))*swivel;
		fprintf(data_log,"%2.2f\n",swivel);
	} 
	// expressivity modulation of swivel angle
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	swivel=swivel+0.5f*(inimanager.GetValueFloat("ENGINE_EXPR_SPC"))*swivel;	
#endif

	
	//NOTE:
	//OpenGL coordinates axes are given this way:
	//      y
	//     |
	//     |
	//    /---- x
	//   /
	//  z
	//
	//while IKAN has a different system:
	// y ----
	//     /|
	//    / |
	//   x  z
	//
	//so proper transformations has to be done between those two systems
	
	// T: shoulder to elbow translation matrix (IKAN system)
	// NOTE: translation is normally positive along z-axis (IKAN system)
	// so direction is not elbow->shoulder as indicated in the documentation
	// but shoudel->elbow
	Matrix T = {{1.0,0.0,0.0,0.0},
				{0.0,1.0,0.0,0.0},
				{0.0,0.0,1.0,0.0},
				{(float)s2e[2], (float)-s2e[0], (float)-(s2e[1]), 1.0f}}; 

	// S: elbow to wrist translation matrix (IKAN system)
	// NOTE: see above
	Matrix S = {{1.0,0.0,0.0,0.0},
				{0.0,1.0,0.0,0.0},
				{0.0,0.0,1.0,0.0},
				{(float)e2w[2], (float)-(e2w[0]), (float)-(e2w[1]), 1.0f}};

	Matrix EE ; // end effector offset matrix
	cpmatrix(EE,idmat); // set to coincide with wrist joint

	Matrix R1; // shoulder rotation matrix; will be computed
	//Matrix R2; // wrist rotation matrix; NOT USED	
	//Matrix G; // goal matrix NOT USED

	// goal position vector; in shoulder's coordinate system applying the
	// proper conversions to transform into IKAN axis system
	//float g[3] = {  float(goal[2]-(shoulder[2])),  
	//				float(-(goal[0]-(shoulder[0]))),  
	//				float(-(goal[1]-(shoulder[1])))
	//			 };
	
	

	// project goal position into reachable space if
	// it is too far removed.
	double maxFactor=0.99; //max percentage of distance to try reaching
	double scaleFactor=0.99;
	double maxLength=maxFactor*(s2e.length()+e2w.length());
	if (rel_goal.length()>=maxLength) {
		fprintf(data_log,"! IkSolveByPosition: Goal too far away (%2.5f > %2.5f); projected back.\n",rel_goal.length(),maxLength);
		rel_goal=rel_goal/rel_goal.length()*scaleFactor*maxLength;
		
	}
	
	//apply proper conversion of goal into IKAN axis system:
	float g[3]={(float)rel_goal[2],-(float)rel_goal[0],-(float)rel_goal[1]};

	float elbow_flexion; // elbow flexion angle in radians; will be computed
	
	
	float a[3]={ 0.0f, 0.0f, 1.0f }; // projection axis (the axis that in the skeleton's
	                                 // rest position is going along the arm; IKAN system)
	float p[3]={ 1.0f, 0.0f, 0.0f }; // positive direction axis (don't know exactly what is
	                                 // used for but it's the axis going ahead from the
	                                 // agent's body; IKAN system)

	
	SRS arm; // the IK solver object
	
	// initialize IK solver with shoulder-elbow, elbow-wrist 
	// translation matrices T and S, projection axis and 
	// positive direction axis
	arm.init(T,S,a,p);
	
	// set reach goal position;
	// given goal position vector relative to shoulder 
	// and end effector offset matrix (rel. to wrist),
	// returns elbow flexion angle in radians:
	int reach;
	reach=arm.SetGoalPos(g, EE, elbow_flexion);
	//m_root->SetRotationX("r_elbow",-RTOD(elbow_flexion));
	*flexion=float(-RTOD(elbow_flexion));

	// solve IK equations:
	// given swivel angle, compute shoulder rotation matrix R1
	arm.SolveR1(swivel, R1);
	
	// or, if we also want wrist orientation: 
	// arm.SolveR1R2(swivel, R1, R2);


	// to extract angles back out:
	// use MagicFM library Matrix3.ToEulerAngles??? function.
	// attention:
	Matrix3f *magic_input = new Matrix3f(R1[0][0], R1[0][1], R1[0][2],
								R1[1][0], R1[1][1], R1[1][2],
								R1[2][0], R1[2][1], R1[2][2]);
	Matrix3f transpose = magic_input->Transpose();

	float yaw,pitch,roll; //extracted Euler angles in radians
	transpose.ToEulerAnglesXZY(yaw,pitch,roll); // yaw->Z,pitch->Y,roll->X
	
	result[0]=-RTOD(roll);
	result[1]=-RTOD(pitch);
	result[2]=RTOD(yaw);

	return result;
}

alg3::vec3 GestureKinematics::FkSolve(enum SideType side, BAPFrame &f, float *swivel)
{
	
	double flexion,twist;
	vec3 sEuler,*shoulder,*s2e,*e2w;
	
	assert((side==r)||(side==l)||(side==both_sides)||(side==assym));

	if (side==r || side==both_sides || side==assym){
		shoulder=&r_shoulder;
		s2e=&r_s2e;
		e2w=&r_e2w;
		// elbow flexion:
		flexion		= f.GetBAPAngle("r_elbow.rotateX",degrees);
		twist		= f.GetBAPAngle("r_elbow.rotateY",degrees);
		// Euler angles for shoulder:
		sEuler[0]	= f.GetBAPAngle("r_shoulder.rotateX",degrees);
		sEuler[1]	= f.GetBAPAngle("r_shoulder.rotateY",degrees);
		sEuler[2]	= f.GetBAPAngle("r_shoulder.rotateZ",degrees);
	}
	else if (side==l) {
		shoulder=&l_shoulder;
		s2e=&l_s2e;
		e2w=&l_e2w;
		// elbow flexion:
		flexion		= f.GetBAPAngle("l_elbow.rotateX",degrees);
		twist		= f.GetBAPAngle("l_elbow.rotateY",degrees);
		// Euler angles for shoulder:
		sEuler[0]	= f.GetBAPAngle("l_shoulder.rotateX",degrees);
		sEuler[1]	= f.GetBAPAngle("l_shoulder.rotateY",degrees);
		sEuler[2]	= f.GetBAPAngle("l_shoulder.rotateZ",degrees);

	}
	mat4 Rsx=rotation3D(vec3(1.,0.,0.),sEuler[0]);
	mat4 Rsy=rotation3D(vec3(0.,1.,0.),sEuler[1]);
	mat4 Rsz=rotation3D(vec3(0.,0.,1.),sEuler[2]);
	mat4 Rex=rotation3D(vec3(1.,0.,0.),flexion);
	mat4 Rey=rotation3D(vec3(0.,1.,0.),twist);
	mat4 Trs=translation3D(*shoulder);
	mat4 Tse=translation3D(*s2e);
	mat4 Tew=translation3D(*e2w);
	
	//wrist position w.r.t. Humanoid Root frame
	vec4 wPos = Trs*Rsz*Rsy*Rsx*Tse*Rey*Rex*Tew*vec4(0.0,0.0,0.0,1.0);
	
	//elbow position w.r.t. Humanoid Root oriented frame at shoulder
	vec4 eWRTs = Rsz*Rsy*Rsx*Tse*vec4(0.0,0.0,0.0,1.0);//elbow w.r.t. shoulder

	//wrist position w.r.t. Humanoid Root-oriented frame at shoulder
	vec4 wWRTs = Rsz*Rsy*Rsx*Tse*Rey*Rex*Tew*vec4(0.0,0.0,0.0,1.0);//wrist w.r.t.shoulder
	
	*swivel=ComputeSwivelAngle(vec3(eWRTs),vec3(wWRTs));
	
	//debug output:
	//printf("FK_pos: %2.2f %2.2f %2.2f; swivel: %2.2f\nFK_inp: sx %2.2f, sy %2.2f, sz %2.2f, ex %2.2f, ey %2.2f\n"
	//	,wPos[0],wPos[1],wPos[2],RTOD(*swivel),sEuler[0],sEuler[1],sEuler[2],flexion,twist);
	
	return alg3::vec3(wPos);
	
}

// taken from e4dKinematics: Bjoern Hartmann/Liwei Zhao
// computed in maya/greta system : y-up
float GestureKinematics::ComputeSwivelAngle(alg3::vec3 &elbow,
											alg3::vec3 &hand)
{
    double beta;
    vec3 v_n, v_e, v_w, v_o, v_oe;	// vectors
    double a, b, c;	// length of edges of the triangle 
                        // formed by shoulder, wrist, and elbow
    double cos_alpha;
    vec3 axis = vec3(0.0, -1.0, 0.0);	// pointing downward
	vec3 a_perp;
	a_perp = vec3(1.0,0.0,0.0);
	
    // compute the swivel angle for each frame
    
    v_n = hand;		// vector from shoulder to wrist
    a = v_n.length();
    v_e = elbow;		// vector from shoulder to elbow
    b = v_e.length();
    v_w = v_n - v_e;	// vector from elbow to wrist
    c = v_w.length();
    cos_alpha = (a*a + b*b - c*c)/(2.0*a*b);  // cos(alpha)
    v_n.normalize();
    v_o = (b * cos_alpha) * v_n;	// from shoulder to center of swivel elipsis
    v_oe = v_e - v_o;

    //safety: arm extended all the way
	if (v_oe.length()<0.00001) return 0.0;
    
	/////////////////////////////////////////////////
	// method1: using dot product and perdendicular dot product		
	
	double cos_swivel=(axis * v_oe)/v_oe.length();
	double sin_swivel=(a_perp * v_oe)/v_oe.length();
	//beta=float(atan2(sin_swivel,cos_swivel));
	beta=ComputeAngle(cos_swivel,sin_swivel);
	return float(beta);
	
	//////////////////////////////////////////////////

	/////////////////////////////////////////////////
	// method2: using dot and cross product
	/*
	double cos_swivel=(axis * v_oe)/v_oe.length();
	double sin_swivel=(axis ^ v_oe).length()/v_oe.length();
	beta=float(atan2(sin_swivel,cos_swivel));
	//beta=float(ComputeAngle(cos_swivel,sin_swivel)); 
	return beta;
	*/
	///////////////////////////////////////////////////

	
	
}


///////////////////////////////////////////////////////////////////////////////
// Function:	computeAngle
// Purpose:		compute an angle from its cosin and sin value
// Arguments:	c and s are the cosin and sin value of the angle, respectively.
// taken from emote wrapper, but written by myself (bjoern)
///////////////////////////////////////////////////////////////////////////////
double GestureKinematics::ComputeAngle(double c, double s)
{
	double angle;
	
	if (c >= 0.0f && s >= 0.0f) 
		angle = (double) (asin(s));
	else if (c >= 0.0f && s <= 0.0f)
		angle = (double) (asin(s));
	else if (c <= 0.0f && s >= 0.0f)
		angle = (double) (acos(c));
	else
		angle = (double) (2*M_PI - acos(c));

	return angle;
}


void GestureKinematics::SetSwivel(float value, enum SideType s)
{
	if(s==r)
		r_swivel=value;
	else if(s==l)
		l_swivel=value;
	
}

double GestureKinematics::FindDistance(BAPFrame &fFrom, BAPFrame &fTo, SideType side)
{
	float dummySwivel; //not used
	
	if(side==both_sides || side==assym)
		side=r;

	vec3 posFrom= FkSolve(side,fFrom,&dummySwivel);
	vec3 posTo  = FkSolve(side,fTo, &dummySwivel);
	return (posTo-posFrom).length();
}
