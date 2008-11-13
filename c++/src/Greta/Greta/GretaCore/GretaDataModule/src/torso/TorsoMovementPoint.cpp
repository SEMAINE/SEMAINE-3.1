//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// TorsoMovementPoint.cpp: implementation of the  TorsoMovementPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "BAPconverter.h"
#include ".\torsomovementpoint.h"
#include "IniManager.h"
 

extern IniManager inimanager;

/**@#-*/
using namespace TorsoSpace;
using namespace std;
/**@#+*/

extern BAPConverter TorsoGlobalBAPconverter;

TorsoMovementPoint::TorsoMovementPoint(void)
{
	baps=new BAPFrame(&TorsoGlobalBAPconverter);

	compensateshoulders=false;

	compensatehead=false;

	this->Hposition=h_center;
	this->Sposition=s_center;
	this->Vposition=v_center;

	this->SpineRotation=r_center;

	this->spatial_fixed=false;

}

TorsoMovementPoint::TorsoMovementPoint(const TorsoMovementPoint &rhs)
{
	this->Hposition=rhs.Hposition;
	this->Sposition=rhs.Sposition;
	this->Vposition=rhs.Vposition;
	this->SpineRotation=rhs.SpineRotation;

	this->compensateshoulders=rhs.compensateshoulders;
	this->compensatehead=rhs.compensatehead;
	this->spatial_fixed=rhs.spatial_fixed;

	this->time=rhs.time;


	BAPFrame *bf=new BAPFrame(&TorsoGlobalBAPconverter);

	bf->CopyAngles(rhs.baps);

	this->baps=bf;
}
	

TorsoMovementPoint::~TorsoMovementPoint(void)
{
	//if(baps!=0)
	//	delete baps;
	//baps=0;
}

TorsoHorizontalRotationType TorsoMovementPoint::GetHpositionId(std::string name)
{
	if(name=="right")
		return h_right;
	if(name=="center")
		return h_center;
	if(name=="left")
		return h_left;
	return h_center;
}

TorsoVerticalRotationType TorsoMovementPoint::GetVpositionId(std::string name)
{
	if(name=="backward")
		return v_backward;
	if(name=="center")
		return v_center;
	if(name=="forward")
		return v_forward;
	return v_center;
}

TorsoSagittalRotationType TorsoMovementPoint::GetSpositionId(std::string name)
{
	if(name=="right")
		return s_left;
	if(name=="center")
		return s_center;
	if(name=="left")
		return s_left;
	return s_center;
}

TorsoSpineRotationType TorsoMovementPoint::GetSpineRotationId(std::string name)
{
	if(name=="right")
		return r_right;
	if(name=="center")
		return r_center;
	if(name=="left")
		return r_left;
	return r_center;
}

int TorsoMovementPoint::LoadPose(int t,float scaling)
{
	char filename[100];
	sprintf(filename,"./torso/poses/%d%d%d_%d.txt",this->Hposition,this->Vposition,this->Sposition,t);
	
	std::string filename_new = inimanager.Program_Path+filename;
	
	//printf(filename_new.c_str());
	return this->baps->ReadPose(filename_new.c_str(),true,false,scaling);
}

int TorsoMovementPoint::LoadPose(int t)
{
	return LoadPose(t,1);
}

int TorsoMovementPoint::LoadPose(std::string filename,float scaling)
{
	filename=inimanager.Program_Path+"./torso/poses/"+filename;
	return this->baps->ReadPose(filename,true,false,scaling);
}

int TorsoMovementPoint::LoadPose(std::string filename)
{
	return LoadPose(filename,1);
}
