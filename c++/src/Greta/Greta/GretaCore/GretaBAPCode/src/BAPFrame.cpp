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

// BAPFrame.cpp: implementation of the BAPFrame class.
//
//////////////////////////////////////////////////////////////////////
//include "stdafx.h"
#include "BAPFrame.h"
#include <cassert>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BAPFrame::BAPFrame(BAPConverter *converter)
{
	this->mConverter=converter;
	mFrameNumber=0;
	Init();
	id="";
}

BAPFrame::BAPFrame(BAPConverter *converter, int num)
{
	this->mConverter=converter;
	mFrameNumber=num;
	Init();
	id="";
}

BAPFrame::BAPFrame(BAPConverter *converter, string filename, int n)
{
	this->mConverter=converter;
	mFrameNumber=n;
	Init();
	ReadPose(filename);
	id="";
}

BAPFrame::~BAPFrame()
{

}

bool BAPFrame::operator<(BAPFrame& a)
{
	if(this->mFrameNumber<=a.mFrameNumber)
		return true;
	return false;
}

// init perfoms the mapping of baps to joint axes:
void BAPFrame::Init()
{
	output=false;
	mBAPVector.clear();
	for (int i=0; i<= NUMBAPS; i++) //pushes 170 baps!
	{
		mBAPVector.push_back(BAP(mConverter->IntToBAP(i)));
		mBAPVector[mBAPVector.size()-1].SetMask(0);
	}
	TCBParam[Tension]=0.8f; //default tension
	TCBParam[Continuity]=1.0f;
	TCBParam[Bias]=0.0f;
}	

void BAPFrame::Print()
{

	cout << "s BAPFrame::Print()\n";
	

	BAPVector::iterator theIterator;
	for (theIterator = mBAPVector.begin(); theIterator != mBAPVector.end(); theIterator++) {
		if((theIterator->GetBAPType()>=32)&&(theIterator->GetBAPType()<=41))
		if(theIterator->GetMask()) {
		  cout << theIterator->GetBAPType()<< ": [mask:"<<(theIterator->GetMask())<<"] "<< theIterator->GetBAPValue()<<" ";
		}
	
	}
	cout << "\n";
	
	
}

int BAPFrame::ReadPose(string file_name, bool skip_zeros, bool switch_sides)
{
	return ReadPose(file_name,skip_zeros,switch_sides,1);
}

int BAPFrame::ReadPose(string file_name, bool skip_zeros, bool switch_sides, float scaling)
{
	if (output)
		cout << "BAPFrame::ReadPose\n("<<file_name<<","<<skip_zeros<<","<<switch_sides<<")\n";
	ifstream infile(file_name.c_str());
	if (!infile) { 
		cout << "! BAPFrame::ReadPose : error opening file \"" << file_name << "\"\n"; 
		return 0;
	}
	else {
		const int MAXLINE = 200;
		char line[MAXLINE];
		int lines_read = 0;
		while (infile.getline(line, MAXLINE)) {
			if ((strncmp (line, "//", 2) != 0)) { //not comment line
				lines_read++;
				if (lines_read < 2) continue; // line 1 after comments is the header

				char jname[60]; 
				float x_rot,y_rot,z_rot;
				sscanf(line, "%s %g %g %g",  jname, &x_rot, &y_rot, &z_rot);
				x_rot=x_rot*scaling;
				y_rot=y_rot*scaling;
				z_rot=z_rot*scaling;

				if(switch_sides) {
					if (strncmp(jname,"l_",2)==0) jname[0]='r';
					else if (strncmp(jname,"r_",2)==0) jname[0]='l';
					y_rot=-y_rot; z_rot=-z_rot;

				}
				string mname(jname); //maya joint name

				// now update the vector:
		
				//bap_vector::iterator iter;
				if ((x_rot!=0.0) || (!skip_zeros)) {
					string mname2 = mname + (".rotateX");
					BAPType this_bap=mConverter->MJointToBAP(mname2); // O(1) lookup in conversion table
					mBAPVector[this_bap].SetAngleValue(degrees,x_rot);
					mBAPVector[this_bap].SetMask(true);
				}
				if ((y_rot!=0.0) || (!skip_zeros)) {
					string mname2 = mname + (".rotateY");
					BAPType this_bap=mConverter->MJointToBAP(mname2);
					mBAPVector[this_bap].SetAngleValue(degrees,y_rot);
					mBAPVector[this_bap].SetMask(true);
				}
				if ((z_rot!=0.0) || (!skip_zeros)) {
					string mname2 = mname + (".rotateZ");
					BAPType this_bap=mConverter->MJointToBAP(mname2);
					mBAPVector[this_bap].SetAngleValue(degrees,z_rot);
					mBAPVector[this_bap].SetMask(true);
				}
			}
		}
	}
	infile.close();
	return 1;
}

string BAPFrame::WriteBAP()
{
	// cout << "BAPFrame::WriteBAP()\n";
	ostringstream buffer,buffer2;
	buffer << mFrameNumber << " ";
	string mask("");
	string data(buffer.str()); // frame number first!
	char s[30];
	BAPVector::iterator iter;
	iter = mBAPVector.begin();
	iter++;
	for (; iter != mBAPVector.end(); iter++) {
		if(iter->GetMask()) {
			mask.append("1 ");
			_itoa(iter->GetBAPValue(),s,10);
			data.append(s);data.append(" ");
		}
		else {
			mask.append("0 ");
		}
	}
	//cout << mask <<"\n"<<data<<"\n";
	//outfile << mask <<"\n"<<data<<"\n";
	buffer2 << mask <<"\n"<<data<<"\n";
	return buffer2.str();
	

}

void BAPFrame::ReadFromBuffer(char *buffer)
{
	int bapnum;
	for(bapnum=1;bapnum<=NUMBAPS;bapnum++)
	{
		sscanf(buffer,"%d",&mBAPVector[bapnum].mMask);
		while((buffer[0]!=' ')&&(buffer[0]!=0))
			buffer++;
		if(buffer!=0)
			buffer++;
		else
			break;
	}

	while((buffer[0]=='\n')||(buffer[0]=='\t')||(buffer[0]==' '))
		buffer++;
	
	for(bapnum=0;bapnum<=NUMBAPS;bapnum++) 
	{
		if(bapnum==0)
		{
			sscanf(buffer,"%d",&this->mFrameNumber);
			while((buffer[0]!=' ')&&(buffer[0]!=0))
				buffer++;
			if(buffer!=0)
				buffer++;
			else
				break;
		}
		else
		{
			if(mBAPVector[bapnum].mMask)
			{
				sscanf(buffer,"%d",&mBAPVector[bapnum].mBAPValue);
				while((buffer[0]!=' ')&&(buffer[0]!=0))
					buffer++;
				if(buffer!=0)
					buffer++;
				else
					break;
			}
		}
	}
}

void BAPFrame::SetBAP(BAPType which, int value)
{
	mBAPVector[which].SetBAPValue(value);
	mBAPVector[which].SetMask(true);
}
void BAPFrame::SetBAP( BAPType which, AngleType t, double a)
{
	mBAPVector[which].SetBAPValue(mConverter->AngleToInt(t,a));
	mBAPVector[which].SetMask(true);
}
void BAPFrame::SetBAP(int which, int value)
{
	mBAPVector[which].SetBAPValue(value);
	mBAPVector[which].SetMask(true);
}
void BAPFrame::SetBAP( int which, AngleType t, double a)
{
	mBAPVector[which].SetBAPValue(mConverter->AngleToInt(t,a));
	mBAPVector[which].SetMask(true);
}

int BAPFrame::GetBAP(BAPType which)
{
	return mBAPVector[which].GetBAPValue();
}

int BAPFrame::GetBAP(int which)
{
	return mBAPVector[which].GetBAPValue();
}

double BAPFrame::GetBAPAngle(BAPType which, AngleType t)
{
	return mConverter->IntToAngle(t,mBAPVector[which].GetBAPValue());
}

double BAPFrame::GetBAPAngle(int which, AngleType t)
{
	return mConverter->IntToAngle(t,mBAPVector[which].GetBAPValue());
}

void BAPFrame::SetBAP(SidelessBAPType which, SideType side, int value)
{
	BAPType theBAP=mConverter->GetSideBAP(which, side);
	SetBAP(theBAP, value);
}
void BAPFrame::SetBAP(SidelessBAPType which, SideType side, AngleType t, double angle)
{
	int value=mConverter->AngleToInt(t,angle);
	BAPType theBAP=mConverter->GetSideBAP(which, side);
	SetBAP(theBAP, value);
}

int BAPFrame::GetBAP(SidelessBAPType which, SideType side)
{
	BAPType theBAP=mConverter->GetSideBAP(which, side);
	return GetBAP(theBAP);
}

int BAPFrame::GetBAP(string mjointname)
{
	BAPType theBAP=mConverter->MJointToBAP(mjointname);
	return GetBAP(theBAP);
}

double BAPFrame::GetBAPAngle(SidelessBAPType which, SideType side, AngleType t)
{
	BAPType theBAP=mConverter->GetSideBAP(which, side);
	return GetBAPAngle(theBAP, t);
}

double BAPFrame::GetBAPAngle(string mjointname, AngleType t)
{
	BAPType theBAP=mConverter->MJointToBAP(mjointname);
	return GetBAPAngle(theBAP, t);
}

void BAPFrame::SetBAP(string mjointname, AngleType t, double angle)
{
	int value=mConverter->AngleToInt(t,angle);
	BAPType theBAP=mConverter->MJointToBAP(mjointname);
	SetBAP(theBAP, value);
}

/** @return if specified BAP is set (0 or 1) */
bool BAPFrame::GetMask(BAPType which)
{
	return mBAPVector[which].GetMask();
}

/** @return if specified BAP is set (0 or 1) */
bool BAPFrame::GetMask(int which)
{
	return mBAPVector[which].GetMask();
}

/** @return if specified BAP is set (0 or 1) */
bool BAPFrame::GetMask(SidelessBAPType which, SideType side)
{
	BAPType theBAP=mConverter->GetSideBAP(which, side);
	return GetMask(theBAP);
}

bool BAPFrame::GetMask(string mjointname)
{
	BAPType theBAP=mConverter->MJointToBAP(mjointname);
	return GetMask(theBAP);
}

bool BAPFrame::IsDefined(ArmGroupType argGroup, SideType argSide)
{
	bool result= false;
	switch (argGroup) {
	case GROUP_ALL:
		result = (IsDefined(GROUP_SHOULDER, argSide) &&
					IsDefined(GROUP_ELBOW, argSide) &&
					IsDefined(GROUP_WRIST, argSide) &&
					IsDefined(GROUP_FINGERS, argSide));
		break;
	case GROUP_SHOULDER:
		if (GetMask(shoulder_flexion, argSide) ||
			GetMask(shoulder_abduct,argSide) ||
			GetMask(shoulder_twisting,argSide)) 
		
			result=true;	
		
		else result=false;
		break;

	case GROUP_ELBOW:
		if (GetMask(elbow_flexion,argSide) ||
			GetMask(elbow_twisting,argSide) ) 
			 result=true;
		else result=false;
		break;

	case GROUP_WRIST:
		if (GetMask(wrist_twisting,argSide) ||
			GetMask(wrist_flexion,argSide) ||
			GetMask(wrist_pivot,argSide)) 
			 result=true;
		else result=false;
		break;
		
	case GROUP_FINGERS:
		if (GetMask(pinky_flexion1,argSide) ||
			GetMask(pinky_abduct,argSide) ||
			GetMask(pinky_twisting,argSide) ||
			GetMask(pinky_flexion2,argSide) ||
			GetMask(pinky_flexion3,argSide) ||
			GetMask(ring_flexion1,argSide) ||
			GetMask(ring_abduct,argSide) ||
			GetMask(ring_twisting,argSide) ||
			GetMask(ring_flexion2,argSide) ||
			GetMask(ring_flexion3,argSide) ||
			GetMask(middle_flexion1,argSide) ||
			GetMask(middle_abduct,argSide) ||
			GetMask(middle_twisting,argSide) ||
			GetMask(middle_flexion2,argSide) ||
			GetMask(middle_flexion3,argSide) ||
			GetMask(index_flexion1,argSide) ||
			GetMask(index_abduct,argSide) ||
			GetMask(index_twisting,argSide) ||
			GetMask(index_flexion2,argSide) ||
			GetMask(index_flexion3,argSide) ||
			GetMask(thumb_flexion1,argSide) ||
			GetMask(thumb_abduct,argSide) ||
			GetMask(thumb_twisting,argSide) ||
			GetMask(thumb_flexion2,argSide) ||
			GetMask(thumb_flexion3) ) 
			 result=true;
		else result=false;
		break;
		
	
	default:
		result=false;
	}
	return result;
}

void BAPFrame::SetMask(BAPType which, bool b)
{
	mBAPVector[which].SetMask(b);
}

void BAPFrame::SetMask(SidelessBAPType which, SideType side, bool b)
{
	BAPType theBAP=mConverter->GetSideBAP(which, side);
	SetMask(theBAP, b);
}

void BAPFrame::SetGroupMask(ArmGroupType group, SideType side, bool b)
{
	switch (group) {
	case GROUP_ALL:
		SetGroupMask(GROUP_SHOULDER, side, b);
		SetGroupMask(GROUP_ELBOW, side, b);
		SetGroupMask(GROUP_WRIST, side, b);
		SetGroupMask(GROUP_FINGERS, side, b);
		break;
	case GROUP_SHOULDER:
		SetMask(shoulder_flexion, side,b);
		SetMask(shoulder_abduct,side,b);
		SetMask(shoulder_twisting,side,b); 
		break;

	case GROUP_ELBOW:
		SetMask(elbow_flexion,side,b);
		SetMask(elbow_twisting,side,b);
		break;

	case GROUP_WRIST:
		SetMask(wrist_twisting,side,b);
		SetMask(wrist_flexion,side,b);
		SetMask(wrist_pivot,side,b); 
		break;
		
	case GROUP_FINGERS:
		SetMask(pinky_flexion1,side,b);
		SetMask(pinky_abduct,side,b);
		SetMask(pinky_twisting,side,b);
		SetMask(pinky_flexion2,side,b);
		SetMask(pinky_flexion3,side,b);
		SetMask(ring_flexion1,side,b);
		SetMask(ring_abduct,side,b);
		SetMask(ring_twisting,side,b);
		SetMask(ring_flexion2,side,b);
		SetMask(ring_flexion3,side,b);
		SetMask(middle_flexion1,side,b);
		SetMask(middle_abduct,side,b);
		SetMask(middle_twisting,side,b);
		SetMask(middle_flexion2,side,b);
		SetMask(middle_flexion3,side,b);
		SetMask(index_flexion1,side,b);
		SetMask(index_abduct,side,b);
		SetMask(index_twisting,side,b);
		SetMask(index_flexion2,side,b);
		SetMask(index_flexion3,side,b);
		SetMask(thumb_flexion1,side,b);
		SetMask(thumb_abduct,side,b);
		SetMask(thumb_twisting,side,b);
		SetMask(thumb_flexion2,side,b);
		SetMask(thumb_flexion3,side,b);
		break;
	default:
		break;
	}
}

void BAPFrame::CopyAnglesNoMask(BAPFrame *pSource)
{
	for (int i=0; i<= NUMBAPS; i++) 
		mBAPVector[i].SetBAPValue(pSource->GetBAP(i));
}

void BAPFrame::CopyAngles(BAPFrame *pSource)
{
	for (int i=0; i<= NUMBAPS; i++)
	{
		mBAPVector[i].SetBAPValue(pSource->GetBAP(i));
		mBAPVector[i].SetMask(pSource->GetMask(i));
	}
}

void BAPFrame::CopyAngles(BAPFrame *pSource,float scalefactor)
{
	for (int i=0; i<= NUMBAPS; i++)
	{
		mBAPVector[i].SetBAPValue(pSource->GetBAP(i)*scalefactor);
		mBAPVector[i].SetMask(pSource->GetMask(i));
	}
}


float BAPFrame::GetTCBParam(TCBParamType which)
{
	return TCBParam[which];
}

void BAPFrame::SetTCBParam(float T, float C, float B)
{
	TCBParam[Tension]=T;
	TCBParam[Continuity]=C;
	TCBParam[Bias]=B;
}

void BAPFrame::SetTension(float T)
{
	TCBParam[Tension]=T;
}

void BAPFrame::SetContinuity(float C)
{
	TCBParam[Continuity]=C;
}

void BAPFrame::SetBias(float B)
{
	TCBParam[Bias]=B;
}

///////////
double BAPFrame::GetBAPLength(SidelessBAPType which, SideType side, LengthType t)
{
	BAPType theBAP=mConverter->GetSideBAP(which, side);
	return GetBAPLength(theBAP, t);
}

double BAPFrame::GetBAPLength(string mjointname, LengthType t)
{
	BAPType theBAP=mConverter->MJointToBAP(mjointname);
	return GetBAPLength(theBAP, t);
}

double BAPFrame::GetBAPLength(BAPType which, LengthType t)
{
	return mConverter->IntToLength(t,mBAPVector[which].GetBAPValue());
}

double BAPFrame::GetBAPLength(int which, LengthType t)
{
	return mConverter->IntToLength(t,mBAPVector[which].GetBAPValue());
}

void BAPFrame::SetBAP( string mjointname, int value)
{
	BAPType theBAP=mConverter->MJointToBAP(mjointname);
	if(theBAP!=null_bap)
		SetBAP(theBAP, value);
}


void BAPFrame::AddBAPFrame(BAPFrame *bframe)
{
	for (int i=0; i<= NUMBAPS; i++)
	{
		if(bframe->GetMask(i)==1)
		{
			this->SetBAP(i,this->GetBAP(i)+bframe->GetBAP(i));
		}
	}
}
