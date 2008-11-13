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

// FaceInterpolator.h: interface for the FaceInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FaceEngineGlobals.h"
//#include "FaceLinearInterpolator.h"
#include "TCBInterpolator.h"
#include "EngineParameter.h"

/**
*
* class :FaceInterpolator
*
*/

class FaceInterpolator  
{
public:

	/**
	*
	* contructor 
	*
	* @param  TotalNumberOfFrames
	* @param engine
	*/

	FaceInterpolator(int TotalNumberOfFrames,void *engine);

	/**
	*
	* destructor 
	*/

	virtual ~FaceInterpolator();


	/**
	*
	*
	*/

	void FaceInterpolator::DeallocKeyFrames();

	/**
	*
	*
	*/

	void FaceInterpolator::InitKeyFrames();


	/**
	*
	*
	* @return 
	* @param  fapnum
	* @param  keyframenumber
	* @param  neutral
	*/

	keyframe *FaceInterpolator::InsertKeyFrame(int fapnum,int keyframenumber,int neutral);

	/**
	*
	*
	* @param  fapnum
	* @param  frame
	* @param  value
	*/

	void FaceInterpolator::InsertKeyFrame2(int fapnum,int frame,float value);


	/**
	*
	*
	* @param fapvector
	* @param eyes_fapmask
	* @param eyes_faps
	* @param fapmask
	* @param faps
	*/

	void FaceInterpolator::InterpolateEyeKeyFrames(std::vector<FAPFrame> *fapvector,int **eyes_fapmask,int **eyes_faps/*,int **fapmask,int **faps*/);

	/**
	*
	*
	* @param fapvector
	* @param eyelids_fapmask
	* @param eyelids_faps
	* @param fapmask
	* @param faps
	*/

	void FaceInterpolator::InterpolateEyelidKeyFrames(std::vector<FAPFrame> *fapvector,int **eyelids_fapmask,int **eyelids_faps/*,int **fapmask,int **faps*/);

	/**
	*
	*
	* @param fapvector
	* @param *faps
	* @param *fapmask
	* @param  float power
	* @param  float fluidity
	*/

	void FaceInterpolator::InterpolateFaps_NoHead(std::vector<FAPFrame> *fapvector,/*int **faps,int **fapmask,*/ float power, float fluidity);

	/**
	*
	*
	* @param fapvector
	* @param *faps
	* @param *fapmask
	* @param *faps_head_expr
	* @param *fapmask_head_expr
	* @param *faps_head_ns
	* @param *fapmask_head_ns
	* @param  float power
	* @param  float fluidity
	*/

	void FaceInterpolator::InterpolateHead(std::vector<FAPFrame> *fapvector,/*int **faps,int **fapmask,*/ int **faps_head_expr,int **fapmask_head_expr, int **faps_head_ns,int **fapmask_head_ns, float power, float fluidity);

	/**
	*
	*
	*/

	void FaceInterpolator::PrintKeyframes();

	/**
	*
	*
	* @return 
	* @param  start_sec
	* @param  secs
	*/

	int CheckForFreeHead(float start_sec,float secs);

	/* OLD and for the very old feedback
	void FaceInterpolator::Interpolate(int **faps,int **fapmask,int fapnum,int k1, int k2);
	*/

private:

	/**
	*
	*
	* @param  fapnum
	* @param  k1
	* @param  int k2
	* @param *eyes_fapmask
	* @param *eyes_faps
	*/

	void FaceInterpolator::eyes_interpolate(int fapnum,int k1, int k2,int **eyes_fapmask,int **eyes_faps);

	/**
	*
	*
	* @param  fapnum
	* @param  k1
	* @param  int k2
	* @param *eyelids_fapmask
	* @param *eyelids_faps
	*/

	void FaceInterpolator::eyelids_interpolate(int fapnum,int k1, int k2,int **eyelids_fapmask,int **eyelids_faps);
	int FramesTotalNumber;
	keyframe *keyframes_first[KEY_PARAMS];
	keyframe *keyframes_last[KEY_PARAMS];

	TCBInterpolator *KeyFrameInt;
	EngineParameter *FLD;
	EngineParameter *PWR;
	TCBInterpolator headinterpolator_x;
	TCBInterpolator headinterpolator_y;
	TCBInterpolator headinterpolator_z;
};

