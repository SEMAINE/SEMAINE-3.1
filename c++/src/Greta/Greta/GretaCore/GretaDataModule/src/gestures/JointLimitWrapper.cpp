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

#include "jointlimitwrapper.h"
#include "Wm4Matrix3.h"
//#include <cassert>
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
extern FILE* data_log;

JointLimitWrapper::JointLimitWrapper(void)
{
	rWristLimits=new Jtl_LimitType;
	lWristLimits=new Jtl_LimitType;
	rShoulderLimits=new Jtl_LimitType;
	lShoulderLimits=new Jtl_LimitType;

	InitRWrist();
	InitLWrist();
	InitRShoulder();
	InitLShoulder();

	
}

void JointLimitWrapper::InitLWrist() {
	//left wrist - we know these are correct from kopp
	//need to be adjusted for right wrist below (how?)
	float visP[3]={0.f,0.f,1.f};
	float p1[3]={0.f,1.f,0.3f};
	float p2[3]={-0.3f,0.f,1.f};
	float p3[3]={0.f,-1.f,0.1f};
	float p4[3]={1.f,0.f,1.f};
	float zLim[2]={-80.f,90.f};
	jtl_init(lWristLimits);
	jtl_setVisiblePoint(lWristLimits, visP);
	jtl_addPoint_withTwist(lWristLimits, p1, zLim);
	jtl_addPoint_withTwist(lWristLimits, p2, zLim);
	jtl_addPoint_withTwist(lWristLimits, p3, zLim);
	jtl_addPoint_withTwist(lWristLimits, p4, zLim);
	lWristLimits->wks.vpt_zlim[0]=-80.f;
	lWristLimits->wks.vpt_zlim[1]=90.f;
	jtl_setAllowableFlag(lWristLimits, JTL_WKS);
	jtl_setAllowableFlag(lWristLimits, JTL_WKSZ);
	//printf("jtl_validate(lWristLimits)=%d\n",jtl_validate(lWristLimits));
}

void JointLimitWrapper::InitRWrist() {
	//ypos-> -ypos ??
	//zrot-> -zrot
	float visP[3]={0.f,0.f,1.f};
	float p3[3]={0.f,-1.f,0.3f};
	float p2[3]={-0.3f,0.f,1.f};
	float p1[3]={0.f,1.f,0.1f};
	float p4[3]={1.f,0.f,1.f};
	float zLim[2]={-90.f,80.f};
	jtl_init(rWristLimits);
	jtl_setVisiblePoint(rWristLimits, visP);
	jtl_addPoint_withTwist(rWristLimits, p1, zLim);
	jtl_addPoint_withTwist(rWristLimits, p2, zLim);
	jtl_addPoint_withTwist(rWristLimits, p3, zLim);
	jtl_addPoint_withTwist(rWristLimits, p4, zLim);
	rWristLimits->wks.vpt_zlim[0]=-90.f;
	rWristLimits->wks.vpt_zlim[1]=80.f;
	jtl_setAllowableFlag(rWristLimits, JTL_WKS);
	jtl_setAllowableFlag(rWristLimits, JTL_WKSZ);
	//printf("jtl_validate(rWristLimits)=%d\n",jtl_validate(rWristLimits));
}

void JointLimitWrapper::InitLShoulder() 
{
	//left shoulder - we know these are correct from kopp
	//need to be adjustedfor right shoulder below (how?)
	float visP[3]={0.f, -1.f, 0.f};
	
	float p1[3]=  {0.f, 0.1f, 1.f};
	float z1[2]={-40.f, 107.f};
	
	float p2[3]=  {-1.f, -0.6f, 0.f};
	float z2[2]={-91.f, 54.f};

	float p3[3]={0.f, 0.2f, -1.f};
	float z3[2]={-45.f, 95.f};

	float p4[3]={1.f,0.6f,0.f};
	float z4[2]={-40.f, 105.f};

	
	jtl_init(lShoulderLimits);
	jtl_setVisiblePoint(lShoulderLimits, visP);
	jtl_addPoint_withTwist(lShoulderLimits, p1, z1);
	jtl_addPoint_withTwist(lShoulderLimits, p2, z2);
	jtl_addPoint_withTwist(lShoulderLimits, p3, z3);
	jtl_addPoint_withTwist(lShoulderLimits, p4, z4);
	lShoulderLimits->wks.vpt_zlim[0]=-85.f;
	lShoulderLimits->wks.vpt_zlim[1]=95.f;
	jtl_setAllowableFlag(lShoulderLimits, JTL_WKS);
	jtl_setAllowableFlag(lShoulderLimits, JTL_WKSZ);
	//printf("jtl_validate(lShoulderLimits)=%d\n",jtl_validate(lShoulderLimits));

}
void JointLimitWrapper::InitRShoulder() 
{

	float visP[3]={0.f, 1.f, 0.f};
	
	float p3[3]=  {0.f, -0.1f, 1.f};
	float z3[2]={-107.f, 40.f};
	
	float p2[3]=  {-1.f, 0.6f, 0.f};
	float z2[2]={-54.f, 91.f};

	float p1[3]={0.f, -0.2f, -1.f};
	float z1[2]={-95.f, 45.f};

	float p4[3]={1.f,-0.6f,0.f};
	float z4[2]={-105.f, 40.f};

	
	jtl_init(rShoulderLimits);
	jtl_setVisiblePoint(rShoulderLimits, visP);
	jtl_addPoint_withTwist(rShoulderLimits, p1, z1);
	jtl_addPoint_withTwist(rShoulderLimits, p2, z2);
	jtl_addPoint_withTwist(rShoulderLimits, p3, z3);
	jtl_addPoint_withTwist(rShoulderLimits, p4, z4);
	rShoulderLimits->wks.vpt_zlim[0]=-95.f;
	rShoulderLimits->wks.vpt_zlim[1]=85.f;
	jtl_setAllowableFlag(rShoulderLimits, JTL_WKS);
	jtl_setAllowableFlag(rShoulderLimits, JTL_WKSZ);
	//printf("jtl_validate(rShoulderLimits)=%d\n",jtl_validate(rShoulderLimits));
}

//in/output in RADIANS!
int JointLimitWrapper::CheckRWristXYZ(float &x, float &y, float &z) {
	return CheckLimits(x,y,z,rWristLimits);
}
//in/output in RADIANS!
int JointLimitWrapper::CheckLWristXYZ(float &x, float &y, float &z)
{
	return CheckLimits(x,y,z,lWristLimits);
}

int JointLimitWrapper::CheckLShoulderXYZ(float &x, float &y, float &z)
{
	return CheckLimits(x,y,z,lShoulderLimits);
}
int JointLimitWrapper::CheckRShoulderXYZ(float &x, float &y, float &z)
{
	return CheckLimits(x,y,z,rShoulderLimits);
}

int JointLimitWrapper::CheckLimits(float &x, float &y, float &z, Jtl_LimitPtr pLimits)
{
	
	Jtl_Allowable status;
	Matrix3f mInRot,mOutRot;
	float x_jtl,y_jtl,z_jtl,x_jtl_alt,y_jtl_alt,z_jtl_alt;

	//calculate greta->jointlimit rotation conversion
	mInRot.FromEulerAnglesXZY(z,(-1.f)*(y),(-1.f)*(x));
	//calculate two possible solutions depending on resolution of asin(sy)
	mInRot.ToEulerAnglesXYZ(x_jtl,y_jtl,z_jtl);
	ToEulerAnglesXYZ2(mInRot,x_jtl_alt,y_jtl_alt,z_jtl_alt);
		
	// HEURISTIC:
	// take solution that minimizes overall magnitude of angles
	if( (fabs(x_jtl)+fabs(y_jtl)+fabs(z_jtl)) >
		(fabs(x_jtl_alt)+fabs(y_jtl_alt)+fabs(z_jtl_alt))) {
		x_jtl=x_jtl_alt;
		y_jtl=y_jtl_alt;
		z_jtl=z_jtl_alt;
	}

	float rotation[3]={0.f}; //not sure what it should be; used if !intersection
	float newRot[3]={RTOD(x_jtl),RTOD(y_jtl),RTOD(z_jtl)}; //desired rotatio - input; 
	float okRot[3]={0.f}; //rotation respecting limits - output
	int intersection=1;//instersect flag=true;
	float matrix[4][4]={0.f};//WHAT TO SET HERE?
	matrix[0][0]=1.f;
	matrix[1][1]=1.f;
	matrix[2][2]=1.f;
	matrix[3][3]=1.f;
	matrix[0][3]=1.f;
	matrix[1][3]=3.f;
	matrix[2][3]=10.f;
	
	//fprintf(data_log,"Greta In  in degrees: x:%2.2f, y:%2.2f, z:%2.2f\n",RTOD(x),RTOD(y),RTOD(z));
	//fprintf(data_log,"Jtl   In  in degrees: x:%2.2f, y:%2.2f, z:%2.2f\n",newRot[0],newRot[1],newRot[2]);

	////////////////////// SOLVE ///////////////////////
	status= jtl_allowableRotation(pLimits, matrix, rotation,newRot,okRot,intersection);
	////////////////////////////////////////////////////
	/*
	switch(status) {
	case  JTL_FAIL_WITH_INTERSECT: 
		fprintf(data_log,"JTL_FAIL_WITH_INTERSECT\n");
		break;
	case JTL_FAIL_NO_INTERSECT: 
		
		printf(data_log,"JTL_FAIL_NO_INTERSECT\n");
		break;
	case JTL_OK: printf(data_log,"JTL_OK\n");
		break;
	}
	*/
	// convert back to greta rotation system	
	mOutRot.FromEulerAnglesXYZ(DTOR(okRot[0]),DTOR(okRot[1]),DTOR(okRot[2]));
	mOutRot.ToEulerAnglesXZY(z,y,x);
	y=-y;
	x=-x;

	//fprintf(data_log,"Jtl   Out in degrees: x:%2.2f, y:%2.2f, z:%2.2f\n",okRot[0],okRot[1],okRot[2]);
	//fprintf(data_log,"Greta Out in degrees: x:%2.2f, y:%2.2f, z:%2.2f\n",RTOD(x),RTOD(y),RTOD(z));

	return status;
	
}
JointLimitWrapper::~JointLimitWrapper(void)
{
	delete rWristLimits;
	delete lWristLimits;
	delete rShoulderLimits;
	delete lShoulderLimits;
}

//----------------------------------------------------------------------------
bool JointLimitWrapper::ToEulerAnglesXYZ2 (Matrix3f &m, float& rfXAngle, float& rfYAngle,
    float& rfZAngle) 
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( m[0][2] < 1.0f )
    {
        if ( m[0][2] > -1.0f )
        {
			rfXAngle = atan2f(m[1][2],-m[2][2]);
            
			rfYAngle = asinf(m[0][2]);
			if((rfYAngle >= -M_PI) && (rfYAngle <= 0.f)) {
				rfYAngle=-M_PI-rfYAngle;
			} else if ((rfYAngle>0.f) && (rfYAngle<=M_PI)){
				rfYAngle=M_PI-rfYAngle;
			}
            
			rfZAngle = atan2f(m[0][1],-m[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -atan2f(m[1][0],m[1][1]);
            rfYAngle = -M_PI/2.f;
            rfZAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = atan2f(m[1][0],m[1][1]);
        rfYAngle = M_PI/2.f;
        rfZAngle = 0.0f;
        return false;
    }
}
