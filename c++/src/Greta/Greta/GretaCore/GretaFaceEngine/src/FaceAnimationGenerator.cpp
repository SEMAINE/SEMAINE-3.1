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

// FaceAnimationGenerator.cpp: implementation of the FaceAnimationGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceAnimationGenerator.h"
#include "RandomGen.h"
#include "LipModel.h"
#include "IniManager.h"
#include "DataViewerWindow.h"

#include "FileNames.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include <malloc.h>

extern RandomGen *randomgen;
extern IniManager inimanager;
extern FileNames filenames;

extern FILE *face_log;

#define OUTPUT

//defaul constructor is used only for inheritance 
FaceAnimationGenerator::FaceAnimationGenerator()
{

	// parameters are scaled from [0,1] or [-1,1]
	// to other ranges, specified in the constructor
	// of each parameter. these ranges have been defined
	// empirically.

	OAC = new EngineParameter(0,1);
	OAC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_OAC"));
	
	SPC = new EngineParameter(0.5, 1.0, 1.5);
	SPC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));

	FLD = new EngineParameter(0,1);
	FLD->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));

	PWR = new EngineParameter(0, 0.5);
	PWR->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));

	REP = new EngineParameter(1.0, 0.5);
	REP->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_REP"));

	GazeSimulator=new FaceGazeSimulator(engine,this);
	FaceDictionary=new FaceExpressionDictionary();

	//it is not used by default
	first_frame_flag=false;

	this->fapframevector=0;
	this->fapframevectorwithoutnoise=0;

	this->FaceInterp=0;
}


FaceAnimationGenerator::FaceAnimationGenerator(void *e,CommunicativeAct *first_comm_act, bool bml)
{


	/* DEBUG VERSION HEAP CORRUPTION CHECK 
	added by bjoern 2004-04-21          */
	/*
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag( tmpFlag );
	*/

	engine=e;
	allocatedstructures=false;

	start_away_time=0;
	total_away_time=0;
	eyes_pitch=0;
	eyes_roll=0;
	this->first_comm_act=first_comm_act;
	this->help_comm_act=first_comm_act;
	EYES_SPEED=1500;

	this->bml=bml;

	last_deg=0;
	last_deg2=0;

	// parameters are scaled from [0,1] or [-1,1]
	// to other ranges, specified in the constructor
	// of each parameter. these ranges have been defined
	// empirically.

	OAC = new EngineParameter(0,1);
	OAC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_OAC"));
	
	SPC = new EngineParameter(0.5, 1.0, 1.5);
	SPC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));

	FLD = new EngineParameter(0,1);
	FLD->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));

	PWR = new EngineParameter(0, 0.5);
	PWR->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));

	REP = new EngineParameter(1.0, 0.5);
	REP->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_REP"));



	//Gaze simulator, initializes the same structure needed by
	//conflict resolver so don't move this instructions from here

	GazeSimulator=new FaceGazeSimulator(engine,this);

	FaceDictionary=new FaceExpressionDictionary();

	//it is not used by default
	first_frame_flag=false;

	this->fapframevector=0;
	this->fapframevectorwithoutnoise=0;

	this->FaceInterp=0;
}

FaceAnimationGenerator::~FaceAnimationGenerator()
{
	if(this->FaceInterp!=0)
	{
		FaceInterp->DeallocKeyFrames();
		delete FaceInterp;
		this->FaceInterp=0;
	}


	Deallocate();
	delete GazeSimulator;

	//AT THE MOMENT DONT CANCEL FACEDICTIONARY AS WE HAVE ONLY ONE COPY IN DATA CONTAINER 
	//IT WILL BE CANCELLED BY DATA CONTAINER 
	//delete FaceDictionary;

	delete OAC;
	delete SPC;
	delete REP;
	delete PWR;
	delete FLD;

}


int FaceAnimationGenerator::setFaceExpressionDictionary(FaceExpressionDictionary* fed)
{

	this->FaceDictionary = fed;

	//if (FaceDictionary==) return 0;

	/*
	if(FaceDictionary->Init((inimanager.Program_Path+inimanager.GetValueString("FACELIBRARY_PATH")).c_str())==0)
	return 0;
	*/

	return 1;
}

int FaceAnimationGenerator::BuildSpeakerFAPFile(std::string fapfilename,float animationlength,int number_of_comm_act,PhonemeVector *PhoVector,phoneme *phonemes,float speech_delay)
{

	FramesTotalNumber=(int)ceil(animationlength*FRAMERATE);

	if(InitializeFaps()==0)
		return 0;

	if(inimanager.GetValueInt("GAZE_MODEL_BN")==1)
	{
		if(this->bml==0)
		{
			if(GazeSimulator->InitGazeSimulation(phonemes)==0)
				return 0;
		}
		else
		{
			if(GazeSimulator->InitGazeSimulationBML(first_comm_act,animationlength)==0)
				return 0;
		}

	}

	FaceInterp=new FaceInterpolator(FramesTotalNumber,engine);
	FaceInterp->InitKeyFrames();

	if(DrawExpressionsKeyframes(number_of_comm_act,phonemes)==0)
		return 0;

	//if the first keyframe should not be neutral
	if (first_frame_flag==true) 
	{			
		int frameind;
		//we generate an artifical expression that is located before the given one
		//in order to impprove the interpolation results

		std::vector<FAPFrame> *newfapframevector;

		newfapframevector=new std::vector<FAPFrame>( (*fapframevector).size()*2,FAPFrame());

		for(frameind=0;frameind<(*fapframevector).size()*2;frameind++)	
			(*newfapframevector)[frameind].SetFAP(0,frameind);

		newfapframevector->resize(0);

		FAPFrame one=first_frame.clone();
		one.ActivateAllFAPs();
		newfapframevector->push_back(one);	

		for(frameind=2;frameind<(int)FramesTotalNumber/4;frameind++){
			FAPFrame newone;
			for(int iii=0;iii<69;iii++)
			{
				if ((iii!=48)&&(iii!=49)&&(iii!=50))  newone.SetFAP(iii,0,false);				
				else 
				newone.SetFAP(iii,first_frame.GetFAP(iii),false);
			}
			newone.id=frameind+1;
			newfapframevector->push_back(newone);
		}

		FAPFrame two = first_frame.clone();
		two.ActivateAllFAPs();	
		newfapframevector->push_back(two);

		for(frameind;frameind<(int)FramesTotalNumber/2;frameind++)
		{
			FAPFrame newone;
			for(int iii=0;iii<69;iii++)
			{
				if ((iii!=48)&&(iii!=49)&&(iii!=50))  newone.SetFAP(iii,0,false);				
				else 
				newone.SetFAP(iii,first_frame.GetFAP(iii),false);
			}
			newone.id=frameind+1;
			newfapframevector->push_back(newone);
		}


		FAPFrame three= first_frame.clone();
		three.ActivateAllFAPs();	
		newfapframevector->push_back(three);

		for(frameind;frameind<FramesTotalNumber-2;frameind++){
			FAPFrame newone;
			for(int iii=0;iii<69;iii++)
			{
				if ((iii!=48)&&(iii!=49)&&(iii!=50))  newone.SetFAP(iii,0,false);				
				else 
				newone.SetFAP(iii,first_frame.GetFAP(iii),false);
			}
			newone.id=frameind+1;
			newfapframevector->push_back(newone);
		}


		FAPFrame four = first_frame.clone();	
		four.ActivateAllFAPs();
		newfapframevector->push_back(four);

		std::vector<FAPFrame>::iterator iter;

		for(iter=fapframevector->begin();iter!=fapframevector->end();iter++) 
		{
			FAPFrame temp = iter->clone();		
			if ( (*iter).isKeyFrame==true) temp.isKeyFrame=true;
			newfapframevector->push_back(  temp );
		}

		(*newfapframevector)[0].isKeyFrame=true;
		(*newfapframevector)[FramesTotalNumber/2].isKeyFrame=true;	
		(*newfapframevector)[FramesTotalNumber/4].isKeyFrame=true;

		CommunicativeAct* comm_act=(CommunicativeAct*)help_comm_act;
		float start_time=0.0f;			
		while  ((comm_act!=NULL)&&(comm_act->start_time==0.0f)) 
			comm_act=(CommunicativeAct*) comm_act->next;					
		if (comm_act!=NULL) start_time=comm_act->start_time;

		/*
		for (int i=0;i<(*newfapframevector).size();i++)
		{
		(*newfapframevector)[i].SetFAP(23,first_frame.GetFAP(23),true);
		(*newfapframevector)[i].SetFAP(24,first_frame.GetFAP(24),true);
		(*newfapframevector)[i].SetFAP(25,first_frame.GetFAP(25),true);
		(*newfapframevector)[i].SetFAP(26,first_frame.GetFAP(26),true);

		(*newfapframevector)[i].SetFAP(48,first_frame.GetFAP(48),true);
		(*newfapframevector)[i].SetFAP(49,first_frame.GetFAP(49),true);
		(*newfapframevector)[i].SetFAP(50,first_frame.GetFAP(50),true);
		}
		*/

		for (int i=3;i<69;i++)
		{

			(*newfapframevector)[FramesTotalNumber].SetFAP(i,first_frame.GetFAP(i),true);
			(*newfapframevector)[FramesTotalNumber].isKeyFrame=true;

			(*newfapframevector)[FramesTotalNumber+(int)start_time*FRAMERATE].SetFAP(i,first_frame.GetFAP(i),true);		
			(*newfapframevector)[ FramesTotalNumber+(int)start_time*FRAMERATE].isKeyFrame=true;
		}// end of for


		FaceInterpolator *FaceInterp2 = new FaceInterpolator(FramesTotalNumber*2,engine);
		FaceInterp2->InitKeyFrames();

		FaceInterp2->InterpolateFaps_NoHead(newfapframevector,/*faps,fapmask,*/ PWR->GetValue(), FLD->GetValue());

		(*fapframevector).clear();
		(*fapframevector).resize(0);

		//copy last elements
		int licznik=1;
		for(iter=newfapframevector->begin();iter!=newfapframevector->end();iter++) 
		{
			if(licznik>FramesTotalNumber) (*fapframevector).push_back(iter->clone());
			licznik++;
		}//end of for

	}//end if if first_frame=false 
	else 
	{
		FaceInterp->InterpolateFaps_NoHead(fapframevector,/*faps,fapmask,*/ PWR->GetValue(), FLD->GetValue());

	}//end of else



	if ( (inimanager.GetValueInt("USE_HEADENGINE")==0 ) )
		FaceInterp->InterpolateHead(fapframevector,/*faps,fapmask,*/faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, PWR->GetValue(), FLD->GetValue());		

	fapframevectorwithoutnoise->resize(0);		
	std::vector<FAPFrame>::iterator iter;
	for(iter=fapframevector->begin();iter!=fapframevector->end();iter++) 
		(*fapframevectorwithoutnoise).push_back(iter->clone());
			
	
		

	if(inimanager.GetValueInt("GAZE_MODEL_BN")==1)
	{
		if(this->bml==0)
			GazeSimulator->SimulGaze(phonemes);
		else
			GazeSimulator->SimulGazeBML();

		if(this->bml==0)
		{
			GazeSimulator->GazeGeneratorSpeaker(phonemes);
		}
		else
		{
			GazeSimulator->GazeGeneratorSpeakerBML();
		}

		if ((inimanager.GetValueInt("GAZE_MODEL_BN")==1)&& (first_frame_flag==false) ) 
		{
			if(this->bml==0)
			{
				if(GazeSimulator->CheckHeadPosition(phonemes,fapframevector,faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, this->FramesTotalNumber))
				{			
					FaceInterp->InterpolateHead(fapframevector,/*faps,fapmask,*/faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, PWR->GetValue(), FLD->GetValue());
				}
			}
			else
			{
				if(GazeSimulator->CheckHeadPositionBML(fapframevector,faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, this->FramesTotalNumber))
				{			
					FaceInterp->InterpolateHead(fapframevector,/*faps,fapmask,*/faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, PWR->GetValue(), FLD->GetValue());
				}

			}

		}
		//TODO: GazeGeneratorSpeaker();
	}

	if (inimanager.GetValueInt("GAZE_MODEL_BN")==1)
	{
		FaceInterp->InterpolateEyeKeyFrames(fapframevector,eyes_fapmask,eyes_faps/*,fapmask,faps*/);
		FaceInterp->InterpolateEyelidKeyFrames(fapframevector,eyelids_fapmask,eyelids_faps/*,fapmask,faps*/);
	}


		

	if(filenames.Phonemes_File!="")
	{


		FILE *pho_file;
		pho_file=fopen(filenames.Phonemes_File.c_str(),"r");
		if (pho_file)
		{
			fseek(pho_file,0L,SEEK_END);
			if (ftell(pho_file)!=0)
			{
				fclose(pho_file);
				//#ifndef face_log
				//printf("lip model started\n");
				//#else
				//				fprintf(face_log,"\nGenerating Lips Animation...\n");
				//#endif


				//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
				//tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
				//_CrtSetDbgFlag( tmpFlag );



				LipModel *Lips;

				Lips=new LipModel(fapframevector,this->first_comm_act);//, FramesTotalNumber);

				if(Lips->CalculateLips(filenames.Phonemes_File,speech_delay)==0)
				{
					delete Lips;
					return 0;
				}
				delete Lips;

				//printf("lip model ended\n");

			}
		}
		else
		{
			printf("FaceEngine: Error opening phonemes file %s\n",filenames.Phonemes_File.c_str());
		}
	}

	//save a copy of facial expressions
	
	ApplyBioBlink();
	if (inimanager.GetValueInt("HEAD_NOISE")!=0)
	{
		AddNoise(60); 
	}

	//TODO: deallocate structures

	if(fapfilename!="")
		WriteFapFile(fapfilename);


	first_frame_flag=false;

	return 1;

}

int FaceAnimationGenerator::BuildListenerFAPFile(float animationlength,phoneme *phonemes)
{

	CommunicativeAct *comm_act;
	//	float prob,probAB;
	//	float onset,apex,offset;
	phoneme* pho;
	float start_pau;
	float attack, decay, sustain, release;
	attack=0.0;
	decay=0.0;
	sustain=0.0;
	release=0.0;


	BuildChannels();

	// add 1 second

	//	MAXFRAMES+=FRAMERATE;

	if(InitializeFaps()==0)
		return 0;

	FaceInterp=new FaceInterpolator(FramesTotalNumber,engine);
	FaceInterp->InitKeyFrames();

	// INTERACTION SYNCHRONY

	if(first_comm_act==NULL)
		return 1;

	comm_act=(CommunicativeAct*)first_comm_act->next;

	while (comm_act!=NULL)
	{
		// reazioni del secondo agente al primo

		// se l'agente 1 sorride calcolo la
		// probabilita' che il secondo sorrida

		if (comm_act->active_channels.mouth==LIP_CORNER_UP)
		{
			//modifica maurizio 26-11-2003
			//CalcAttackDecaySustainRelease(comm_act,&attack,&decay,&sustain,&release);
			//CalcOnsetApexOffset2(comm_act,&onset,&apex,&offset);
			//	onset=comm_act->onset;
			//	apex=comm_act->apex;
			//	offset=comm_act->offset;
			//fine modifica
			//positive_feedback
			//FeedBack("joy",comm_act->GetPhase(caphase_attack)->duration,comm_act->GetPhase(caphase_decay)->duration+comm_act->GetPhase(caphase_sustain)->duration,comm_act->GetPhase(caphase_release)->duration, comm_act->start_time+comm_act->GetPhase(caphase_attack)->duration);
		}

		if (comm_act->active_channels.eyebrows==FROWN)
		{
			//modifica maurizio 26-11-2003
			//CalcOnsetApexOffset2(comm_act,&onset,&apex,&offset);
			//onset=comm_act->onset;
			//apex=comm_act->apex;
			//offset=comm_act->offset;
			//fine modifica
			//FeedBack("negative_feedback",onset,apex-onset+(offset/2),offset/2,comm_act->start_time+onset);
		}

		comm_act=(CommunicativeAct*)comm_act->next;

		// BACK CHANNEL

		// determino il back channel sulle pause
		// del primo agente escluse la prima
		// e l'ultima che sono relative al turn switch

		pho=phonemes;

		start_pau=0;

		// molto probabilmente sara' meglio poi usare
		// una lista che contiene tutte le pause e poi
		// scandirla almeno posso gestire la pausa successiva
		// in modo tale che 2 azioni non si sovrappongano.

		while (pho!=NULL)
		{

			if ((!strcmp(pho->pho,"pau")||
				!strcmp(pho->pho,"PAU")||
				!strcmp(pho->pho,"#")||
				!strcmp(pho->pho,"_"))
				&&
				(pho!=phonemes)&&
				(pho->next))
			{

				//			float prob;

				//float onset,apex,offset;
				int num_cycles;

				num_cycles=randomgen->GetRand01()*2.0f+1; // era 3 invece che 2

				if (start_pau+pho->dur*5<animationlength)
				{
					//onset=pho->dur*5*0.3;
					//apex=pho->dur*5*0.4;
					//offset=pho->dur*5*0.3;
					// we use this 'cause we have no comm_act for CalcOnsetApexOffset2
					//may 2005
					//((FaceEngine*)engine)->timingscalculator->CalcOnsetApexOffset(&onset,&apex,&offset,pho->dur*5);
					//FeedBack("backchannel_feedback",onset,apex,offset,start_pau); 
				}
			}
			start_pau+=pho->dur;
			pho=(phoneme*)pho->next;
		}

		//#ifdef OLDCODE
		//		FaceInterp->InterpolateKeyFrames(faps,fapmask, head_faps, head_fapmask);
		//#endif
	}

	FaceInterp->InterpolateFaps_NoHead(fapframevector,/*faps,fapmask,*/ PWR->GetValue(), FLD->GetValue());
	FaceInterp->InterpolateHead(fapframevector,/*faps,fapmask,*/faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, PWR->GetValue(), FLD->GetValue());


	// GENERAZIONE ANIMAZIONE
	if (inimanager.GetValueInt("GAZE_MODEL_BN")==1)
	{
		if(GazeSimulator->CheckHeadPosition(phonemes, fapframevector,/*faps,fapmask,*/faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, this->FramesTotalNumber))
			FaceInterp->InterpolateHead(fapframevector,/*faps,fapmask,*/faps_head,fapmask_head,faps_head_ns,fapmask_head_ns, PWR->GetValue(), FLD->GetValue());
		GazeSimulator->GazeGeneratorListener(phonemes);
	}

	if (inimanager.GetValueInt("GAZE_MODEL_BN")==1)
	{
		FaceInterp->InterpolateEyeKeyFrames(fapframevector,eyes_fapmask,eyes_faps/*,fapmask,faps*/);
		FaceInterp->InterpolateEyelidKeyFrames(fapframevector,eyelids_fapmask,eyelids_faps/*,fapmask,faps*/);
	}

	//FaceInterp->DeallocKeyFrames();

	//copy animation without the noise


	ApplyBioBlink();
	if (inimanager.GetValueInt("HEAD_NOISE")!=0)
	{
		AddNoise(60); 
	}

	//fix_gaze();

	return 1;
}

int FaceAnimationGenerator::WriteFapFile(std::string fapfilename)
{
	FAPwriter fw;
	fw.WriteToFile(fapframevector,fapfilename,25);
	return 1;
}

// For the Listener agent
// TODO: use InsertKeyFrame instead of interpolate
//! 
//! Compute the animation for the listener feedback given:
//! - signal: facial signal for giving feedback (positive_feedback, negative_feedback, backchannel_feedback)
//! - start_sec: start time of feedback
//! - onset, apex, offset
//!
/*
void FaceAnimationGenerator::FeedBack(char *signal,float onset,float apex,float offset,float start_sec)
{
int **faps_id;
//channel* fs;
int head_m_type;
float head_m_amplitude;
float head_m_period;
int head_m_result;
int i;
int start_frame,end_frame;
int onset_frame;
int offset_frame;
float head_m_duration;
float rand;
RandomGen *r;

r = new RandomGen();

rand=r->GetRand01();

faps_id=comm_act->face_expr_faps;
//faps_id=get_fap_id(signal,rand);

if (!faps_id)
return;

start_frame=start_sec*FRAMERATE;
onset_frame=start_frame+(onset*FRAMERATE);
offset_frame=onset_frame+(apex*FRAMERATE);
end_frame=offset_frame+(offset*FRAMERATE);

for (i=3;i<69;i++)
{
if (faps_id[0][i])
{
fapmask[start_frame][i]=1;						
fapmask[onset_frame][i]=1;

if (i>=19&&i<=20)
faps[onset_frame][i]=faps_id[1][i];
else
if (i>=48&&i<=50)
faps[onset_frame][i]=LIN_ATT(faps_id[1][i],onset);
else
faps[onset_frame][i]=EXP_ATT(faps_id[1][i],onset);

FaceInterp->Interpolate(faps,fapmask,i,start_frame,onset_frame);

fapmask[offset_frame][i]=1;
if (i>=19&&i<=20)
faps[offset_frame][i]=faps_id[1][i];
else
if (i>=48&&i<=50)
faps[offset_frame][i]=LIN_ATT(faps_id[1][i],offset);
else
faps[offset_frame][i]=EXP_ATT(faps_id[1][i],offset);

FaceInterp->Interpolate(faps,fapmask,i,onset_frame,offset_frame);						

fapmask[end_frame][i]=1;
FaceInterp->Interpolate(faps,fapmask,i,offset_frame,end_frame);			
}
}

head_m_duration=onset+apex+offset;

head_m_result=get_head_m_id(signal,rand,&head_m_type,&head_m_amplitude,&head_m_period);

if (head_m_result&&head_m_type==HEAD_M_NOD)
{
head_nod(EXP_ATT(head_m_amplitude,head_m_duration),0,head_m_period,head_m_duration,start_sec);
}
else
if (head_m_result&&head_m_type==HEAD_M_SHAKE)
{
head_shake(EXP_ATT(head_m_amplitude,head_m_duration),0,head_m_period,head_m_duration,start_sec);
}

}
*/

//!
//!
//! Generates keyframes for all the communicative acts 
//!
//old DrawExpressionAni
int FaceAnimationGenerator::DrawExpressionsKeyframes(int number_of_comm_act,phoneme *phonemes)
{

	int current=0;
	CommunicativeAct *comm_act;

	//	BuildChannels(); maybe not needed

	if(first_comm_act==NULL)
		return 1;

	comm_act=(CommunicativeAct*)first_comm_act->next;

	while (comm_act)
	{
		if((strcmp(comm_act->function,"text")!=0)&&(strcmp(comm_act->function,"deictic")!=0))
		{
			if (inimanager.GetValueInt("CONFLICTS_RESOLVER")==1)
			{
				if(ScanForConflicts(comm_act,number_of_comm_act)==0)
					return 0;
				if (comm_act->conflict_comm_act_list)
				{
					ResolveConflicts(comm_act,number_of_comm_act,phonemes);
				}
				ScanForOverAction(comm_act,number_of_comm_act);

			}
			DrawSingleExpr(comm_act, first_comm_act);
		}

		comm_act=(CommunicativeAct*)comm_act->next;
	}
	return 1;
}

//!
//! Get the facial channels from our FD Dictionary
//! and associates them to every expression 
//! parsed from the XML file.
//!
void FaceAnimationGenerator::BuildChannels()
{
	CommunicativeAct *comm_act;
	channel *fs; // temp channel

	if(first_comm_act==NULL)
		return;

	comm_act=(CommunicativeAct*)first_comm_act->next;

	std::string function,value;
	while (comm_act!=NULL)
	{
		function=comm_act->function;
		value=comm_act->value;
		if((function!="text")&&(function!="deictic"))
		{	
			//find channels for a given communicaive act
			FaceExpression *fe;
			fe=this->FaceDictionary->GetExpression(comm_act->function,comm_act->value);
			if(fe!=0)
				fs=fe->getChannels();
			else
				fs=0;

			if((fs!=0)||(bml==1))
			{
				if(this->bml!=1)
				{
					comm_act->active_channels.eyebrows=fs->eyebrows;
					comm_act->active_channels.gaze=fs->gaze;
					comm_act->active_channels.head_direction=fs->head_direction;
					comm_act->active_channels.head_movement=fs->head_movement;
					comm_act->active_channels.mouth=fs->mouth;
				}
				else //If the imput file is a BML we need to activate the right channel and disactivate the others
				{
					//comm_act->active_channels=(channel*)malloc(sizeof channel);

					if(comm_act->type=="face") //if communicative act concern only the face
					{
						if(fs!=0) //copy hannels from the expression to a commnicative act
						{
							comm_act->active_channels.eyebrows=fs->eyebrows;
							comm_act->active_channels.mouth=fs->mouth;
						}
						else //in no channels for the expression
						{
							comm_act->active_channels.eyebrows=1;
							comm_act->active_channels.mouth=1;
						}

						//other channels are not used
						comm_act->active_channels.gaze=NEUTRAL;
						comm_act->active_channels.head_direction=NEUTRAL;
						comm_act->active_channels.head_movement=NEUTRAL;

					}
					if(comm_act->type=="gaze") 
					{
						if(fs!=0) 
							comm_act->active_channels.gaze=fs->gaze;
						else
							comm_act->active_channels.gaze=1;

						comm_act->active_channels.mouth=NEUTRAL;
						comm_act->active_channels.eyebrows=NEUTRAL;
						comm_act->active_channels.head_direction=NEUTRAL;
						comm_act->active_channels.head_movement=NEUTRAL;
					}
					if(comm_act->type=="head")
					{			
						if(fs!=0)
						{
							comm_act->active_channels.head_direction=fs->head_direction;
							comm_act->active_channels.head_movement=fs->head_movement;
						}
						else
						{
							comm_act->active_channels.head_direction=1;
							comm_act->active_channels.head_movement=1;
						}

						comm_act->active_channels.eyebrows=NEUTRAL;
						comm_act->active_channels.mouth=NEUTRAL;
						comm_act->active_channels.gaze=NEUTRAL;
					}
				}
			}

			if((function=="affect")&&(value.substr(0,8)=="complex:")&&(bml==0))
			{
				//TRICK: what to do for complex expressions?
				comm_act->active_channels.eyebrows=1;
				comm_act->active_channels.gaze=NEUTRAL;
				comm_act->active_channels.mouth=1;
				comm_act->active_channels.head_direction=NEUTRAL;
				comm_act->active_channels.head_movement=NEUTRAL;
			}
		}
		comm_act=(CommunicativeAct*)comm_act->next;
	}
}

int FaceAnimationGenerator::CommunicativeActsOverlap(CommunicativeAct *comm_act1,CommunicativeAct *comm_act2)
{
	int start1,end1,start2,end2;
	start1=(int)(comm_act1->start_time*FRAMERATE);
	end1=(int)((comm_act1->start_time+comm_act1->dur_time)*FRAMERATE);
	start2=(int)(comm_act2->start_time*FRAMERATE);
	end2=(int)((comm_act2->start_time+comm_act2->dur_time)*FRAMERATE);

	if((start1<=start2)&&(end1>=end2))
		return 1;
	if((start2<=start1)&&(end2>=end1))
		return 1;
	if((start1>=start2)&&(start1<end2)&&(end1>end2))
		return 1;
	if((start2>=start1)&&(start2<end1)&&(end2>end1))
		return 1;

	return 0;
}
void FaceAnimationGenerator::ScanForOverAction(CommunicativeAct *comm_act,int number_of_expressions)
{
	CommunicativeAct *comm_act2;

	comm_act2=(CommunicativeAct*)first_comm_act->next;

	while (comm_act2!=NULL)
	{

		if (CommunicativeActsOverlap(comm_act,comm_act2)&&
			(strcmp(comm_act2->function,"text")!=0)&&(strcmp(comm_act2->function,"deictic")!=0)&&(comm_act!=comm_act2))
		{
			int mask=OverActionOnChannel(comm_act,comm_act2);
			if(mask!=0)
			{
				if(strcmp(comm_act->function,"boundary")==0)
				{
					if((mask&0x01)==0x01) //entrambi attivi sulle eyebrows
						comm_act->active_channels.eyebrows=-1;

					if((mask&0x02)==0x02) //entrambi attivi sul gaze
						comm_act->active_channels.gaze=-1;

					if((mask&0x04)==0x04) //entrambi attivi sulla head_direction
						comm_act->active_channels.head_direction=-1;

					if((mask&0x08)==0x08) //entrambi attivi sulla head_movement
						comm_act->active_channels.head_movement=-1;

					if((mask&0x10)==0x10) //entrambi attivi sulla mouth
						comm_act->active_channels.mouth=-1;
				}
				else
				{
					if((mask&0x01)==0x01) //entrambi attivi sulle eyebrows
						comm_act2->active_channels.eyebrows=-1;

					if((mask&0x02)==0x02) //entrambi attivi sul gaze
						comm_act2->active_channels.gaze=-1;

					if((mask&0x04)==0x04) //entrambi attivi sulla head_direction
						comm_act2->active_channels.head_direction=-1;

					if((mask&0x08)==0x08) //entrambi attivi sulla head_movement
						comm_act2->active_channels.head_movement=-1;

					if((mask&0x10)==0x10) //entrambi attivi sulla mouth
						comm_act2->active_channels.mouth=-1;
				}	
			}

		}
		comm_act2=(CommunicativeAct*)comm_act2->next;
	}
}

//!
//! Build a list of expressions which conflicts with expression 'comm_act'
//! in the comm_act->conflicts_expr_list field
//!
int FaceAnimationGenerator::ScanForConflicts(CommunicativeAct *comm_act,int number_of_expressions)
{
	CommunicativeAct *comm_act2;
	int i=0;


	comm_act2=(CommunicativeAct*)first_comm_act->next;


	while ((comm_act2!=NULL)&&(i<number_of_expressions))
	{

		if (CommunicativeActsOverlap(comm_act,comm_act2)&&
			(strcmp(comm_act2->function,"text")!=0)&&(strcmp(comm_act2->function,"deictic")!=0)&&(comm_act!=comm_act2)&&
			ConflictOnChannel(comm_act,comm_act2))
		{
			if (comm_act->conflict_comm_act_list==NULL)
			{
				comm_act->conflict_comm_act_list=(void**)malloc(sizeof(CommunicativeAct*)*number_of_expressions);
				if (!comm_act->conflict_comm_act_list)
				{
#ifdef OUTPUT
					fprintf(face_log,"out of memory\n");
#endif
					return 0;
				}			
				else
				{
					int j;
					for (j=0;j<number_of_expressions;j++)
						//suspect
						comm_act->conflict_comm_act_list[j]=NULL;
				}
			}
			//suspect
			comm_act->conflict_comm_act_list[i]=comm_act2;			
			i++;
		}
		comm_act2=(CommunicativeAct*)comm_act2->next;
	}
	return 1;
}



//!
//! Resolve the facial conflicts for the expression 'comm_act'
//!
void FaceAnimationGenerator::ResolveConflicts(CommunicativeAct *comm_act,int number_of_expressions,phoneme *phonemes)
{
	printf("Belief Network is not used :\n");
}



void FaceAnimationGenerator::maskrefresh(CommunicativeAct *other_comm_act,int position)
{

	//facial expression

	if  (
		( strcmp (other_comm_act->type.c_str(), "face" ) ==0 ) 
		&&
		( strcmp (other_comm_act->function, "affect" )==0 )
		)
	{
		for (int i=0;i<69;i++)  
		{
			if ( (i!=23)&&(i!=24)&&(i!=25)&&(i!=26)&&(i!=48)&&(i!=49)&&(i!=50) )  
				(joinmask)[position][i]=1;
			// else 				(joinmask)[position][i]=0;
		}
	}

	//head
	if  (
		( strcmp (other_comm_act->type.c_str(), "head" ) ==0 ) 
		||
		( strcmp (other_comm_act->function, "head" )==0 )
		)
		{	
		for (int i=0;i<69;i++) 
		{
			if ( (i==48)||(i==49)||(i==50) ) 			
				(joinmask)[position][i]=1;
			//else (joinmask)[position][i]=0;
		}
		}

		if  (
			( strcmp (other_comm_act->type.c_str(), "gaze" ) ==0 ) 
			||
			( strcmp (other_comm_act->function, "gaze" )==0 )
			)
		{		
			for (int i=0;i<69;i++) 	
			{
				if ( (i==23)||(i==24)||(i==25)||(i==26) ) 
					(joinmask)[position][i]=1;		
			//else 	(joinmask)[position][i]=0;
			}
		}
}//met

//!
//! Generates keyframes for the expression 'comm_act' 
//!
void FaceAnimationGenerator::DrawSingleExpr(CommunicativeAct *comm_act, CommunicativeAct *first_comm_act)
{

	
	if (comm_act->dur_time<MIN_HEAD_TIME)//&&comm_act->active_channels.head_direction!=NEUTRAL)
	{
		comm_act->active_channels.head_direction=NEUTRAL;
		comm_act->active_channels.head_movement=NEUTRAL;
		comm_act->active_channels.eyebrows=NEUTRAL;
	}

	if (comm_act->GetPhaseDur(caphase_attack)==0)  // no enough time to draw the expression!
		return;

	if (comm_act->activation<(1 - OAC->GetValue())) 
		return;


	//if the neiberhood comm acts start when it finishes etc.
	//int jointleft=0;
	//int jointright=0;

	int joint[5];
	//int joinmask[69];

	for (int i=0;i<69;i++) 
	{
		joinmask[0][i]=0;
		joinmask[1][i]=0;
		joinmask[2][i]=0;
		joinmask[3][i]=0;
		joinmask[4][i]=0;
	}

	joint[0]=0;
	joint[1]=0;
	joint[2]=0;
	joint[3]=0;
	joint[4]=0;

	if (inimanager.GetValueInt("JOIN_FACES_ON")==1) 
	{

		CommunicativeAct* other_comm_act=first_comm_act;

			float comm_start,comm_end,comm_attack,comm_decay,comm_sustain, comm_s,comm_d,comm_a, comm_r;

			comm_a=comm_act->GetPhaseDur(caphase_attack);
			comm_d=comm_act->GetPhaseDur(caphase_decay);
			comm_s=comm_act->GetPhaseDur(caphase_sustain);
			comm_r=comm_act->GetPhaseDur(caphase_release);

			comm_start=comm_act->start_time;
			comm_attack=(comm_start+comm_a);
			comm_decay=(comm_start+comm_a+comm_d);
			comm_sustain=(comm_start+comm_a+comm_d+comm_s);
			comm_end=(comm_start+comm_act->dur_time);

		while (other_comm_act) 	
		{
		
			float other_start, other_end, other_attack,other_decay,other_sustain, other_s,other_d,other_a, other_r;

			other_a=other_comm_act->GetPhaseDur(caphase_attack);
			other_d=other_comm_act->GetPhaseDur(caphase_decay);
			other_s=other_comm_act->GetPhaseDur(caphase_sustain);
			other_r=other_comm_act->GetPhaseDur(caphase_release);

			other_start=other_comm_act->start_time;
			other_attack=(other_start+other_a);
			other_decay=(other_start+other_a+other_d);
			other_sustain=(other_start+other_a+other_d+other_s);
			other_end=(other_start+other_comm_act->dur_time);

			if (other_comm_act!=comm_act) 
			{
				// do not consider 			
				if ( strcmp(other_comm_act->type.c_str(),"")==0 ) 
				{
					other_comm_act=(CommunicativeAct*)other_comm_act->next;	
					continue;
				}
				
				//This code allow to generate animation for signals which overlap
				//signals can overlap on the same modality (head, gaze, face)
				//or on different modalities

				//in the first case some keyframes of the first (in time) expression are cancelled
				//in the second case the overlapping is possible without changing the animation
				
				//this part of code is used instead of using the channels
				//thus the probability is not used and only the start and end time are considered
				//there is no prority related to the meaning
				//and you can not force greta to finish one expression if the other one starts to early

				
				//if one is head other face etc we dont do 
				/*
				if  (
				( strcmp (other_comm_act->type.c_str(), "face" ) ==0 ) 
				&&
				( strcmp (other_comm_act->function, "affect" )==0 )
				)
				{
				for (int i=0;i<69;i++)  
				if ( (i!=23)&&(i!=24)&&(i!=25)&&(i!=26)&&(i!=48)&&(i!=49)&&(i!=50) )  joinmask[i]=1;
				}

				if  (
				( strcmp (other_comm_act->type.c_str(), "head" ) ==0 ) 
				||
				( strcmp (other_comm_act->function, "head" )==0 )
				)
				for (int i=0;i<69;i++) 
				{
				if ( (i==48)||(i==49)||(i==50) ) 
				{
				joinmask[i]=1;
				}
				}

				if  (
				( strcmp (other_comm_act->type.c_str(), "gaze" ) ==0 ) 
				||
				( strcmp (other_comm_act->function, "gaze" )==0 )
				)
				for (int i=0;i<69;i++) 	
				if ( (i==23)||(i==24)||(i==25)||(i==26) ) 
				joinmask[i]=1;


				*/
				/*
				if  (
				( strcmp (other_comm_act->type.c_str(), "face" ) ==0 ) 
				&&
				( strcmp ( "face" , comm_act->type.c_str() ) ==0 ) 
				)
				{
				for (int i=0;i<69;i++)  if ( (i!=23)&&(i!=24)&&(i!=25)&&(i!=26)&&(i!=48)&&(i!=49)&&(i!=50) )  joinmask[i]=1;
				}


				if  (
				( strcmp (other_comm_act->type.c_str(), "head" ) ==0 ) 
				&&
				( strcmp ( "head" , comm_act->type.c_str() ) ==0 ) 
				)
				for (int i=0;i<69;i++) 
				{
				if ( (i==48)||(i==49)||(i==50) ) 
				{
				joinmask[i]=1;
				}
				}

				if  (
				( strcmp (other_comm_act->type.c_str(), "face" ) ==0 ) 
				&&
				( strcmp ( "head" , comm_act->type.c_str() ) ==0 ) 
				)
				for (int i=0;i<69;i++) 
				if ( (i!=23)&&(i!=24)&&(i!=25)&&(i!=26)&&(i!=48)&&(i!=49)&&(i!=50) )  joinmask[i]=1;

				if  (
				( strcmp (other_comm_act->type.c_str(), "head" ) ==0 ) 
				&&
				( strcmp ( "face" , comm_act->type.c_str() ) ==0 ) 
				)

				for (int i=0;i<69;i++)
				if ( (i==48)||(i==49)||(i==50) )  joinmask[i]=1;

				if  (
				( strcmp (other_comm_act->type.c_str(), "face" ) ==0 ) 
				&&
				( strcmp ( "head" , comm_act->type.c_str() ) ==0 ) 
				)

				for (int i=0;i<69;i++)  if ( (i!=23)&&(i!=24)&&(i!=25)&&(i!=26)&&(i!=48)&&(i!=49)&&(i!=50) )  joinmask[i]=1;

				for (int i=0;i<69;i++)
				if ( (i==48)||(i==49)||(i==50) )  joinmask[i]=1;

				if  (
				( strcmp (other_comm_act->type.c_str(), "face" ) ==0 ) 
				&&
				( strcmp ( "head" , comm_act->type.c_str() ) ==0 ) 
				)

				for (int i=0;i<69;i++)  if ( (i!=23)&&(i!=24)&&(i!=25)&&(i!=26)&&(i!=48)&&(i!=49)&&(i!=50) )  joinmask[i]=1;

				*/



				//TO DO: add bpth start at the same time but finish differently

				//TO DO: add both stop at the same time

				//BOTH START AND STOP at the same time
				if (
					(other_comm_act-> start_time == comm_act -> start_time) 			
					&& 
					( 
					( comm_act->start_time + comm_act->dur_time) == (other_comm_act->start_time + other_comm_act->dur_time)
					)
					)
					//TO DO!!!!!!
					; 
				else 

				{

					// THISCOMM IS INCLUDED IN OTHER -> CANCEL BEGIN AND END
					if (
						(other_comm_act-> start_time <= comm_act -> start_time) 
						&& 
						( comm_act->start_time <= (other_comm_act->start_time + other_comm_act->dur_time))
						&&
						(other_comm_act->start_time <= ( comm_act->start_time + comm_act->dur_time) ) 
						&& 
						( 
						( comm_act->start_time + comm_act->dur_time) <= (other_comm_act->start_time + other_comm_act->dur_time)
						)
						)  
					{
						maskrefresh(other_comm_act,0);
						joint[0]=1;
						maskrefresh(other_comm_act,4);
						joint[4]=1;
					}
					else 
					{

						//tHIS COMM COMPLETELY INCLUDES THE OTHER ONE
						if (
							(comm_act-> start_time <= other_comm_act -> start_time) 
							&& 
							( other_comm_act->start_time <= (comm_act->start_time + comm_act->dur_time))
							&&
							(comm_act->start_time <= ( other_comm_act->start_time + other_comm_act->dur_time) ) 
							&& 
							( 
							( other_comm_act->start_time + other_comm_act->dur_time) <= (comm_act->start_time + comm_act->dur_time)
							)
							) 
						{

							//cut all inside
							if (
								(other_comm_act-> start_time <= comm_attack) 
								&& 
								( comm_attack <= (other_comm_act->start_time + other_comm_act->dur_time))								)   
							{
								maskrefresh(other_comm_act,1);
								joint[1]=1;
							}

							if (
								(other_comm_act-> start_time <= comm_decay) 
								&& 
								( comm_decay <= (other_comm_act->start_time + other_comm_act->dur_time))								)  
							{ 
								maskrefresh(other_comm_act,2);					
								joint[2]=1;
							}

							if (
								(other_comm_act-> start_time <= comm_sustain) 
								&& 
								( comm_sustain <= (other_comm_act->start_time + other_comm_act->dur_time))								)  
							{ 						
								joint[3]=1;
								maskrefresh(other_comm_act,3);	
							}

						} 
						else 
						{
							//IF THERE IS ONLY SOME PARTIAL INCLUDING AND IF "THIS" STARTS BEFORE THE "OTHER" STARTS
							if (
								(comm_act-> start_time <= other_comm_act -> start_time) 
								&& 
								( other_comm_act->start_time <= (comm_act->start_time + comm_act->dur_time))
								) 
							{

								if (
									(other_comm_act-> start_time <= comm_start) 
									&& 
									( comm_start <= (other_comm_act->start_time + other_comm_act->dur_time))		)  
								{								
									joint[0]=1;
									maskrefresh(other_comm_act,0);					
								}

								if (
									(other_comm_act-> start_time <= comm_attack) 
									&& 
									( comm_attack <= (other_comm_act->start_time + other_comm_act->dur_time))	)  
								{
									joint[1]=1;
									maskrefresh(other_comm_act,1);				
								}

								if (
									(other_comm_act-> start_time <= comm_decay) 
									&& 
									( comm_decay <= (other_comm_act->start_time + other_comm_act->dur_time))	)   
								{
									joint[2]=1;
									maskrefresh(other_comm_act,2);				
								}

								if (
									(other_comm_act-> start_time <= comm_sustain) 
									&& 
									( comm_sustain <= (other_comm_act->start_time + other_comm_act->dur_time))								)   
								{
									joint[3]=1;
									maskrefresh(other_comm_act,3);				
								}

								if (
									(other_comm_act-> start_time <= comm_end) 
									&& 
									( comm_end <= (other_comm_act->start_time + other_comm_act->dur_time)))  
								{																
									joint[4]=1;
									maskrefresh(other_comm_act,4);				
								}

							} 
							else
							{						
								if (
									(other_comm_act-> start_time <= comm_start) 
									&& 
									( comm_start <= (other_comm_act->start_time + other_comm_act->dur_time))	
									)
								{
									joint[0]=1;						
									maskrefresh(other_comm_act,0);


									if ( strcmp (other_comm_act->function, comm_act->function )!=0) 
									{
										//if different modalities then


										if (
											(other_comm_act-> start_time <= comm_attack) 
											&& 
											( comm_attack <= (other_comm_act->start_time + other_comm_act->dur_time))	
											)
										{
											joint[1]=1;						
											maskrefresh(other_comm_act,1);								
										}

										if (
											(other_comm_act-> start_time <= comm_decay) 
											&& 
											( comm_decay <= (other_comm_act->start_time + other_comm_act->dur_time))	
											)
										{
											joint[2]=1;						
											maskrefresh(other_comm_act,2);								
										}
										if (
											(other_comm_act-> start_time <= comm_sustain) 
											&& 
											( comm_sustain <= (other_comm_act->start_time + other_comm_act->dur_time))	
											)
										{
											joint[3]=1;						
											maskrefresh(other_comm_act,3);								
										}

									}

								}

							}//end else

						}//end else

					}//end else

				}//end else			

			}	//end if head != comm

			other_comm_act=(CommunicativeAct*)other_comm_act->next;	

		}// end of while

	} // end if join_faces_on = 1

	DrawFacialDisplay(comm_act,comm_act->start_time,comm_act->dur_time,joint);
	if(inimanager.GetValueInt("USE_HEADENGINE")==0)
		DrawHeadNodsAndShakes(comm_act);

}



//!
//! Generates keyframes for the expression 'comm_act' in the range (start_time,end_time).
//! It calculates the keyframe values for attack, decay, sustain, release
//! The parameter 'flag' can take the following values:
//! - 0 : generate keyframes for boundaries
//! - 1 : generate keyframes for all the others expression (facial signals)
//!
void FaceAnimationGenerator::DrawFacialDisplay(CommunicativeAct *comm_act,float start_time,float dur_time, int joint[])
{
	int **faps_id;
	int fap_behind, fap_forward;

	if(comm_act->expressivitySPC>=-1 && comm_act->expressivitySPC<=1)
		SPC->SetValue(comm_act->expressivitySPC);

	//0 : generate keyframes for boundaries
	//1 : generate keyframes for all the others expression (facial signals)

	faps_id=0;

	std::string function=comm_act->function;
	//if the expression is not a deictic
	if(function!="deictic")
	{
		faps_id=comm_act->face_expr_faps;
	}
	else
	{
		//TODO: deictic expressions
	}

	if (faps_id==0)
	{
#ifdef OUTPUT
		fprintf(face_log,"warning: expression %s/%s is not defined in the face library!\n",comm_act->function,comm_act->value);
#endif
	}
	else
	{
		int i;
		float attack,decay,sustain,release;
		int start_frame,end_frame,attack_frame, decay_frame, sustain_frame;
		int temp_fapmask[69];

		memset(temp_fapmask,0,69);

		attack=comm_act->GetPhaseDur(caphase_attack);
		decay=comm_act->GetPhaseDur(caphase_decay);
		sustain=comm_act->GetPhaseDur(caphase_sustain);
		release=comm_act->GetPhaseDur(caphase_release);

		start_frame=start_time*FRAMERATE;
		attack_frame=(start_time+attack)*FRAMERATE;
		decay_frame=(start_time+attack+decay)*FRAMERATE;
		sustain_frame=(start_time+attack+decay+sustain)*FRAMERATE;
		end_frame=(start_time+dur_time)*FRAMERATE;


		//printf("\n%s %s  S:%d  A:%d  D:%d  S:%d E:%d\n", comm_act->function, comm_act->value, start_frame, attack_frame, decay_frame,sustain_frame, end_frame);

		for (i=3;i<69;i++)
		{
			if (faps_id[0][i]==1)
				temp_fapmask[i]=1;						
		}
		

		int j;

		if (inimanager.GetValueInt("CHANNELS_OFF")!=1) 
		{

			if (comm_act->active_channels.eyebrows==NEUTRAL)
			{

				// Eyebrows deactivate: 31 32 33 34 35 36 37 38 

				for (j=31;j<=38;j++)
					temp_fapmask[j]=0;
			}
			if (comm_act->active_channels.mouth==NEUTRAL)
			{
				// Mouth deactivate: 3 4 5 6 7 8 9 10 11 12 13 16 18 51 52 53 54 55 56 57 58 59 60 

				for (j=3;j<=13;j++)
					temp_fapmask[j]=0;
				temp_fapmask[16]=0;
				temp_fapmask[18]=0;
				for (j=51;j<=60;j++)
					temp_fapmask[j]=0;
			}

			if (comm_act->active_channels.gaze==NEUTRAL)
			{
				// Gaze deactivate: 23 24 25 26

				for (j=23;j<=26;j++)
					temp_fapmask[j]=0;
			}

			if (comm_act->active_channels.head_direction==NEUTRAL)
			{
				// Head direction deactivate: 48 49 50

				for (j=48;j<=50;j++)
				{
					temp_fapmask[j]=0;
				}
			}
		}//end of CHANNELS_OFF

		

		for (i=3;i<69;i++)
		{
			if (temp_fapmask[i]&&faps_id[0][i])
			{	
				if(i!=48 && i!=49 && i!=50 && i!=19 && i!=20 && i!=21 && i!=22)
				{
					j=start_frame;
					fap_behind=-1;
					fap_forward=-1;
					while(j>=(start_frame-5) && j>=0)
					{
						if((*fapframevector)[j].FAPs[i].active==1)
						{
							//comincio da j perché devo vedere anche lì se già c'era un punto
							fap_behind=j;
							break;
						}
						/*
						if(fapmask[j][i]==1)
						{
						//comincio da j perché devo vedere anche lì se già c'era un punto
						fap_behind=j;
						break;
						}*/
						j-=1;
					}

					j=start_frame;
					while(j<=(start_frame+5) && j<this->FramesTotalNumber)
					{
						if((*fapframevector)[j].FAPs[i].active)
						{
							fap_forward=j;
							break;
						}
						/*
						if(fapmask[j][i]==1)
						{
						fap_forward=j;
						break;
						}
						*/
						j+=1;
					}


					if(fap_behind==-1 && fap_forward==-1)
					{
						//fapmask[start_frame][i]=1;
						if (joinmask[0][i]!=1) (*fapframevector)[start_frame].FAPs[i].active=true;
					}
					else
					{
						//fapmask[start_frame][i]=0;
						if (joinmask[0][i]==1) (*fapframevector)[start_frame].FAPs[i].active=false;
					}

					//se c'è un precedente ritorno a 0 troppo vicino lo tolgo per passare gradatamente
					//da un movimento all'altro
					/*
					if(fap_behind!=-1 && fap_behind!=0 && faps[fap_behind][i]==0)
					{
					//fapmask[fap_behind][i]=0;
					(*fapframevector)[fap_behind].FAPs[i].active=false;
					}
					*/

					if(fap_behind!=-1 && fap_behind!=0 && (((*fapframevector)[fap_behind].GetFAP(i))==0))
					{
						//fapmask[fap_behind][i]=0;
						(*fapframevector)[fap_behind].FAPs[i].active=false;
					}


					if(start_frame==0)
					{
						//fapmask[start_frame][i]=1;
						if (joinmask[0][i]!=1) (*fapframevector)[start_frame].FAPs[i].active=true;
					}

				}

				if (i>=19&&i<=22)
				{
					//fapmask[start_frame][i]=1;
					//faps[start_frame][i]=0;
					if (joinmask[0][i]!=1) (*fapframevector)[start_frame].SetFAP(i,0);

					//fapmask[attack_frame][i]=1;
					if (joinmask[1][i]!=1) (*fapframevector)[attack_frame].FAPs[i].active=true;

					if(sustain_frame!=attack_frame)
					{
						//fapmask[sustain_frame][i]=1;
						if (joinmask[3][i]!=1) (*fapframevector)[sustain_frame].FAPs[i].active=true;
					}

					//fapmask[end_frame][i]=1;
					if (joinmask[4][i]!=1) (*fapframevector)[end_frame].FAPs[i].active=true;


					if((faps_id[1][i] * SPC->GetScaledValue())<=1024)
					{

						//faps[attack_frame][i]=faps_id[1][i] * SPC;
						if (joinmask[1][i]!=1) (*fapframevector)[attack_frame].SetFAP(i,faps_id[1][i] * SPC->GetScaledValue());
						if(sustain_frame!=attack_frame)
						{
							//faps[sustain_frame][i]=faps_id[1][i] * SPC;
							if (joinmask[3][i]!=1) (*fapframevector)[sustain_frame].SetFAP(i,faps_id[1][i] * SPC->GetScaledValue());
						}
					}
					else
					{
						//faps[attack_frame][i]=faps_id[1][i];
						if (joinmask[1][i]!=1) (*fapframevector)[attack_frame].SetFAP(i,faps_id[1][i]);
						if(sustain_frame!=attack_frame)
						{
							//faps[sustain_frame][i]=faps_id[1][i];
							if (joinmask[3][i]!=1) (*fapframevector)[sustain_frame].SetFAP(i,faps_id[1][i]);
						}
					}

					//faps[end_frame][i]=0;
					if (joinmask[4][i]!=1) (*fapframevector)[end_frame].SetFAP(i,0);
				}
				else
					if ((i>=3&&i<=13)||i==16||(i>=51&&i<=60))
					{
						if((*fapframevector)[start_frame].FAPs[i].active)
						{
							//faps[start_frame][i]=0;
							if (joinmask[0][i]!=1) (*fapframevector)[start_frame].SetFAP(i,0);
						}
						/*
						if(fapmask[start_frame][i]==1)
						{
						//faps[start_frame][i]=0;
						(*fapframevector)[start_frame].SetFAP(i,0);
						}*/

						//fapmask[attack_frame][i]=1;
						if (joinmask[0][i]!=1) (*fapframevector)[start_frame].FAPs[i].active=true;

						if(decay_frame!=attack_frame)
						{
							//fapmask[decay_frame][i]=1;
							if (joinmask[2][i]!=1) (*fapframevector)[decay_frame].FAPs[i].active=true;
						}
						if(sustain_frame!=attack_frame)
						{
							//fapmask[sustain_frame][i]=1;
							if (joinmask[3][i]!=1) (*fapframevector)[sustain_frame].FAPs[i].active=true;
						}

						//fapmask[end_frame][i]=1;
						if (joinmask[4][i]!=1) (*fapframevector)[end_frame].FAPs[i].active=true;

						//faps[attack_frame][i]=(faps_id[1][i] * SPC->GetScaledExpressivity()) + (faps_id[1][i]*SPC)*PWR->GetScaledExpressivity();
						if (joinmask[1][i]!=1) (*fapframevector)[attack_frame].SetFAP(i,(faps_id[1][i] * SPC->GetScaledValue()) + (faps_id[1][i]*SPC->GetScaledValue())*PWR->GetScaledValue());

						if(decay_frame!=attack_frame)
						{
							//faps[decay_frame][i]=faps_id[1][i] * SPC->GetScaledExpressivity();
							if (joinmask[2][i]!=1) (*fapframevector)[decay_frame].SetFAP(i,faps_id[1][i] * SPC->GetScaledValue());
						}
						if(sustain_frame!=attack_frame)
						{
							//faps[sustain_frame][i]=faps_id[1][i] * SPC->GetScaledExpressivity();
							if (joinmask[3][i]!=1) (*fapframevector)[sustain_frame].SetFAP(i,faps_id[1][i] * SPC->GetScaledValue());
						}
						//faps[end_frame][i]=0;
						if (joinmask[4][i]!=1) (*fapframevector)[end_frame].SetFAP(i,0);
					}
					else
						if (i>=48&&i<=50)
						{
							if(inimanager.GetValueInt("USE_HEADENGINE")==0)
								//if (false)

							{
								j=start_frame;
								fap_behind=-1;
								fap_forward=-1;
								while(j>=(start_frame-5) && j>0)
								{
									if(fapmask_head[j][i-48]==1)
									{
										//comincio da j perché devo vedere anche lì se già c'era un punto
										fap_behind=j;
										break;
									}
									j-=1;
								}

								j=start_frame;
								while(j<=(start_frame+5) && j<this->FramesTotalNumber)
								{
									if(fapmask_head[j][i-48]==1)
									{
										fap_forward=j;
										break;
									}
									j+=1;
								}

								if(fap_behind==-1 && fap_forward==-1)
									fapmask_head[start_frame][i-48]=1;
								else
									fapmask_head[start_frame][i-48]=0;


								//se c'è un precedente ritorno a 0 troppo vicino lo tolgo per passare gradatamente
								//da un movimento all'altro
								if(fap_behind!=-1 && fap_behind!=0 && faps_head[fap_behind][i-48]==0)
									fapmask_head[fap_behind][i-48]=0;

								if(start_frame==0)
									fapmask_head[start_frame][i-48]=1;

								if(fapmask_head[start_frame][i-48]==1)
									faps_head[start_frame][i-48]=0;

								fapmask_head[attack_frame][i-48]=1;
								if(decay_frame!=attack_frame)
									fapmask_head[decay_frame][i-48]=1;
								if(sustain_frame!=attack_frame)
									fapmask_head[sustain_frame][i-48]=1;

								fapmask_head[end_frame][i-48]=1;

								faps_head[attack_frame][i-48]=(NO_ATT(faps_id[1][i],attack) * SPC->GetScaledValue()) + (NO_ATT(faps_id[1][i],attack)*SPC->GetScaledValue())*PWR->GetScaledValue()*0.2;
								if(decay_frame!=attack_frame)
									faps_head[decay_frame][i-48]=NO_ATT(faps_id[1][i],attack) * SPC->GetScaledValue();
								if(sustain_frame!=attack_frame)
									faps_head[sustain_frame][i-48]=NO_ATT(faps_id[1][i],attack) * SPC->GetScaledValue();

								faps_head[end_frame][i-48]=0;
							}

						}
						else
						{
							if((*fapframevector)[start_frame].FAPs[i].active)
							{
								if (joinmask[0][i]!=1) (*fapframevector)[start_frame].SetFAP(i,0);
							}
							/*
							if(fapmask[start_frame][i]==1)
							{
							faps[start_frame][i]=0;
							(*fapframevector)[start_frame].SetFAP(i,0);
							}*/

							//fapmask[attack_frame][i]=1;
							if (joinmask[1][i]!=1) (*fapframevector)[attack_frame].FAPs[i].active=true;

							if(decay_frame!=attack_frame && (i!=23 && i!=24 && i!=25 && i!=26))
							{
								//fapmask[decay_frame][i]=1;
								if (joinmask[2][i]!=1) (*fapframevector)[decay_frame].FAPs[i].active=true;
							}
							if(sustain_frame!=attack_frame)
							{
								//fapmask[sustain_frame][i]=1;
								if (joinmask[3][i]!=1) (*fapframevector)[sustain_frame].FAPs[i].active=true;
							}
							//fapmask[end_frame][i]=1;
							if (joinmask[4][i]!=1) (*fapframevector)[end_frame].FAPs[i].active=true;


							//faps[attack_frame][i]=(EXP_ATT(faps_id[1][i],attack) * SPC->GetScaledExpressivity()) + (EXP_ATT(faps_id[1][i],attack)*SPC->GetScaledExpressivity())*PWR->GetScaledExpressivity();
							if (joinmask[1][i]!=1) (*fapframevector)[attack_frame].SetFAP(i,(EXP_ATT(faps_id[1][i],attack) * SPC->GetScaledValue()) + (EXP_ATT(faps_id[1][i],attack)*SPC->GetScaledValue())*PWR->GetScaledValue());

							if(i!=23 && i!=24 && i!=25 && i!=26)
							{
								if(decay_frame!=attack_frame)
								{
									//faps[decay_frame][i]=EXP_ATT(faps_id[1][i],attack) * SPC->GetScaledExpressivity();
									if (joinmask[3][i]!=1) (*fapframevector)[decay_frame].SetFAP(i,EXP_ATT(faps_id[1][i],attack) * SPC->GetScaledValue());
								}
							}
							if(sustain_frame!=attack_frame)
							{
								//faps[sustain_frame][i]=EXP_ATT(faps_id[1][i],attack) * SPC->GetScaledExpressivity();
								if (joinmask[3][i]!=1) (*fapframevector)[sustain_frame].SetFAP(i,EXP_ATT(faps_id[1][i],attack) * SPC->GetScaledValue());
							}
							//faps[end_frame][i]=0;
							if (joinmask[4][i]!=1) (*fapframevector)[end_frame].SetFAP(i,0);
						}
			}		
		}

		//added by radek		

		//printf("\n%s %s  S:%d  A:%d  D:%d  S:%d E:%d\n", comm_act->function, comm_act->value, start_frame, attack_frame, decay_frame,sustain_frame, end_frame);
		//printf("\n  1:%d  2:%d  3:%d  4:%d 5:%d\n", joint[0],joint[1],joint[2],joint[3],joint[4]);

		//printf("\n keyframes %d \n", comm_act->phases.size());
		//printf("SPC is:%d \n", SPC->GetScaledValue());

		/*
		for (int i=3;i<69;i++)
		{	

			printf("\n  1:%d  2:%d  3:%d  4:%d 5:%d\n", joinmask[0][i],joinmask[1][i],joinmask[2][i],joinmask[3][i],joinmask[4][i]);

			//printf ("joinmask[i] : %d, \n",joinmask[i]);

			if (joint[0] *joinmask[0][i]==1)
			{
				//(*fapframevector)[start_frame].FAPs[i].value=0;
				(*fapframevector)[start_frame].FAPs[i].active=false;
				if ( (i==48)||(i==49)||(i==50) ) fapmask_head[start_frame][i-48]=0;
			}
			if (joint[1]*joinmask[1][i]==1) 
			{
				//(*fapframevector)[attack_frame].FAPs[i].value=0;
				(*fapframevector)[attack_frame].FAPs[i].active=false;
				if ( (i==48)||(i==49)||(i==50) )  fapmask_head[attack_frame][i-48]=0;
			}
			if (joint[2]*joinmask[2][i]==1)
			{
				//(*fapframevector)[decay_frame].FAPs[i].value=0;
				(*fapframevector)[decay_frame].FAPs[i].active=false;
				if ( (i==48)||(i==49)||(i==50) )  fapmask_head[decay_frame][i-48]=0;
			}
			if (joint[3]*joinmask[3][i]==1) 
			{
				//(*fapframevector)[sustain_frame].FAPs[i].value=0;
				(*fapframevector)[sustain_frame].FAPs[i].active=false;
				if ( (i==48)||(i==49)||(i==50) )   fapmask_head[sustain_frame][i-48]=0;
			}			
			if (joint[4]*joinmask[4][i]==1)
			{
				//(*fapframevector)[end_frame].FAPs[i].value=0;
				(*fapframevector)[end_frame].FAPs[i].active=false;			
				if ( (i==48)||(i==49)||(i==50) )  fapmask_head[end_frame][i-48]=0;
			}
		}
	*/
	}
}


//!
//! Generates keyframes (head movement) for the expression 'comm_act' in the range (start_time,end_time).
//! 
void FaceAnimationGenerator::DrawHeadNodsAndShakes(CommunicativeAct *comm_act)
{
	float start_time, dur_time;
	float degrees;
	start_time=comm_act->start_time;
	dur_time=comm_act->dur_time;

	//	if (!strcmp(comm_act->function,"deictic")&&comm_act->value!=NULL)
	//		deictic(comm_act);
	//	else

	if (strcmp(comm_act->function,"deictic")!=0)
	{

		//head_m_result=get_head_m_id(comm_act->value,comm_act->prob,&head_m_type,&head_m_amplitude,&head_m_period);

		if(comm_act->active_channels.head_movement==NEUTRAL)
			comm_act->head_m_type=HEAD_NOTHING;

		if(min((exp(dur_time/comm_act->head_m_period)/MAX_EXP_ATT_TIME)*abs(comm_act->head_m_amplitude),abs(comm_act->head_m_amplitude))==abs(comm_act->head_m_amplitude))
		{
			degrees=comm_act->head_m_amplitude;
		}
		else
		{
			degrees=(exp(dur_time/comm_act->head_m_period)/MAX_EXP_ATT_TIME)*comm_act->head_m_amplitude;
		}

		if (comm_act->head_m_type==HEAD_NOD)
		{
			head_nod(degrees,0,comm_act->head_m_period,dur_time,start_time);
		}
		else
			if (comm_act->head_m_type==HEAD_SHAKE)
			{
				head_shake(degrees,0,comm_act->head_m_period,dur_time,start_time);
			}
	}
}

//!
//! Generate keyframes for the 'head nod' movement starting from time
//! 'start_sec' with duration 'secs' and period 'cycle_secs' given the 
//! final position in horizantal (deg1) and vertical (deg2) degrees.
//!
void FaceAnimationGenerator::head_nod(float deg1,float deg2,float cycle_duration,float secs,float start_sec)
{

	int i;
	float start_next_cycle;
	int num_of_cycles;

	if (secs<cycle_duration) 
	{
		// esperimento
		if (FaceInterp->CheckForFreeHead(start_sec,secs))
		{
			num_of_cycles=1;
			cycle_duration=secs;
		}
		else
			return;
	}
	else
		num_of_cycles=secs/cycle_duration;  //durata in secondi di un ciclo

	start_next_cycle=start_sec;


	for (i=0;i<num_of_cycles;i++)
	{
		if(inimanager.GetValueInt("USE_HEADENGINE")==0)
		{
			if (deg1<0)
			{
				head_move(deg2,deg1,0,cycle_duration/4,start_next_cycle);
				head_move(deg2,0,0,cycle_duration/4,start_next_cycle+cycle_duration/4);
				head_move(deg2,-deg1,0,cycle_duration/4,start_next_cycle+cycle_duration/2);
				head_move(0,0,0,cycle_duration/4,start_next_cycle+cycle_duration/4+cycle_duration/2);
				start_next_cycle+=cycle_duration;
			}
			else
				if (deg1>=0)
				{
					head_move(deg2,deg1,0,cycle_duration/2,start_next_cycle);
					head_move(0,0,0,cycle_duration/2,start_next_cycle+cycle_duration/2);
					start_next_cycle+=cycle_duration;
				}
		}

	}
}

//!
//! Generate keyframes for the head movement starting from time
//! 'start_sec' with duration 'secs' given the final position 
//! in horizontal (deg1) and vertical (deg2) degrees.
//! Used for head nod & head shake.
//!

void FaceAnimationGenerator::head_move(float deg1,float deg2,float deg3,float duration,float starttime)
{
	float rad1,rad2,rad3;
	int keyframe1;
	int keyframe2;

	//	deg1=((exp(secs)/2.0)*deg1);
	//	deg2=((exp(secs)/2.0)*deg2);


	rad1=((M_PI/180)*deg1)*100000.0;
	rad2=((M_PI/180)*deg2)*100000.0;
	rad3=((M_PI/180)*deg3)*100000.0;


	keyframe1=starttime*FRAMERATE;
	keyframe2=(starttime+duration)*FRAMERATE;

	//Elisabetta december 2005
	fapmask_head_ns[keyframe1][0]=1;
	fapmask_head_ns[keyframe1][1]=1;

	fapmask_head_ns[keyframe2][0]=1;
	fapmask_head_ns[keyframe2][1]=1;

	faps_head_ns[keyframe2][0]=rad2;
	faps_head_ns[keyframe2][1]=rad1;

	FaceInterp->InsertKeyFrame(HEADX,keyframe1,0);
	FaceInterp->InsertKeyFrame(HEADX,keyframe2,0);

	FaceInterp->InsertKeyFrame(HEADY,keyframe1,0);
	FaceInterp->InsertKeyFrame(HEADY,keyframe2,0);
}

//!
//! Generate keyframes for the 'head shake' movement starting from time
//! 'start_sec' with duration 'secs' and period 'cycle_secs' given the 
//! final position in horizantal (deg1) and vertical (deg2) degrees.
//!

void FaceAnimationGenerator::head_shake(float deg1,float deg2,float cycle_secs,float secs,float start_sec)
{
	int i;
	float start_next_cycle;
	int num_of_cycles;

	//	if (secs<0.5) 
	//		return;

	if (secs<cycle_secs) 
	{
		return;
		//		num_of_cycles=1;
		//		cycle_secs=secs;
	}
	else
		num_of_cycles=secs/cycle_secs;

	start_next_cycle=start_sec;

	for (i=0;i<num_of_cycles;i++)
	{
		
		head_move(deg1,deg2,0,cycle_secs/4,start_next_cycle);
		head_move(0,deg2,0,cycle_secs/4,start_next_cycle+cycle_secs/4);
		head_move(-deg1,deg2,0,cycle_secs/4,start_next_cycle+cycle_secs/2);
		head_move(0,0,0,cycle_secs/4,start_next_cycle+cycle_secs/4+cycle_secs/2);
	
		start_next_cycle+=cycle_secs;
	}



}

//!
//! Compute the biological blink starting from frame 'frameind'
//!
void FaceAnimationGenerator::bioblink(int frameind)
{	
	if (
		(*fapframevector)[frameind].GetFAP(19)>700||
		(*fapframevector)[frameind+1].GetFAP(19)>700||
		(*fapframevector)[frameind+2].GetFAP(19)>700||
		(*fapframevector)[frameind+3].GetFAP(19)>700||
		(*fapframevector)[frameind+4].GetFAP(19)>700||
		(*fapframevector)[frameind+5].GetFAP(19)>700||
		(*fapframevector)[frameind+6].GetFAP(19)>700||
		(*fapframevector)[frameind+7].GetFAP(19)>700||
		(*fapframevector)[frameind].GetFAP(20)>700||
		(*fapframevector)[frameind+1].GetFAP(20)>700||
		(*fapframevector)[frameind+2].GetFAP(20)>700||
		(*fapframevector)[frameind+3].GetFAP(20)>700||
		(*fapframevector)[frameind+4].GetFAP(20)>700||
		(*fapframevector)[frameind+5].GetFAP(20)>700||
		(*fapframevector)[frameind+6].GetFAP(20)>700||
		(*fapframevector)[frameind+7].GetFAP(20)>700)
		return;

	(*fapframevector)[frameind+1].SetFAP(19,(*fapframevector)[frameind+1].GetFAP(19) + (1024-(*fapframevector)[frameind+1].GetFAP(19))/3);
	(*fapframevector)[frameind+1].SetFAP(20,(*fapframevector)[frameind+1].GetFAP(20) + (1024-(*fapframevector)[frameind+1].GetFAP(20))/3);
	(*fapframevector)[frameind+2].SetFAP(19,(*fapframevector)[frameind+1].GetFAP(19) + (1024-(*fapframevector)[frameind+1].GetFAP(19))*2/3);
	(*fapframevector)[frameind+2].SetFAP(20,(*fapframevector)[frameind+1].GetFAP(20) + (1024-(*fapframevector)[frameind+1].GetFAP(20))*2/3);
	(*fapframevector)[frameind+3].SetFAP(19,1024);
	(*fapframevector)[frameind+3].SetFAP(20,1024);
	(*fapframevector)[frameind+4].SetFAP(19,1024);
	(*fapframevector)[frameind+4].SetFAP(20,1024);
	(*fapframevector)[frameind+5].SetFAP(19,(*fapframevector)[frameind+6].GetFAP(19) + (1024-(*fapframevector)[frameind+6].GetFAP(19))*2/3);
	(*fapframevector)[frameind+5].SetFAP(20,(*fapframevector)[frameind+6].GetFAP(20) + (1024-(*fapframevector)[frameind+6].GetFAP(20))*2/3);
	(*fapframevector)[frameind+6].SetFAP(19,(*fapframevector)[frameind+6].GetFAP(19) + (1024-(*fapframevector)[frameind+6].GetFAP(19))*2/3);
	(*fapframevector)[frameind+6].SetFAP(20,(*fapframevector)[frameind+6].GetFAP(20) + (1024-(*fapframevector)[frameind+6].GetFAP(20))*2/3);
}					

//!
//! Compute biological blink on all the whole face animation
//!
void FaceAnimationGenerator::ApplyBioBlink()
{
	int frameind;

	//srand((unsigned)time(NULL)); 
	//init_genrand(clock());
	frameind=randomgen->GetRand01()*40.0f;
	while (frameind<FramesTotalNumber-8)
	{
		bioblink(frameind);
		frameind+=71+(randomgen->GetRand01()*50); // Avviene ogni 4.8 sec ~= 120 frames
		//		frameind+=(200*randomgen->GetRand01());
	}
}

//!
//! Given two communicative acts returns a conflict mask 
//!
int FaceAnimationGenerator::ConflictOnChannel(CommunicativeAct *comm_act,CommunicativeAct *comm_act2)
{
	int conflict_mask;

	conflict_mask=0x00;

	if (comm_act->active_channels.eyebrows!=NEUTRAL&&
		comm_act2->active_channels.eyebrows!=NEUTRAL&&
		comm_act->active_channels.eyebrows!=comm_act2->active_channels.eyebrows)
		conflict_mask|=0x01;
	if (comm_act->active_channels.gaze!=NEUTRAL&&
		comm_act2->active_channels.gaze!=NEUTRAL&&
		comm_act->active_channels.gaze!=comm_act2->active_channels.gaze)
		conflict_mask|=0x02;
	if (comm_act->active_channels.head_direction!=NEUTRAL&&
		comm_act2->active_channels.head_direction!=NEUTRAL&&
		comm_act->active_channels.head_direction!=comm_act2->active_channels.head_direction)
		conflict_mask|=0x04;
	if (comm_act->active_channels.head_movement!=NEUTRAL&&
		comm_act2->active_channels.head_movement!=NEUTRAL&&
		comm_act->active_channels.head_movement!=comm_act2->active_channels.head_movement)
		conflict_mask|=0x08;
	if (comm_act->active_channels.mouth!=NEUTRAL&&
		comm_act2->active_channels.mouth!=NEUTRAL&&
		comm_act->active_channels.mouth!=comm_act2->active_channels.mouth)
		conflict_mask|=0x10;
	return conflict_mask;
}

int FaceAnimationGenerator::OverActionOnChannel(CommunicativeAct *comm_act,CommunicativeAct *comm_act2)
{
	int conflict_mask;

	conflict_mask=0x00;

	if (comm_act->active_channels.eyebrows!=NEUTRAL&&
		comm_act2->active_channels.eyebrows!=NEUTRAL&&
		comm_act->active_channels.eyebrows==comm_act2->active_channels.eyebrows)
		conflict_mask|=0x01;
	if (comm_act->active_channels.gaze!=NEUTRAL&&
		comm_act2->active_channels.gaze!=NEUTRAL&&
		comm_act->active_channels.gaze==comm_act2->active_channels.gaze)
		conflict_mask|=0x02;
	if (comm_act->active_channels.head_direction!=NEUTRAL&&
		comm_act2->active_channels.head_direction!=NEUTRAL&&
		comm_act->active_channels.head_direction==comm_act2->active_channels.head_direction)
		conflict_mask|=0x04;
	if (comm_act->active_channels.head_movement!=NEUTRAL&&
		comm_act2->active_channels.head_movement!=NEUTRAL&&
		comm_act->active_channels.head_movement==comm_act2->active_channels.head_movement)
		conflict_mask|=0x08;
	if (comm_act->active_channels.mouth!=NEUTRAL&&
		comm_act2->active_channels.mouth!=NEUTRAL&&
		comm_act->active_channels.mouth==comm_act2->active_channels.mouth)
		conflict_mask|=0x10;
	return conflict_mask;
}

//!
//! Returns TRUE if expression 'comm_act' appears in the range (start_time,end_time)
//!
int FaceAnimationGenerator::CommActAroundRange(CommunicativeAct *comm_act,float start_time,float end_time)
{
	int s_ca,e_ca;
	int a,b;
	float realduration;

	realduration=comm_act->dur_time;

	s_ca=(int)(FRAMERATE*(comm_act->start_time));
	e_ca=(int)(FRAMERATE*(comm_act->start_time+comm_act->dur_time));

	a=(int)(FRAMERATE*start_time);
	b=(int)(FRAMERATE*end_time);

	if (s_ca<=a&&b<=e_ca)
		//if ((s_e>=a)&&(e_e<=b))?????
		return 1;
	return 0;
}

//!
//! Initialise the structure for storing computed fapmask and faps for each frame
//! 
int FaceAnimationGenerator::InitializeFaps()
{
	int frameind;
	
	if(allocatedstructures==true)
		Deallocate();

	allocatedstructures=true;

	fapframevector=new std::vector<FAPFrame>(FramesTotalNumber,FAPFrame());
	fapframevectorwithoutnoise=new std::vector<FAPFrame>(FramesTotalNumber,FAPFrame());

	for(frameind=0;frameind<FramesTotalNumber;frameind++)
		(*fapframevector)[frameind].SetFAP(0,frameind);

	faps_head=(int**)malloc(sizeof(int*)*FramesTotalNumber);
	fapmask_head=(int**)malloc(sizeof(int*)*FramesTotalNumber);
	faps_head_ns=(int**)malloc(sizeof(int*)*FramesTotalNumber);
	fapmask_head_ns=(int**)malloc(sizeof(int*)*FramesTotalNumber);
	//
	head_faps[0]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	head_faps[1]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	head_fapmask[0]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	head_fapmask[1]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_faps[0]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_faps[1]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_faps[2]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_faps[3]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_fapmask[0]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_fapmask[1]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_fapmask[2]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyes_fapmask[3]=(int*)malloc(sizeof(int)*FramesTotalNumber);

	//Elisabetta 10-10-2005
	//For the eyelids
	eyelids_faps[0]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyelids_faps[1]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyelids_faps[2]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyelids_faps[3]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyelids_fapmask[0]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyelids_fapmask[1]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyelids_fapmask[2]=(int*)malloc(sizeof(int)*FramesTotalNumber);
	eyelids_fapmask[3]=(int*)malloc(sizeof(int)*FramesTotalNumber);


	if (/*!faps||*/
		!fapmask_head||!faps_head||
		!fapmask_head_ns||!faps_head_ns||
		!head_faps[0]||!head_faps[1]||
		!head_fapmask[0]||!head_fapmask[1]||
		!eyes_faps[0]||!eyes_faps[1]||!eyes_faps[2]||!eyes_faps[3]||
		!eyes_fapmask[0]||!eyes_fapmask[1]||!eyes_fapmask[2]||!eyes_fapmask[3]||
		!eyelids_faps[0]||!eyelids_faps[1]||!eyelids_faps[2]||!eyelids_faps[3]||
		!eyelids_fapmask[0]||!eyelids_fapmask[1]||!eyelids_fapmask[2]||!eyelids_fapmask[3])
	{
		printf("ERROR: can't allocate memory for faps\n");
		return 0;
	}

	//Elisabetta 2005
	for(frameind=0;frameind<FramesTotalNumber;frameind++) 
	{
		fapmask_head[frameind]=(int*)malloc(sizeof(int)*3);
		if (!fapmask_head[frameind])
		{
			printf("ERROR: can't allocate memory for fapmask_head\n");
			return 0;
		}

		faps_head[frameind]=(int*)malloc(sizeof(int)*3);
		if (!faps_head[frameind])
		{
			printf("ERROR: can't allocate memory for faps_head\n");
			return 0;
		}

		fapmask_head_ns[frameind]=(int*)malloc(sizeof(int)*3);
		if (!fapmask_head_ns[frameind])
		{
			printf("ERROR: can't allocate memory for fapmask_head_ns\n");
			return 0;
		}

		faps_head_ns[frameind]=(int*)malloc(sizeof(int)*3);
		if (!faps_head_ns[frameind])
		{
			printf("ERROR: can't allocate memory for faps_head_ns\n");
			return 0;
		}
	}

	//

	for(frameind=0;frameind<FramesTotalNumber;frameind++) 
	{
		fapmask_head[frameind][0]=0;
		fapmask_head[frameind][1]=0;
		fapmask_head[frameind][2]=0;

		faps_head[frameind][0]=0;
		faps_head[frameind][1]=0;
		faps_head[frameind][2]=0;

		fapmask_head_ns[frameind][0]=0;
		fapmask_head_ns[frameind][1]=0;
		fapmask_head_ns[frameind][2]=0;

		faps_head_ns[frameind][0]=0;
		faps_head_ns[frameind][1]=0;
		faps_head_ns[frameind][2]=0;
		//

		head_faps[0][frameind]=0;
		head_faps[1][frameind]=0;
		head_fapmask[0][frameind]=0;
		head_fapmask[1][frameind]=0;
		eyes_faps[0][frameind]=0;
		eyes_faps[1][frameind]=0;
		eyes_faps[2][frameind]=0;
		eyes_faps[3][frameind]=0;
		eyes_fapmask[0][frameind]=0;
		eyes_fapmask[1][frameind]=0;
		eyes_fapmask[2][frameind]=0;
		eyes_fapmask[3][frameind]=0;

		//Elisabetta 11-10-2005
		//for the eyelids
		eyelids_faps[0][frameind]=0;
		eyelids_faps[1][frameind]=0;
		eyelids_faps[2][frameind]=0;
		eyelids_faps[3][frameind]=0;
		eyelids_fapmask[0][frameind]=0;
		eyelids_fapmask[1][frameind]=0;
		eyelids_fapmask[2][frameind]=0;
		eyelids_fapmask[3][frameind]=0;

	}


	joinmask[0]=(int*)malloc(sizeof(int)*69);
	joinmask[1]=(int*)malloc(sizeof(int)*69);
	joinmask[2]=(int*)malloc(sizeof(int)*69);
	joinmask[3]=(int*)malloc(sizeof(int)*69);
	joinmask[4]=(int*)malloc(sizeof(int)*69);

	return 1;
}


//!
//! Generate a 'look at' eyes movement starting from 'start_sec' with duration 'secs'
//! It computes also an additional 'look away' head movement if a previous
//! 'look away' eyes movement last for a predefined time
//!
void FaceAnimationGenerator::LookAt(float secs,float start_sec)
{
	int k1,k2,k;
	int onset;

	k1=(int)(start_sec*FRAMERATE);
	k2=(int)(secs*FRAMERATE);

	if(k2>FramesTotalNumber)
		k2=FramesTotalNumber;

	if(k1==0)
		eyes_key(0, 0, k1);

	if(abs(eyes_faps[0][k1])>abs(eyes_faps[2][k1]))
		onset=abs(eyes_faps[0][k1])/EYES_SPEED;
	else
		onset=abs(eyes_faps[2][k1])/EYES_SPEED;

	if(onset==0 || (k1+onset)>= k2) 
		onset=(k2-k1)/2;

	//eyes_key(-faps[k1+onset][49], -faps[k1+onset][48],k1+onset);

	eyes_key(-(*fapframevector)[k1+onset].GetFAP(49), -(*fapframevector)[k1+onset].GetFAP(48),k1+onset);

	for(k=k1+onset+1;k<k2;k++)
	{
		if ((*fapframevector)[k].FAPs[48].active || (*fapframevector)[k].FAPs[49].active)
		{
			eyes_key(-(*fapframevector)[k1].GetFAP(49), -(*fapframevector)[k1].GetFAP(48),k1);
		}
	}
	eyes_key(-(*fapframevector)[k2].GetFAP(49), -(*fapframevector)[k2].GetFAP(48),k2);
}

void FaceAnimationGenerator::eyes_key(int deg1, int deg2, int k)
{
	fprintf(face_log,"eyes_move(hor:%d,ver:%d,at:%d)\n",deg1,deg2,k);

	eyes_fapmask[0][k]=1;
	eyes_fapmask[1][k]=1;
	eyes_fapmask[2][k]=1;
	eyes_fapmask[3][k]=1;

	eyes_faps[0][k]=deg1;
	eyes_faps[1][k]=deg1;
	eyes_faps[2][k]=deg2;
	eyes_faps[3][k]=deg2;

	eyelids_fapmask[0][k]=1;
	eyelids_fapmask[1][k]=1;
	eyelids_faps[0][k]=(int)(((float)deg2)/60);
	eyelids_faps[1][k]=(int)(((float)deg2)/60);
}

void FaceAnimationGenerator::eyes_position(int k, int *eye_roll, int *eye_pitch, float sec)
{
	float result;
	int direction_eye_X,direction_eye_Y;
	int direction_head_X,direction_head_Y;
	float MAX_LOOK_AWAY_DEGREE=1000.0f;

	direction_eye_X=0;
	direction_eye_Y=0;
	direction_head_X=0;
	direction_head_Y=0;
	/*
	if(faps[k][23]!=0)
	direction_eye_X=faps[k][23]/abs(faps[k][23]);
	*/
	if((*fapframevector)[k].GetFAP(23)!=0)
		direction_eye_X=(*fapframevector)[k].GetFAP(23)/abs((*fapframevector)[k].GetFAP(23));
	/*
	if(faps[k][25]!=0)
	direction_eye_Y=faps[k][25]/abs(faps[k][25]);
	*/
	if((*fapframevector)[k].GetFAP(25)!=0)
		direction_eye_Y=(*fapframevector)[k].GetFAP(25)/abs((*fapframevector)[k].GetFAP(25));
	/*
	if(faps[k][49]!=0)
	direction_eye_X=faps[k][49]/abs(faps[k][49]);
	*/
	if((*fapframevector)[k].GetFAP(49)!=0)
		direction_eye_X=(*fapframevector)[k].GetFAP(49)/abs((*fapframevector)[k].GetFAP(49));
	/*
	if(faps[k][48]!=0)
	direction_eye_Y=faps[k][48]/abs(faps[k][48]);
	*/
	if((*fapframevector)[k].GetFAP(48)!=0)
		direction_eye_Y=(*fapframevector)[k].GetFAP(48)/abs((*fapframevector)[k].GetFAP(48));

	if(direction_eye_X==0&&direction_eye_Y==0&&direction_head_X==0&&direction_head_Y==0)
	{
		result=randomgen->GetRand01();
		if(result<0.4f) direction_eye_X=-1;
		else 
			if(result>0.6f) direction_eye_X=1;
			else direction_eye_X=0;
			result=randomgen->GetRand01();
			if(result<0.33333f) direction_eye_Y=-1;
			else 
				if(result>0.66666f) direction_eye_Y=1;
				else direction_eye_Y=0;
	}
	else
		if((direction_eye_X==0&&direction_eye_Y==0)&&(direction_head_X!=0||direction_head_Y!=0))
		{
			direction_eye_X=direction_head_X;
			direction_eye_Y=direction_head_Y;
		}

		//left-right
		*eye_roll=direction_eye_X*(EXP_ATT(10000,sec/10.0f));//((r->GetRand01()*10000)+4000);
		//up-down
		*eye_pitch=direction_eye_Y*(EXP_ATT(17000,sec/10.0f));//((r->GetRand01()*14000)+4000);

}

void FaceAnimationGenerator::LookAway(float secs,float start_sec)
{
	int k1,k2,k;
	int eye_roll, eye_pitch;
	int last_key_frame;
	int onset;

	eye_roll=0; eye_pitch=0;
	k1=(int)(start_sec*FRAMERATE);
	k2=(int)(secs*FRAMERATE);


	if(k2>FramesTotalNumber)
		k2=FramesTotalNumber;

	if(k1==0)
		eyes_key((*fapframevector)[k1].GetFAP(49), (*fapframevector)[k1].GetFAP(48), k1);

	eyes_position(k1, &eye_roll, &eye_pitch, secs);

	if(abs(eye_roll)>abs(eye_pitch))
		onset=abs(eye_roll)/EYES_SPEED;
	else
		onset=abs(eye_pitch)/EYES_SPEED;

	if(onset==0 || (k1+onset)>= k2) 
		onset=(k2-k1)/2;

	eyes_position(k1+onset, &eye_roll, &eye_pitch, secs);
	eyes_key(eye_pitch, eye_roll, k1+onset);

	last_key_frame=k1+onset;

	for(k=k1+onset+1; k<=k2; k++)
	{
		if (((k-last_key_frame>30) && (k2-k>(k2-k1)/3)))// ||
			//((abs(faps[k][48]+faps[k][25])<500)&&(abs(faps[k][49]+faps[k][23])<500)&&(k2-k>(k2-k1)/3)))
			//if((head_fapmask[1][k]!=0||head_fapmask[0][k]!=0)&&(head_faps[1][k]!=0||head_faps[0][k]!=0))
		{
			eyes_key(eyes_faps[0][last_key_frame], eyes_faps[2][last_key_frame], k-20);

			eyes_position(k, &eye_roll, &eye_pitch, secs);//-((float)k/FRAMERATE));//
			eyes_key(eye_pitch, eye_roll, k);

			last_key_frame=k;
			eyes_key(eyes_faps[0][last_key_frame], eyes_faps[2][last_key_frame], k-10);
		}

	}

	if (last_key_frame!=k2)
		eyes_key(eyes_faps[0][last_key_frame], eyes_faps[2][last_key_frame], k2);
}


// TODO: eyes noise on an variable point in the space
//! Add noise to face animation given a 'speed' variable (noise keframe)
void FaceAnimationGenerator::AddNoise(int speed)
{
	int framecount;
	int NEXT_NOISE_KEYFRAME;

	NEXT_NOISE_KEYFRAME=speed;

	float MAX_NOISE_HEAD=inimanager.GetValueFloat("HEAD_NOISE");

	init_tables();

	framecount=0;

	//srand((unsigned)time(NULL)); 

	while (framecount<FramesTotalNumber)
	{
		if (framecount+NEXT_NOISE_KEYFRAME>=FramesTotalNumber)
		{
			head_noise_table[FramesTotalNumber-1][0]=0;
			head_noise_table[FramesTotalNumber-1][1]=0;
			head_noise_table[FramesTotalNumber-1][2]=0;

			eyes_noise_table[FramesTotalNumber-1][0]=0;
			eyes_noise_table[FramesTotalNumber-1][1]=0;

			interpolate_noise(framecount,FramesTotalNumber-1);
			framecount=FramesTotalNumber;
		}
		else
			if (framecount+NEXT_NOISE_KEYFRAME<FramesTotalNumber)
			{
				head_noise_table[framecount+NEXT_NOISE_KEYFRAME][0]=MAX_NOISE_HEAD*PerlinNoise_1D((float)(framecount+NEXT_NOISE_KEYFRAME)+48);
				head_noise_table[framecount+NEXT_NOISE_KEYFRAME][1]=MAX_NOISE_HEAD*PerlinNoise_1D((float)(framecount+NEXT_NOISE_KEYFRAME)+49);
				head_noise_table[framecount+NEXT_NOISE_KEYFRAME][2]=MAX_NOISE_HEAD*PerlinNoise_1D((float)(framecount+NEXT_NOISE_KEYFRAME)+50);

				eyes_noise_table[framecount+NEXT_NOISE_KEYFRAME][0]=MAX_NOISE_EYES*PerlinNoise_1D((float)(framecount+NEXT_NOISE_KEYFRAME)+23);
				eyes_noise_table[framecount+NEXT_NOISE_KEYFRAME][1]=MAX_NOISE_EYES*PerlinNoise_1D((float)(framecount+NEXT_NOISE_KEYFRAME)+25);

				interpolate_noise(framecount,framecount+NEXT_NOISE_KEYFRAME);
				framecount+=NEXT_NOISE_KEYFRAME;
			}

	}
	apply_noise();
	dealloc_tables();
}

//! Deallocate noise structures
void FaceAnimationGenerator::dealloc_tables()
{
	int frameind;

	for(frameind=0;frameind<FramesTotalNumber;frameind++) 
	{
		free(head_noise_table[frameind]);
		free(eyes_noise_table[frameind]);
	}	

	free(head_noise_table);
	free(eyes_noise_table);
}

//! Computes Perlin's noise
float FaceAnimationGenerator::PerlinNoise_1D(float x)
{
	float total,p,frequency,amplitude;
	int i,n;


	total = 0;
	p = persistence;
	n = Number_Of_Octaves - 1;

	for (i=0;i<=n;i++)
	{
		//CHRIS MSVC2005 fix
		frequency = (float)pow((float)2,i);
		amplitude = (float)pow(p,i);

		total = total + InterpolatedNoise1(x * frequency) * amplitude;
	}

	return total;
}

//! Computed interpolated noise given input 'x'
float FaceAnimationGenerator::InterpolatedNoise1(float x)
{ 
	int integer_X;
	float fractional_X;
	float v1,v2;

	integer_X    = x;
	fractional_X = (x - (float)integer_X);

	v1 = SmoothedNoise1((float)integer_X);
	v2 = SmoothedNoise1((float)(integer_X + 1));

	return InterpolateNoise(v1 , v2 , fractional_X);
}

//! Computed smoothed noise given input 'x'
float FaceAnimationGenerator::SmoothedNoise1(float x)
{
	return Noise(x)*.5  +  Noise(x-1.0)*.25  +  Noise(x+1.0)*.25;
}

//! Random noise generator given an input 'x'
float FaceAnimationGenerator::Noise(int x)
{
	//	x = (x<<13) ^ x;
	//	return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);    
	return (float)((randomgen->GetRand01() * (x + x)) - x) / x;
}

//! Interpolating function called by InterpolatedNoise1
float FaceAnimationGenerator::InterpolateNoise(float a,float b,float x)
{
	return  a*(1-x) + b*x;
}

//! Add the noise on head/eyes faps
void FaceAnimationGenerator::apply_noise()
{
	int frameind;

	for (frameind=0;frameind<FramesTotalNumber;frameind++)
	{

		(*fapframevector)[frameind].AddToFAP(48,head_noise_table[frameind][0]);
		(*fapframevector)[frameind].AddToFAP(49,head_noise_table[frameind][1]);
		(*fapframevector)[frameind].AddToFAP(50,head_noise_table[frameind][2]);
		(*fapframevector)[frameind].AddToFAP(23,eyes_noise_table[frameind][0]);
		(*fapframevector)[frameind].AddToFAP(24,eyes_noise_table[frameind][0]);
		(*fapframevector)[frameind].AddToFAP(25,eyes_noise_table[frameind][1]);
		(*fapframevector)[frameind].AddToFAP(26,eyes_noise_table[frameind][1]);
	}
}

//! Interpolate noise from keyframe 'k1' to keyframe 'k2'
void FaceAnimationGenerator::interpolate_noise(int k1, int k2)
{
	int frameind;
	float ft;
	float f;

	for (frameind=k1;frameind<=k2;frameind++)
	{
		ft=((float)(frameind-k1)/((float)(k2-k1)))*M_PI;

		f=(1.0-cos(ft))*0.5;

		// noise on x,y,z axes

		head_noise_table[frameind][0]=head_noise_table[k1][0]*(1.0-f)+head_noise_table[k2][0]*f;
		head_noise_table[frameind][1]=head_noise_table[k1][1]*(1.0-f)+head_noise_table[k2][1]*f;
		head_noise_table[frameind][2]=head_noise_table[k1][2]*(1.0-f)+head_noise_table[k2][2]*f;

		// noise on x,y axes
		eyes_noise_table[frameind][0]=eyes_noise_table[k1][0]*(1.0-f)+eyes_noise_table[k2][0]*f;
		eyes_noise_table[frameind][1]=eyes_noise_table[k1][1]*(1.0-f)+eyes_noise_table[k2][1]*f;
	}
}

//! Initialise noise structures
void FaceAnimationGenerator::init_tables()
{
	int frameind;

	head_noise_table=(float**)malloc(sizeof(float*)*FramesTotalNumber);
	if (!head_noise_table)
	{
		printf("ERROR: can't allocate memory!\n");
		exit(2);
	}

	eyes_noise_table=(float**)malloc(sizeof(float*)*FramesTotalNumber);
	if (!eyes_noise_table)
	{
		printf("ERROR: can't allocate memory!\n");
		exit(2);
	}

	for(frameind=0;frameind<FramesTotalNumber;frameind++) 
	{
		head_noise_table[frameind]=(float*)malloc(sizeof(float)*3);
		if (!head_noise_table[frameind])
		{
			printf("ERROR: can't allocate memory!\n");
			exit(2);
		}

		eyes_noise_table[frameind]=(float*)malloc(sizeof(float)*2);
		if (!eyes_noise_table[frameind])
		{
			printf("ERROR: can't allocate memory!\n");
			exit(2);
		}
	}	

	for(frameind=0;frameind<FramesTotalNumber;frameind++) 
	{
		head_noise_table[frameind][0]=0.0;
		head_noise_table[frameind][1]=0.0;
		head_noise_table[frameind][2]=0.0;
		eyes_noise_table[frameind][0]=0.0;
		eyes_noise_table[frameind][1]=0.0;
	} 
}

void FaceAnimationGenerator::DebugConflicts(CommunicativeAct *comm_act,float start_time,float end_time,int number_of_expressions)
{
	CommunicativeAct *comm_act2;
	int i;

#ifdef OUTPUT
	fprintf(face_log,"\nE: %s - T: %s - S: %.2f - E: %.2f\n",comm_act->function,comm_act->value,start_time,end_time);	
	fprintf(face_log,"E G HD HM M\n");
	if (comm_act->active_channels.eyebrows!=NEUTRAL)
		fprintf(face_log,"%d ",comm_act->active_channels.eyebrows);
	else
		fprintf(face_log,"  ");
	if (comm_act->active_channels.gaze!=NEUTRAL)		
		fprintf(face_log,"%d ",comm_act->active_channels.gaze);
	else
		fprintf(face_log,"  ");
	if (comm_act->active_channels.head_direction!=NEUTRAL)
		fprintf(face_log,"%d  ",comm_act->active_channels.head_direction);
	else
		fprintf(face_log,"   ");

	if (comm_act->active_channels.head_movement!=NEUTRAL)
		fprintf(face_log,"%d  ",comm_act->active_channels.head_movement);
	else
		fprintf(face_log,"   ");
	if (comm_act->active_channels.mouth!=NEUTRAL)
		fprintf(face_log,"%d\n",comm_act->active_channels.mouth);
	else
		fprintf(face_log,"  \n");
#endif

	for (i=0;i<number_of_expressions;i++)
	{
		if (comm_act->conflict_comm_act_list)
		{
			comm_act2=(CommunicativeAct*)((CommunicativeAct **)comm_act->conflict_comm_act_list)[i];

			if (comm_act2==NULL) ;
			//	printf("\tNO CONFLICTS!\n");
			else
			{
#ifdef OUTPUT
				fprintf(face_log,"\tCONFLICT->E: %s - T: %s - S: %.2f - E: %.2f\n",comm_act2->function,comm_act2->value,comm_act2->start_time,comm_act2->start_time+comm_act2->dur_time);	
				if (comm_act2->active_channels.eyebrows!=NEUTRAL)
					fprintf(face_log,"%d ",comm_act2->active_channels.eyebrows);
				else
					fprintf(face_log,"  ");
				if (comm_act2->active_channels.gaze!=NEUTRAL)		
					fprintf(face_log,"%d ",comm_act2->active_channels.gaze);
				else
					fprintf(face_log,"  ");
				if (comm_act2->active_channels.head_direction!=NEUTRAL)
					fprintf(face_log,"%d  ",comm_act2->active_channels.head_direction);
				else
					fprintf(face_log,"   ");			
				if (comm_act2->active_channels.head_movement!=NEUTRAL)
					fprintf(face_log,"%d  ",comm_act2->active_channels.head_movement);
				else
					fprintf(face_log,"   ");
				if (comm_act2->active_channels.mouth!=NEUTRAL)
					fprintf(face_log,"%d\n",comm_act2->active_channels.mouth);
				else
					fprintf(face_log,"  \n");
				fprintf(face_log,"\n");
#endif
			}

		}

	}
#ifdef OUTPUT
	fprintf(face_log,"\n");
#endif
}

//given a frame number it forces all the faps to reach the value of zero
//within one second. it helps to cut the animation at a given point
void FaceAnimationGenerator::CutFAPs(int n)
{
	float decrements[69];
	//if there is at least one second between the given frame number and the end
	if((n>0)&&(n<(this->FramesTotalNumber-25)))
	{
		//CHRIS MSVC2005 fix
		int j = 0;
		for(j=1; j<69; j++)
		{
			if((*fapframevector)[n-1].FAPs[j].active)
			{
				decrements[j]=(*fapframevector)[n-1].GetFAP(j)/25.0f;
			}
		}

		//CHRIS MSVC2005 fix
		int i = 0;
		for(i=n; i<=n+25; i++)
		{
			for(j=1; j<69; j++)
			{
				if((*fapframevector)[i-1].FAPs[j].active==0)
				{
					(*fapframevector)[i].SetFAP(j,0,0);
				}
				else
				{
					(*fapframevector)[i].SetFAP(j,(*fapframevector)[i-1].GetFAP(j)-decrements[j]);
				}
			}
		}
		for(i=n+26; i<this->FramesTotalNumber; i++)
		{
			//CHRIS MSVC2005 fix
			for(j=1; j<69; j++)
			{
				//fapmask[i][j]=0;
				//faps[i][j]=0;
				(*fapframevector)[i].SetFAP(j,0,0);
			}
		}
	}
}

void FaceAnimationGenerator::Deallocate()
{	

	if(allocatedstructures==false)
	{
		return;
	}

	free(head_fapmask[0]);		
	free(head_fapmask[1]);		
	free(head_faps[0]);		
	free(head_faps[1]);		
	free(eyes_fapmask[0]);		
	free(eyes_fapmask[1]);		
	free(eyes_fapmask[2]);		
	free(eyes_fapmask[3]);		
	free(eyes_faps[0]);		
	free(eyes_faps[1]);		
	free(eyes_faps[2]);		
	free(eyes_faps[3]);		

	//Elisabetta 10-10-2005
	//for the eyelids
	free(eyelids_fapmask[0]);		
	free(eyelids_fapmask[1]);		
	free(eyelids_fapmask[2]);		
	free(eyelids_fapmask[3]);		
	free(eyelids_faps[0]);		
	free(eyelids_faps[1]);		
	free(eyelids_faps[2]);		
	free(eyelids_faps[3]);	
}

