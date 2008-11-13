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

// FaceExpression.cpp: implementation of the FaceExpression class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceExpression.h"
#include "FAPFrame.h"
#include <math.h>
//#include "FaceLinearInterpolator.h"
//#include "FaceCosineInterpolator.h"

extern IniManager inimanager;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern FILE *data_log;


FaceExpression::FaceExpression()
{
	frames.clear();
	animation.clear();
	start=0;
	end=0;
	emphasis=0;
	onset=0;
	apex=0;
	offset=0;
	intensity=1;
	classname="";
	instance="";
	channels=new channel;
	channels->eyebrows=-1;
	channels->gaze=-1;
	channels->head_direction=-1;
	channels->head_movement=-1;
	channels->mouth=-1;
}

FaceExpression::~FaceExpression()
{

}

void FaceExpression::rescaleIntensity(float intensity) {
if ((intensity>1.0f)||(intensity<0.0f)) return;

	std::vector<FAPFrame>::iterator iter;


	for(iter=this->frames.begin();iter!=this->frames.end();iter++)
	{
						
			for(int i=0;i<69;i++){
				int value=(*iter).GetFAP(i);
				value=(int) (value*sqrt(intensity));
				(*iter).SetFAP(i,value);	
	}

			}
}//rescaleIntensity


FaceExpression* FaceExpression::clone()
{
	FaceExpression *new_expression = new FaceExpression();
	
	//animation.clear();
	new_expression->setAnimation(this->getAnimation());
	
	//apex=0;
	new_expression->setApex(this->getApex());
	
	//channels=new channel;
	//channels->eyebrows=-1;
	//channels->gaze=-1;
	//channels->head_direction=-1;
	//channels->head_movement=-1;
	//channels->mouth=-1;
	new_expression->setChannels(this->getChannels());
	
	//classname="";
	new_expression->setClassname(this->getClassname());
	
	//emphasis=0;
	new_expression->setEmphasis(this->getEmphasis());
	
	//end=0;
	new_expression->setEnd(this->getEnd());
	
	//frames.clear();
	new_expression->SetFrames(this->GetFrames());
	
	//instance="";
	new_expression->setInstance(this->getInstance());
		
	//intensity=1;
	new_expression->setIntensity(this->getIntensity());
	
	//offset=0;
	new_expression->setOffset(this->getOffset());
	
	//onset=0;
	new_expression->setOnset(this->getOnset());
	
	//start=0;
	new_expression->setStart(this->getStart());
	
	return new_expression;	
	
}

	void FaceExpression::setOffset (float offset1){this->offset=offset1;};
	void FaceExpression::setApex (float apex1){this->apex=apex1;};
	void FaceExpression::setOnset (float onset1){this->onset=onset1;};
	void FaceExpression::setIntensity (float intensity1){this->intensity=intensity1;};
	void FaceExpression::setEmphasis (float emphasis1){this->emphasis=emphasis1;};
	void FaceExpression::setEnd (float end1){this->end=end1;};
	void FaceExpression::setStart (float start1){this->start=start1;};
	void FaceExpression::setInstance (std::string instance1){this->instance=instance1;};
	void FaceExpression::setClassname (std::string classname1){this->classname=classname1;};
	void FaceExpression::SetFrames(FAPFrameVector frames1){this->frames=frames1;};

	float FaceExpression::getOffset (){return this->offset;};
	float FaceExpression::getApex (){return this->apex;};
	float FaceExpression::getOnset (){return this->onset;};
	float FaceExpression::getIntensity (){return this->intensity;};
	float FaceExpression::getEmphasis (){return this->emphasis;};
	float FaceExpression::getEnd (){return this->end;};
	float FaceExpression::getStart (){return this->start;};
	FAPFrameVector FaceExpression::getAnimation(){return this->animation;};


	FAPFrameVector FaceExpression::GetFrames(){return this->frames;};
	void FaceExpression::setAnimation(FAPFrameVector animation1){
		animation=animation1;
	
	};
	void FaceExpression::setChannels(channel *channels1){channels=channels1;};


void FaceExpression::Print()
{
	fprintf(data_log,"expression %s::%s\n",classname.c_str(),instance.c_str());
	std::vector<FAPFrame>::iterator iter;
		if(!this->frames.empty())
			for(iter=this->frames.begin();iter!=this->frames.end();iter++)
			{
				(*iter).Print();
			}
}

FAPFrame* FaceExpression::GetFAPFrame(int n)
{
	std::vector<FAPFrame>::iterator iter;
	int i=0;
	iter=frames.begin();
	while((i<n)&&(iter!=frames.end()))
	{
		i++;
		iter++;
	}
	if(i==n)
		return &(*iter);
	return 0;

}

int FaceExpression::GetNumberOfFrames()
{
	std::vector<FAPFrame>::iterator iter;
	int i=0;
	for(iter=frames.begin();iter!=frames.end();iter++)
		i++;
	return i;
}

int **FaceExpression::GetRawFaps(float prob)
{
	int **raw_faps;
	int i;

	raw_faps=(int**)malloc(sizeof(int*)*2);
	for(i=0;i<2;i++)
	{
		raw_faps[i]=(int*)malloc(sizeof(int)*69);
		for(int j=0;j<69;j++)
			raw_faps[i][j]=0;
	}

	float total_prob;
	total_prob=0.0f;

	std::vector<FAPFrame>::iterator iter;
	for(iter=this->frames.begin();iter!=this->frames.end();iter++)
	{
		total_prob=total_prob+(*iter).probability;
		if(total_prob>=prob)
		{
			for(i=0;i<69;i++)

			{
				raw_faps[0][i]=(*iter).FAPs[i].active;
				raw_faps[1][i]=(*iter).GetFAP(i);
			}
			return raw_faps;
		}
	}
	return 0;
}

//! 
//! Given the probability 'prob' returns TRUE if
//! the FaceExpression has an associated head movement and store in:
//! - head_m_type: the head movement type (HEAD_M_NOD or HEAD_M_SHAKE)
//! - head_m_type: amplitude of head movement (in degrees)
//! - head_m_period: period duration of the head movement (in seconds)
//! Otherwise returns FALSE
//!
void FaceExpression::GetMovement(float prob,int *movementtype,float *movementamplitude,float *movementperiod,float notscaledREP,float notscaledSPC)
{
	std::vector<FAPFrame>::iterator iter;
	float total_prob;
	total_prob=0.0f;

	EngineParameter *SPC;
	EngineParameter *REP;

	SPC = new EngineParameter(0.5, 1.0, 1.5);
	SPC->SetValue(notscaledSPC);

	REP = new EngineParameter(1.0, 0.5);
	REP->SetValue(notscaledREP);

	for(iter=this->frames.begin();iter!=this->frames.end();iter++)
	{
		total_prob=total_prob+(*iter).probability;
		if(total_prob>=prob)
		{
			if((*iter).headmov.active==false)	
			{
				*movementtype=HEAD_NOTHING;
				return;
			}
			if((*iter).headmov.type=="shake")
				*movementtype=HEAD_SHAKE;
			if((*iter).headmov.type=="nod")
				*movementtype=HEAD_NOD;
			*movementamplitude=(*iter).headmov.amplitude;
			*movementperiod=(*iter).headmov.period;
			(*movementperiod)*=((REP->GetScaledValue()*2)+1);
			*movementamplitude=*movementamplitude*SPC->GetScaledValue();

			return;
		}
	}
}

std::string FaceExpression::getInstance ()
{
	return this->instance;
}

std::string FaceExpression::getClassname ()
{
	return this->classname;
}
	

channel* FaceExpression::getChannels()
{
	return this->channels;
}

/*
void FaceExpression::DetermineOnsetOffset()
{
	float duration=end-start;
	if((duration/10.0f)<0.1)
	{
		onset=0.1f;
		offset=0.1f;
	}
	else
	{
		onset=duration/10.0f;
		offset=duration/10.0f;
	}
	apex=duration-onset-offset;
}

void FaceExpression::CreateAnimation()
{
	if((end-start)==0)
		return;
	
	DetermineOnsetOffset();

	int numberofframes=ceil((end-start)*inimanager.GetValueFloat("FPS"));
	FAPFrame *f;
	for(int i=0;i<numberofframes;i++)
	{
		f=new FAPFrame();
		animation.push_back(*f);
	}

	int framenumber;
	FAPFrame *emptyframe;
	
	emptyframe=frames[0].GetCorrespondingZeroFrame();
	//printf("generating keyframes for %s:%s\n",(*expression).classname.c_str(),(*expression).instance.c_str());
	framenumber=0;
	animation[framenumber]=*emptyframe;
	animation[framenumber].isKeyFrame=true;
	framenumber=numberofframes-1;
	animation[framenumber]=*emptyframe;
	animation[framenumber].isKeyFrame=true;
	framenumber=ceil(onset*inimanager.GetValueFloat("FPS"))-1;
	animation[framenumber].CopyFrom(&frames[0]);
	animation[framenumber].isKeyFrame=true;
	framenumber=ceil(((end-start)-offset)*inimanager.GetValueFloat("FPS"))-1;
	animation[framenumber].CopyFrom(&frames[0]);
	animation[framenumber].isKeyFrame=true;

	for(i=0;i<NUMBER_OF_FAPS;i++)
	{
//		FaceCosineInterpolator interpolator;
//		interpolator.InterpolateFAP(i,animation);
	}
}
*/