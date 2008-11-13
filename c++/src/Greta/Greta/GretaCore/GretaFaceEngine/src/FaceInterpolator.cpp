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

// FaceInterpolator.cpp: implementation of the FaceInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include "FaceInterpolator.h"
#include "FaceEngine.h"
#include "DataViewerWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//extern "C" bspline(int,int,int,float[],float[]);

extern FILE *face_log;

FaceInterpolator::FaceInterpolator(int TotalNumberOfFrames,void *engine)
{
	int i;

	FramesTotalNumber=TotalNumberOfFrames;

	FLD = new EngineParameter(0,1);

	PWR = new EngineParameter(0, 0.5);

	for (i=0;i<KEY_PARAMS;i++)
	{
		keyframes_first[i]=0;
		keyframes_last[i]=0;
	}

}

FaceInterpolator::~FaceInterpolator()
{
	delete FLD;
	delete PWR;
}

//!
//! Deallocate keyframe structure
//!
void FaceInterpolator::DeallocKeyFrames()
{
	int i;
	keyframe *key_ptr;
	keyframe *key_ptr2;

	for (i=0;i<KEY_PARAMS;i++)
	{
		key_ptr=keyframes_first[i];
		if(keyframes_first[i]!=0)
		{
			while (key_ptr)
			{
				key_ptr2=key_ptr;
				key_ptr=(keyframe*)key_ptr->next;
				free(key_ptr2);
			}
		}
	}

	memset(keyframes_first,0,KEY_PARAMS);
	memset(keyframes_last,0,KEY_PARAMS);
}

//!
//! Initialise keyframe structures
//! 
void FaceInterpolator::InitKeyFrames()
{
	int i;

	for (i=0;i<KEY_PARAMS;i++)
	{
		keyframes_first[i]=NULL;
		keyframes_last[i]=NULL;
	}
}

//!
//! Insert 'keyframe' for fap 'fapnum': 
//! neutral=1 if fapnum is part of a neutral face expression
//!
keyframe *FaceInterpolator::InsertKeyFrame(int fapnum,int keyframenumber,int neutral)
{
	keyframe *key_ptr;
	keyframe *key_ptr2;

	// case	1: list is empty
	if (!keyframes_first[fapnum])
	{
		key_ptr=(keyframe*)malloc(sizeof(keyframe));
		key_ptr->keyframenumber=keyframenumber;
		key_ptr->next=NULL;
		key_ptr->prev=NULL;
		key_ptr->neutral=neutral;
		keyframes_first[fapnum]=key_ptr;
		keyframes_last[fapnum]=key_ptr;
		return key_ptr;
	}

	if (keyframenumber<keyframes_first[fapnum]->keyframenumber)
	{
		key_ptr2=(keyframe*)malloc(sizeof(keyframe));
		key_ptr2->keyframenumber=keyframenumber;
		key_ptr2->next=keyframes_first[fapnum];
		key_ptr2->prev=NULL;
		key_ptr2->neutral=neutral;
		keyframes_first[fapnum]->prev=key_ptr2;
		keyframes_first[fapnum]=key_ptr2;
		return key_ptr2;
	}


	if (keyframenumber>keyframes_last[fapnum]->keyframenumber)
	{

		key_ptr2=(keyframe*)malloc(sizeof(keyframe));
		key_ptr2->keyframenumber=keyframenumber;
		key_ptr2->next=NULL;
		key_ptr2->prev=keyframes_last[fapnum];
		key_ptr2->neutral=neutral;
		keyframes_last[fapnum]->next=key_ptr2;
		keyframes_last[fapnum]=key_ptr2;
		return key_ptr2;

	}


	// search for the element >= of the the one to insert

	key_ptr=keyframes_first[fapnum];

	while (key_ptr&&keyframenumber>key_ptr->keyframenumber)
		key_ptr=(keyframe*)key_ptr->next;

	// case 2: the element exists
	
	if (keyframenumber<key_ptr->keyframenumber&&key_ptr)
	{
		key_ptr2=(keyframe*)malloc(sizeof(keyframe));
		key_ptr2->keyframenumber=keyframenumber;
		key_ptr2->next=key_ptr;
		((keyframe*)(key_ptr->prev))->next=key_ptr2;
		key_ptr2->prev=key_ptr->prev;
		key_ptr2->neutral=neutral;
		key_ptr->prev=key_ptr2;
		return key_ptr2;
	}
	else
	// case 3: the element is already present
	if (keyframenumber==key_ptr->keyframenumber&&key_ptr)
	{
		return key_ptr;
	}

	return 0;

}

void FaceInterpolator::InsertKeyFrame2(int fapnum,int frame,float value)
{
	if(fapnum==48)
		headinterpolator_x.AddPoint(frame,value,1,0,0);
	if(fapnum==49)
		headinterpolator_y.AddPoint(frame,value,1,0,0);
	if(fapnum==50)
		headinterpolator_z.AddPoint(frame,value,1,0,0);
}


//Elisabetta december 2005
//This function interpolates head faps key frame deriving from expression (through b-spline that 
//does not reach the points) and head faps key frame deriving form head-nods and head-shakes
//(through TCB). Finally, it adds the resultats in **faps
void FaceInterpolator::InterpolateHead(std::vector<FAPFrame> *fapvector,/*int **faps,int **fapmask,*/ int **faps_head_expr,int **fapmask_head_expr, int **faps_head_ns,int **fapmask_head_ns, float power, float fluidity)
{
	int  i, k;
//	int fapnum;

	FLD->SetValue(fluidity);
	PWR->SetValue(power);

	//TODO: see if it's possible to use TCB splines instead of B-splines
	//the problem is to delete some of the keyframes to obtain better animation
/*
	for(i=48;i<51;i++)
	{
		KeyFrameInt = new TCBInterpolator(PWR->GetScaledExpressivity(),FLD->GetScaledExpressivity(),0);
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			if(fapmask_head_expr[k][i-48]!=0)
				KeyFrameInt->AddPoint(k,(float)faps_head_expr[k][i-48]);
		}
	
		if(KeyFrameInt->points.size()>3)
			KeyFrameInt->Interpolate();
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
		
			fapmask[k][i]=1;
			faps[k][i]=(int)(KeyFrameInt->GetNextValue());
		}
		delete(KeyFrameInt);
	}


	for(i=48;i<51;i++)
	{
		KeyFrameInt = new TCBInterpolator(PWR->GetScaledExpressivity(),FLD->GetScaledExpressivity(),0);
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			if(fapmask_head_ns[k][i-48]!=0)
				KeyFrameInt->AddPoint(k,(float)faps_head_ns[k][i-48]);
		}
	
		if(KeyFrameInt->points.size()>3)
			KeyFrameInt->Interpolate();
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
		
			fapmask[k][i]=1;
			faps[k][i]+=(int)(KeyFrameInt->GetNextValue());
		}
		delete(KeyFrameInt);
	}
	*/

	for(i=48;i<51;i++)
	{
		KeyFrameInt = new TCBInterpolator(PWR->GetScaledValue(),FLD->GetScaledValue(),0);
	
/*	
		for(k=0; k<(*fapvector).size(); k++)
		{
				if(((*fapvector)[k]).FAPs[i].active==true)
					KeyFrameInt->AddPoint(k,((*fapvector)[k]).GetFAP(i));
		}
*/

		//printf("dddd %i \n", fapvector->size() );		
		//printf("FramesTotalNumber %i \n", FramesTotalNumber );
		
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			if(fapmask_head_expr[k][i-48]==1)
			{
				KeyFrameInt->AddPoint(k,(float)faps_head_expr[k][i-48]);
			}
		}
			

 		if(KeyFrameInt->points.size()>0)
		{
			//Maurizio 28-6-2008
			//trick: if there is only a short movement we could have
			//less than 4 keyframes, which are not enough to perform
			//interpolation. so we manually add a few fake keyframes
			//at the end of the keyframe vector.
			while(KeyFrameInt->points.size()<4)
			{
				KeyPoint lastpoint=KeyFrameInt->points[KeyFrameInt->points.size()-1];
				KeyFrameInt->AddPoint(lastpoint.time+1,lastpoint.value);
			}
			KeyFrameInt->Interpolate();
		}
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			(*fapvector)[k].SetFAP(i,(int)(KeyFrameInt->GetNextValue()));
		}
		delete(KeyFrameInt);
	}


	for(i=48;i<51;i++)
	{
		KeyFrameInt = new TCBInterpolator(PWR->GetScaledValue(),FLD->GetScaledValue(),0);
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			if(fapmask_head_ns[k][i-48]!=0)
			{
				KeyFrameInt->AddPoint(k,(float)faps_head_ns[k][i-48]);
				//printf("key frame head fap %d value %d frame %d\n",i,faps_head_ns[k][i-48],k);
			}
		}
	
		if(KeyFrameInt->points.size()>0)
		{
			//Maurizio 28-6-2008
			//trick: if there is only a short movement we could have
			//less than 4 keyframes, which are not enough to perform
			//interpolation. so we manually add a few fake keyframes
			//at the end of the keyframe vector.
			while(KeyFrameInt->points.size()<4)
			{
				KeyPoint lastpoint=KeyFrameInt->points[KeyFrameInt->points.size()-1];
				KeyFrameInt->AddPoint(lastpoint.time+1,lastpoint.value);
			}
			KeyFrameInt->Interpolate();
		}
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			(*fapvector)[k].AddToFAP(i,(int)(KeyFrameInt->GetNextValue()));
			//printf("fram: %d   fap: %d   value: %d\n", k, i, (int)(KeyFrameInt->GetNextValue()));
		}
		delete(KeyFrameInt);
	}
}

//Elisabetta december 2005
//This function interpolates all faps but those of the head (48, 49, 50)
//using TCB interpolation
void FaceInterpolator::InterpolateFaps_NoHead(std::vector<FAPFrame> *fapvector,/*int **faps,int **fapmask,*/ float power, float fluidity)
{
	int  i, k;

	FLD->SetValue(fluidity);
	PWR->SetValue(power);
	
	for(i=3;i<69;i++)
	{
		if(i!=48 && i!=49 && i!=50)
		{
			KeyFrameInt = new TCBInterpolator(PWR->GetScaledValue(),FLD->GetScaledValue(),0);
	
			for(k=0; k<(*fapvector).size(); k++)
			{
				if(((*fapvector)[k]).FAPs[i].active==true)
					KeyFrameInt->AddPoint(k,((*fapvector)[k]).GetFAP(i));
			}
		
			//printf (" KeyFrameInt->points.size() %d \n " , KeyFrameInt->points.size() );
		
			if(KeyFrameInt->points.size()>3)
			{
				KeyFrameInt->Interpolate();
		
				for(k=0; k<this->FramesTotalNumber; k++)
				{
					((*fapvector)[k]).SetFAP(i,(int)(KeyFrameInt->GetNextValue()));

					//if(i==23)	//visualizzare la curva del sopracciglio sinistro
					//	dv->AddPointToGraph("23",k,faps[k][i]);	//verde
				}
			}
			delete(KeyFrameInt);
		}
	}
}

//Elisabetta 12-10-2005
void FaceInterpolator::InterpolateEyeKeyFrames(std::vector<FAPFrame> *fapvector,int **eyes_fapmask,int **eyes_faps/*,int **fapmask,int **faps*/)
{
	int fapnum,frameind;
	int key1,key2;

	//for the new structure:
	for (fapnum=0;fapnum<=3;fapnum++)
	{
		int i=0;

		for(i=0;i<FramesTotalNumber;i++)
			if(eyes_fapmask[fapnum][i]==1)
			{
				key1=i;
				i++;
				break;
			}
		
		if(i==FramesTotalNumber)
			continue;

		while (i<FramesTotalNumber)
		{
			if(eyes_fapmask[fapnum][i]==1)
			{
				key2=i;
				eyes_interpolate(fapnum,key1,key2,eyes_fapmask,eyes_faps);
				key1=key2;
				i=key2;
			}
			i++;
		}
	}

	for (frameind=0;frameind<FramesTotalNumber;frameind++)
	{
		int i;

		for (i=0;i<=3;i++)
		{
			//maurizio
			//changed SetFap to AddToFAP
			//TODO: check that the rotations are not exagerated
			(*fapvector)[frameind].AddToFAP(23+i,eyes_faps[i][frameind]);//,eyes_fapmask[i][frameind]);
		}
	}
}

void FaceInterpolator::InterpolateEyelidKeyFrames(std::vector<FAPFrame> *fapvector,int **eyelids_fapmask,int **eyelids_faps/*,int **fapmask,int **faps*/)
{
	int  i, k;
//	int fapnum;
	float tension, continuity;
	float m, c, x1, x2, y1, y2;

	tension=1.0;
	continuity=0;
	
	x1=0;
	x2=1100;
	y1=1;
	y2=0;
	
	m = (y2-y1)/(x2-x1);
	c = y1 - m*x1;

	for(i=19;i<21;i++)
	{
		KeyFrameInt = new TCBInterpolator(tension,continuity,0);
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			if(eyelids_fapmask[i-19][k]!=0)
				KeyFrameInt->AddPoint(k,(float)eyelids_faps[i-19][k]);
		}
	
		if(KeyFrameInt->points.size()>3)
			KeyFrameInt->Interpolate();
	
		for(k=0; k<this->FramesTotalNumber; k++)
		{
			(*fapvector)[k].AddToFAP(i,(int)(KeyFrameInt->GetNextValue()*(m*(*fapvector)[k].GetFAP(i)+c)));
		}
		delete(KeyFrameInt);
	}
}

//!
//! Interpolate the fap 'fapnum' (LEYEX, LEYEy, REYEX, REYEY) from keyframe 'k1' to keyframe 'k2'
//! This function is used for the eyes movement computed by the eye gaze model 
//!
void FaceInterpolator::eyes_interpolate(int fapnum,int k1, int k2,int **eyes_fapmask,int **eyes_faps)
{
	int frameind;
	float ft;
	float f;

	for (frameind=k1+1;frameind<k2;frameind++)
	{
		ft=((float)(frameind-k1)/((float)(k2-k1)))*M_PI;
		f=(1.0-cos(ft))*0.5;

		eyes_fapmask[fapnum][frameind]=1;
		
		eyes_faps[fapnum][frameind]=(((float)eyes_faps[fapnum][k1])*(1.0-f)+((float)eyes_faps[fapnum][k2])*f);
	}
}

//!
//! Interpolate the fap 'fapnum' (LEYEX, LEYEy, REYEX, REYEY) from keyframe 'k1' to keyframe 'k2'
//! This function is used for the eyes movement computed by the eye gaze model 
//!
void FaceInterpolator::eyelids_interpolate(int fapnum,int k1, int k2,int **eyelids_fapmask,int **eyelids_faps)
{
	int frameind;
	float ft;
	float f;

	for (frameind=k1+1;frameind<k2;frameind++)
	{
		ft=((float)(frameind-k1)/((float)(k2-k1)))*M_PI;
		f=(1.0-cos(ft))*0.5;

		eyelids_fapmask[fapnum][frameind]=1;
		
		eyelids_faps[fapnum][frameind]=(((float)eyelids_faps[fapnum][k1])*(1.0-f)+((float)eyelids_faps[fapnum][k2])*f);
	}
}

int FaceInterpolator::CheckForFreeHead(float start_sec,float secs)
{
	keyframe *key_ptr;
	int k1,k2;
//	int empty;


	k1=start_sec*FRAMERATE;
	k2=(start_sec+secs)*FRAMERATE;

	key_ptr=keyframes_first[HEADY];

	while (key_ptr)
	{
		if (key_ptr->keyframenumber>=k1&&
			key_ptr->keyframenumber<=k2)
			return 0;
		key_ptr=(keyframe*)key_ptr->next;
	}
	return 1;
}
