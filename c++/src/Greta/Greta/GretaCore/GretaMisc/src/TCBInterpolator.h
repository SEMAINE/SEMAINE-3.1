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

// TCBInterpolator.h: interface for the TCBInterpolator class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FAPFrame.h"
#include "KeyPoint.h"
#include <vector>
#include "WildMagic/Wm4TCBSpline2.h"


/**
*
* class :TCBInterpolator
*
*/

class TCBInterpolator  
{
public:

	/**
	* this method 
	* 
	*
	* @param  fanumber
	* @param  &animation
	*/

	void InterpolateFAP(int fanumber,FAPFrameVector &animation);

	/**
	*
	* contructor 
	*
	* @param  T
	* @param  C
	* @param  B
	*/

	TCBInterpolator(float T,float C,float B);

	/**
	*
	* contructor 
	*
	*/

	TCBInterpolator();

	/**
	*
	* destructor 
	*/

	virtual ~TCBInterpolator();

	/**
	* this method 
	* 
	*
	*/

	void Prepare();

	/**
	* this method 
	* 
	*
	*/

	void Interpolate();

	/**
	* this method 
	* 
	*
	* @param  time
	* @param  value
	*/

	void AddPoint(float time,float value);

	/**
	* this method 
	* 
	*
	* @param  time
	* @param  value
	* @param  pT
	* @param  pC
	* @param  pB
	*/

	void AddPoint(float time,float value,float pT,float pC,float pB);

	/**
	* this method 
	* 
	*
	* @param  time
	* @param  value
	*/

	void AddPointNoSort(float time,float value);

	/**
	* this method 
	* 
	*
	* @param  time
	* @param  value
	* @param  pT
	* @param  pC
	* @param  pB
	*/

	void AddPointNoSort(float time,float value,float pT,float pC,float pB);
	std::vector<KeyPoint> points;
	std::vector<float> result;
	Wm4::Vector2f* sTCBpoint;
	float* sT;
	float* sC;
	float* sB;
	float* sTime;
	Wm4::TCBSpline2f *mpTCB2_Swivel;
	float T,C,B;
	bool in_use;

	/**
	* this method 
	* 
	*
	*/

	void Reset();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	float GetNextValue();

	/**
	* this method 
	* 
	*
	* @return 
	* @param  fn
	*/

	float GetFrameN(int fn);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  n
	*/

	float GetNextNValue(int n);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  s
	*/

	float GetAfterMillisecs(float s);

	/**
	* this method 
	* 
	*
	* @return 
	* @param  time
	*/

	float GetAtTime(float time);

	/**
	* this method 
	* 
	*
	* @param  T
	* @param  C
	* @param  float B
	*/

	void SetTCB(float T,float C, float B);
	int internal_counter;
	float fps;
	bool verbose;

private:

	float millisec_accum;
};
