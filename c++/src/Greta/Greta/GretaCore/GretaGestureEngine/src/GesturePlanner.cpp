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

// GesturePlanner.cpp: implementation of the GesturePlanner class.
//
//////////////////////////////////////////////////////////////////////

//comment out for old explicit timing
//uncomment for new fitt-s law based timing - needs stroke!
#define USE_NEW_TIMING

#include "GestureKinematics.h"
#include "GesturePlanner.h"
#include "ReachSpace.h"
#include "IniManager.h"
#include "BAPconverter.h"
#include "RandomGen.h"

#include "DataContainer.h"

extern IniManager inimanager;
extern DataContainer *datacontainer;

extern BAPConverter GestureGlobalBAPconverter;

extern RandomGen *randomgen;

/**@#-*/ 
using namespace GestureSpace;
/**@#+*/ 
extern FILE* gesture_log;
#define OUTPUT
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GesturePlanner::GesturePlanner()
{
	//srand( (unsigned)time( NULL ) );	
	mpvGesture = new GestureVector;
	output=true;
}

//////////////////////////////////////////////////////////////////////

GesturePlanner::~GesturePlanner()
{
	mpvGesture->clear();
	//do not dellete mGesutary as we have only one copy!!!
	//delete mGestuary
}

//////////////////////////////////////////////////////////////////////

void GesturePlanner::Init()
{
	mpvGesture->clear();
	mvLeftGesture.clear();
	mvRightGesture.clear();
	//mGestuary.LoadLibrary();
	//use data container
	//mGestuary=(*(datacontainer->getGestuary()));
	
	mGestuary=datacontainer->getGestuary();
}


//////////////////////////////////////////////////////////////////////

void GesturePlanner::Schedule(Gesture* pG, 
							  float start, 
							  float stroke, 
							  float end,
							  FloatVector repetitions)
{
#ifdef OUTPUT
	fprintf(gesture_log,"GesturePlanner::Schedule(%s, start:%f, stroke:%f, end:%f, repetitions:%d)\n",
		(pG->GetDescription().c_str()),start,stroke,end,repetitions.size());
#endif

	static int c=1;
	
	// GENERATE NEW INSTANCE OF BASIC GESTURE
	// USE OVERLOADED COPY CONSTRUCTOR!
	Gesture *newG = new Gesture(*pG);
	

	// ASSIGN A SIDE TO THE CURRENT GESTURE:
	// IF GESTURE HAS NO DEFAULT SIDE STORED IN LIBRARY, PICK ONE
	// CRITERIA: 70% chance right, 30% left
	// :TODO: CURRENTLY DISABLED TO PRODUCE PREDICTABLE ASSIGNMENTS
	if (newG->GetSide() == no_side) {
		int rnum=randomgen->GetRand01();
		if (rnum<=0.7) 
			newG->SetSide(r);
		else newG->SetSide(l);
	}

	
	// FIND ABSOLUTE TIMING OF FRAMES:
	// SIMPLE GESTURE:
	if(repetitions.size()==0)
	{
#ifdef USE_NEW_TIMING
		FindAbsoluteTimes(newG,start,stroke,end);
#else
		newG->SetTimeframe(start,stroke, end);
		newG->ExpandLoop();
		newG->ScalePhaseTimes();
#endif
	} 

	// ADDITIONAL repetitions EXIST: INSERT REPETITIONS
	else 
	{
#ifdef OUTPUT
		fprintf(gesture_log,"D Schedule: called with repetitions!\n");
#endif
		newG->SetHasRepetitions(true);

#ifdef USE_NEW_TIMING
  		FindAbsoluteTimes(newG,start,stroke,end);
#else
		newG->SetTimeframe(start,stroke, end);
		newG->ScalePhaseTimes();
#endif
		newG->AddRepetitions(repetitions);
		// 20032007
		//newG->ExpandRepetitions();
	}

	
	// ITERATE THROUGH EXISTING GESTURE VECTOR AND SEE IF DEFAULT SIDE
	// IS AVAILABLE
	// :TODO: - REFINE METHOD TO ALLOW COARTICULATION OF BEAT GESTURES
	//        - TRY ADJUSTING THE TIMING CONSTRAINTS  
	bool failure=false;
	bool iter2=false;
	Gesture* pConflict=0;
	GestureVector::iterator iter;
	for(iter=mpvGesture->begin();iter!=mpvGesture->end();iter++) {
		
		float anothers,anothere;
		anothere=(*iter)->GetFinishTime();
		anothers=(*iter)->GetStartTime();
		// IF THERE IS A PREframes. SCHEDULED OVERLAPPING GESTURE:
		// :KLUDGE: hardcoded 0.2 secods as minimum transition time.
		if ( (((*iter)->GetFinishTime()+0.2) > start) && 
			 (((*iter)->GetStartTime()) < end+0.2) ) {
			
			// IF SIDES OF THE TWO GESTURES ARE DIFFERENT: WE'RE OK
			if (((*iter)->GetSide()!=both_sides) && (newG->GetSide()!=both_sides)
				&& ((*iter)->GetSide()!=assym) && (newG->GetSide()!=assym)
				&& ((*iter)->GetSide() != newG->GetSide())) {
				continue;
			}
			
			// IF ONE OF THE GESTURES USES BOTH HANDS: CAN'T SCHEDULE
			else if ( ((*iter)->GetSide()==both_sides) || 
					  (newG->GetSide()==both_sides) ||
					  ((*iter)->GetSide()==assym) || 
					  (newG->GetSide()==assym)) {
				pConflict=(*iter);
				failure=true;
				break;
			}
			
			// ELSE: TRY SWITCHING ARM AND ITERATE AGAIN (ONLY ONCE!)
			else {
				//NO SWITCHING OF SIDES!
				failure=true;
				break;
				if(iter2) {
					failure=true;
					break;
				}
				
#ifdef OUTPUT
					fprintf(gesture_log,". Scheduler: Trying to switch side to avoid conflict.\n");
#endif

				if (newG->GetSide() == r) newG->SetSide(l);
				else newG->SetSide(r);
				pConflict=(*iter);
				iter2=true;
				iter=mpvGesture->begin()-1;
			}
		}
	}

	if (failure) {
		// ARM ALLOCATION FAILED! MITIGATION STRATEGY?
		// - TRY TO CHANGE PREVIOUS GESTURE ? -> COULD UNRAVEL ENTIRE
		//   SEQUENCE!
		// - TRY TO ADJUST TIMING OF THIS/CONFLICTING GESTURE: 
		//   USE FITT'S LAW ?
#ifdef OUTPUT
		fprintf(gesture_log,"! Could not schedule gesture. No free arm available\n");
#endif
		
		if (pConflict!=0) {
			float overlap = pConflict->GetFinishTime() - newG->GetStartTime();
#ifdef OUTPUT
			fprintf(gesture_log,". Overlap in seconds: %f\n",overlap);
#endif
			// POSSIBILITIES:
			// CHECK IF WE CAN MOVE BOUNDARIES OF GESTURES WITHOUT
			// DISTURBING STROKE TIME (EITHER PUSH_FORWARD END POINT
			// OF EXISTING G OR PUSH_BACK START OF OUR GESTURE 
			// THEN TRY TO RESCHEDULE BY RECURSING 
			// (FLAG TO PREVENT INF. LOOP)

		}

	} else {
		// SUCCESS: ENQUEUE GESTURE
#ifdef OUTPUT
		fprintf(gesture_log,"* Scheduled Gesture successfully. Side: %d\n",newG->GetSide());
#endif
		if(mpvGesture->empty())
			mpvGesture->push_back(newG);
		else
			for(iter=mpvGesture->begin();iter!=mpvGesture->end();iter++)
			{
				if(newG->GetStartTime()<(*iter)->GetStartTime())
				{
					mpvGesture->insert(iter,newG);
					break;
				}
			}
		if(iter==mpvGesture->end())
			mpvGesture->push_back(newG);
	}
}


//////////////////////////////////////////////////////////////////////
// GesturePlanner::Request
// called from outside to request a gesture
// tries to find gesture in gestuary and calls scheduler on success

bool GesturePlanner::Request(string description, 
							float start, float stroke, float end, float SPC, float TMP, float FLD, float PWR, float REP)
{	
	// LOOK IF WE HAVE A DEFINITION IN THE LIBRARY
	Gesture *pGesture = mGestuary->FindBest(description);
	if (pGesture==0) {
		// :TODO: CHECK IF WE CAN GENERATE DEICTIC GESTURE W/ WORLD KNOWLEDGE
		return false;
	} else {
		
		assert(inimanager.initialized);

		if((SPC>=-1)&&(SPC<=1))
			pGesture->SetExpressivitySPC(SPC);
		else
			pGesture->SetExpressivitySPC(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));

		if((TMP>=-1)&&(TMP<=1))
			pGesture->SetExpressivityTMP(TMP);
		else
			pGesture->SetExpressivityTMP(inimanager.GetValueFloat("ENGINE_EXPR_TMP"));

		if((FLD>=-1)&&(FLD<=1))
			pGesture->SetExpressivityFLD(FLD);
		else
			pGesture->SetExpressivityFLD(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));

		if((PWR>=-1)&&(PWR<=1))
			pGesture->SetExpressivityPWR(PWR);
		else
			pGesture->SetExpressivityPWR(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));

		if((REP>=-1)&&(REP<=1))
			pGesture->SetExpressivityREP(REP);
		else
			pGesture->SetExpressivityREP(inimanager.GetValueFloat("ENGINE_EXPR_REP"));

		FloatVector empty_vector;
		empty_vector.clear();
		Schedule(pGesture, start, stroke, end,empty_vector);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////

void GesturePlanner::InsertRestKeys(bool skiprestinsertion)
{
	GestureVector::iterator iter;

	if(mpvGesture->empty())
		return;
	// CREATE TWO LISTS: GESTURES ON RIGHT SIDE/ ON LEFT SIDE
	for(iter=mpvGesture->begin();iter<=(mpvGesture->end()-1);iter++) {
		

		if( (*iter)->GetSide()==r)
		{
			(*iter)->ExpandRepetitions();
			mvRightGesture.push_back(*iter);	
		}
		else if ( (*iter)->GetSide()==l )
		{
			(*iter)->ExpandRepetitions();
			mvLeftGesture.push_back(*iter);	
		}
		else if ( (*iter)->GetSide()==both_sides){
			
			Gesture *g = new Gesture(**iter);
			/*
			Gesture* g = new Gesture;
			*g=**iter;					// :TRICKY: THIS USES DEFAULT COPY! NOT OVERLOADED
										// COPY CONSTRUCTOR! CAREFUL W/ POINTER
										// CORRECT HERE SINCE THE TWO VERSIONS SHOULD BE
										// IDENTICAL!
			*/
			
			(*iter)->SetSide(r);

			g->SetSide(l);

			(*iter)->ExpandRepetitions();

			g->ExpandRepetitions();

			mvLeftGesture.push_back(g);	
			mvRightGesture.push_back(*iter);
			//printf("duplicated gesture\n");
			//printf("original:\n");
			//(*iter)->PrintOnScreen();
			//printf("copy:\n");
			//g->PrintOnScreen();
		}
		else if((*iter)->GetSide()==assym){
			Gesture *g = new Gesture(**iter);
			(*iter)->SetSide(r);
			g->SwitchPhasesVectors();
			g->SetSide(l);
			(*iter)->ExpandRepetitions();
			g->ExpandRepetitions();
			mvLeftGesture.push_back(g);	
			mvRightGesture.push_back(*iter);
		}
	}
	
	if(skiprestinsertion==0)
	{
		// INSERT REST KEYS FOR EACH SIDE SEPARATELY
		InsertRestLateral(&mvRightGesture);
		InsertRestLateral(&mvLeftGesture);
	}

	// MERGE THE TWO LISTS AGAIN
	GestureVector*  pv_merged = new GestureVector;

	pv_merged->clear();
	for (iter=mvRightGesture.begin(); iter!=mvRightGesture.end(); iter++) 
		pv_merged->push_back(*iter);
	for (iter=mvLeftGesture.begin(); iter!=mvLeftGesture.end(); iter++) 
		pv_merged->push_back(*iter);

	mpvGesture=pv_merged;

	// SORT THE ARRAY BY NON-DECREASING START TIME
	GestureVector::iterator start, end;
	start=mpvGesture->begin();
	end=mpvGesture->end();
	sort(start,end,AscGestureSort());
}


//////////////////////////////////////////////////////////////////////
// :TODO: review part about initial rest position
// maybe unnecessary since rest postures are now inserted in 
// TurnPlanner

void GesturePlanner::InsertRestLateral(GestureVector *pvG)
{

	float thresh0= float(1.0);
	float thresh = float(2.0);		// INTER-GESTURAL THRESHOLD VALUE
	float thresh2= float(6.0);

	float startRest,finishRest,strokeRest; // calculated timings in absolute seconds of start, end and stroke points of inserted rest gestures
	GesturePhaseVector::iterator thisGestureBack; //iterator to last gesture phase of a previous gesture in sequence: this-rest-next
	GesturePhaseVector::iterator restGestureBack; //iterator to last gesture phase of a rest     gesture in sequence: this-rest-next

	// if we are executing movement with positive fluidity, 
	// increase the lower rest-insertion threshold so
	// arms stay in motion longer.
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	//float flt=inimanager.GetValueFloat("ENGINE_EXPR_FLD");
		//GlobalEngineCfg->Engine_Expr_Flt;
	//if(flt>0) {
	//	thresh+=2*flt;
	//} else {
		//NO going < 2.0 sec cuases assert to fail below.
		//NO thresh+=0.45*flt;
	//}

#endif

	GestureVector::iterator iter, next;
	if (pvG->size()>=2) {
		iter=pvG->begin();

		////////////////////////////////////////////////////////////////////
		// OUTPUT INITIAL LOG INFO

#ifdef OUTPUT
		fprintf(gesture_log,"BEFORE inserting rest keys on side %d\n",(*iter)->GetSide());
		for (iter=pvG->begin(); iter!=pvG->end(); iter++) {
			fprintf(gesture_log,"%s",(*iter)->GetDescription().c_str()); 
			fprintf(gesture_log," starting at %f",(*iter)->GetStartTime());
			fprintf(gesture_log," and lasting %f",(*iter)->GetDuration());
			fprintf(gesture_log," secs\n");
		}
#endif
		for (iter=pvG->begin(); iter!=pvG->end(); iter++) {
			next=iter+1;
			if (next==pvG->end()) break;
			float space = (*iter)->GetFinishTime() - (*next)->GetStartTime();

			if (space < 0.0) 
				space = -space;

			//////////////////////////////////////////////////////////////////////////
			// CATEGORY 1: LOTS OF SPACE - INSERT 4 keys = 2 rest gesture
			if ((space > thresh2)&&((*iter)->GetStartTime()>=0)) {
				float restDuration=1.0f;

				Gesture *pInsert=new Gesture(*(mGestuary->FindBest("REST=LEGS")));
				Gesture *pInsert2=new Gesture(*(mGestuary->FindBest("REST=LEGS")));

				pInsert->SetSide((*iter)->GetSide());
				pInsert2->SetSide((*iter)->GetSide());
				pInsert->SetExpressivityFLD((*iter)->GetExpressivityFLD());
				pInsert2->SetExpressivityFLD((*iter)->GetExpressivityFLD());
				pInsert->SetExpressivityPWR((*iter)->GetExpressivityPWR());
				pInsert2->SetExpressivityPWR((*iter)->GetExpressivityPWR());
				
				// pInsert (rest gesture 1):
#ifndef VC6_COMPILER
				thisGestureBack = --((*iter)->phases.end());
#else
				thisGestureBack = ((*iter)->phases.end());
				thisGestureBack--;
#endif
				startRest  = (*iter)->GetFinishTime() +
					FittsDuration(
					thisGestureBack,
					pInsert->phases.begin(),
					(*iter)->GetExpressivityTMP(),
					(*iter)->GetExpressivityPWR(),
					AST_Slow);
				finishRest = startRest+restDuration;
				strokeRest = (startRest+finishRest)/2.f;

				pInsert->SetStartTime(startRest);
				pInsert->SetFinishTime(finishRest);
				pInsert->SetStrokeTime(strokeRest);
				pInsert->SetSide((*iter)->GetSide());
				pInsert->ScalePhaseTimes();

				// pInsert2 (rest gesture 2):
#ifndef VC6_COMPILER
				restGestureBack = --(pInsert2->phases.end());
#else
				restGestureBack = (pInsert2->phases.end());
				restGestureBack--;
#endif
				finishRest = (*next)->GetStartTime() -
					FittsDuration(
					restGestureBack,
					(*next)->phases.begin(),
					(*next)->GetExpressivityTMP(),
					(*next)->GetExpressivityPWR(),
					AST_Slow);
				startRest  = finishRest-restDuration;
				strokeRest = (startRest+finishRest)/2.f;
				
				pInsert2->SetStartTime(startRest);
				pInsert2->SetFinishTime(finishRest);
				pInsert2->SetStrokeTime(strokeRest);
				pInsert2->SetSide((*iter)->GetSide());
				pInsert2->ScalePhaseTimes();

				iter=pvG->insert(next,pInsert);	
				next=iter+1;
				iter=pvG->insert(next,pInsert2);	
				
			}
			else if ((space > thresh2)&&((*iter)->GetStartTime()<0)) {
				Gesture *pInsert=new Gesture(*(mGestuary->FindBest("REST=LEGS")));
				pInsert->SetExpressivityFLD((*iter)->GetExpressivityFLD());
				pInsert->SetExpressivityPWR((*iter)->GetExpressivityPWR());
				pInsert->SetSide((*iter)->GetSide());
#ifndef VC6_COMPILER
				restGestureBack = --(pInsert->phases.end());
#else
				restGestureBack = (pInsert->phases.end());
				restGestureBack--;
#endif
				finishRest = (*next)->GetStartTime() -
					FittsDuration(
					restGestureBack,
					(*next)->phases.begin(),
					(*next)->GetExpressivityTMP(),
					(*next)->GetExpressivityPWR(),
					AST_Slow);
				startRest  = finishRest-1.0f;
			    strokeRest=(startRest+finishRest)/2.f;
				
				pInsert->SetStartTime(startRest);  
				pInsert->SetFinishTime(finishRest); 				
				pInsert->SetStrokeTime(strokeRest);
				pInsert->SetSide((*iter)->GetSide());
				pInsert->ScalePhaseTimes();
				iter=pvG->insert(next,pInsert);		
			}
			
			//////////////////////////////////////////////////////////////////
			// CATEGORY 2: MEDIUM GAP (>thresh)
			// insert middle rest pose
			else if ((space > thresh)&&((*iter)->GetStartTime()>=0)) 
			{ 
				
				//assert( ((*iter)->GetFinishTime()+1.5) < ((*next)->GetStartTime()-0.5) );
				Gesture *pInsert=new Gesture(*(mGestuary->FindBest("REST=MIDDLE2")));
				pInsert->SetSide((*iter)->GetSide());
				pInsert->SetExpressivityFLD((*iter)->GetExpressivityFLD());
				pInsert->SetExpressivityPWR((*iter)->GetExpressivityPWR());
				
				// New rest duration calculation w/ Fitt's law:

#ifndef VC6_COMPILER
				thisGestureBack = --((*iter)->phases.end());
#else
				thisGestureBack = ((*iter)->phases.end());
				thisGestureBack--;
#endif

#ifndef VC6_COMPILER
				restGestureBack = --(pInsert->phases.end());
#else
				restGestureBack = (pInsert->phases.end());
				restGestureBack--;
#endif
				startRest  = (*iter)->GetFinishTime() +
					FittsDuration(
					thisGestureBack,
					pInsert->phases.begin(),
					(*iter)->GetExpressivityTMP(),
					(*iter)->GetExpressivityPWR(),
					AST_Slow);
				finishRest = (*next)->GetStartTime() - 
					FittsDuration(
					restGestureBack,
					(*next)->phases.begin(),
					(*next)->GetExpressivityTMP(),
					(*next)->GetExpressivityPWR(),
					AST_Slow);
				strokeRest = (startRest+finishRest)/2.f;

				pInsert->SetStartTime(startRest);
				pInsert->SetFinishTime(finishRest);
				pInsert->SetStrokeTime(strokeRest);
				pInsert->SetSide((*iter)->GetSide());
				pInsert->ScalePhaseTimes();
				iter=pvG->insert(next,pInsert);	
				//search for post_stroke_hold
			}
			
			// gap > 1st threshold at beginning of animation
			else if ((space > thresh)&&((*iter)->GetStartTime()<0)) {

				Gesture *pInsert=new Gesture(*(mGestuary->FindBest("REST=MIDDLE2")));
				pInsert->SetSide((*iter)->GetSide());
				pInsert->SetExpressivityFLD((*iter)->GetExpressivityFLD());
				pInsert->SetExpressivityPWR((*iter)->GetExpressivityPWR());
#ifndef VC6_COMPILER
				restGestureBack = --(pInsert->phases.end());
#else
				restGestureBack = (pInsert->phases.end());
				restGestureBack--;
#endif
				finishRest = (*next)->GetStartTime() -
					FittsDuration(
					restGestureBack,
					(*next)->phases.begin(),
					(*next)->GetExpressivityTMP(),
					(*next)->GetExpressivityPWR(),
					AST_Slow);
				startRest  = finishRest-1.0f;
			    strokeRest=(startRest+finishRest)/2.f;
				
				pInsert->SetStartTime(startRest);  
				pInsert->SetFinishTime(finishRest); 				
				pInsert->SetStrokeTime(strokeRest);
				pInsert->SetSide((*iter)->GetSide());
				pInsert->ScalePhaseTimes();
				iter=pvG->insert(next,pInsert);		
			}
			
			/////////////////////////////////////////////////////////
			// CATEGORY 3: SMALLEST GAP - adjust post stroke hold
			else if ((space < thresh)&&(space > thresh0)&&((*iter)->GetStartTime()>=0)) { 
				GesturePhaseVector::iterator psh_iter; //post stroke hold iter;
				GesturePhaseVector::iterator dly_iter; //iterator over delayed phases;
				float delay; //time by which gesture phases of prev gesture will be shifted
				float trans; //transition time needed to get from end of prev gesture to start of next gesture
				if((*iter)->HasPostStrokeHold(psh_iter)) {
					trans=FittsDuration(psh_iter,(*next)->phases.begin(),(*iter)->GetExpressivityTMP(),(*iter)->GetExpressivityPWR(),AST_Slow);
					delay=space-trans; 
					for(dly_iter=psh_iter;dly_iter!=(*iter)->phases.end();dly_iter++) {
						(*dly_iter)->SetAbsTime((*dly_iter)->GetAbsTime()+delay);
					}
				} else if((*iter)->HasStroke(psh_iter)) {
					// INCORRECT! need to insert post stroke hold
					trans=FittsDuration(psh_iter,(*next)->phases.begin(),(*iter)->GetExpressivityTMP(),(*iter)->GetExpressivityPWR(),AST_Slow);
					delay=space-trans; 
					for(dly_iter=psh_iter;dly_iter!=(*iter)->phases.end();dly_iter++) {
						(*dly_iter)->SetAbsTime((*dly_iter)->GetAbsTime()+delay);
					}
				} 
			}
			
		}
		iter=pvG->begin();

		////////////////////////////////////////////////////////////////////
		// OUTPUT FINAL DESCRIPTIVE LOG INFO
#ifdef OUTPUT
		fprintf(gesture_log,"after inserting rest keys on side %d\n",(*iter)->GetSide());
		for (iter=pvG->begin(); iter!=pvG->end(); iter++) {
			fprintf(gesture_log,"%s",(*iter)->GetDescription().c_str()); 
			fprintf(gesture_log," starting at %f",(*iter)->GetStartTime());
			fprintf(gesture_log," and lasting %f",(*iter)->GetDuration());
			fprintf(gesture_log," secs\n");
		}
#endif

	}
}

//////////////////////////////////////////////////////////////////////
// :TODO: Arm position adjustment does not work properly
// the idea is to move keyframes to form a continuous path connecting end of previous with beginning of next gesture
// should be done through inverse kinematics. old code doing this baed on abstract gesture was removed on july 09 2004.
// NOT CURRENTLY CALLED (04 2004 bjoern)
void GesturePlanner::AdjustRestKey(Gesture *rest, Gesture *prev, Gesture *next)
{

}

//////////////////////////////////////////////////////////////////////

void GesturePlanner::Print()
{
#ifdef OUTPUT
	fprintf(gesture_log,"\n///GesturePlanner::Print()////START///////////////////////////////////\n");
#endif
	GestureVector::iterator iter;
	for(iter=mpvGesture->begin();iter!=mpvGesture->end();iter++) {
		(*iter)->Print();
	}
#ifdef OUTPUT
	fprintf(gesture_log,"///GesturePlanner::Print()////END/////////////////////////////////////\n\n");
#endif
}


//////////////////////////////////////////////////////////////////////
bool GesturePlanner::RequestWithRepetition(string description, 
										   float start, 
										   float stroke, 
										   float end,
										   float SPC,
										   float TMP,
										   float FLD,
										   float PWR,
										   float REP,
										   FloatVector repetitions)
{
	Gesture *pGesture = mGestuary->FindBest(description);
	if (pGesture==0) {
		// :TODO: CHECK IF WE CAN GENERATE DEICTIC GESTURE W/ WORLD KNOWLEDGE
		return false;
	} else {
		assert(inimanager.initialized);

		if((SPC>=-1)&&(SPC<=1))
			pGesture->SetExpressivitySPC(SPC);
		else
			pGesture->SetExpressivitySPC(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));

		if((TMP>=-1)&&(TMP<=1))
			pGesture->SetExpressivityTMP(TMP);
		else
			pGesture->SetExpressivityTMP(inimanager.GetValueFloat("ENGINE_EXPR_TMP"));

		if((FLD>=-1)&&(FLD<=1))
			pGesture->SetExpressivityFLD(FLD);
		else
			pGesture->SetExpressivityFLD(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));

		if((PWR>=-1)&&(PWR<=1))
			pGesture->SetExpressivityPWR(PWR);
		else
			pGesture->SetExpressivityPWR(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));

		if((REP>=-1)&&(REP<=1))
			pGesture->SetExpressivityREP(REP);
		else
			pGesture->SetExpressivityREP(inimanager.GetValueFloat("ENGINE_EXPR_REP"));


		Schedule(pGesture, start, stroke, end,repetitions);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////
//
void GesturePlanner::FindAbsoluteTimes(Gesture *pG, float start, float stroke, float end) {
	FAT(pG,start,stroke,end);
	if(pG->GetSide()==assym) {
		pG->SwitchPhasesVectors();
		FAT(pG,start,stroke,end);
		pG->SwitchPhasesVectors();
	}
}
void GesturePlanner::FAT(Gesture *pG, float start, float stroke, float end) {

	
	GesturePhaseVector::iterator stroke_iter,phase_iter;
	bool has_stroke=false;
	
	// for now, set some default values for the duration of
	// pre and post-stroke holds. later on these values should be
	// dynamically calculated (also taking into account rest phases)
	float postStrokeHoldTime=0.5f;
	float preStrokeHoldTime=0.4f;
	

	for(phase_iter=pG->phases.begin();phase_iter!=pG->phases.end(); phase_iter++) {
		(*phase_iter)->SetSide(pG->GetSide());
	}

	// ITERATE OVER FRAME ARRAY UNTIL WE FIND STROKE FRAME
	for(stroke_iter=pG->phases.begin();stroke_iter!=pG->phases.end(); stroke_iter++) {
		if ( (*stroke_iter)->GetType() == STROKE_END ) {
			has_stroke=true;
			break;
		}
	}

	if(!has_stroke) {
#ifdef OUTPUT
		fprintf(gesture_log,"FindAbsTime: NO STROKE WAS FOUND. SKIPPING NEW CALCULATION\n");
		printf("FindAbsTime: Scheduling a Gesture without Temporal expressivity: %s=%s\n",
			pG->GetClass().c_str(),pG->GetInstance().c_str());
#endif
		pG->SetTimeframe(start,stroke,end);
		pG->ScalePhaseTimes();
		return;
	}

	//SET TIME FOR STROKE:
	(*stroke_iter)->SetAbsTime(stroke);

	//DISTANCE/FITT BASED TIME CALCULATION
	GesturePhaseVector::iterator prev_iter=stroke_iter;
	GesturePhaseVector::iterator post_iter=stroke_iter+1;
	GesturePhaseVector::iterator pre_iter=stroke_iter+1;
	float T;
	float lastTime=stroke;
	
	double calcEndTime,calcStartTime;
	calcStartTime=calcEndTime=stroke; //calculated gesture boundary times as opposed to given ones from xml tree
	
#ifdef OUTPUT	
	fprintf(gesture_log,"Stroke->End:\n");
#endif
	//FROM STROKE_END ON FORWARDS TO END OF GESTURE
	for (; post_iter!=pG->phases.end(); post_iter++) {
		
		//movement: calculate time by distance
		T=(float)FittsDuration(prev_iter,post_iter,pG->GetExpressivityTMP(),pG->GetExpressivityPWR(),AST_Normal);

		//hold: calculate hold time by ???
		if ((*post_iter)->GetType()==POST_STROKE_HOLD) {
			T=postStrokeHoldTime;
		}

		(*post_iter)->SetAbsTime(lastTime+T);
		prev_iter=post_iter;
		lastTime=lastTime+T;
	
	}
	calcEndTime=lastTime;
	prev_iter=stroke_iter;
	lastTime=stroke;


#ifdef OUTPUT	
	fprintf(gesture_log,"Start<-Stroke:\n");
#endif

	//FROM STROKE_END BACKWARDS TO START OF GESTURE
	for (pre_iter=stroke_iter-1; pre_iter!=pG->phases.begin()-1; pre_iter--) {
		
		//movement: calculate time by distance
		T=(float)FittsDuration(pre_iter,prev_iter,pG->GetExpressivityTMP(),pG->GetExpressivityPWR(),AST_Normal);
		
		//hold: calculate hold time by ???
		if ((*pre_iter)->GetType()==PRE_STROKE_HOLD) {
			T=preStrokeHoldTime;
		}
		(*pre_iter)->SetAbsTime(lastTime-T);
		prev_iter=pre_iter;
		lastTime=lastTime-T;
	}
	calcStartTime=lastTime;
	pG->SetTimeframe(float(calcStartTime), float(stroke), float(calcEndTime));

#ifdef OUTPUT	
	fprintf(gesture_log,"GP::FindAbsTime(%s, start:%f, stroke:%f, end:%f, side:%d)\n",
		pG->GetDescription().c_str(),calcStartTime,stroke,calcEndTime,pG->GetSide());
	//MAURIZIO 9-6-2008: store the real start, end and duration of the gesture
	pG->SetStartTime(calcStartTime);
	pG->SetFinishTime(calcEndTime);
	pG->SetDuration(calcEndTime-calcStartTime);
//		pG->GetDescription().c_str(),calcStartTime,stroke,calcEndTime,pG->GetSide());
#endif
}


//////////////////////////////////////////////////////////////////////
// Fitt's duration:
// calculate time T in seconds that is needed for arm movement from
// keyframe described by from_iter to keyframe described by to_iter
// 
double GesturePlanner::FittsDuration(GesturePhaseVector::iterator from_iter, 
									 GesturePhaseVector::iterator to_iter,
									 float TMP,float PWR,
									 ArmSpeedType speed) 
{

	double T=0.0; // duration for this movement segment; OUTPUT
	double A;	  // Amplitude of movement (distance traveled); calculated from input
	double b=0.1; // velocity coefficient for Fitt's law
	double a=0.1; // velocity offset for Fitt's law
	
	//take default speed into account.
	b=(1.f-0.25f*int(speed))*b;

	// calculation is affected by TEMPORAL expressivity parameter
	#ifdef WITH_EXPRESSIVITY
		//assert(inimanager.initialized);
		
		//float temporal=inimanager.GetValueFloat("ENGINE_EXPR_TMP");
			//GlobalEngineCfg->Engine_Expr_Tmp;
		float temporal=TMP;
		//float power=inimanager.GetValueFloat("ENGINE_EXPR_PWR");
		float power=PWR;
		//GlobalEngineCfg->Engine_Expr_Pwr;

		b=(1.0-0.7*temporal)*b;
		if ((*to_iter)->GetType()==STROKE_END) {
			b=(1.0-(0.7*power))*b;
		} 

	#ifdef OUTPUT
		fprintf(gesture_log,"EXPRESSIVITY: setting velocity coefficient to %2.2f (expr_tmp=%2.2f)\n",b,temporal);
	#endif
	#endif
//-------------------------------------------------------------------------------------
	SideType side=(*to_iter)->GetSide();
	if((*from_iter)->HasTransition()){
		(*from_iter)->GetCurvedTransition()->setStartPosition((*from_iter)->GetPArm()->GetCoordinates());
		(*from_iter)->GetCurvedTransition()->setEndPosition((*to_iter)->GetPArm()->GetCoordinates());
		A=(*from_iter)->GetCurvedTransition()->length();
	}
	else{
		GestureKinematics gK;
	
		BAPFrame fFrom(&GestureGlobalBAPconverter),fTo(&GestureGlobalBAPconverter);
		(*from_iter)->GetAngles(fFrom);
		(*to_iter)->GetAngles(fTo);

		A=gK.FindDistance(fFrom,fTo,side);
	}
//-------------------------------------------------------------------------------------
	if(A>5.0) {
		T=a+b*log(A+1.0); //Fitts' derived log law 
		//Fitts' law is in log base 2. it may be :
		// T=a+b*log(A+1.0)/log(2.0);
	} else {
		T=a; //no movement; default minimum duration
	}

#ifdef OUTPUT	
	fprintf(gesture_log,"GesturePlanner::FittsDuration: T=%2.2f for A=%2.2f, side %d\n",
		T,A,side);
#endif
	return T;
}