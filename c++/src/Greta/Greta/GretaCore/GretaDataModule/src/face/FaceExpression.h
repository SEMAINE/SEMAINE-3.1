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

// FaceExpression.h: interface for the FaceExpression class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include "FAPFrame.h"
#include "IniManager.h"
#include "FaceEngineGlobals.h"
#include "EngineParameter.h"

class FaceExpression  
{
public:
	float offset;
	float apex;
	float onset;
	float intensity;
	float emphasis;
	float end;
	float start;
	std::string instance;
	std::string classname;

	FAPFrameVector animation;
	channel *channels;

private:

	FAPFrameVector frames;

public:

	void rescaleIntensity(float intensity);
	FaceExpression *clone();
	void setOffset (float offset1);
	void setApex (float apex1);
	void setOnset (float onset1);
	void setIntensity (float intensity1);
	void setEmphasis (float emphasis1);
	void setEnd (float end1);
	void setStart (float start1);
	void setInstance (std::string instance1);
	void setClassname (std::string classname1);
	void SetFrames(FAPFrameVector frames1);
	void setAnimation(FAPFrameVector animation1);
	void setChannels(channel *channels1);
	FAPFrameVector GetFrames();

	std::string getInstance ();
	std::string getClassname ();

	float getOffset ();
	float getApex ();
	float getOnset ();
	float getIntensity ();
	float getEmphasis ();
	float getEnd ();
	float getStart ();
	FAPFrameVector getAnimation();

	void CreateAnimation();
	void DetermineOnsetOffset();
	int GetNumberOfFrames();
	FAPFrame* GetFAPFrame(int n);
	int **GetRawFaps(float prob);
	void GetMovement(float prob,int *movementtype,float *movementamplitude,float *movementperiod,float notscaledREP,float notscaledSPC);
	channel* getChannels();
	void Print();
	FaceExpression();
	virtual ~FaceExpression();
};

typedef std::vector<FaceExpression> FaceExpressionVector;
