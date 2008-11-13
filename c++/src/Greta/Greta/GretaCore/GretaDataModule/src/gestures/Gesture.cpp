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

// Gesture.cpp: implementation of the Gesture class.
//
//////////////////////////////////////////////////////////////////////
//#include <cassert>

#include "Gesture.h"
#include "IniManager.h"

extern IniManager inimanager;

/**@#-*/
using namespace std ;
using namespace GestureSpace;
/**@#+*/

extern FILE* data_log;
#define OUTPUT

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////

Gesture::Gesture()
{
	phases.clear();
	side		= no_side;
	output		= false;	
	mStroke		= -1.0;
	mDuration	= -1.0;
	mDefDuration= -1.0;
	mHasLoop	= false;
	mHasRepetitions=false;
	//CHRIS MSVC2005 fix 
	//mStartLoop	= NULL;
	//mEndLoop	= NULL;
	mLoopCycles	= -1;
	mClass		= "";
	mDescription= "";
	mInstance	= "";
	expressivitySPC=0;
	expressivityTMP=0;
	expressivityFLD=0;
	expressivityPWR=0;
	mRepetitions.clear();
	phasesAssym.clear();
}
//////////////////////////////////////////////////////////////////////
// Destruction
//////////////////////////////////////////////////////////////////////
Gesture::~Gesture()
{
	/*
	GesturePhaseVector::iterator iter,iter2;
	for(iter=phases.begin();iter!=phases.end();iter++)
	{
		GesturePhase *gp;

		gp=(*iter);

		for(iter2=phases.begin();iter2!=phases.end();iter2++)
		{
			if(((*iter)==(*iter2))&&((*iter)!=0))
				(*iter2)=0;
		}

		if(gp!=0)
			delete gp;

		(*iter)=0;
	}*/
	phases.clear();
	phasesAssym.clear();


}
//////////////////////////////////////////////////////////////////////
void Gesture::SetDescription()
{
	mDescription = mClass + "=" + mInstance;
}
//////////////////////////////////////////////////////////////////////
bool Gesture::operator<(const Gesture &rhs) {
	return mStart<rhs.mStart;
}
//////////////////////////////////////////////////////////////////////
void Gesture::Print()
{
#ifdef OUTPUT
	fprintf(data_log,". GESTURE: %s Side: %d Start: %2.2f Stroke: %2.2f Finish: %2.2f\n",
		mDescription.c_str(),side,mStart,mStroke,mFinish);
#endif

	GesturePhaseVector::iterator iter;
	for (iter=phases.begin();iter!=phases.end();iter++) {
		(*iter)->PrintTimes();
	}
}

void Gesture::PrintOnScreen()
{
#ifdef OUTPUT
	printf(". GESTURE: %s Side: %d Start: %2.2f Stroke: %2.2f Finish: %2.2f\n",
		mDescription.c_str(),side,mStart,mStroke,mFinish);
#endif

	GesturePhaseVector::iterator iter;
	for (iter=phases.begin();iter!=phases.end();iter++) {
		(*iter)->PrintTimesOnScreen();
	}
}

//////////////////////////////////////////////////////////////////////
// ScalePhaseTimes converts the relative timings of the gesture (0 to x)?
// into absolute time values through 2-part linear interpolation
//
void Gesture::ScalePhaseTimes()
{
	//ITERATE OVER PHASE ARRAY UNTIL WE FIND STROKE PHASE
	GesturePhaseVector::iterator stroke_iter;
	bool has_stroke=false;
	for(stroke_iter=phases.begin();stroke_iter!=phases.end(); stroke_iter++) {
		if ( ((*stroke_iter)->GetType() == STROKE) ||
			((*stroke_iter)->GetType() == STROKE_END)) {
			has_stroke=true;
			break;
		}
			
	}
	// IF THERE IS NO STROKE: DO SIMPLE LINEAR SCALING 
	if (!has_stroke || mStroke<=0.0) {

#ifdef OUTPUT
			fprintf(data_log,". Gesture::ScalePhaseTimes - NO stroke found.\n");
#endif
		GesturePhaseVector::iterator nostroke_iter;
		for(nostroke_iter=phases.begin();nostroke_iter!=phases.end(); nostroke_iter++) {
			float rel_t = (*nostroke_iter)->time;
			//DANGER: abs_t can be < 1 here!
			float abs_t = (mFinish-mStart) * rel_t + mStart;
			(*nostroke_iter)->SetAbsTime(abs_t);
			
#ifdef OUTPUT
			fprintf(data_log,". Mapping: %f -> %f\n",rel_t,abs_t);
#endif
		}
	}
	
	// IF THERE IS A STROKE DEFINED: DO 2-PART LINEAR SCALING
	else {
		
		GesturePhaseVector::iterator post_stroke_iter=stroke_iter+1;
		GesturePhaseVector::iterator pre_iter, post_iter;
		float rel_stroke = (*stroke_iter)->time;
		
#ifdef OUTPUT
			fprintf(data_log,". Gesture::ScalePhaseTimes - stroke found at %f\n",rel_stroke);
#endif
		
		for (pre_iter=phases.begin(); pre_iter!=post_stroke_iter; pre_iter++) {
			float rel_t = (*pre_iter)->time;
			float abs_t = float((rel_t / (rel_stroke-0.0)) * (mStroke-mStart) + mStart);
			(*pre_iter)->SetAbsTime(abs_t);
			
#ifdef OUTPUT
				fprintf(data_log,". Pre-Mapping: %f -> %f\n",rel_t,abs_t);
#endif
				
		}
		for (post_iter=post_stroke_iter; post_iter!=phases.end(); post_iter++) {
			float rel_t = (*post_iter)->time;
			float abs_t = float(((rel_t-rel_stroke) / (1.0-rel_stroke)) *
						(mFinish-mStroke) + mStroke);
			(*post_iter)->SetAbsTime(abs_t);
#ifdef OUTPUT
				fprintf(data_log,". Post-Mapping: %f -> %f\n",rel_t,abs_t);
#endif
		}

	}
}


//////////////////////////////////////////////////////////////////////
// COPY CONSTRUCTOR
//////////////////////////////////////////////////////////////////////
Gesture::Gesture(const Gesture &rhs)
{
	mStroke=rhs.mStroke;
	mDuration=rhs.mDuration;
	mDefDuration=rhs.mDefDuration;
	mFinish=rhs.mFinish;
	mClass=rhs.mClass;
	mDescription=rhs.mDescription;
	mInstance=rhs.mInstance;
	output=rhs.output;
	side=rhs.side;
	mStart=rhs.mStart;
	
	mEndLoop=rhs.mEndLoop;
	mHasLoop=rhs.mHasLoop;
	mHasRepetitions=rhs.mHasRepetitions;
	mLoopCycles=rhs.mLoopCycles;
	mStartLoop=rhs.mStartLoop;
	
	GesturePhaseVector::const_iterator iter;
	phases.clear();
	mRepetitions.clear();
	expressivitySPC=rhs.expressivitySPC;
	expressivityTMP=rhs.expressivityTMP;
	expressivityFLD=rhs.expressivityFLD;
	expressivityPWR=rhs.expressivityPWR;
	expressivityREP=rhs.expressivityREP;
	for (iter=rhs.phases.begin();iter!=rhs.phases.end();iter++) {
		GesturePhase *pF= new GesturePhase(**iter);
		pF->SetSPC(this->expressivitySPC);
		pF->SetPWR(this->expressivityPWR);
		phases.push_back(pF);
	}
	FloatVector::const_iterator repetition_iter;
	repetition_iter=(rhs.mRepetitions.begin());
	for (; repetition_iter!=(rhs.mRepetitions.end()); repetition_iter++)
	{
		mRepetitions.push_back(*repetition_iter);
	}
	
	GesturePhaseVector::const_iterator iter2;
	phasesAssym.clear();
	for (iter2=rhs.phasesAssym.begin();iter2!=rhs.phasesAssym.end();iter2++) {
		GesturePhase *pF= new GesturePhase(**iter2);
		pF->SetSPC(this->expressivitySPC);
		pF->SetPWR(this->expressivityPWR);
		phasesAssym.push_back(pF);
	}	
	
}

//////////////////////////////////////////////////////////////////////
//
void Gesture::FindLoopInfo()
{
	mHasLoop=false;
	bool has_start=false, has_end=false;
	GesturePhaseVector::iterator iter;
	for(iter=phases.begin();iter!=phases.end();iter++)
	{
		if ((*iter)->GetLoopType() == LOOP_START) {
			has_start=true;
			mStartLoop=iter;
		}
		else if ((*iter)->GetLoopType() == LOOP_END) {
			has_end=true;
			mEndLoop=iter;
		}
	}
	if (has_start && has_end) {
#ifdef OUTPUT
			fprintf(data_log,". Gesture::FindLoopInfo: Valid loop found.\n");
#endif
		mHasLoop=true;
	} else {
		mHasLoop=false;
		//CHRIS MSVC2005 fix
		//mStartLoop=NULL;
		//mEndLoop=NULL;
	}

}

/////////////////////////////////////////////////////////////////////
// ExpandLoop repeats the "looping" part of a gesture 
// to fill the time interval assigned to the gesture optimally
// NOT SURE WHETHER THIS FEATURE STILL WORKS (June 2004 Bjoern)
void Gesture::ExpandLoop()
{
	FindLoopInfo();
	// CHECK THAT THERE IS ACTUALLY SOMETHING TO DO:
	if (!mHasLoop || (mDuration <= mDefDuration)) {
#ifdef OUTPUT
		fprintf(data_log,". Gesture::ExpandLoop - Nothing to do! Returning.\n");
		fprintf(data_log,"  Duration: %f, DefDuration: %f\n",mDuration,mDefDuration);
#endif
		
		return;
	}
		
	// YES, THERE IS A VALID LOOP TO BE EXPANDED:
#ifdef OUTPUT
	fprintf(data_log,". Gesture::ExpandLoop: \n");
	fprintf(data_log,"  Duration: %f\n",mDuration);
#endif
	
	
	// CALCULATE LOOP DURATION TIME
	float rt_start, rt_last, rt_end, rt_dur, rt_post;
	float last_time;
	rt_start = (*mStartLoop)->GetRelTime();
	rt_last  = (*mEndLoop)->GetRelTime();
	if ((mEndLoop+1) != phases.end()) {
		rt_end   = (*(mEndLoop+1))->GetRelTime();
		
	}
	else { 
		// :KLUDGE: don't assume 1/10th of a sec as default
		rt_end	 = (*mEndLoop)->GetRelTime() + .1f;
	}
	rt_dur=rt_end-rt_start;
	rt_post=rt_end-rt_last;
	int counter=1;
	//NUMBER OF ITERATIONS:
	int iterations = int(floor((mDuration-mDefDuration)/rt_dur));
	
#ifdef OUTPUT
		fprintf(data_log,". Loop Iterations: %d\n",iterations);
#endif
	
	if (iterations <=1) return;
	// save old end
	GesturePhaseVector::iterator old_end;
	old_end=phases.end();
	
	GesturePhaseVector v2;
	v2.clear();

	GesturePhaseVector::iterator li;
	for (int i=1; i<iterations; i++) {
		for (li=mStartLoop; li!=(mEndLoop+1); li++) {
			//(*li)->PrintTimes();
			GesturePhase *pF = new GesturePhase;
			*pF=**li;
			//calculate new rel. phase time;
			last_time=pF->GetRelTime()+counter*rt_dur;
			pF->SetRelTime(last_time);
			// INSERT NEW GESTURE PHASES INTO TEMP VECTOR
			v2.push_back(pF);			
		}
		counter++;
	}
	float add_time= last_time-rt_end+rt_post;
	// ADJUST RELATIVE TIMES FOR POST-LOOP PHASES
	for (li=(mEndLoop+1); li!=old_end; li++) {
		(*li)->SetRelTime((*li)->GetRelTime()+add_time);
	}
	// APPEND NEWLY CREATED PHASES
	for(li=v2.begin();li!=v2.end();li++) {
		phases.push_back(*li);
	}
	// THEN SORT GesturePhaseS (SEE GESTURE SORT)
	sort(phases.begin(),phases.end(),AscGesturePhaseSort());

	// DEBUG:
	for (li=phases.begin();li!=phases.end();li++) {
		(*li)->PrintTimes();
	}
	
	// FINALLY SCALE ALL RELATIVE TIMES BACK TO 0..1
	ScaleRelTimes();
	for (li=phases.begin();li!=phases.end();li++) {
		(*li)->PrintTimes();
	}
}

//////////////////////////////////////////////////////////////////////
void Gesture::ScaleRelTimes()
{
	float low = phases.front()->GetRelTime();
	float high= phases.back()->GetRelTime();
	float new_time;
	GesturePhaseVector::iterator iter;
	for (iter=phases.begin();iter!=phases.end();iter++) {
		new_time=((*iter)->GetRelTime()-low)/(high-low);
		(*iter)->SetRelTime(new_time);
	}
}

//////////////////////////////////////////////////////////////////////
// ExpandRepetitions inserts new Keys into the GesturePhaseVector
// to repeat stroke movements
void Gesture::ExpandRepetitions()
{
	if (mRepetitions.size()==0) return;
	// retraction_ratio defines at what point in time between successive repetitions
	// the retraction key is placed; range [0.0 .. 1.0]
	const float retraction_ratio=0.7f;

	// retraction_mag defined how far back the arm goes from the stroke end back to the
	// stroke start for the repetition
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	//const float retraction_mag=.6f + 0.3f*inimanager.GetValueFloat("ENGINE_EXPR_PWR");
	const float retraction_mag=.6f + 0.3f*this->expressivityPWR;
#else
	//magnitude of first retraction
	const float retraction_mag=0.7f; 
#endif

	
	// FUNCTION OPERATES ON ABSOLUTE TIMES ONLY!
	// THESE ARE IMPLIED TO HAVE BEEN CALCULATED PREVIOUSLY
	// VIA ScalePhaseTimes() OR OTHER METHOD
	
	
	//ITERATE OVER PHASE ARRAY UNTIL WE FIND STROKE PHASE
	GesturePhaseVector::iterator stroke_iter;
	bool has_stroke=false;
	for(stroke_iter=phases.begin();stroke_iter!=phases.end(); stroke_iter++) {
		if ( ((*stroke_iter)->GetType() == STROKE) ||
			((*stroke_iter)->GetType() == STROKE_END)) {
			has_stroke=true;
			break;
		}
			
	}
	GesturePhaseVector::iterator pre_stroke_iter=stroke_iter-1;
	GesturePhaseVector::iterator post_stroke_iter=stroke_iter+1;

	assert(has_stroke==true);
	// stroke_iter NOW REFERENCES STROKE_END PHASE, 
	// post_stroke_iter THE SUBSEQUENT ELEMENT.
	
	
	// INSERT NEW REPETITION SEGMENTS INTO TEMPORARY VECTOR:
	float stroke_time=(*stroke_iter)->GetAbsTime();
	float time_inc=0.0;
	float prev_time=stroke_time;

#ifdef OUTPUT
	fprintf(data_log,"D ExpandRepetitions:: Stroke_Time: %f\n",stroke_time);
#endif

	GesturePhaseVector v2;
	FloatVector::iterator repetition_iter;
	int count=3;
	
	for (repetition_iter=mRepetitions.begin(); repetition_iter!=mRepetitions.end(); repetition_iter++) {
	
		
		// FIRST INSERT A HOLD PHASE AFTER THE ORIGINAL STROKE END
		GesturePhase *pFHold=new GesturePhase;
		pFHold->SetPWR(this->expressivityPWR);
		pFHold->SetAbsTime(prev_time+0.2f*(*repetition_iter-prev_time));
		pFHold->AddTweenComponent(*pre_stroke_iter,*stroke_iter,0.95f);

		// THEN INSERT THE RETRACTION PHASE
		GesturePhase *pF1=new GesturePhase;
		pF1->SetPWR(this->expressivityPWR);
		pF1->SetAbsTime(prev_time+retraction_ratio*(*repetition_iter-prev_time));
		pF1->AddTweenComponent(*pre_stroke_iter,*stroke_iter,1.0f-retraction_mag);

		// FINALLY COPY THE ORIGINAL STROKE_END PHASE
		// (ADD NOISE?)
		GesturePhase *pF2=new GesturePhase(**stroke_iter);
		pF2->SetPWR(this->expressivityPWR);
		pF2->SetAbsTime(*repetition_iter);
		v2.push_back(pFHold);
		v2.push_back(pF1);
		v2.push_back(pF2);
		prev_time=*repetition_iter;

		#ifdef OUTPUT
		fprintf(data_log,"D Repetition Segment times: %f, %f\n",pF1->GetAbsTime(),pF2->GetAbsTime());
		#endif
	
	}

	// ADJUST ABSOLUTE TIMES OF ORIGINAL POST-STROKE PHASES:
	float post_offset = mRepetitions.back()-stroke_time;
	GesturePhaseVector::iterator iter;
	for (iter=post_stroke_iter; iter!=phases.end(); iter++) {
		(*iter)->SetAbsTime((*iter)->GetAbsTime()+post_offset);
	}
	SetFinishTime(phases.back()->GetAbsTime());
	
	// APPEND NEWLY CREATED PHASES
	for(iter=v2.begin();iter!=v2.end();iter++) {
		phases.push_back(*iter);
	}
	// THEN SORT GesturePhaseS (SEE GESTURE SORT)
	sort(phases.begin(),phases.end(),AscAbsGesturePhaseSort());
	
	// FINALLY RECALCULATE PHASE RELATIVE TIMES (0..1) FROM ABSOLUTE
	// TIMES JUST TO BE CONSISTENT:
	for(iter=phases.begin(); iter!=phases.end();iter++) {
		(*iter)->SetRelTime( ((*iter)->GetAbsTime()-mStart)/(mFinish-mStart) );
	}
	
	#ifdef OUTPUT
	Print();	
	#endif
}

//////////////////////////////////////////////////////////////////////
void Gesture::AddRepetitions(FloatVector v)
{

#ifdef OUTPUT
	fprintf(data_log,"D AddRepetitions: %d\n",phases.size());
#endif

	FloatVector::iterator i;
	mRepetitions.clear();
	for (i=v.begin();i!=v.end();i++) {
		mRepetitions.push_back(*i);
	}
}

//////////////////////////////////////////////////////////////////////
void Gesture::SetTimeframe(float start, float stroke, float finish)
{
	SetStartTime(start);
	SetStrokeTime(stroke);
	SetFinishTime(finish);
}

//////////////////////////////////////////////////////////////////////
void Gesture::SetSide(SideType s)
{
	GesturePhaseVector::iterator iter;
	for(iter=phases.begin(); iter!=phases.end(); iter++) {
		(*iter)->SetSide(s);
	}
	side = s;
}

/////////////////////////////////////////////////////////////////
bool Gesture::HasStroke(GesturePhaseVector::iterator &iter)
{
	//ITERATE OVER PHASE ARRAY UNTIL WE FIND STROKE PHASE
	GesturePhaseVector::iterator stroke_iter;
	bool has_stroke=false;
	for(stroke_iter=phases.begin();stroke_iter!=phases.end(); stroke_iter++) {
		if ( ((*stroke_iter)->GetType() == STROKE) ||
			((*stroke_iter)->GetType() == STROKE_END)) {
			iter=stroke_iter;
			has_stroke=true;
			break;
		}
	}
	return has_stroke;
}

//////////////////////////////////////////////////////////////////////
bool Gesture::HasPostStrokeHold(GesturePhaseVector::iterator &iter)
{
	//ITERATE OVER PHASE ARRAY UNTIL WE FIND STROKE PHASE
	GesturePhaseVector::iterator stroke_iter;
	bool has_stroke=false;
	for(stroke_iter=phases.begin();stroke_iter!=phases.end(); stroke_iter++) {
		if ((*stroke_iter)->GetType() == POST_STROKE_HOLD) {
			iter=stroke_iter;
			has_stroke=true;
			break;
		}
	}
	return has_stroke;
}

//////////////////////////////////////////////////////////////////////
void Gesture::SetExpressivitySPC(float SPC)
{
	this->expressivitySPC=SPC;
}
//////////////////////////////////////////////////////////////////////
void Gesture::SetExpressivityTMP(float TMP)
{
	this->expressivityTMP=TMP;
}
//////////////////////////////////////////////////////////////////////
void Gesture::SetExpressivityFLD(float FLD)
{
	this->expressivityFLD=FLD;
}
//////////////////////////////////////////////////////////////////////
void Gesture::SetExpressivityPWR(float PWR)
{
	this->expressivityPWR=PWR;
	GesturePhaseVector::iterator iter;
	for(iter=phases.begin(); iter!=phases.end(); iter++) {
		(*iter)->SetPWR(PWR);
	}
}
//////////////////////////////////////////////////////////////////////
void Gesture::SetExpressivityREP(float REP)
{
	this->expressivityREP=REP;
}
//////////////////////////////////////////////////////////////////////
float Gesture::GetExpressivityTMP()
{
	return this->expressivityTMP;
}
//////////////////////////////////////////////////////////////////////
float Gesture::GetExpressivityFLD()
{
	return this->expressivityFLD;
}
//////////////////////////////////////////////////////////////////////
float Gesture::GetExpressivityPWR()
{
	return this->expressivityPWR;
}

void Gesture::Load(string fileName)
{
	GesturePhase* phase=new GesturePhase;
	//phase->Start();
	bool otherArm = false;

	int i;
	bool set_loop_start=false;
	ifstream infile(fileName.c_str());
	if (!infile) { 
		cout << "! Gesture::Load : error opening file \"" << fileName << "\"\n"; 
		return;
	}
	int lines_read = 0;
	string theLine,classifier,tempString;
	vector<string>words;
	while (getline(infile,theLine,'\n')) {
	
		// SKIP COMMENT LINES AND EMTPY LINES
		if( theLine.compare(0,2,string("//"))==0) {
				continue;
		}
		if (theLine.size()==0) {
			continue;
		}
		// TOKENIZE LINE INTO WORDS
		theLine=theLine+ " ";
		words.clear();
		string::size_type pos=0, prev_pos=0;
		while ((pos=theLine.find_first_of(' ',pos))!=string::npos) {
			tempString = theLine.substr(prev_pos, pos-prev_pos);
			
			// CONVERT EACH WORD TO UPPERCASE
			tempString = string(_strupr((char *)tempString.c_str()));
			
			if (tempString.length() != 0)
				words.push_back(tempString);
			prev_pos= ++pos;
		}
		
		int tokens = words.size();
		
		// BRANCH ACCORDING TO LINE CLASSIFIER
		classifier = words[0];
		if (classifier.compare("GESTURECLASS")==0) 
		{
			if (output) cout <<". Gesture.mClass=\""<<words[1]<<"\""<<endl;
			SetClass(words[1]);
		} 
		else if (classifier.compare("GESTUREINSTANCE")==0) 
		{
			if (output) cout <<". Gesture.mInstance=\""<<words[1]<<"\""<<endl;
			SetInstance(words[1]);
		} 
		else if (classifier.compare("DURATION")==0) 
		{
			float d=(float)atof(words[1].c_str());
			if (output) cout <<". Gesture.duration="<<d<<endl;
			SetDefDuration(d); // SET _DEFAULT_ DURATION
		} 
		else if (classifier.compare("STOREDPOSE")==0) 
		{
			assert(tokens >= 3);
			GesturePhase * poseframe = new GesturePhase;
			poseframe->Start();
			poseframe->time= (float)atof(words[2].c_str());
			poseframe->AddStoredPose(words[1]);
			if (tokens >= 4) {
				poseframe->SetType(words[3]);
			}
			if(otherArm) { phasesAssym.push_back(poseframe); }
			else{ phases.push_back(poseframe); }
		}
		else if (classifier.compare("ADDNOISE")==0) //keyword within a phase
		{
			phase->AddNoise();
		}
		else if (classifier.compare("STARTLOOP")==0) 
		{
			if (output) cout <<". Found STARTLOOP"<<endl;
			set_loop_start=true;
		}
		else if (classifier.compare("ENDLOOP")==0) 
		{
			// modify last inserted phase
			if (output) cout <<". Found ENDLOOP"<<endl;
			if(otherArm){ phasesAssym.back()->SetLoopType(LOOP_END); }
			else{ phases.back()->SetLoopType(LOOP_END); }
		}

		else if (classifier.compare("STARTFRAME")==0) 
		{
			float t=(float)atof(words[1].c_str());
			if (output) cout << ". Starting new GesturePhase at time "<<t<<endl;
			phase = new GesturePhase;
			phase->Start();
			phase->time=t;
			if(set_loop_start) {
				phase->SetLoopType(LOOP_START);
				set_loop_start=false;
			}
			
		} 
		else if (classifier.compare("ENDFRAME")==0) 
		{
			if (output) cout << ". Ending GesturePhase"<<endl;
			phase->Finalize();
			//push completed phase:
			if(otherArm){ phasesAssym.push_back(phase); }
			else{ phases.push_back(phase); }
		} 
		// OPTIONAL FRAMETYPE (STROKE, ETC.)
		else if (classifier.compare("FRAMETYPE")==0) 
		{
			if (tokens > 1) {
				phase->SetType(words[1]);
			}
		}
		// OPTIONAL: SIDE r, l or both
		else if (classifier.compare("SIDE")==0) 
		{
			string whichSide = words[1];
			if (whichSide.compare("LEFT")==0) {
				if (output) cout << ". Side = left"<<endl;
				SetSide(l);
			} else if (whichSide.compare("RIGHT")==0) {
				if (output) cout << ". Side = right"<<endl;
				SetSide(r);
			} else if (whichSide.compare("BOTH")==0) {
				if (output) cout << ". Side = both"<<endl;
				SetSide(both_sides);
			} else if (whichSide.compare("ASSYM")==0) {
				if (output) cout << ". Side = assymetric"<<endl;
				SetSide(assym);
			} else {
				if (output) cout << ". Side = unspecified"<<endl;
				SetSide(no_side);
			}
		} 
		else if (classifier.compare("ARM")==0) 
		{
			if (output) cout << ". Arm definition: "<<tokens<<" tokens."<<endl;
			for (i=1; i<tokens; i++) {
				phase->Add(words[i]);
			}
		} 
		else if (classifier.compare("WRIST")==0) 
		{
			if (output) cout << ". Wrist definition: "<<tokens<<" tokens."<<endl;
			if(tokens==3) {
				// old wrist processing of form
				// "WRIST FingerBaseType PalmType"
				for (i=1; i<tokens; i++) {
					phase->Add(words[i]);
				}
			} else if(tokens==6) {
				//add a new interpolating wrist
				if (output) cout<<". Adding interpolating wrist"<<endl;
				phase->AddWrist(words[1],words[2],false);
				phase->AddWrist(words[3],words[4],true);
				phase->AddWristMagnitude((float)atof(words[5].c_str()));
				
			}
			
		
		} 
		else if (classifier.compare("HAND")==0) 
		{
			if (output) cout << ". Hand definition: "<<tokens<<" tokens."<<endl;
			if (!(tokens>1)) {
				cout << ". Default Hand added."<<endl;
				phase->Add(shape_default);
				continue;
			}
			for (i=1; i<tokens; i++) {
				phase->Add(words[i]);
			}

		} 
		else if (classifier.compare("FINGER")==0) 
		{
			if (output) cout << ". Finger definition: "<<tokens<<" tokens."<<endl;
			if (tokens>=3) {
				phase->Add(words[1],words[2]);			
			}
		} 
		else if (classifier.compare("CURVE")==0) {phase->AddTransition(words);}
		else if (classifier.compare("OTHERARM")==0) { otherArm = true; }
		else 
		{
			cout <<"! GestureReader::LoadGesture(): Unknown Classifier read: "<<endl
				<<"  \""<<classifier<<"\""<<endl;
		}
		
	}
	SetDescription();
	FindLoopInfo();
}

const char * arm_x_lookup[]={"XEP", "XP", "XC", "XCC", "XOppC"};
const char * arm_y_lookup[]={"YUpperEP", "YUpperP", "YUpperC", "YCC", "YLowerC","YLowerP","YLowerEP"};
const char * arm_z_lookup[]={"ZNear", "ZMiddle", "ZFar"};


const char * phasetype_lookup[]={"PHASETYPE_DEFAULT", "PREPARATION", "PRE_STROKE_HOLD","STROKE",
								"STROKE_START", "STROKE_END","POST_STROKE_HOLD", "RETRACTION"};

const char * form_lookup[]={"form_default","form_fist","form_open","form_point1","form_point2",
								"form_2apart","form_openapart"};

const char * palm_lookup[]={"PalmDefault","PalmUp","PalmDown","PalmInwards","PalmOutwards","PalmAway","PalmTowards","PalmNone"};

const char * fingerbase_lookup[]={"FBDefault","FBUp","FBDown","FBInwards","FBOutwards","FBAway","FBTowards","FBNone"};

void Gesture::Save(string fileName)
{
	bool set_loop_start=false;
	ofstream outputfile(fileName.c_str());
	if (!outputfile) { 
		cout << "! Gesture::Save : error opening file \"" << fileName << "\"\n"; 
		return;
	}

	
		outputfile << "GESTURECLASS " << GetClass() << "\n";
		outputfile << "GESTUREINSTANCE " << GetInstance() << "\n";
		outputfile << "DURATION " << GetDuration() << "\n";
		if(GetSide()==l)
			outputfile << "SIDE LEFT\n";
		if(GetSide()==r)
			outputfile << "SIDE RIGHT\n";
		if(GetSide()==both_sides)
			outputfile << "SIDE BOTH\n";
		if(GetSide()==assym)
			outputfile << "SIDE ASSYM\n";

		outputfile << "\n";

		savePhases(&outputfile, &phases);

		if(side==assym){
			outputfile << "OTHERARM\n\n";
			savePhases(&outputfile, &phasesAssym);
		}
	
	outputfile.close();
//#endif
}


void Gesture::savePhases(ofstream *outputfile, GesturePhaseVector *phasesVector){
	
		std::vector<GesturePhase*>::iterator iter;
		for(iter=phasesVector->begin();iter!=phasesVector->end();iter++)
		{
			if((*iter)->GetStoredPose()!="")
			{
				*outputfile << "STOREDPOSE " << (*iter)->GetStoredPose() << " " << (*iter)->time << "\n";
				continue;
			}
			*outputfile << "STARTFRAME " << (*iter)->time << "\n";
			*outputfile << "FRAMETYPE " << phasetype_lookup[(*iter)->type] << "\n";
			
			*outputfile << "ARM";

			*outputfile <<" " << arm_x_lookup[(*iter)->GetPArm()->AbstractX];

			if((*iter)->GetPArm()->fixedX==true)
				*outputfile << ":FIXED";

			*outputfile <<" " << arm_y_lookup[(*iter)->GetPArm()->AbstractY];
			
			if((*iter)->GetPArm()->fixedY==true)
				*outputfile << ":FIXED";
			
			*outputfile <<" " << arm_z_lookup[(*iter)->GetPArm()->AbstractZ];
			
			if((*iter)->GetPArm()->fixedZ==true)
				*outputfile << ":FIXED";

			*outputfile << "\n";

			if(((*iter)->GetPHand()->type==shape_default)&&((*iter)->GetPHand()->fixedForm==false))
				*outputfile << "HAND FORM_DEFAULT ADDNOISE\n";
			if(((*iter)->GetPHand()->type==shape_default)&&((*iter)->GetPHand()->fixedForm==true))
				*outputfile << "HAND FORM_DEFAULT:FIXED ADDNOISE\n";

			std::string s,t;

			if((*iter)->GetPHand()->type==shape_form)
			{	
				BasicForm* pForm = (BasicForm*) (*iter)->GetPHand()->shape;

				s=form_lookup[pForm->type];

				*outputfile << "HAND " << s;

				if(((*iter)->GetPHand()->fixedForm==true)
					&&((pForm->type==form_fist)||(pForm->type==form_open)))
					*outputfile << ":FIXED";

				if(pForm->thumb==thumb_default)
					s="thumb_default";
				if(pForm->thumb==thumb_away)
					s="thumb_away";
				if(pForm->thumb==thumb_over)
					s="thumb_over";

				*outputfile << " " << s << " ADDNOISE\n";
			}

			if((*iter)->GetPHand()->type==shape_symbol)
			{
				BasicSymbol* pSymbol = (BasicSymbol*) (*iter)->GetPHand()->shape;
				if(pSymbol->type==symbol_default)
					s="symbol_default";
				if(pSymbol->type==symbol_1_open)
					s="symbol_1_open";
				if(pSymbol->type==symbol_2_open)
					s="symbol_2_open";
				if(pSymbol->type==symbol_3_open)
					s="symbol_3_open";
				if(pSymbol->type==symbol_1_closed)
					s="symbol_1_closed";
				if(pSymbol->type==symbol_2_closed)
					s="symbol_2_closed";
				if(pSymbol->type==symbol_3_closed)
					s="symbol_3_closed";
				
				if(s.find("open")!=std::string::npos)
				{
					if(pSymbol->opentype==open_default)
						t="open_default";
					if(pSymbol->opentype==open_thumbout)
						t="open_thumbout";
					if(pSymbol->opentype==open_thumbin)
						t="open_thumbin";
				}
				else
				{
					if(pSymbol->opentype==closed_default)
						t="closed_default";
					if(pSymbol->opentype==closed_straight)
						t="closed_straight";
					if(pSymbol->opentype==closed_inside)
						t="closed_inside";
					if(pSymbol->opentype==closed_tight)
						t="closed_tight";
				}
				*outputfile << "HAND " << s << " " << t << " ADDNOISE\n";
			}

			s=fingerbase_lookup[(*iter)->GetPWrist()->GetFromFingerBase()];

			t=palm_lookup[(*iter)->GetPWrist()->GetFromPalm()];

			*outputfile << "WRIST " << s << " " << t << " ";

			if((*iter)->GetPWrist()->GetWristMagnitude()<1)
			{
				s=fingerbase_lookup[(*iter)->GetPWrist()->GetFingerBase()];

				t=palm_lookup[(*iter)->GetPWrist()->GetPalm()];

				*outputfile << s << " " << t << " " << (*iter)->GetPWrist()->GetWristMagnitude() << "\n";
			}

			*outputfile << "\n";
			if((*iter)->HasTransition())
				*outputfile << ((*iter)->GetCurvedTransition()->toString(true));
			*outputfile << "ENDFRAME\n\n";
		}
}



void Gesture::SwitchPhasesVectors(){
	if(side == assym){
		GesturePhaseVector vect = phases;
		phases = phasesAssym;
		phasesAssym = vect;
	}


}
