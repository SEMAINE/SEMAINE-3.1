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

// EulerInterpolator.cpp: implementation of the EulerInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#include "EulerInterpolator.h"
using namespace GestureSpace;
using namespace Wm4;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EulerInterpolator::EulerInterpolator(enum SideType s)
{
	side=s;
}

EulerInterpolator::~EulerInterpolator()
{

}

/** Given a vector of pointers to BAPFrames, creates TCB splines */ 
void EulerInterpolator::Init(BAPFrameVector v, enum DOFGroup t) 
{
	
	int size = v.size();
	int i;

	//if(size<4)
		//printf("GestureEngine: warning, interpolation failed (less than 4 points)\n");

	///////////////////// create spline for arm //////////////////////////////
	if (t==ARM_DOFs) {
		
		//////////////// create spline for shoulder //////////////////////////	
		Vector3f* sPts = new Vector3f[size]; 
			
		float* sT = new float[size];
		float* sC = new float[size];
		float* sB = new float[size];
		float* sTime = new float[size];
		for (i=0; i<size; i++) {
			
			sPts[i]  = Vector3f( float(v[i]->GetBAP(shoulder_flexion, side)),
								float(v[i]->GetBAP(shoulder_abduct, side)),
								float(v[i]->GetBAP(shoulder_twisting, side)) );
			
			// set tension, continuity, bias, time:
			sT[i]=v[i]->TCBParam[Tension];
			sC[i]=v[i]->TCBParam[Continuity];
			sB[i]=v[i]->TCBParam[Bias];
			sTime[i] = (float(v[i]->GetFrameNumber()));
			//printf("kf t=%.2f,x=%.2f,y=%.2f,z=%.2f\n",
			//	sTime[i],sPts[i][0],sPts[i][1],sPts[i][2]);
		}
		mpTCB3_Shoulder= new TCBSpline3f( (size-1),sTime,sPts,sT,sC,sB );
		

		//////////////// create spline for elbow //////////////////////////
		Vector2f* ePts= new Vector2f[size];
			
		float* eT = new float[size];
		float* eC = new float[size];
		float* eB = new float[size];
		float* eTime = new float[size];
		for (i=0; i<size; i++) {
			ePts[i]  = Vector2f(float(v[i]->GetBAP(elbow_flexion,side)),
				float(v[i]->GetBAP(elbow_twisting,side)));
			// set tension, continuity, bias, time:
			eT[i]=v[i]->TCBParam[Tension];
			eC[i]=v[i]->TCBParam[Continuity];
			eB[i]=v[i]->TCBParam[Bias];
			eTime[i] = (float(v[i]->GetFrameNumber()));
		}
		mpTCB2_ElbowFlex = new TCBSpline2f((size-1),eTime,ePts,eT,eC,eB );
	}

	///////////////////// create spline for wristOrient //////////////////////////////
	if (t==WRIST_DOFs) {
		Vector3f* wPts= new Vector3f[size];
			

		float* wT = new float[size];
		float* wC = new float[size];
		float* wB = new float[size];
		float* wTime = new float[size];
		for (i=0; i<size; i++) {
			wPts[i]  = Vector3f( float(v[i]->GetBAP(wrist_twisting,side)),
								float(v[i]->GetBAP(wrist_flexion,side)),
								float(v[i]->GetBAP(wrist_pivot,side)) );
			
			// set tension, continuity, bias, time:
			wT[i]=v[i]->TCBParam[Tension];
			wC[i]=v[i]->TCBParam[Continuity];
			wB[i]=v[i]->TCBParam[Bias];
			wTime[i] = (float(v[i]->GetFrameNumber()));
		}
		mpTCB3_WristOrient = new TCBSpline3f( (size-1),wTime,wPts,wT,wC,wB );
	}		
	
	
	///////////////////// create splines for finger1 //////////////////////////////
	if (t==FINGER_DOFs) {
		Vector3f* f1Pts[5];
		float* f1T[5]; float* f1C[5]; float* f1B[5];
		float* f1Time[5]; int j;
		
		for (j=0; j<5; j++) {

			f1Pts[j] = new Vector3f[size];
		
			f1T[j]= new float[size];
			f1C[j]= new float[size];
			f1B[j]= new float[size];
			f1Time[j] = new float[size];
			
			for (i=0; i<size; i++) {
				if (j==0) {
					f1Pts[j][i]  = Vector3f( float(v[i]->GetBAP(thumb_flexion1,side)),
									float(v[i]->GetBAP(thumb_abduct,side)),
									float(v[i]->GetBAP(thumb_twisting,side)) );
				}
				else if (j==1) {
					f1Pts[j][i]  = Vector3f( float(v[i]->GetBAP(index_flexion1,side)),
									float(v[i]->GetBAP(index_abduct,side)),
									float(v[i]->GetBAP(index_twisting,side)) );
				}
				else if (j==2) {
					f1Pts[j][i]  = Vector3f( float(v[i]->GetBAP(middle_flexion1,side)),
									float(v[i]->GetBAP(middle_abduct,side)),
									float(v[i]->GetBAP(middle_twisting,side)) );
				}
				else if (j==3) {
					f1Pts[j][i]  = Vector3f( float(v[i]->GetBAP(ring_flexion1,side)),
									float(v[i]->GetBAP(ring_abduct,side)),
									float(v[i]->GetBAP(ring_twisting,side)) );
				}
				else if (j==4) {
					f1Pts[j][i]  = Vector3f( float(v[i]->GetBAP(pinky_flexion1,side)),
									float(v[i]->GetBAP(pinky_abduct,side)),
									float(v[i]->GetBAP(pinky_twisting,side)) );
				}
				f1T[j][i]=v[i]->TCBParam[Tension];
				f1C[j][i]=v[i]->TCBParam[Continuity];
				f1B[j][i]=v[i]->TCBParam[Bias];
				
				//kLUDGE:	
				f1T[j][i]=0.75f;
				f1C[j][i]=0.f;
				f1B[j][i]=0.f;
			
				f1Time[j][i] = (float(v[i]->GetFrameNumber()));
			}
		}
		mpTCB3_Finger1[0]= new TCBSpline3f( (size-1),f1Time[0],f1Pts[0],f1T[0],f1C[0],f1B[0] );
		mpTCB3_Finger1[1]= new TCBSpline3f( (size-1),f1Time[1],f1Pts[1],f1T[1],f1C[1],f1B[1] );
		mpTCB3_Finger1[2]= new TCBSpline3f( (size-1),f1Time[2],f1Pts[2],f1T[2],f1C[2],f1B[2] );
		mpTCB3_Finger1[3]= new TCBSpline3f( (size-1),f1Time[3],f1Pts[3],f1T[3],f1C[3],f1B[3] );
		mpTCB3_Finger1[4]= new TCBSpline3f( (size-1),f1Time[4],f1Pts[4],f1T[4],f1C[4],f1B[4] );
		
		///////////////////// create splines for finger23 /////////////////////////////
		Vector2f* f23Pts[5];
		float* f23T[5]; float* f23C[5]; float* f23B[5];
		float* f23Time[5];
		
		for (j=0; j<5; j++) {

			f23Pts[j] = new Vector2f[size];
			f23T[j]= new float[size];
			f23C[j]= new float[size];
			f23B[j]= new float[size];
			f23Time[j] = new float[size];
			
			for (i=0; i<size; i++) {
				if (j==0) {
					f23Pts[j][i]  = Vector2f( float(v[i]->GetBAP(thumb_flexion2,side)),
											 float(v[i]->GetBAP(thumb_flexion3,side)) );
				}
				else if (j==1) {
					f23Pts[j][i]  = Vector2f( float(v[i]->GetBAP(index_flexion2,side)),
											 float(v[i]->GetBAP(index_flexion3,side)) );
				}
				else if (j==2) {
					f23Pts[j][i]  = Vector2f( float(v[i]->GetBAP(middle_flexion2,side)),
											 float(v[i]->GetBAP(middle_flexion3,side)) );
				}
				else if (j==3) {
					f23Pts[j][i]  = Vector2f( float(v[i]->GetBAP(ring_flexion2,side)),
											 float(v[i]->GetBAP(ring_flexion3,side)) );
				}
				else if (j==4) {
					f23Pts[j][i]  = Vector2f( float(v[i]->GetBAP(pinky_flexion2,side)),
											 float(v[i]->GetBAP(pinky_flexion3,side)) );
				}

			
				f23T[j][i]=v[i]->TCBParam[Tension];
				f23C[j][i]=v[i]->TCBParam[Continuity];
				f23B[j][i]=v[i]->TCBParam[Bias];

								//kLUDGE:	
				f23T[j][i]=0.75f;
				f23C[j][i]=0.f;
				f23B[j][i]=0.f;

				f23Time[j][i] = (float(v[i]->GetFrameNumber()));
			}
		}
		
		mpTCB2_Finger23[0]= new TCBSpline2f ( (size-1),f23Time[0],f23Pts[0],f23T[0],f23C[0],f23B[0] );
		mpTCB2_Finger23[1]= new TCBSpline2f ( (size-1),f23Time[1],f23Pts[1],f23T[1],f23C[1],f23B[1] );
		mpTCB2_Finger23[2]= new TCBSpline2f ( (size-1),f23Time[2],f23Pts[2],f23T[2],f23C[2],f23B[2] );
		mpTCB2_Finger23[3]= new TCBSpline2f ( (size-1),f23Time[3],f23Pts[3],f23T[3],f23C[3],f23B[3] );
		mpTCB2_Finger23[4]= new TCBSpline2f ( (size-1),f23Time[4],f23Pts[4],f23T[4],f23C[4],f23B[4] );
		
	}
}



void EulerInterpolator::GetFrame(BAPFrame &f, int n, enum DOFGroup t)
{
	f.SetFrameNumber(n);
	
	///////////////////// get arm DOF values /////////////////////////////
	if (t==ARM_DOFs) {
		
		///////////////// get shoulder values ////////////////////////////
		Vector3f	rShoulder   = mpTCB3_Shoulder->GetPosition(float(n));
		f.SetBAP(shoulder_flexion, side, int(rShoulder.X()));
		f.SetBAP(shoulder_abduct, side, int(rShoulder.Y()));
		f.SetBAP(shoulder_twisting, side, int(rShoulder.Z()));				
		
		///////////////////// get elbow values ///////////////////////////
		Vector2f rElbowFlex  = mpTCB2_ElbowFlex->GetPosition(float(n));
		f.SetBAP(elbow_flexion, side, int(rElbowFlex.X()));
		f.SetBAP(elbow_twisting,side, int(rElbowFlex.Y()));
	}
	
	///////////////////// get wrist DOF values ///////////////////////////
	if (t==WRIST_DOFs) {
		Vector3f rWristOrient= mpTCB3_WristOrient->GetPosition(float(n));
		f.SetBAP(wrist_twisting, side, int(rWristOrient.X()));
		f.SetBAP(wrist_flexion, side, int(rWristOrient.Y()));
		f.SetBAP(wrist_pivot, side, int(rWristOrient.Z()));
	}

	///////////////////// get finger DOF values ///////////////////////////
	if (t==FINGER_DOFs) {
		Vector3f rFinger1[5];
		Vector2f rFinger23[5];
		int i;
		for (i=0; i<5; i++) {
			rFinger1[i]  = mpTCB3_Finger1[i]->GetPosition(float(n));
			rFinger23[i] = mpTCB2_Finger23[i]->GetPosition(float(n));
		}
		//thumb
		f.SetBAP(thumb_flexion1, side, int(rFinger1[0].X()));
		f.SetBAP(thumb_abduct, side,   int(rFinger1[0].Y()));
		f.SetBAP(thumb_twisting, side, int(rFinger1[0].Z()));
		f.SetBAP(thumb_flexion2, side, int(rFinger23[0].X()));
		f.SetBAP(thumb_flexion3, side, int(rFinger23[0].Y()));

		//index
		f.SetBAP(index_flexion1, side, int(rFinger1[1].X()));
		f.SetBAP(index_abduct, side,   int(rFinger1[1].Y()));
		f.SetBAP(index_twisting, side, int(rFinger1[1].Z()));
		f.SetBAP(index_flexion2, side, int(rFinger23[1].X()));
		f.SetBAP(index_flexion3, side, int(rFinger23[1].Y()));

		//middle
		f.SetBAP(middle_flexion1, side, int(rFinger1[2].X()));
		f.SetBAP(middle_abduct, side,   int(rFinger1[2].Y()));
		f.SetBAP(middle_twisting, side, int(rFinger1[2].Z()));
		f.SetBAP(middle_flexion2, side, int(rFinger23[2].X()));
		f.SetBAP(middle_flexion3, side, int(rFinger23[2].Y()));

		//ring
		f.SetBAP(ring_flexion1, side, int(rFinger1[3].X()));
		f.SetBAP(ring_abduct, side,   int(rFinger1[3].Y()));
		f.SetBAP(ring_twisting, side, int(rFinger1[3].Z()));
		f.SetBAP(ring_flexion2, side, int(rFinger23[3].X()));
		f.SetBAP(ring_flexion3, side, int(rFinger23[3].Y()));

		//pinky
		f.SetBAP(pinky_flexion1, side, int(rFinger1[4].X()));
		f.SetBAP(pinky_abduct, side,   int(rFinger1[4].Y()));
		f.SetBAP(pinky_twisting, side, int(rFinger1[4].Z()));
		f.SetBAP(pinky_flexion2, side, int(rFinger23[4].X()));
		f.SetBAP(pinky_flexion3, side, int(rFinger23[4].Y()));
	
	}
		
}
