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

// TCBInterpolator.cpp: implementation of the TCBInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#include "TCBInterpolator.h"
#include <algorithm>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include <algorithm>

TCBInterpolator::TCBInterpolator(float T,float C,float B)
{
	fps=25;
	sT=0;
	sC=0;
	sB=0;
	sTime=0;
	sTCBpoint=0;
	mpTCB2_Swivel=0;
	SetTCB(T,C,B);
	verbose=false;
	Reset();
}

TCBInterpolator::TCBInterpolator()
{
	fps=25;
	sT=0;
	sC=0;
	sB=0;
	sTime=0;
	sTCBpoint=0;
	mpTCB2_Swivel=0;
	SetTCB(1,1,0);
	verbose=false;
	Reset();
}

TCBInterpolator::~TCBInterpolator()
{
	//if((mpTCB2_Swivel!=0)&&(sT!=0)&&(sC!=0)&&(sB!=0)&&(sTime!=0)&&(sTCBpoint!=0))
	//	delete mpTCB2_Swivel;
	if(sT!=0)
		delete sT;
	if(sC!=0)
		delete sC;
	if(sB!=0)
		delete sB;
	if(sTime!=0)
		delete sTime;
	if(sTCBpoint!=0)
		delete sTCBpoint;
}

void TCBInterpolator::AddPoint(float time, float value)
{
	AddPoint(time,value,this->T,this->C,this->B);
}

void TCBInterpolator::AddPointNoSort(float time, float value)
{
	AddPointNoSort(time,value,this->T,this->C,this->B);
}

void TCBInterpolator::AddPoint(float time, float value,float pT,float pC,float pB)
{
	KeyPoint *p;
	p=new KeyPoint;
	p->time=time;
	p->value=value;
	p->T=pT;
	p->C=pC;
	p->B=pB;
	points.push_back(*p);
	std::sort(points.begin(),points.end());
}

void TCBInterpolator::AddPointNoSort(float time, float value,float pT,float pC,float pB)
{
	KeyPoint *p;
	p=new KeyPoint;
	p->time=time;
	p->value=value;
	p->T=pT;
	p->C=pC;
	p->B=pB;
	points.push_back(*p);
}

void TCBInterpolator::Interpolate()
{
	Prepare();

	Wm4::Vector2f TCBpoint;

	if(points.size()==0)
	{
		in_use=false;
		return;
	}

	for(int i=0; i<=points[points.size()-1].time; i++)
	{
		TCBpoint=mpTCB2_Swivel->GetPosition(float(i));
		result.push_back(TCBpoint[1]);
	}
	in_use=true;
}

void TCBInterpolator::Prepare()
{
	result.clear();
	if(points.size()<4)
	{
		if(verbose)
			printf("warning: TCB interpolation failed, at least 4 points needed...\n");
		return;
	}

	in_use=true;

	sTCBpoint = new Wm4::Vector2f[points.size()];

	sT = new float[points.size()];
	sC = new float[points.size()];
	sB = new float[points.size()];
	sTime = new float[points.size()];

	for(int j=0; j<points.size(); j++)
	{
		sT[j]=points[j].T;
		sC[j]=points[j].C;
		sB[j]=points[j].B;
		sTime[j]=points[j].time;
		sTCBpoint[j][0]=points[j].time;
		sTCBpoint[j][1]=points[j].value;
	}

	mpTCB2_Swivel=new Wm4::TCBSpline2f((points.size()-1),sTime,sTCBpoint,sT,sC,sB);

}

void TCBInterpolator::InterpolateFAP(int fanumber, FAPFrameVector &animation)
{
	std::vector<FAPFrame>::iterator frame;
	int counter;
	int firstframe;

	counter=0;
	firstframe=-1;

	for(frame=animation.begin();frame!=animation.end();frame++)
	{
		if(((*frame).isKeyFrame)&&((*frame).FAPs[fanumber].active))
		{
			if(firstframe==-1)
				firstframe=counter;
			this->AddPoint(counter,(*frame).FAPs[fanumber].value);
		}
		counter++;
	}
	
	this->Interpolate();
	if(!result.empty())
	{
		std::vector<float>::iterator iter;
		
		//frame=&animation[firstframe];
		//CHRIS fix
		frame= animation.begin() + firstframe;

		for(iter=result.begin();iter!=result.end();iter++)
		{
			(*frame).SetFAP(fanumber,(*iter));
			//frame->Print();
			frame++;
		}
	}
}

float TCBInterpolator::GetNextValue()
{
	float r;
	if(!in_use)
		return 0;
	r=result[internal_counter];
	internal_counter++;
	millisec_accum=0.0f;
	if(internal_counter==result.size())
		Reset();
	return r;
}

float TCBInterpolator::GetNextNValue(int n)
{
	float r;
	if(n==0)
		return 0;
	if(!in_use)
		return 0;
	r=result[internal_counter];
	internal_counter=internal_counter+n;
	millisec_accum=0.0f;
	if(internal_counter>=result.size())
		Reset();
	return r;
}

float TCBInterpolator::GetAfterMillisecs(float s)
{
	float r;
	if(!in_use)
		return 0;
	r=0;
	//printf("getting frame %d %f\n",internal_counter,millisec_accum);
	millisec_accum=millisec_accum+s;
	if(millisec_accum>=(1000.0f/fps))
	{
		internal_counter=internal_counter+(millisec_accum/(1000.0f/fps));
		int res=millisec_accum/(1000.0f/fps);
		millisec_accum=millisec_accum-res*(1000.0f/fps);
	}
	if(internal_counter>=result.size())
	{
		internal_counter=result.size()-1;
		r=result[result.size()-1];
		Reset();
	}
	else
		r=result[internal_counter];
	return r;
}

float TCBInterpolator::GetFrameN(int fn)
{
	if((fn>=0)&&(result.empty()))
	{
		return mpTCB2_Swivel->GetPosition(float(fn))[1];
	}

	if((fn>=0)&&(fn<result.size()))
	{
		return result[fn];
	}
	else
		return 0;
}

float TCBInterpolator::GetAtTime(float time)
{
	if((time>=0)&&(result.empty()))
	{
		return mpTCB2_Swivel->GetPosition(float(time))[1];
	}
	else
		return 0;
}

void TCBInterpolator::Reset()
{
	points.clear();
	result.clear();
	internal_counter=0;
	millisec_accum=0;
	in_use=false;
}

void TCBInterpolator::SetTCB(float T,float C, float B)
{
	this->T=T;
	this->C=C;
	this->B=B;
}
