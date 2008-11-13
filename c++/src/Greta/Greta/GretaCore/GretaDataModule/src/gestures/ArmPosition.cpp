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

// ArmPosition.cpp: implementation of the ArmPosition class.
//
//////////////////////////////////////////////////////////////////////

#include "ArmPosition.h"
#include "IniManager.h"

extern IniManager inimanager;

/**@#-*/
using namespace std ;
using namespace GestureSpace;
/**@#+*/

extern FILE* data_log;
#define OUTPUT

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ArmPosition::ArmPosition()
{
	
	fileFolder=fileFolder+"armpositions\\";
	status=abstract;
	
	AbstractX=XDefault;
	AbstractY=YDefault;
	AbstractZ=ZDefault;
	output=true;
	useIK=true;//switch between forward and inverse kinematics
	scaleX=scaleY=scaleZ=1.0;
	fixedX=fixedY=fixedZ=false;

	expressivitySPC=0;
}

////////////////////////////////////////////////////////////////////
// ArmPosition copy constructor
// called when a gesture is copied from its generic version
// to be instantiated in a particular context. see:
// GesturePlanner::Schedule()
//
ArmPosition::ArmPosition(const ArmPosition &rhs) {
	AbstractX	=rhs.AbstractX;
	AbstractY	=rhs.AbstractY;
	AbstractZ	=rhs.AbstractZ;
	certainty	=rhs.certainty;
	endTime		=rhs.endTime;
	fileFolder	=rhs.fileFolder;
	filePrefix	=rhs.filePrefix;
	IK			=rhs.IK;
	side		=rhs.side;
	startTime	=rhs.startTime;
	status		=rhs.status;

	float fx,fy,fz;
	scaleX=scaleY=scaleZ=1.0f;
	expressivitySPC=rhs.expressivitySPC;

#ifdef WITH_EXPRESSIVITY
	//assert(inimanager.initialized);
	//if (inimanager.initialized) {
		// agent's preference of using a particular dimension 
		// in spatial use; observation: x has largest range, esp. outside body.
		// negative y values quickly exceed reach of arm
		// negative z values lead to self intersection with body.
		/*
		if(inimanager.GetValueFloat("ENGINE_EXPR_SPC") >= 0) {
			fx=1.3f;
			fy=0.60f;
			fz=0.25f;
		} else {
			fx=0.7f;
			fy=0.25f;
			fz=0.25f;
		}
		if(!rhs.fixedX) scaleX=1.0f+(inimanager.GetValueFloat("ENGINE_EXPR_SPC")*fx);
		if(!rhs.fixedY) scaleY=1.0f+(inimanager.GetValueFloat("ENGINE_EXPR_SPC")*fy);
		if(!rhs.fixedZ) scaleZ=1.0f+(inimanager.GetValueFloat("ENGINE_EXPR_SPC")*fz);*/
	//}
		if(this->expressivitySPC >= 0) {
			fx=1.3f;
			fy=0.60f;
			fz=0.25f;
		} else {
			fx=0.7f;
			fy=0.25f;
			fz=0.25f;
		}
		if(!rhs.fixedX) scaleX=1.0f+(this->expressivitySPC*fx);
		if(!rhs.fixedY) scaleY=1.0f+(this->expressivitySPC*fy);
		if(!rhs.fixedZ) scaleZ=1.0f+(this->expressivitySPC*fz);

#endif
	fixedX= rhs.fixedX;
	fixedY= rhs.fixedY;
	fixedZ= rhs.fixedZ;
	useIK=rhs.useIK;
	x=rhs.x;
	y=rhs.y;
	z=rhs.z;
}

ArmPosition::~ArmPosition()
{
	
}

void ArmPosition::ComputeScale()
{
	float fx,fy,fz;
	if(this->expressivitySPC >= 0) {
		fx=1.3f;
		fy=0.60f;
		fz=0.25f;
	} else {
		fx=0.7f;
		fy=0.25f;
		fz=0.25f;
	}
	if(!fixedX) scaleX=1.0f+(this->expressivitySPC*fx);
	if(!fixedY) scaleY=1.0f+(this->expressivitySPC*fy);
	if(!fixedZ) scaleZ=1.0f+(this->expressivitySPC*fz);
}

void ArmPosition::Print()
{
#ifdef OUTPUT
	fprintf(data_log,"ArmPosition::Print()\n"); 
	fprintf(data_log,"+--- X : %d\n",x); 
	fprintf(data_log,"+--- Y : %d\n",y);
	fprintf(data_log,"+--- Z : %d\n",z);
#endif
}

ArmPosition::ArmPosition(ArmPositionX _x, ArmPositionY _y, ArmPositionZ _z)
{
	x=_x; y=_y; z=_z;
	fileFolder=fileFolder+"armpositions\\";
	useIK=false;
	//this();
}

void ArmPosition::SetXYZ(ArmPositionX _x, ArmPositionY _y, ArmPositionZ _z)
{
	x=_x; y=_y; z=_z;
}

void ArmPosition::GetAngles(BAPFrame &f)
{
#ifdef OUTPUT
		fprintf(data_log,"s ArmPosition::GetAngles(BAPFrame &f)\n");
#endif
	
	// MAKE SURE POSITION IS CONCRETE
	if (status==abstract) {
#ifdef OUTPUT
		fprintf(data_log,"! ArmPosition::GetAngles: Position still ABSTRACT. Returning.\n");
#endif
		return;
	}
	// IF DEFAULT POSITION, DO NOT LOAD ANYTHING
	if ((x==X_default) || (y==Y_default) || (z==Z_default)) {
#ifdef OUTPUT
		fprintf(data_log,". Default Arm Position. Not reading pose. Returning.\n");
#endif
		return;
	}
	// OTHERWISE: LOAD ANGLES FOR NON DEFAULT POSITION
	if (!useIK) {
		// IF USING FK: LOAD ARM ANGLES FROM FILE
		string fileName;
		ostringstream buffer;
		buffer << x << y << z;
		fileName = fileFolder + filePrefix + "arm_" + buffer.str() + ".txt";

#ifdef OUTPUT
		fprintf(data_log,"  Reading File: %s\n",fileName.c_str());
#endif
		if (side == r) 
			f.ReadPose(fileName, false, false);
		else 
			f.ReadPose(fileName, false, true);
	} else {
		// USING INVERSE KINEMATICS: CALL SOLVER IN GestureKinematics CLASS
		IK.IkSolveBySector(AbstractX,AbstractY,AbstractZ,side,f,scaleX,scaleY,scaleZ);
		// apply joint limits to f?

	}
	// MAYBE HERE: COMPUTE SCAPULA+CLAVICLE ANGLES
	// f = BAPframe that now has shoulder angles
	// compute angles and use f.SetBAP....
}
alg3::vec3 ArmPosition::GetCoordinates(){
	return IK.GetCoordinates(AbstractX,AbstractY,AbstractZ,side,scaleX,scaleY,scaleZ);
}

// concretize was used for mapping the entire set of reach sectors
// to a subset defined for forward kinematics lookup.
// it is now obsolete?
void ArmPosition::Concretize()
{
#ifdef OUTPUT
	fprintf(data_log,". ArmPosition::Concretize: AbsX=%d, AbsY=%d, AbsZ=%d\n",AbstractX,AbstractY,AbstractZ);
#endif
	// translate abstract xyz coordinates to concrete coordinates:
	switch (AbstractX) {
	case XEP:
	case XP:
		x=X_outside; break;
	case XC:
		x=X_same_shoulder; break;
	case XCC:
		x=X_spine; break;
	case XOppC:
		x=X_opposite_shoulder; break;
	default:
		x=X_default;
	}

	switch(AbstractY) {
	case YUpperEP:
		y=Y_head; break;
	case YUpperP:
		y=Y_shoulder; break;
	case YUpperC:
	case YCC:
	case YLowerC:
		y=Y_chest; break;
	case YLowerP:
		y=Y_hip; break;
	case YLowerEP:
		y=Y_leg; break;
	default:
		y=Y_default;
	}
	switch(AbstractZ) {
	case ZNear:
		z=Z_near;break;
	case ZMiddle:
		z=Z_middle; break;
	case ZFar:
		z=Z_far; break;
	default:
		z=Z_default;
		
	}
	status = concrete;
}
