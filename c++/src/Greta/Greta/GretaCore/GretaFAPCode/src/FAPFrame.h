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

#pragma once
#include <vector>
#include "fap.h"

	/*! \brief structure to hold eyes direction
	*
	* the structure was realized when implementing some pieces
	* of the new face engine; it is not used in normal Greta face engine
	*/

struct eyesdirection{
	bool active;
	float v;
	float h;
};
	/*! \brief structure to hold head direction
	*
	* the structure was realized when implementing some pieces
	* of the new face engine; it is not used in normal Greta face engine
	*/

struct headdirection{
	bool active;
	float v;
	float h;
	float t;
};

	/*! \brief structure to hold head movement
	*
	* the structure was realized when implementing some pieces
	* of the new face engine; it is not used in normal Greta face engine
	*/


struct headmovement{
	bool active;
	std::string type;
	float amplitude;
	float period;
};

	/*! \brief class that represent a FAP frame
	*
	* the class holds a vector of FAPs and enables to do some
	* operations on the vector
	* @see FAP
	* @author Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com
	*/
#ifndef TCBPARAMS
#define TCBPARAMS
	enum TCBParamType {Tension,Continuity,Bias};
#endif

class FAPFrame
{
public:
	/*! \brief obtains a copy of the FAP frame with all FAPs to zero
	*
	* it is used when creating keyframes because to animate you need to go
	* from zero values to activation values; so you sometimes need to
	* get a FAP frame which has the right mask (1s in the right places)
	* but with all the values set to 0
	*/
	FAPFrame *GetCorrespondingZeroFrame();
	/*! \brief stores a given FAP frame into the current
	*
	* is helpful to copy another frame in the current
	*/
	void CopyFrom(FAPFrame *origin);
	/*! \brief merges a given FAP frame into the current
	*
	* is helpful to merge another frame in the current
	*/
	void MergeFrom(FAPFrame *origin);
	/*! \brief merge a FAP value to the current frame
	*
	* given a FAP number and a value it sums the value to the corresponding
	* FAP in the FAP frame
	*/
	void MergeFAP(int num, int value);
	/*! \brief probability of the frame
	*
	*/
	float probability;
	/*! \brief prints the frame on the screen
	*
	*/
	void Print();
	/*! \brief sets FAP value
	*
	* given a FAP number and a value sets the corresponding FAP in the frame
	* to that value
	*/
	int SetFAP(int num,int value,bool activate=true);
	/*! \brief retrieves FAP value
	*
	* given a FAP number it gets the corresponding value
	*/
	int GetFAP(int num);
	int GetMask(int num){return FAPs[num].active;};
	/*! \brief merge a FAP value to the current frame
	*
	* given a FAP number and a value it sums the value to the corresponding
	* FAP in the FAP frame
	*/
	int AddToFAP(int num,int value);
	/*! \brief class constructor
	*
	*/
	FAPFrame(void);
	/*! \brief class destructor
	*
	*/
	~FAPFrame(void);
	/*! \brief array of the frame's FAPs
	*
	*/
	FAP FAPs[NUMBER_OF_FAPS];
	/*! \brief indicates if the frame is a keyframe
	*
	*/
	bool isKeyFrame;
	/*! \brief puts all the FAP masks to 1
	*
	*/
	void ActivateAllFAPs(void);
	/*! \brief puts all the FAP masks to 0
	*
	* given a FAP number and a value it sums the value to the corresponding
	* FAP in the FAP frame
	*/
	void DeactivateAll(void);
	/*! \brief reset all the FAP values to 0 and deactivate all the masks
	*
	*/
	void ResetAllToZero();
	/*! \brief sets all the FAP values to a given value
	*
	*/
	void ActivateAndSetAllFAPsTo(int v);
	/*! \brief saves the frame to a file
	*
	*/
	void SaveToFile(FILE *f,int num);
	std::string WriteFAP(int num);
	void ReadFromBuffer(char *buffer);
//	float time;
	/*! \brief eyes direction for this frame
	*
	*/
	eyesdirection eyesdir;
	/*! \brief head direction for this frame
	*
	*/
	headdirection headdir;
	/*! \brief head movement for this frame
	*
	*/
	headmovement headmov;
	int framenumber;
	float	TCBParam[3];
	float GetTCBParam(TCBParamType which);
	void SetTCBParam(float T, float C, float B);
	void SetTension(float T);
	void SetContinuity(float C);
	void SetBias(float B);
	
	int use_at;
	int use_before;
	int received_at;

	std::string id;

	bool operator<(FAPFrame& a);

	std::string toString();

	FAPFrame clone();

};

typedef std::vector<FAPFrame> FAPFrameVector;

