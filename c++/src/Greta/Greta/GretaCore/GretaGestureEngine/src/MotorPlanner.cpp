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

// MotorPlanner.cpp: implementation of the MotorPlanner class.
//
//////////////////////////////////////////////////////////////////////
#include "MotorPlanner.h"
#include "QuatInterpolator.h"
#include "EulerInterpolator.h"
#include "PathInterpolator.h"
#include "PerlinNoise.h"
#include "IniManager.h"
#include "BAPwriter.h"
#include "BAPconverter.h"

#include <crtdbg.h>

extern IniManager inimanager;

BAPConverter GestureGlobalBAPconverter;

/**@#-*/
using namespace GestureSpace;
/**@#+*/
extern FILE* gesture_log;


#define OUTPUT
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MotorPlanner::MotorPlanner()
{
	mInitialized=false;
	output=false;
}
//////////////////////////////////////////////////////////////////////
MotorPlanner::~MotorPlanner()
{

}

//////////////////////////////////////////////////////////////////////

void MotorPlanner::Init(GestureVector *pGestureVector, int sframe, int eframe)
{
	BAPFrame* prev_pBAPf = 0;
	int prev_frame[2] = {0,0};
	mStartFrame=sframe; mEndFrame=eframe;
	GestureKinematics GK;
#ifdef OUTPUT
		fprintf(gesture_log,"\n////////////////////////////////////\nMotorPlanner::Init : \n");
#endif
	
	// ITERATE OVER GESTURE VECTOR
#ifdef OUTPUT
		fprintf(gesture_log,". Size of GestureVector: %d\n",pGestureVector->size());
#endif
	int counter=0;
	
	GestureVector::iterator gestureIter;	
	for(gestureIter=pGestureVector->begin(); gestureIter!=pGestureVector->end(); gestureIter++) {
#ifdef OUTPUT
		
		fprintf(gesture_log,". Gesture: %s St: %f Fn: %f Sk: %f D: %f side: %d Addr: %d\n" 
				,(*gestureIter)->GetDescription().c_str()
				,(*gestureIter)->GetStartTime()
				,(*gestureIter)->GetFinishTime()
				,(*gestureIter)->GetStrokeTime()
				,(*gestureIter)->GetDuration()
				,(*gestureIter)->GetSide()
				,(*gestureIter));

#endif
			
		// FOR EACH GESTURE: ITERATE OVER GesturePhases
		GesturePhaseVector* pGesturePhaseVector = &((*gestureIter)->phases);
		GesturePhaseVector::iterator phaseIter;
		
#ifdef OUTPUT
		fprintf(gesture_log,". Size of GesturePhaseVector: %d\n",pGesturePhaseVector->size());
#endif
		SideType side = (*gestureIter)->GetSide();
		GesturePhase * prev_phase = NULL;
		for(phaseIter=pGesturePhaseVector->begin(); phaseIter!=pGesturePhaseVector->end(); phaseIter++) {
			counter++;
			// SET SIDE FOR GESTURE PHASE (TAKE OVER FROM GESTURE)
			(*phaseIter)->SetSide(side);
			
			// MAKE ALL FRAMES CONCRETE - removed june 2004; concretize now internally called in GesturePhase::GetAngles
				
			// FRAME TIME CALCULATION
			/*float gesture_start=(*gestureIter)->GetStartTime();
			float gesture_end=(*gestureIter)->GetFinishTime();
			float duration=gesture_end-gesture_start;*/				//IT'S NEVER USED ! and why in this loop ?
			int frametime=int(GLOBAL_FPS*((*phaseIter)->GetAbsTime()));
			
#ifdef OUTPUT
			fprintf(gesture_log,". GesturePhaseTime=%d AbsTime=%f\n"
				,frametime,(*phaseIter)->GetAbsTime());
#endif
			
			BAPFrame* pBAPf = new BAPFrame(&GestureGlobalBAPconverter,frametime);

						// COPY PREVIOUS ANGLES WITHOUT SETTING MASK:
			// NEEDED FOR PROPER WRIST ORIENTATION CALCULATION
			if (prev_pBAPf!=0) {
				pBAPf->CopyAnglesNoMask(prev_pBAPf);
			}
			
			// GET BAP ANGLES
			(*phaseIter)->GetAngles(*pBAPf);
			
			if(prev_phase!=NULL && prev_phase->HasTransition()){
				prev_phase->GetCurvedTransition()->setEndFrame(frametime);
				prev_phase->GetCurvedTransition()->setEndPosition((*phaseIter)->GetPArm()->GetCoordinates());
				//ajout des bapframe de la transition precedante sur intervalle relatif ]0,1[
				int nbFrame = frametime-prev_frame[side];
				for(int i=1; i<nbFrame; i++){
					BAPFrame* additiveBAPf = new BAPFrame(&GestureGlobalBAPconverter,frametime-nbFrame+i);
					additiveBAPf->CopyAnglesNoMask(prev_pBAPf); //prev_pBAPf cannot be NULL
					GK.IkSolveByPositionAndWriteInBAP(prev_phase->GetCurvedTransition()->getPosition(i,side),side,*additiveBAPf);

					AddKeyFrames(*gestureIter, prev_phase, additiveBAPf, prev_frame);
					prev_frame[side]=frametime-nbFrame+i;
					prev_pBAPf = additiveBAPf;
				}
				if((*phaseIter)->HasTransition()){
					(*phaseIter)->GetCurvedTransition()->setRealStartPosition(prev_phase->GetCurvedTransition()->getPosition(nbFrame,side),side);
					(*phaseIter)->GetCurvedTransition()->setStartFrame(frametime);
				}
				GK.IkSolveByPositionAndWriteInBAP(prev_phase->GetCurvedTransition()->getPosition(nbFrame,side),side,*pBAPf);
			}
			else{
				if((*phaseIter)->HasTransition()){
					(*phaseIter)->GetCurvedTransition()->setStartFrame(frametime);
					(*phaseIter)->GetCurvedTransition()->setStartPosition((*phaseIter)->GetPArm()->GetCoordinates());
					GK.IkSolveByPositionAndWriteInBAP((*phaseIter)->GetCurvedTransition()->getPosition(0,side),side,*pBAPf);
				}
			}
			AddKeyFrames(*gestureIter, *phaseIter, pBAPf, prev_frame);
			prev_frame[side]=frametime;
			prev_pBAPf = pBAPf;
			prev_phase = *phaseIter;
		}
	}

	mInitialized=true;
}

void MotorPlanner::AddKeyFrames(Gesture* gesture, GesturePhase * phase, BAPFrame* pBAPf, int * prev_frame){
	// GLOBAL EXPRESSIVITY CONTROL: FLUIDITY AND POWER
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	float fluidity=gesture->GetExpressivityFLD();
	//fluidity=inimanager.GetValueFloat("ENGINE_EXPR_FLD");
	//GlobalEngineCfg->Engine_Expr_Flt;
	//float power=inimanager.GetValueFloat("ENGINE_EXPR_PWR");
	float power=phase->GetPWR();
	//GlobalEngineCfg->Engine_Expr_Pwr;
	pBAPf->TCBParam[Continuity]=fluidity;
	pBAPf->TCBParam[Tension]=power;
	pBAPf->TCBParam[Bias]=power;
#ifdef OUTPUT
	fprintf(gesture_log,"EXPRESSIVITY: setting fluidity to %2.2f, power to %2.2f\n",fluidity,power);
#endif
#endif
	// HACK: rest gesture
	if ((gesture->GetDescription()).compare(0,5,"REST=")==0) {
		//cout<< (*gestureIter)->GetDescription()<<" Adjusting TCB params"<<endl;
		pBAPf->TCBParam[Continuity]=-0.8f;
		pBAPf->TCBParam[Tension]=0.f;
	}
	
	// ENFORCE JOINT ROTATION LIMITS - NOT IMPLEMENTED YET
	//ApplyJointLimits(*pBAPf);
	
	// FOLLOW-THROUGH: ADJUST WRIST TIMING RELATIVE TO FINGER TIMING
	FollowThrough(pBAPf,phase,gesture->GetSide(), prev_frame[gesture->GetSide()]);
		
	//prev_frame[gesture->GetSide()]=frametime;
	//prev_pBAPf = pBAPf;

	// ENQUEUE KEY IN ITS RESPECTIVE BAP VECTOR:
	if (gesture->GetSide() == r)
		mpRightArmKeys.push_back(pBAPf);
	else
		mpLeftArmKeys.push_back(pBAPf);
}
//////////////////////////////////////////////////////////////////////
// THIS IS THE ROUTINE CURRENTLY USED
bool MotorPlanner::InitSplines2(AbstractInterpolator &rInterpolator, BAPFrameVector &rArmKeys, SideType side, DOFGroup group)
{

	const int min_keys = 4; //minimum keys needed for interpolation
	BAPFrameVector::iterator iter;
	BAPFrameVector arm_keys;
	BAPFrameVector wrist_keys;
	BAPFrameVector finger_keys;
	
	arm_keys.clear(); 
	wrist_keys.clear();
	finger_keys.clear();
	
	for(iter=rArmKeys.begin(); iter!=rArmKeys.end(); iter++)
	{
		// IF SHOULDER BAPS ARE SET, ENQUEUE CURRENT BAP IN SHOULDER VECTOR
		if ((*iter)->IsDefined(GROUP_SHOULDER, side)||(*iter)->IsDefined(GROUP_ELBOW, side))
		{
			arm_keys.push_back(*iter);
		}


		// IF WRIST BAPS ARE SET, ENQUEUE CURRENT BAP FRAME IN WRIST VECTOR
		if ((*iter)->IsDefined(GROUP_WRIST, side))
		{
			wrist_keys.push_back(*iter);
		}

		// IF FINGER BAPS ARE SET, ENQUEUE CURRENT BAP FRAME IN FINGER VECTOR
		if ((*iter)->IsDefined(GROUP_FINGERS, side))
		{
			finger_keys.push_back(*iter);
		}
	}
	
	// DEBUG OUTPUT
#ifdef OUTPUT
		fprintf(gesture_log,". Total Keys:  Arm: %d; Wrist: %d Fingers: %d\n"
			,arm_keys.size(),wrist_keys.size(),finger_keys.size());
#endif
	
		if(group==ARM_DOFs) {
			UpToFour(&arm_keys);
			if(arm_keys.size() >= min_keys) {
				rInterpolator.Init(arm_keys, group);
				return true;
			}
			else
			{
				//printf("GestureEngine: warning, interpolation failed (only %d keyframes)\n",arm_keys.size());
				return false;
			}
		}

		if(group==WRIST_DOFs) {
			UpToFour(&wrist_keys);
			if(wrist_keys.size() >= min_keys) {
				rInterpolator.Init(wrist_keys, group);
				return true;
			}
			else
			{
				//printf("GestureEngine: warning, interpolation failed (only %d keyframes)\n",wrist_keys.size());
				return false;
			}
		}
		if(group==FINGER_DOFs) {
			UpToFour(&finger_keys);
			if(finger_keys.size() >= min_keys) {
				rInterpolator.Init(finger_keys, group);
				return true;
			}
			else
			{
				//printf("GestureEngine: warning, interpolation failed (only %d keyframes)\n",finger_keys.size());
				return false;
			}
		}
		return false;
}

//////////////////////////////////////////////////////////////////////

void MotorPlanner::FollowThrough(BAPFrame* pBFrame, GesturePhase* pGFrame, SideType side, int prevFrame)
{
	
	if (pBFrame->IsDefined(GROUP_SHOULDER, side) &&
		pBFrame->IsDefined(GROUP_ELBOW, side) && 
		pBFrame->IsDefined(GROUP_FINGERS, side)) {
		
		// COMPUTE OFFSET TIME HERE: [0..10] FRAMES
		int frame_offset;
		int this_frame=pBFrame->GetFrameNumber();
		if (prevFrame >= 0) {
			int min_offset = 0; int max_offset = 10;
			frame_offset=int(0.2*float(this_frame-prevFrame));
			if (frame_offset<min_offset) frame_offset=min_offset;			
			if (frame_offset>max_offset) frame_offset=max_offset;			
		} else {
			frame_offset=0;
		}

#ifdef OUTPUT
			fprintf(gesture_log,". MotorPlanner::FollowThrough : Calculated frame offset: %d\n",frame_offset);
#endif

		// ADJUSTMENT FRAME IS EARLIER AND HAS EVERYTHING BUT FINGERS
		// NORMAL FRAME ONLY FINGERS OR COPY KEYS??
		BAPFrame* pBAPAdjust = new BAPFrame(&GestureGlobalBAPconverter);
		*pBAPAdjust = *pBFrame;
		
		pBAPAdjust->SetFrameNumber(this_frame-frame_offset);
		
		pBAPAdjust->SetGroupMask(GROUP_FINGERS,side, 0);
		pBFrame->SetGroupMask(GROUP_SHOULDER,side, 0);
		pBFrame->SetGroupMask(GROUP_ELBOW,side, 0);
		pBFrame->SetGroupMask(GROUP_WRIST,side, 0);
		pBFrame->SetGroupMask(GROUP_FINGERS,side, 1);
		if (side == l)
			mpLeftArmKeys.push_back(pBAPAdjust);
		else
			mpRightArmKeys.push_back(pBAPAdjust);				
	
	} else {;}
	
}

//////////////////////////////////////////////////////////////////////
BAPFrame* MotorPlanner::GetFrame(int number)
{
	assert(number>=0);
	if (number >= (int)mpAnimationFrames.size())
	{
		number=mpAnimationFrames.size()-1;
	}
	return mpAnimationFrames[number];
}

BAPFrameVector* MotorPlanner::GetAnimation()
{
	return &mpAnimationFrames;
}

//////////////////////////////////////////////////////////////////////
// WRITEANIMATION
void MotorPlanner::WriteAnimation(string argFileName)
{
    //ADDME: This function is almost a copy of BAPWriter::WriteToFile ...
	BAPWriter bw;
	bw.WriteToFile(&this->mpAnimationFrames,argFileName);
}

void MotorPlanner::AddBAPFrame(BAPFrame *bf, int framenumber)
{
	BAPFrameVector::iterator iter;
	bf->SetFrameNumber(framenumber);

	if(bf->IsDefined(GROUP_FINGERS,r)||
		bf->IsDefined(GROUP_WRIST,r)||
		bf->IsDefined(GROUP_ELBOW,r)||
		bf->IsDefined(GROUP_SHOULDER,r))
	{
		bool inserted;
		iter=this->mpRightArmKeys.begin();
		inserted=false;
		while((iter!=this->mpRightArmKeys.end())&&(inserted==false))
		{
			
			if(framenumber==(*iter)->GetFrameNumber())
			{
				printf("fatal error adding bap frame!\n");
				assert(false);
			}
			if(framenumber<=(*iter)->GetFrameNumber())
			{
				this->mpRightArmKeys.insert(iter,bf);
				inserted=true;
			}		
		iter++;
		}
		if(inserted==false)
		{
			this->mpRightArmKeys.push_back(bf);
		}
	}

	if(bf->IsDefined(GROUP_FINGERS,l)||
		bf->IsDefined(GROUP_WRIST,l)||
		bf->IsDefined(GROUP_ELBOW,l)||
		bf->IsDefined(GROUP_SHOULDER,l))
	{
		bool inserted;
		iter=this->mpLeftArmKeys.begin();
		inserted=false;
		while((iter!=this->mpLeftArmKeys.end())&&(inserted==false))
		{
			if(framenumber==(*iter)->GetFrameNumber())
			{
				printf("fatal error adding bap frame!\n");
				assert(false);
			}
			if(framenumber<=(*iter)->GetFrameNumber())
			{
				this->mpLeftArmKeys.insert(iter,bf);
				inserted=true;
			}
		}
		if(inserted==false)
		{
			this->mpLeftArmKeys.push_back(bf);
		}
	}
}

BAPFrame *MotorPlanner::GetLastBAPKeyFrame()
{
	BAPFrame *lastleft;
	BAPFrame *lastright;

	if(mpLeftArmKeys.empty())
	{
		lastleft=0;
	}
	else
	{
		lastleft=*(mpLeftArmKeys.end()-1);
	}

	if(mpRightArmKeys.empty())
	{
		lastright=0;
	}
	else
	{
		lastright=*(mpRightArmKeys.end()-1);
	}

	if((lastleft==0)&&(lastright==0))
		return 0;

	if((lastleft!=0)&&(lastright==0))
		return lastleft;

	if((lastleft==0)&&(lastright!=0))
		return lastright;

	lastright->AddBAPFrame(lastleft);

	if(lastright->GetFrameNumber()<lastleft->GetFrameNumber())
		lastright->SetFrameNumber(lastleft->GetFrameNumber());

	return lastright;
}

void MotorPlanner::GetCopyLastBAPKeyFrame(BAPFrame *returnbf)
{
	BAPFrame *lastframe;
	lastframe=GetLastBAPKeyFrame();
	returnbf->CopyAngles(lastframe);
}

BAPFrame *MotorPlanner::GetLastBAPFrame()
{
	if(mpAnimationFrames.empty()==false)
	{
		return mpAnimationFrames[mpAnimationFrames.size()-1];
	}
}

void MotorPlanner::GetCopyLastBAPFrame(BAPFrame *returnbf)
{
	BAPFrame *lastframe;
	lastframe=GetLastBAPFrame();
	returnbf->CopyAngles(lastframe);
}

//////////////////////////////////////////////////////////////////////
void MotorPlanner::ApplyTorsoPerlinNoise(BAPFrame &f, int fnum) {
	static	PerlinNoise perlinTilt;
	static	PerlinNoise perlinRoll;
	static	PerlinNoise perlinTorsion;
	static  PerlinNoise perlinScap;
	static	float tiltStep  = 1.f/85.f; //frequency
	static	float rollStep  = 1.f/80.f; //frequency
	static	float torsionStep  = 1.f/95.f; //frequency
	static  float tiltFactor= 3.0f; //maximum displacement in degrees
	static	float rollFactor= 2.5f; //2.5 maximum displacement in degrees
	static	float torsionFactor= 3.f; //maximum displacement in degrees
	static  float scapFactor= 1.5f;//1.5
	static  float scapStep=1.f/60.f;

	// apply perlin noise onto torso:
	// NOTE: depends on which BAP set we are using;
	// tested only with USEOLDBAPS
	
#ifdef USEOLDBAPS
	f.SetBAP(l3roll,degrees,perlinRoll.noise1(fnum*rollStep)*rollFactor);	
	f.SetBAP(l5tilt,degrees,perlinTilt.noise1(fnum*tiltStep)*tiltFactor);
	f.SetBAP(t10torsion,degrees,perlinTorsion.noise1(fnum*torsionStep)*torsionFactor);
	f.SetBAP(l_scap_abduct,degrees,perlinScap.noise1(fnum*scapStep)*scapFactor);
	f.SetBAP(r_scap_abduct,degrees,perlinScap.noise1((fnum-50)*scapStep)*scapFactor);
	f.SetBAP(c4roll,degrees,perlinTilt.noise1(fnum*tiltStep)*2.f);
	f.SetBAP(c4tilt,degrees,perlinRoll.noise1(fnum*tiltStep)*3.f);
#else
	f.SetBAP(vl3_roll,degrees,perlinRoll.noise1(fnum*rollStep)*rollFactor);	
	f.SetBAP(vl5_tilt,degrees,perlinTilt.noise1(fnum*tiltStep)*tiltFactor);
	f.SetBAP(vt10_torsion,degrees,perlinTorsion.noise1(fnum*torsionStep)*torsionFactor);
	f.SetBAP(l_acromioclavicular_abduct,degrees,perlinScap.noise1(fnum*scapStep)*scapFactor);
	f.SetBAP(r_acromioclavicular_abduct,degrees,perlinScap.noise1((fnum-50)*scapStep)*scapFactor);
	f.SetBAP(vc4_roll,degrees,perlinTilt.noise1(fnum*tiltStep)*2.f);
	f.SetBAP(vc4_tilt,degrees,perlinRoll.noise1(fnum*tiltStep)*3.f);
#endif
}


/////////////////////////////////////////////////////////////////////
// REMOVED ROUTINES BELOW
/////////////////////////////////////////////////////////////////////
void MotorPlanner::ApplyJointLimits(BAPFrame &rFrame)
{
	/* 
	2004-05-19 bjoern:
	we could check joint angles for every frame here
	using the JointLimitWrapper class.
	for now, we are only checking the wrist angles
	of key frames in class WristAngleSkeleton.
	*/
}

//////////////////////////////////////////////////////////////////////
void MotorPlanner::GenerateAnimation()
{
	assert(mInitialized);
	
	// bool variables hold whether a specific 
	// DOF group is animated at all or not. if not, we don't call
	// the interpolator
	bool hasLArm, hasRArm, 
			hasLWrist, hasRWrist,
			hasLFingers, hasRFingers;

	//std::sort(mpRightArmKeys.begin(),mpRightArmKeys.end());
	//std::sort(mpLeftArmKeys.begin(),mpLeftArmKeys.end());
	
	// create one interpolator for each dof group in each arm
	AbstractInterpolator *lArmInterp, *lWristInterp, *lFingerInterp,
						 *rArmInterp, *rWristInterp, *rFingerInterp;
	
	// Pick interpolator class based on configuration file;
	// throw exception if invalid type is specified
	if(inimanager.GetValueInt("ENGINE_ARM_INTERP_TYPE")==EulerInterpType) {
		lArmInterp = new EulerInterpolator(l);
		rArmInterp = new EulerInterpolator(r);
	} else if(inimanager.GetValueInt("ENGINE_ARM_INTERP_TYPE")==QuatInterpType) {
		lArmInterp = new QuatInterpolator(l);
		rArmInterp = new QuatInterpolator(r);
	} else if(inimanager.GetValueInt("ENGINE_ARM_INTERP_TYPE")==PathInterpType) {
		lArmInterp = new PathInterpolator(l);
		rArmInterp = new PathInterpolator(r);
	} else assert(false);

	if(inimanager.GetValueInt("ENGINE_WRIST_ORIENTATION_INTERP_TYPE")==EulerInterpType) {
		lWristInterp = new EulerInterpolator(l);
		rWristInterp = new EulerInterpolator(r);
	} else if(inimanager.GetValueInt("ENGINE_WRIST_ORIENTATION_INTERP_TYPE")==QuatInterpType) {
		lWristInterp = new QuatInterpolator(l);
		rWristInterp = new QuatInterpolator(r);
	} else assert(false);

	if(inimanager.GetValueInt("ENGINE_FINGER_INTERP_TYPE")==EulerInterpType) {
		lFingerInterp = new EulerInterpolator(l);
		rFingerInterp = new EulerInterpolator(r);
	} else assert(false);
	

	// INITIALIZE INTERPOLATORS
	hasLArm    = InitSplines2(*lArmInterp,mpLeftArmKeys,l,ARM_DOFs);
	hasLWrist  = InitSplines2(*lWristInterp,mpLeftArmKeys,l,WRIST_DOFs);
	hasLFingers= InitSplines2(*lFingerInterp,mpLeftArmKeys,l,FINGER_DOFs);

	hasRArm    = InitSplines2(*rArmInterp,mpRightArmKeys,r,ARM_DOFs);
	hasRWrist  = InitSplines2(*rWristInterp,mpRightArmKeys,r,WRIST_DOFs);
	hasRFingers= InitSplines2(*rFingerInterp,mpRightArmKeys,r,FINGER_DOFs);

	// ITERATE THROUGH OUTPUT FRAMES: GET INTERPOLATED ANGLES AND WRITE THEM TO FILE
	for(int frame=mStartFrame; frame<mEndFrame; frame++)
	{

		BAPFrame* AnimationFrame = new BAPFrame(&GestureGlobalBAPconverter,frame);
		
		if(hasLArm)     lArmInterp->GetFrame(*AnimationFrame,frame,ARM_DOFs);
		if(hasLWrist)   lWristInterp->GetFrame(*AnimationFrame,frame,WRIST_DOFs);
		if(hasLFingers) lFingerInterp->GetFrame(*AnimationFrame,frame,FINGER_DOFs);

		if(hasRArm)     rArmInterp->GetFrame(*AnimationFrame,frame,ARM_DOFs);
		if(hasRWrist)   rWristInterp->GetFrame(*AnimationFrame,frame,WRIST_DOFs);
		if(hasRFingers) rFingerInterp->GetFrame(*AnimationFrame,frame,FINGER_DOFs);
		
		if (inimanager.GetValueInt("ENGINE_USE_PERLIN_TORSO_NOISE")==1)
		{
			ApplyTorsoPerlinNoise(*AnimationFrame,frame);		
		}
		
		mpAnimationFrames.push_back(AnimationFrame);
	} 	
}

void MotorPlanner::UpToFour(BAPFrameVector *frame_vector)
{
	return;

	BAPFrame *AnimationFrame;
	int i;
	const int min_keys = 4; //minimum keys needed for interpolation
	if(frame_vector->size()>=min_keys)
		return;
	if(frame_vector->size()==0)
		return;
	if(frame_vector->size()==1)
	{
		AnimationFrame=(*frame_vector)[0];
		for(i=0;i<3;i++)
		{
			BAPFrame *NewFrame = new BAPFrame(&GestureGlobalBAPconverter,AnimationFrame->GetFrameNumber()-((i+1)*2));
			NewFrame->CopyAngles(AnimationFrame);
			frame_vector->insert(frame_vector->begin(),NewFrame);
		}
	}
	if(frame_vector->size()==2)
	{
		AnimationFrame=(*frame_vector)[0];
		for(i=0;i<2;i++)
		{
			BAPFrame *NewFrame = new BAPFrame(&GestureGlobalBAPconverter,AnimationFrame->GetFrameNumber()-((i+1)*2));
			NewFrame->CopyAngles(AnimationFrame);
			frame_vector->insert(frame_vector->begin(),NewFrame);
		}
	}
	if(frame_vector->size()==3)
	{
		AnimationFrame=(*frame_vector)[0];
		for(i=0;i<1;i++)
		{
			BAPFrame *NewFrame = new BAPFrame(&GestureGlobalBAPconverter,AnimationFrame->GetFrameNumber()-((i+1)*2));
			NewFrame->CopyAngles(AnimationFrame);
			frame_vector->insert(frame_vector->begin(),NewFrame);
		}
	}
}