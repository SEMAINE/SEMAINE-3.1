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

/// @file ReachSpace.cpp
/// implementation of the ReachSpace class.
///
//////////////////////////////////////////////////////////////////////


//#include <stdio.h>
#include "ReachSpace.h"


using namespace GestureSpace;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ReachSpace::ReachSpace()
{
	InitializePoints();
}

ReachSpace::~ReachSpace()
{

}

void ReachSpace::ScaleSpace(float horiz, float vert, float front)
{
	//CAREFUL: right now, scaling is w.r.t humanoid root!
	//should be closer to chest
	InitializePoints();
	xform = alg3::scaling3D(alg3::vec3(horiz,vert,front));
	for(int x=0;x<GS_NUMPOINTS_X;x++) {
		for(int y=0;y<GS_NUMPOINTS_Y;y++) {
			for(int z=0;z<GS_NUMPOINTS_Z;z++) {
				//printf("h: %2.2f, v:%2.2f, f=%2.2f\n",horiz,vert,front);
				//printf("points[%d][%d][%d]=(%2.2f, %2.2f, %2.2f)\n",x,y,z,points[x][y][z][0],points[x][y][z][1],points[x][y][z][2]);
				
				points[x][y][z]=(xform*points[x][y][z]);
				
				//printf("points[%d][%d][%d]=(%2.2f, %2.2f, %2.2f)\n\n",x,y,z,points[x][y][z][0],points[x][y][z][1],points[x][y][z][2]);

			}
		}
	}
}


alg3::vec3 ReachSpace::GetCoordinates(int sectorX, int sectorY, int sectorZ, SideType side)
{
	if (side==r)
		return alg3::vec3(origin+points[sectorX][sectorY][sectorZ]);
	else {
		alg3::vec3 result=(origin+points[sectorX][sectorY][sectorZ]);
		result[0]=-result[0];

		return result;
	}

}

void ReachSpace::InitializePoints()
{
	
	//model-dependent initialization of reference points
	
	//set origin relative to humanoid root joint
	origin[0]=0.0f;
	origin[1]=20.0f;
	origin[2]=0.0f;
	origin[3]=0.0f;

	//relative to origin point:
	float valx,valy,valz;
	for(int x=0;x<GS_NUMPOINTS_X;x++) {
		for(int y=0;y<GS_NUMPOINTS_Y;y++) {
			for(int z=0;z<GS_NUMPOINTS_Z;z++) {
				switch(x) {
				//for right arm; for left arm, reverse sign 
				case XEP: valx=-50.0f; break;
				case XP: valx=-30.0f; break;
				case XC: valx=-15.0f; break;
				case XCC: valx=-5.0f; break;
				case XOppC: valx=20.0f; break;
				}
				switch(y) {
				case YUpperEP: valy=60.0f; break;
				case YUpperP: valy=40.0f; break;
				case YUpperC: valy=20.0f; break;
				case YCC: valy=10.0f;break;
				case YLowerC: valy=00.0f;break;
				case YLowerP: valy=-10.0f;break;
				case YLowerEP: valy=-20.0f;break;
				}
				switch(z) {
				case ZNear: valz=15.0f;break;
				case ZMiddle: valz=25.0f;break;
				case ZFar: valz=35.0f;break;
				}
				// at low heights, arm is extended earlier, so move distances closer to body
				// (a quick hack; elliptival model would be better)
				if(valy<0.0) valz=float(0.5*valz);
				points[x][y][z]=alg3::vec4(valx,valy,valz,1.0);
			}
		}
	}
}
