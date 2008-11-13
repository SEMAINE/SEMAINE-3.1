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

// FaceAnimationGenerator.h: interface for the FaceAnimationGenerator class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4786)

//#include "FaceEngineGlobals.h"
#include "APMLCommunicativeAct.h"
#include "FaceInterpolator.h"
#include "FaceGazeSimulator.h"
#include "FaceExpressionDictionary.h"
#include "Phoneme.h"
#include "TCBInterpolator.h"
#include "EngineParameter.h"
#include "FAPwriter.h"

#include <stdio.h>
#include <vector>
#include <string>

//extern "C" fap_id_list *dictionary;

/**
*
* class :FaceAnimationGenerator
*
*/

class FaceAnimationGenerator  
{
public:


	/**
	*
	* contructor *
	*/

	FaceAnimationGenerator();


	/**
	*
	* contructor *
	* @param e
	* @param first_comm_act
	* @param  bool bml
	*/


	FaceAnimationGenerator(void *e,CommunicativeAct *first_comm_act, bool bml);


	/**
	*
	*
	* @return 
	* @param  fed
	*/

	int setFaceExpressionDictionary(FaceExpressionDictionary* fed);

	FaceExpressionDictionary *FaceDictionary;


	/**
	*
	*
	* @return 
	* @param  fapfilename
	* @param  animationlength
	* @param  number_of_comm_act
	* @param PhoVector
	* @param phonemes
	* @param  speech_delay=0.0f
	*/

	int BuildSpeakerFAPFile(std::string fapfilename,float animationlength,int number_of_comm_act,PhonemeVector *PhoVector,phoneme *phonemes,float speech_delay=0.0f);

	/**
	*
	*
	* @return 
	* @param  animationlength
	* @param phonemes
	*/

	int BuildListenerFAPFile(float animationlength,phoneme *phonemes);
	//	void FeedBack(char *signal,float onset,float apex,float offset,float start_sec);

	/**
	*
	*
	* @return 
	* @param  fapfilename
	*/

	int WriteFapFile(std::string fapfilename);
	//int OldWriteFapFile(std::string fapfilename);

	/**
	*
	*
	*/

	void BuildChannels();


	/**
	*
	*
	* @return 
	* @param  number_of_comm_act
	* @param phonemes
	*/

	int DrawExpressionsKeyframes(int number_of_comm_act,phoneme *phonemes);

	/**
	*
	*
	* @return 
	* @param comm_act1
	* @param  number_of_comm_act
	*/

	int ScanForConflicts(CommunicativeAct *comm_act1,int number_of_comm_act);

	/**
	*
	*
	* @return 
	* @param comm_act1
	* @param comm_act2
	*/

	int CommunicativeActsOverlap(CommunicativeAct *comm_act1,CommunicativeAct *comm_act2);

	/**
	*
	*
	* @param comm_act
	* @param  number_of_comm_act
	* @param phonemes
	*/

	void ResolveConflicts(CommunicativeAct *comm_act,int number_of_comm_act,phoneme *phonemes);

	/**
	*
	*
	* @param comm_act
	* @param first_comm_act
	*/

	void DrawSingleExpr(CommunicativeAct *comm_act, CommunicativeAct *first_comm_act);

	/**
	*
	*
	* @param comm_act
	* @param  start_time
	* @param  dur_time
	* @param  joint[]
	*/

	void DrawFacialDisplay(CommunicativeAct *comm_act,float start_time,float dur_time,int joint[]);

	/**
	*
	*
	* @param comm_act
	* @param  number_of_comm_act
	*/

	void ScanForOverAction(CommunicativeAct *comm_act,int number_of_comm_act);

	/**
	*
	*
	* @return 
	* @param comm_act
	* @param comm_act2
	*/

	int OverActionOnChannel(CommunicativeAct *comm_act,CommunicativeAct *comm_act2);


	/**
	*
	*
	* @param comm_act
	*/

	void DrawHeadNodsAndShakes(CommunicativeAct *comm_act);

	/**
	*
	*
	* @param  deg1
	* @param  deg2
	* @param  deg3
	* @param  duration
	* @param  starttime
	*/

	void head_move(float deg1,float deg2,float deg3,float duration,float starttime);

	/**
	*
	*
	* @param  deg1
	* @param  deg2
	* @param  cycle_secs
	* @param  secs
	* @param  start_sec
	*/

	void head_nod(float deg1,float deg2,float cycle_secs,float secs,float start_sec);

	/**
	*
	*
	* @param  deg1
	* @param  deg2
	* @param  cycle_secs
	* @param  secs
	* @param  start_sec
	*/

	void head_shake(float deg1,float deg2,float cycle_secs,float secs,float start_sec);

	//keyframe *InsertKeyFrame(int fapnum,int keyframe,int neutral);

	/**
	*
	*
	* @param  frameind
	*/

	void bioblink(int frameind);

	/**
	*
	*
	*/

	void ApplyBioBlink();

	//channel* get_channels_id(char *id);

	/**
	*
	*
	* @return 
	* @param comm_act
	* @param comm_act2
	*/

	int ConflictOnChannel(CommunicativeAct *comm_act,CommunicativeAct *comm_act2);

	/**
	*
	*
	* @return 
	* @param comm_act
	* @param  start_time
	* @param  end_time
	*/

	int CommActAroundRange(CommunicativeAct *comm_act,float start_time,float end_time);

	/**
	*
	*
	* @return 
	*/

	int InitializeFaps();

	/**
	*
	*
	*/

	void Deallocate();

	/**
	*
	*
	* @return 
	*/

	std::vector<FAPFrame> *GetAnimation()
	{
		return fapframevector;
	};


	/**
	*
	*
	* @return 
	*/

	std::vector<FAPFrame> *GetAnimationWithoutNoise()
	{
		return fapframevectorwithoutnoise;
	};

	//Elisabetta settembre 2005

	/**
	*
	*
	* @param  secs
	* @param  start_sec
	*/

	void LookAt(float secs,float start_sec);

	/**
	*
	*
	* @param  secs
	* @param  start_sec
	*/

	void LookAway(float secs,float start_sec);

	/**
	*
	*
	* @param  deg1
	* @param  int deg2
	* @param  int k
	*/

	void eyes_key(int deg1, int deg2, int k);

	/**
	*
	*
	* @param  k
	* @param eye_roll
	* @param eye_pitch
	* @param  float sec
	*/

	void eyes_position(int k, int *eye_roll, int *eye_pitch, float sec);
	//void eyes_away_key(int deg1, int deg2, int k);

	//void InitKeyFrames();

	/**
	*
	*
	* @param  speed
	*/

	void AddNoise(int speed);

	/**
	*
	*
	*/

	void dealloc_tables();

	/**
	*
	*
	* @return 
	* @param  x
	*/

	float PerlinNoise_1D(float x);

	/**
	*
	*
	* @return 
	* @param  x
	*/

	float InterpolatedNoise1(float x);

	/**
	*
	*
	* @return 
	* @param  x
	*/

	float SmoothedNoise1(float x);

	/**
	*
	*
	* @return 
	* @param  x
	*/

	float Noise(int x);

	/**
	*
	*
	* @return 
	* @param  a
	* @param  b
	* @param  x
	*/

	float InterpolateNoise(float a,float b,float x);

	/**
	*
	*
	*/

	void apply_noise();

	/**
	*
	*
	* @param  k1
	* @param  int k2
	*/

	void interpolate_noise(int k1, int k2);

	/**
	*
	*
	*/

	void init_tables();

	/**
	*
	*
	* @param comm_act
	* @param  start_time
	* @param  end_time
	* @param  number_of_expressions
	*/

	void DebugConflicts(CommunicativeAct *comm_act,float start_time,float end_time,int number_of_expressions);

	/**
	*
	* destructor 
	*/

	virtual ~FaceAnimationGenerator();



	bool bml;

	FaceGazeSimulator *GazeSimulator;

	//first frame of animation (if different from a neutral one)
	bool first_frame_flag;
	FAPFrame first_frame;

protected:

	EngineParameter *OAC;
	EngineParameter *SPC;
	EngineParameter *REP;
	EngineParameter *PWR;
	EngineParameter *FLD;

	FaceInterpolator *FaceInterp;

	//flag to check if we are computing apml or bml file (to disactivate the gaze)

	//	keyframe *keyframes_first[KEY_PARAMS];
	//	keyframe *keyframes_last[KEY_PARAMS];
	int FramesTotalNumber;
	//int **fapmask;
	//int **faps;

	std::vector<FAPFrame> *fapframevector;
	std::vector<FAPFrame> *fapframevectorwithoutnoise;

	float **head_noise_table;
	float **eyes_noise_table;

	//Elisabetta  dicembre 2005
	int **faps_head;
	int **fapmask_head;
	int **faps_head_ns;
	int **fapmask_head_ns;
	//
	int *head_faps[2];
	int *head_fapmask[2];
	int *eyes_faps[4];
	int *eyes_fapmask[4];

	//Elisabetta 10-10-2005
	//fore the eyelids
	int *eyelids_faps[4];
	int *eyelids_fapmask[4];

private:


	/**
	*
	*
	* @param  n
	*/

	void CutFAPs(int n);

	void *engine;
	CommunicativeAct *first_comm_act;
	CommunicativeAct *help_comm_act;

	//Elisabetta  dicembre 2005

	float start_away_time;
	float total_away_time;
	float last_deg;
	float last_deg2;
	float eyes_pitch;
	float eyes_roll;

	int EYES_SPEED;
	int last_frame_eye;

	/*	float activation;
	float spatial;
	float fluidity;
	float power;
	float repetitivity;*/

	bool allocatedstructures;

	TCBInterpolator *KeyFrameInt;


	/**
	*
	*
	* @param other_comm_act
	* @param  position
	*/

	void	maskrefresh(CommunicativeAct *other_comm_act,int position);


	int *joinmask[5];
};
