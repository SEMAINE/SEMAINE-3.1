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

// FaceGazeSimulator.cpp: implementation of the FaceGazeSimulator class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceGazeSimulator.h"
#include "FaceAnimationGenerator.h"
#include "FaceEngine.h"

#include "IniManager.h"

#include "DataViewerWindow.h"
#include "aiPersonMemory.h"
#include "GazePseudoBelief.h"

//#include "belief.h"

extern IniManager inimanager;
extern RandomGen *randomgen;

extern GazePseudoBelief gpf;

//BN are not used
//extern void CreateStateTransitions(int n,int state);
//extern void SetState(unsigned char *speaker_or_listener,int new_state);
//extern int GetState(unsigned char *speaker_or_listener);
//extern void PrevState(unsigned char **speaker_or_listener);
//extern void NextState(unsigned char **speaker_or_listener);
//extern h_domain_t belief_network_face;
//extern h_domain_t belief_network_gaze;
//extern void init_belief_networks(void);
//extern int SelectLabelState(h_domain_t d,char *node,char *state);
//extern void Propagate(h_domain_t d);
//extern int GetGazeState(h_domain_t d,char *node,int prev_state_speaker,int prev_state_listener,float LIMIT,float SCALE);

extern FILE *face_log;

#define OUTPUT


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



FaceGazeSimulator::FaceGazeSimulator(void* e,void* a)
{
	engine=e;
	animationcalculator=a;
	INTEREST_THRESHOLD=0.80f;
	EFFECTIVENESS_THRESHOLD=0.5f;
	gaze_resolution=0.1f;
}

FaceGazeSimulator::~FaceGazeSimulator()
{
}

//! Initialise the eye gaze model.
int FaceGazeSimulator::InitGazeSimulation(phoneme *phonemes)
{
	CommunicativeAct *comm_act;
	CommunicativeAct *comm_act2;
	phoneme *phon;

	num_of_transitions=0;
	phon=phonemes;
	speaker_state=first_speaker_state;
	listener_state=first_listener_state;

	while (phon!=NULL)
	{
		num_of_transitions++;
		phon=(phoneme*)phon->next;
	}

	//((FaceEngine*)engine)->FAPFileGenerator->BuildChannels();
	//may be already called	build_channels();

	//if(CreateStateTransitions(num_of_transitions,2)==0)
	//	return 0;

	CreateStateTransitions(num_of_transitions,2);

	if(((FaceEngine*)engine)->first_comm_act==NULL)
		return 1;

	comm_act=(CommunicativeAct*)(((FaceEngine*)engine)->first_comm_act->next);

	while (comm_act!=NULL) // per ogni espressione
	{
		if (!strcmp(comm_act->function,"text")) // se una frase di testo
		{
			phon=comm_act->phonemes_list;
			comm_act2=FindCommActFromText(comm_act->value); // trova la fun.comm. piu' annidata
			while (phon!=NULL) // per ogni fonema
			{
				if (comm_act2) //se esiste
				{
					if (comm_act2->active_channels.gaze!=NEUTRAL) // se il segnale gaze e' attivo
						SetState(speaker_state,comm_act2->active_channels.gaze); // imposta gaze speaker

					if (!strcmp(comm_act2->function,"emphasis")||
						!strcmp(comm_act2->function,"topic_comment"))
					{
						SetState(listener_state,LOOK_AT); //imposta gaze listener
					}

					if ((!strcmp(comm_act2->function,"turnallocation")&& // se e' una give turn
						!strcmp(comm_act2->value,"give"))||
						(!strcmp(comm_act2->function,"turn_allocation")&& // se e' una give turn
						!strcmp(comm_act2->value,"give_turn")))
					{
						SetState(listener_state,LOOK_AT); //imposta gaze listener
					}

				}
				NextState(&speaker_state);
				NextState(&listener_state);
				phon=(phoneme*)phon->next;
			}
		}
		comm_act=(CommunicativeAct*)comm_act->next;
	}

	listener_state=first_listener_state;
	phon=phonemes;

	while (phon)
	{
		if ((!strcmp(phon->pho,"pau")||!strcmp(phon->pho,"#"))&&
			(phon!=phonemes)&&
			(phon->next))
			SetState(listener_state,LOOK_AT);
		else
			if (phon==phonemes)
			{
				// first pau => take turn
				SetState(listener_state,LOOK_AT);
			}
			else
				if (phon->next==NULL)
				{
					// last pau => give turn 
					SetState(listener_state,LOOK_AT);
				}

				NextState(&listener_state);
				phon=(phoneme*)phon->next;
	}
	return 1;
}


int FaceGazeSimulator::InitGazeSimulationBML(phoneme *phonemes)
{
	CommunicativeAct *comm_act;
	phoneme *phon;

	num_of_transitions=0;
	phon=phonemes;
	speaker_state=first_speaker_state;
	listener_state=first_listener_state;

	while (phon!=NULL)
	{
		num_of_transitions++;
		phon=(phoneme*)phon->next;
	}

	//((FaceEngine*)engine)->FAPFileGenerator->BuildChannels();
	//may be already called	build_channels();

	//if(CreateStateTransitions(num_of_transitions,2)==0)
	//	return 0;
	CreateStateTransitions(num_of_transitions,1);

	if(((FaceEngine*)engine)->first_comm_act==NULL)
		return 1;

	comm_act=(CommunicativeAct*)(((FaceEngine*)engine)->first_comm_act->next);

	while (comm_act!=NULL) // per ogni espressione
	{
		if (!strcmp(comm_act->function,"gaze") && strcmp(comm_act->value,"look_at")) // se una frase di testo
		{
			phon=comm_act->phonemes_list;
			while (phon!=NULL) // per ogni fonema
			{
				SetState(speaker_state,LOOK_AWAY); //imposta gaze listener
				NextState(&speaker_state);
				phon=(phoneme*)phon->next;
			}
		}
		comm_act=(CommunicativeAct*)comm_act->next;
	}
	return 1;
}

int FaceGazeSimulator::InitGazeSimulationBML(CommunicativeAct *comm_act,float animationlength)
{

	float time,castart,caend;

	num_of_transitions=animationlength/gaze_resolution;
	speaker_state=first_speaker_state;
	listener_state=first_listener_state;

	CreateStateTransitions(num_of_transitions,1);

	return 1;

	//maurizio:
	//by exiting here we are creating a gaze that always looks at the
	//listener. in this way, eye movements due to bml tags are drawn as
	//they are considered as normal facial expressions. later on, the method
	//for the gaze generation will add gaze due to gaze parameters

	comm_act=(CommunicativeAct *)comm_act->next;


	while(comm_act!=0)
	{
		castart=comm_act->start_time;
		caend=comm_act->start_time+comm_act->dur_time;
		speaker_state=first_speaker_state;
		listener_state=first_listener_state;
		time=0;

		for(int i=0;i<num_of_transitions;i++)
		{


			if((time>=castart)&&(time<=caend))
			{
				if(comm_act->active_channels.gaze!=NEUTRAL) // se il segnale gaze e' attivo
					SetState(speaker_state,comm_act->active_channels.gaze); // imposta gaze speaker

				if (!strcmp(comm_act->function,"emphasis")||
					!strcmp(comm_act->function,"topic_comment"))
				{
					SetState(listener_state,LOOK_AT); //imposta gaze listener
				}

				if ((!strcmp(comm_act->function,"turnallocation")&& // se e' una give turn
					!strcmp(comm_act->value,"give"))||
					(!strcmp(comm_act->function,"turn_allocation")&& // se e' una give turn
					!strcmp(comm_act->value,"give_turn")))
				{
					SetState(listener_state,LOOK_AT); //imposta gaze listener
				}
			}

			NextState(&speaker_state);
			NextState(&listener_state);
			time=time+gaze_resolution;
		}
		comm_act=(CommunicativeAct *)comm_act->next;
	}
	return 1;
}

//! Given a text string 's' return the expression 'comm_act'
CommunicativeAct *FaceGazeSimulator::FindCommActFromText(char *s)
{
	CommunicativeAct *comm_act;
	text_l *txt;

	// faccio una ricerca all'indietro
	// xche' devo cercare l'ultima espressione
	// che contiene la frase

	comm_act=((FaceEngine*)engine)->last_comm_act;

	while (comm_act!=NULL)
	{
		txt=comm_act->text_list;
		while (txt!=NULL)
		{
			if (!strcmp(txt->s,s))
				return comm_act;
			txt=(text_l*)txt->next;
		}
		comm_act=(CommunicativeAct*)comm_act->prev;
	}
	return NULL;
}

//! Execute the eye gaze model and store the results
//! for further eye gaze animation.

void FaceGazeSimulator::SimulGaze(phoneme *phonemes)
{
	int i;
	int prev_state_speaker,prev_state_listener;
	int curr_state_speaker,curr_state_listener;
	int mutual_gaze;
	int speaker_duration;
	int listener_duration;
	int trigger_flag;
	float trigger_time;
	float t_s1;
	float t_l1;
	float t_s0;
	float t_l0;
	phoneme *pho;

	t_s1=0;
	t_l1=0;
	t_s0=0;
	t_l0=0;

	pho=phonemes;

	speaker_state=first_speaker_state;
	listener_state=first_listener_state;

	speaker_duration=_LESS;
	listener_duration=_LESS;
	trigger_flag=0;

	for (i=0;i<num_of_transitions;i++)
	{
		//init_belief_networks();

		if (trigger_flag)
			trigger_time+=pho->dur;
		else
			if (!trigger_flag)
			{
				if ((t_s1>inimanager.GetValueFloat("ENGINE_MAX_T_S1_L1"))&&
					(t_l1>inimanager.GetValueFloat("ENGINE_MAX_T_S1_L1")))
				{
#ifdef DEBUG_GAZE
#ifdef OUTPUT
					fprintf(face_log,"\nt_s1_l1:%f (max:%f)\n",t_s1,inimanager.GetValueFloat("ENGINE_MAX_T_S1_L1"));
#endif
#endif
					speaker_duration=_GREATER;
					listener_duration=_GREATER;
				}
				else
					if ((t_s1>inimanager.GetValueFloat("ENGINE_MAX_T_S1"))
						||(t_s0>inimanager.GetValueFloat("ENGINE_MAX_T_S0")))
					{
#ifdef DEBUG_GAZE
						if (t_s1>inimanager.GetValueFloat("ENGINE_MAX_T_S1"))
						{
#ifdef OUTPUT
							fprintf(face_log,"\nt_s1:%f (max:%f)\n",t_s1,inimanager.GetValueFloat("ENGINE_MAX_T_S1"));
#endif
						}
						else
							if (t_s0>inimanager.GetValueFloat("ENGINE_MAX_T_S0"))
							{
#ifdef OUTPUT
								fprintf(face_log,"\nt_s0:%f (max:%f)\n",t_s0,inimanager.GetValueFloat("ENGINE_MAX_T_S0"));
#endif
							}
#endif
							speaker_duration=_GREATER;
					}
					else
						if ((t_l1>inimanager.GetValueFloat("ENGINE_MAX_T_L1"))
							||(t_l0>inimanager.GetValueFloat("ENGINE_MAX_T_L0")))
						{
#ifdef DEBUG_GAZE
							if (t_l1>inimanager.GetValueFloat("ENGINE_MAX_T_L1"))
							{
#ifdef OUTPUT
								fprintf(face_log,"\nt_l1:%f (max:%f)\n",t_l1,inimanager.GetValueFloat("ENGINE_MAX_T_L1"));
#endif
							}
							else
								if (t_l0>inimanager.GetValueFloat("ENGINE_MAX_T_L0"))
								{
#ifdef OUTPUT
									fprintf(face_log,"\nt_l0:%f (max:%f)\n",t_l0,inimanager.GetValueFloat("ENGINE_MAX_T_L0"));
#endif
								}
#endif
								listener_duration=_GREATER;
						}

						if (i==0)
						{
							prev_state_speaker=LOOK_AWAY;
							prev_state_listener=LOOK_AT;
							SetState(speaker_state,LOOK_AWAY);
							SetState(listener_state,LOOK_AT);
						}

						curr_state_speaker=GetState(speaker_state);

						curr_state_listener=GetState(listener_state);

						//printf("static gaze evolution: phoneme=%s  speaker=%d  listener=%d\n",pho->pho,curr_state_speaker,curr_state_listener);

#ifdef DEBUG_GAZE
#ifdef OUTPUT
						fprintf(face_log,"(PrevGS: S%d) - ",prev_state_speaker);
						fprintf(face_log,"(PrevGL: L%d) - ",prev_state_listener);
						fprintf(face_log,"(GS: S%d) - ",curr_state_speaker);
						fprintf(face_log,"(GL: L%d) -",curr_state_listener);
#endif
#endif
						if (prev_state_speaker==LOOK_AWAY) 
						{

							//SelectLabelState(belief_network_gaze,"PrevGazeSpeaker","S0");
							gpf.setPreviousGazeSpeaker(LOOK_AWAY);

						}
						else 
						{
							if (prev_state_speaker==LOOK_AT)
							{
								//SelectLabelState(belief_network_gaze,"PrevGazeSpeaker","S1");
								gpf.setPreviousGazeSpeaker(LOOK_AT);

							}
						}

						if (prev_state_listener==LOOK_AWAY)
						{
							// SelectLabelState(belief_network_gaze,"PrevGazeList","L0");
							gpf.setPreviousGazeListener(LOOK_AWAY);

						}
						else
						{
							if (prev_state_listener==LOOK_AT)
							{			
								// SelectLabelState(belief_network_gaze,"PrevGazeList","L1");
								gpf.setPreviousGazeListener(LOOK_AT);
							}
						}
						if (curr_state_speaker==LOOK_AWAY)
						{
							// SelectLabelState(belief_network_gaze,"MoveSpeaker","S0");
							gpf.setMoveSpeaker(LOOK_AWAY);
						}
						else
						{	
							if (curr_state_speaker==LOOK_AT)
							{			
								// SelectLabelState(belief_network_gaze,"MoveSpeaker","S1");
								gpf.setMoveSpeaker(LOOK_AT);
							}
						}
						if (curr_state_listener==LOOK_AWAY)
						{
							// SelectLabelState(belief_network_gaze,"MoveListener","L0");
							gpf.setMoveListener(LOOK_AWAY);
						}
						else
						{
							if (curr_state_listener==LOOK_AT)
							{			
								// SelectLabelState(belief_network_gaze,"MoveListener","L1");
								gpf.setMoveListener(LOOK_AT);
							}
						}

						if (speaker_duration==_LESS)
						{			
							//BN is not used any more
							// SelectLabelState(belief_network_gaze,"SpeakerDuration","lessS");
							gpf.setSpeakerDuration(1);
						}
						else
						{
							if (speaker_duration==_GREATER)
							{	
								//BN is not used any more
								// SelectLabelState(belief_network_gaze,"SpeakerDuration","greaterS");
								gpf.setSpeakerDuration(0);
							}
						}
						if (listener_duration==_LESS)
						{
							// BN is not used any more
							// SelectLabelState(belief_network_gaze,"ListenerDuration","lessL");
							gpf.setListenerDuration(1);
						}
						else
						{
							if (listener_duration==_GREATER)
							{
								// BN is not used any more
								// SelectLabelState(belief_network_gaze,"ListenerDuration","greaterL");
								gpf.setListenerDuration(0);
							}
						}

						//BN is not used any more
						//Propagate(belief_network_gaze);

						//mutual_gaze=GetNodeState2(belief_network_gaze,"nextGaze");

#ifdef DEBUG_GAZE
						fprintf(face_log,"MoveSpeaker: %s\n",curr_state_speaker==LOOK_AWAY?"S0":(curr_state_speaker==LOOK_AT?"S1":"ND"));
						fprintf(face_log,"MoveListener: %s\n",curr_state_listener==LOOK_AWAY?"L0":(curr_state_listener==LOOK_AT?"L1":"ND"));
						fprintf(face_log,"SpeakerDuration: %s\n",speaker_duration==_LESS?"LESS":"GREATER");
						fprintf(face_log,"ListenerDuration: %s\n",listener_duration==_LESS?"LESS":"GREATER");
#endif
						mutual_gaze=GetGazeState(prev_state_speaker,prev_state_listener,inimanager.GetValueFloat("ENGINE_LIMIT"),inimanager.GetValueFloat("ENGINE_SCALE"));

			} // end !trigger_flag

			switch (mutual_gaze)
			{
			case S1L1:
				SetState(speaker_state,LOOK_AT);
				SetState(listener_state,LOOK_AT);
				if (prev_state_speaker==LOOK_AT&&
					prev_state_listener==LOOK_AT)
				{
					t_s1+=pho->dur;
					t_l1+=pho->dur;
				}
				if (prev_state_speaker!=LOOK_AT||
					prev_state_listener!=LOOK_AT)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AWAY)
						t_s0=0;
					if (prev_state_listener==LOOK_AWAY)
						t_l0=0;
				}
				prev_state_speaker=LOOK_AT;
				prev_state_listener=LOOK_AT;
				break;
			case S0L1:
				SetState(speaker_state,LOOK_AWAY);
				SetState(listener_state,LOOK_AT);
				if (prev_state_speaker==LOOK_AWAY)
				{
					t_s0+=pho->dur;
				}
				if (prev_state_listener==LOOK_AT)
				{
					t_l1+=pho->dur;
				}
				if (prev_state_speaker!=LOOK_AWAY||
					prev_state_listener!=LOOK_AT)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AT)
						t_s1=0;
					if (prev_state_listener==LOOK_AWAY)
						t_l0=0;					
				}

				prev_state_speaker=LOOK_AWAY;
				prev_state_listener=LOOK_AT;
				break;
			case S1L0:
				SetState(speaker_state,LOOK_AT);
				SetState(listener_state,LOOK_AWAY);
				if (prev_state_speaker==LOOK_AT)
				{
					t_s1+=pho->dur;
				}
				if (prev_state_listener==LOOK_AWAY)
				{
					t_l0+=pho->dur;
				}
				if (prev_state_speaker!=LOOK_AT||
					prev_state_listener!=LOOK_AWAY)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AWAY)
						t_s0=0;
					if (prev_state_listener==LOOK_AT)
						t_l1=0;					
				}

				prev_state_speaker=LOOK_AT;
				prev_state_listener=LOOK_AWAY;
				break;
			case S0L0:
				SetState(speaker_state,LOOK_AWAY);
				SetState(listener_state,LOOK_AWAY);
				if (prev_state_speaker==LOOK_AWAY)
				{
					t_s0+=pho->dur;
				}
				if (prev_state_listener==LOOK_AWAY)
				{
					t_l0+=pho->dur;
				}
				if (prev_state_speaker!=LOOK_AWAY||
					prev_state_listener!=LOOK_AWAY)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AT)
						t_s1=0;
					if (prev_state_listener==LOOK_AT)
						t_l1=0;					
				}
				prev_state_speaker=LOOK_AWAY;
				prev_state_listener=LOOK_AWAY;
				break;
			} //end trigger_flag


#ifdef DEBUG_GAZE	
			//		if (!trigger_flag)
#ifdef OUTPUT
			fprintf(face_log,"---->(S%d,L%d) in %f sec\n",prev_state_speaker,prev_state_listener,pho->dur);
#endif
#endif
			if (trigger_flag&&(trigger_time>inimanager.GetValueFloat("ENGINE_DT")||!(pho->next)))
			{
#ifdef DEBUG_GAZE
#ifdef OUTPUT
				fprintf(face_log,"\nDT:%f (max: %f)\n",trigger_time,inimanager.GetValueFloat("ENGINE_DT"));
#endif
#endif
				speaker_duration=_LESS;
				listener_duration=_LESS;
				trigger_flag=0;
				trigger_time=0;
			}

			//SetState(speaker_state,LOOK_AT);

			NextState(&speaker_state);		
			NextState(&listener_state);
			pho=(phoneme*)pho->next;
	}

}

void FaceGazeSimulator::SimulGazeBML()
{
	int i;
	int prev_state_speaker,prev_state_listener;
	int curr_state_speaker,curr_state_listener;
	int mutual_gaze;
	int speaker_duration;
	int listener_duration;
	int trigger_flag;
	float trigger_time;
	float t_s1;
	float t_l1;
	float t_s0;
	float t_l0;
	float time;

	t_s1=0;
	t_l1=0;
	t_s0=0;
	t_l0=0;


	speaker_state=first_speaker_state;
	listener_state=first_listener_state;

	speaker_duration=_LESS;
	listener_duration=_LESS;
	trigger_flag=0;

	time=0;

	for (i=0;i<num_of_transitions;i++)
	{
		//BN not used
		//init_belief_networks();

		if (trigger_flag!=0)
			trigger_time+=gaze_resolution;
		else
			if (trigger_flag==0)
			{
				if ((t_s1>inimanager.GetValueFloat("ENGINE_MAX_T_S1_L1"))&&
					(t_l1>inimanager.GetValueFloat("ENGINE_MAX_T_S1_L1")))
				{
					speaker_duration=_GREATER;
					listener_duration=_GREATER;
				}
				else
					if ((t_s1>inimanager.GetValueFloat("ENGINE_MAX_T_S1"))
						||(t_s0>inimanager.GetValueFloat("ENGINE_MAX_T_S0")))
					{
						speaker_duration=_GREATER;
					}
					else
						if ((t_l1>inimanager.GetValueFloat("ENGINE_MAX_T_L1"))
							||(t_l0>inimanager.GetValueFloat("ENGINE_MAX_T_L0")))
						{
							listener_duration=_GREATER;
						}

						if (i==0)
						{
							prev_state_speaker=LOOK_AWAY;
							prev_state_listener=LOOK_AT;
							SetState(speaker_state,LOOK_AWAY);
							SetState(listener_state,LOOK_AT);
						}

						curr_state_speaker=GetState(speaker_state);
						curr_state_listener=GetState(listener_state);

						if (prev_state_speaker==LOOK_AWAY)
						{
							//BN is not used any more
							// SelectLabelState(belief_network_gaze,"PrevGazeSpeaker","S0");				
							gpf.setPreviousGazeSpeaker(LOOK_AWAY);
						}
						else
						{
							if (prev_state_speaker==LOOK_AT)
							{
								//BN is not used any more
								// SelectLabelState(belief_network_gaze,"PrevGazeSpeaker","S1");			
								gpf.setPreviousGazeSpeaker(LOOK_AT);
							}
						}

						if (prev_state_listener==LOOK_AWAY) 
						{
							//BN is not used any more
							// SelectLabelState(belief_network_gaze,"PrevGazeList","L0");					
							gpf.setPreviousGazeListener(LOOK_AWAY);
						}
						else
						{
							if (prev_state_listener==LOOK_AT)
							{
								//BN is not used any more
								// SelectLabelState(belief_network_gaze,"PrevGazeList","L1");					
								gpf.setPreviousGazeListener(LOOK_AT);
							}
						}

						if (curr_state_speaker==LOOK_AWAY)
						{
							//BN is not used any more
							// SelectLabelState(belief_network_gaze,"MoveSpeaker","S0");					
							gpf.setMoveSpeaker(LOOK_AWAY);
						}
						else
						{
							if (curr_state_speaker==LOOK_AT)
							{			
								//BN is not used any more
								// SelectLabelState(belief_network_gaze,"MoveSpeaker","S1");					
								gpf.setMoveSpeaker(LOOK_AT);
							}
						}
						if (curr_state_listener==LOOK_AWAY)
						{
							//BN is not used any more
							// SelectLabelState(belief_network_gaze,"MoveListener","L0");				
							gpf.setMoveListener(LOOK_AWAY);
						}
						else
						{
							if (curr_state_listener==LOOK_AT)
							{
								//BN is not used any more
								// SelectLabelState(belief_network_gaze,"MoveListener","L1");				
								gpf.setMoveListener(LOOK_AT);
							}
						}
						if (speaker_duration==_LESS)
						{
							//BN is not used any more
							// SelectLabelState(belief_network_gaze,"SpeakerDuration","lessS");				
							gpf.setSpeakerDuration(1);
						}
						else
						{
							if (speaker_duration==_GREATER)
							{
								//BN is not used any more
								// SelectLabelState(belief_network_gaze,"SpeakerDuration","greaterS");				
								gpf.setSpeakerDuration(0);
							}
						}
						if (listener_duration==_LESS)
						{
							//BN is not used any more
							// SelectLabelState(belief_network_gaze,"ListenerDuration","lessL");				
							gpf.setListenerDuration(1);
						}
						else
						{
							if (listener_duration==_GREATER)
							{

								//  BN is not used any more
								//	SelectLabelState(belief_network_gaze,"ListenerDuration","greaterL");				
								gpf.setListenerDuration(0);

							}
						}

						//BN is not used any more
						//Propagate(belief_network_gaze);

						//mutual_gaze=GetNodeState2(belief_network_gaze,"nextGaze");
						mutual_gaze=GetGazeState(prev_state_speaker,prev_state_listener,inimanager.GetValueFloat("ENGINE_LIMIT"),inimanager.GetValueFloat("ENGINE_SCALE"));									

			} // end !trigger_flag

			switch (mutual_gaze)
			{
			case S1L1:
				SetState(speaker_state,LOOK_AT);
				SetState(listener_state,LOOK_AT);
				if (prev_state_speaker==LOOK_AT&&
					prev_state_listener==LOOK_AT)
				{
					t_s1+=gaze_resolution;
					t_l1+=gaze_resolution;
				}
				if (prev_state_speaker!=LOOK_AT||
					prev_state_listener!=LOOK_AT)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AWAY)
						t_s0=0;
					if (prev_state_listener==LOOK_AWAY)
						t_l0=0;
				}
				prev_state_speaker=LOOK_AT;
				prev_state_listener=LOOK_AT;
				break;
			case S0L1:
				SetState(speaker_state,LOOK_AWAY);
				SetState(listener_state,LOOK_AT);
				if (prev_state_speaker==LOOK_AWAY)
				{
					t_s0+=gaze_resolution;
				}
				if (prev_state_listener==LOOK_AT)
				{
					t_l1+=gaze_resolution;
				}
				if (prev_state_speaker!=LOOK_AWAY||
					prev_state_listener!=LOOK_AT)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AT)
						t_s1=0;
					if (prev_state_listener==LOOK_AWAY)
						t_l0=0;					
				}

				prev_state_speaker=LOOK_AWAY;
				prev_state_listener=LOOK_AT;
				break;
			case S1L0:
				SetState(speaker_state,LOOK_AT);
				SetState(listener_state,LOOK_AWAY);
				if (prev_state_speaker==LOOK_AT)
				{
					t_s1+=gaze_resolution;
				}
				if (prev_state_listener==LOOK_AWAY)
				{
					t_l0+=gaze_resolution;
				}
				if (prev_state_speaker!=LOOK_AT||
					prev_state_listener!=LOOK_AWAY)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AWAY)
						t_s0=0;
					if (prev_state_listener==LOOK_AT)
						t_l1=0;					
				}

				prev_state_speaker=LOOK_AT;
				prev_state_listener=LOOK_AWAY;
				break;
			case S0L0:
				SetState(speaker_state,LOOK_AWAY);
				SetState(listener_state,LOOK_AWAY);
				if (prev_state_speaker==LOOK_AWAY)
				{
					t_s0+=gaze_resolution;
				}
				if (prev_state_listener==LOOK_AWAY)
				{
					t_l0+=gaze_resolution;
				}
				if (prev_state_speaker!=LOOK_AWAY||
					prev_state_listener!=LOOK_AWAY)
				{
					trigger_flag=1;
					trigger_time=0;
					if (prev_state_speaker==LOOK_AT)
						t_s1=0;
					if (prev_state_listener==LOOK_AT)
						t_l1=0;					
				}
				prev_state_speaker=LOOK_AWAY;
				prev_state_listener=LOOK_AWAY;
				break;
			} //end trigger_flag

			if (trigger_flag&&(trigger_time>inimanager.GetValueFloat("ENGINE_DT")))
			{
				speaker_duration=_LESS;
				listener_duration=_LESS;
				trigger_flag=0;
				trigger_time=0;
			}

			//SetState(speaker_state,LOOK_AT);

			NextState(&speaker_state);		
			NextState(&listener_state);
			time=time+gaze_resolution;
	}

	//for (i=0;i<num_of_transitions;i++) printf(" %i, ", speaker_state[i]);

}


int FaceGazeSimulator::CheckHeadPosition(phoneme *phonemes,std::vector<FAPFrame> *fapvector,int **faps_head_expr, int **fapmask_head_expr,int **faps_head_ns, int **fapmask_head_ns, int FramesTotalNumber)
{
	phoneme *phon;
	float start_sec, end_sec;
	int state, i;
	int start_frame, end_frame;
	int headX, headY;
	int headShake[2]={-1,-1};
	int headNod[2]={-1,-1};
	int headExprX[2]={-1,-1};
	int headExprY[2]={-1,-1};
	int y1, y2;
	int MAX_VERTICAL=17000;
	int MAX_ORIZONTAL=9000;
	int head_direction;
	int flag=0;


	speaker_state=first_speaker_state;
	start_sec=0;
	end_sec=0;

	phon=phonemes;

	while (phon!=NULL)
	{	//put a LOOK_AT when a deictic occurs
		CommunicativeAct *comm_act;
		comm_act=FindCommActFromPhoneme(start_sec);
		if (!comm_act||(comm_act&&(strcmp(comm_act->function,"deictic")==0)))	
			SetState(speaker_state, LOOK_AT);
		NextState(&speaker_state);
		start_sec+=phon->dur;
		phon=(phoneme*)phon->next;
	}

	//Elisabetta dicembre 2005
	//Valutazione del movimento della testa e degli occhi
	speaker_state=first_speaker_state;
	start_sec=0;
	phon=phonemes;

	while (phon!=NULL)
	{
		state=GetState(speaker_state);
		end_sec=start_sec;
		if (state==LOOK_AT)
		{
			end_sec+=phon->dur;
			/*	while (phon!=NULL && state==LOOK_AT)
			{
			end_sec+=phon->dur;	
			NextState(&speaker_state);
			state=GetState(speaker_state);
			phon=(phoneme*)phon->next;	
			}
			*/
			//Qui ho una lookat da start_sec a end_sec e devo controllare dove sta la testa!!
			start_frame=(int)(start_sec*FRAMERATE);
			end_frame=(int)(end_sec*FRAMERATE);

			headX=-1;
			headY=-1;
			for (i=start_frame; i<=end_frame; i++)
			{
				//if(abs(faps[i][48])>MAX_VERTICAL)  //la testa è troppo alzata o troppo abbassata
				if(abs((*fapvector)[i].GetFAP(48))>MAX_VERTICAL)  //la testa è troppo alzata o troppo abbassata
					headY=i;
				//if(abs(faps[i][49])>MAX_ORIZONTAL)  //la testa è troppo a destra o troppo a sinistra
				if(abs((*fapvector)[i].GetFAP(49))>MAX_ORIZONTAL)  //la testa è troppo a destra o troppo a sinistra
					headX=i;
			}

			for(i=0;i<2;i++)
			{
				headShake[i]=-1;
				headNod[i]=-1;
				headExprX[i]=-1;
				headExprY[i]=-1;
			}
			if((headY!=-1)||(headX!=-1))
			{
				flag=1;
				//cerco i key frame che causano il movimento eccessivo della testa
				if((headY!=-1)&&(headY!=FramesTotalNumber))
				{
					i=headY;
					while((i>=0)&&(fapmask_head_expr[i][0]==0))
						i--;
					headExprY[0]=i;

					i=headY;
					while((i<FramesTotalNumber-1)&&(fapmask_head_ns[i][0]==0))
						i++;
					headExprY[1]=i;

					i=headY;
					while((i>=0)&&(fapmask_head_expr[i][0]==0))
						i--;
					headNod[0]=i;
					//Il fap ha senso solo se è diverso da 0 e se è nella stessa direzione
					//del punto esagerato della testa
					if(headNod[0]!=-1)
						if((faps_head_ns[headNod[0]][0]==0)||((abs(faps_head_ns[headNod[0]][0])/faps_head_ns[headNod[0]][0])!=(abs((*fapvector)[headY].GetFAP(48))/(*fapvector)[headY].GetFAP(48)))) 
							headNod[0]=-1;

					i=headY;
					while((fapmask_head_ns[i][0]==0)&&(i<FramesTotalNumber-1))
						i++;
					headNod[1]=i;
					//Il fap ha senso solo se è diverso da 0 e se è nella stessa direzione
					//del punto esagerato della testa
					if(headNod[1]!=-1)
						if((faps_head_ns[headNod[1]][0]==0)||((abs(faps_head_ns[headNod[1]][0])/faps_head_ns[headNod[1]][0])!=(abs((*fapvector)[headY].GetFAP(48))/(*fapvector)[headY].GetFAP(48)))) 
							headNod[1]=-1;

					//Il nod intervalla sempre tra un fap a 0 e uno con un valore
					//tanto vale considerarne subito solo uno
					if(headNod[1]!=-1)
						headNod[0]=headNod[1];
				}

				if((headX!=-1)&&(headX!=FramesTotalNumber))
				{
					i=headX;
					while((i>=0)&&(fapmask_head_expr[i][1]==0))
						i--;
					headExprX[0]=i;

					i=headX;
					while((i<FramesTotalNumber-1)&&(fapmask_head_ns[i][1]==0))
						i++;
					headExprX[1]=i;

					i=headX;
					while((i>=0)&&(fapmask_head_expr[i][1]==0))
						i--;
					headShake[0]=i;
					//Il fap ha senso solo se è diverso da 0 e se è nella stessa direzione
					//del punto esagerato della testa
					if(headShake[0]!=-1)
						if((faps_head_ns[headShake[0]][1]==0)||((abs(faps_head_ns[headShake[0]][1])/faps_head_ns[headShake[0]][1])!=(abs((*fapvector)[headX].GetFAP(48))/(*fapvector)[headX].GetFAP(48)))) 

							i=headX;
					while((fapmask_head_ns[i][1]==0)&&(i<FramesTotalNumber-1))
						i++;
					headShake[1]=i;
					if(headShake[1]!=-1)
						if((faps_head_ns[headShake[1]][1]==0)||((abs(faps_head_ns[headShake[1]][1])/faps_head_ns[headShake[1]][1])!=(abs((*fapvector)[headX].GetFAP(48))/(*fapvector)[headX].GetFAP(48)))) 
							headShake[1]=-1;

					//Lo shake intervalla sempre tra un fap a 0 e uno con un valore
					//tanto vale considerarne subito solo uno
					if(headShake[1]!=-1)
						headShake[0]=headShake[1];
				}

				if(headNod[0]!=-1)
				{
					if((headExprY[0]==-1)&&(headExprY[1]==-1))
					{
						head_direction=faps_head_ns[headNod[0]][0]/abs(faps_head_ns[headNod[0]][0]);
						faps_head_ns[headNod[0]][0]=head_direction*MAX_VERTICAL;
					}
					else
					{
						if((headExprY[0]==-1) && (faps_head_expr[headExprY[0]][0]!=0))
						{
							head_direction=faps_head_expr[headExprY[0]][0]/abs(faps_head_expr[headExprY[0]][0]);
							y1=(faps_head_expr[headExprY[0]][0]*head_direction*MAX_VERTICAL)/(faps_head_expr[headExprY[0]][0]+faps_head_ns[headNod[0]][0]);
							y2=head_direction*MAX_VERTICAL-y1;
							faps_head_expr[headExprY[0]][0]=y1;
							faps_head_ns[headNod[0]][0]=y2;
						}

						if((headExprY[1]==-1) && (faps_head_expr[headExprY[1]][0]!=0))
						{
							head_direction=faps_head_expr[headExprY[1]][0]/abs(faps_head_expr[headExprY[1]][0]);
							y1=(faps_head_expr[headExprY[1]][0]*head_direction*MAX_VERTICAL)/(faps_head_expr[headExprY[1]][0]+faps_head_ns[headNod[0]][0]);
							y2=head_direction*MAX_VERTICAL-y1;
							faps_head_expr[headExprY[1]][0]=y1;
							faps_head_ns[headNod[0]][0]=y2;
						}
					}
				}
				else
				{
					if((headExprY[0]!=-1) && (faps_head_expr[headExprY[0]][0]!=0))
					{
						head_direction=faps_head_expr[headExprY[0]][0]/abs(faps_head_expr[headExprY[0]][0]);
						faps_head_expr[headExprY[0]][0]=head_direction*MAX_VERTICAL;
					}
					if((headExprY[1]!=-1) && (faps_head_expr[headExprY[1]][0]!=0))
					{
						head_direction=faps_head_expr[headExprY[1]][0]/abs(faps_head_expr[headExprY[1]][0]);
						faps_head_expr[headExprY[1]][0]=head_direction*MAX_VERTICAL;
					}
				}


				if(headShake[0]!=-1)
				{
					if((headExprX[0]==-1)&&(headExprX[1]==-1))
					{
						head_direction=faps_head_ns[headShake[0]][1]/abs(faps_head_ns[headShake[0]][1]);
						faps_head_ns[headShake[0]][1]=head_direction*MAX_ORIZONTAL;
					}
					else
					{
						if((headExprX[0]!=-1) && (faps_head_expr[headExprX[0]][1]!=0))
						{
							head_direction=faps_head_expr[headExprX[0]][1]/abs(faps_head_expr[headExprX[0]][1]);
							y1=(faps_head_expr[headExprX[0]][1]*head_direction*MAX_ORIZONTAL)/(faps_head_expr[headExprX[0]][1]+faps_head_ns[headShake[0]][1]);
							y2=head_direction*MAX_ORIZONTAL-y1;
							faps_head_expr[headExprX[0]][1]=y1;
							faps_head_ns[headShake[0]][1]=y2;
						}

						if((headExprX[1]!=-1) && (faps_head_expr[headExprX[1]][1]!=0))
						{
							head_direction=faps_head_expr[headExprX[1]][1]/abs(faps_head_expr[headExprX[1]][1]);
							y1=(faps_head_expr[headExprX[1]][1]*head_direction*MAX_ORIZONTAL)/(faps_head_expr[headExprX[1]][1]+faps_head_ns[headShake[0]][1]);
							y2=head_direction*MAX_ORIZONTAL-y1;
							faps_head_expr[headExprX[1]][1]=y1;
							faps_head_ns[headShake[0]][1]=y2;
						}
					}
				}
				else
				{
					if((headExprX[0]!=-1) && (faps_head_expr[headExprX[0]][1]!=0))
					{
						head_direction=faps_head_expr[headExprX[0]][1]/abs(faps_head_expr[headExprX[0]][1]);
						faps_head_expr[headExprX[0]][1]=head_direction*MAX_ORIZONTAL;
					}
					if((headExprX[1]!=-1) && (faps_head_expr[headExprX[1]][1]!=0))
					{
						head_direction=faps_head_expr[headExprX[1]][1]/abs(faps_head_expr[headExprX[1]][1]);
						faps_head_expr[headExprX[0]][1]=head_direction*MAX_ORIZONTAL;
					}
				}
			}

			NextState(&speaker_state);
			state=GetState(speaker_state);
			start_sec=end_sec;
			phon=(phoneme*)phon->next;
		}
		else
		{
			end_sec+=phon->dur;	
			NextState(&speaker_state);
			state=GetState(speaker_state);
			start_sec=end_sec;
			phon=(phoneme*)phon->next;
		}
	}
	return(flag);
}

int FaceGazeSimulator::CheckHeadPositionBML(std::vector<FAPFrame> *fapvector,int **faps_head_expr, int **fapmask_head_expr,int **faps_head_ns, int **fapmask_head_ns, int FramesTotalNumber)
{
	float start_sec, end_sec;
	int state, i;
	int start_frame, end_frame;
	int headX, headY;
	int headShake[2]={-1,-1};
	int headNod[2]={-1,-1};
	int headExprX[2]={-1,-1};
	int headExprY[2]={-1,-1};
	int y1, y2;
	int MAX_VERTICAL=17000;
	int MAX_ORIZONTAL=9000;
	int head_direction;
	int flag=0;

	speaker_state=first_speaker_state;
	start_sec=0;
	end_sec=0;



	//Elisabetta dicembre 2005
	//Valutazione del movimento della testa e degli occhi
	speaker_state=first_speaker_state;
	start_sec=0;

	for(int j=0;j<num_of_transitions;j++)
	{
		state=GetState(speaker_state);
		end_sec=start_sec;
		if (state==LOOK_AT)
		{
			end_sec+=gaze_resolution;
			/*	while (phon!=NULL && state==LOOK_AT)
			{
			end_sec+=phon->dur;	
			NextState(&speaker_state);
			state=GetState(speaker_state);
			phon=(phoneme*)phon->next;	
			}
			*/
			//Qui ho una lookat da start_sec a end_sec e devo controllare dove sta la testa!!
			start_frame=(int)(start_sec*FRAMERATE);
			end_frame=(int)(end_sec*FRAMERATE);

			headX=-1;
			headY=-1;
			for (i=start_frame; i<=end_frame; i++)
			{
				//if(abs(faps[i][48])>MAX_VERTICAL)  //la testa è troppo alzata o troppo abbassata
				if(abs((*fapvector)[i].GetFAP(48))>MAX_VERTICAL)  //la testa è troppo alzata o troppo abbassata
					headY=i;
				//if(abs(faps[i][49])>MAX_ORIZONTAL)  //la testa è troppo a destra o troppo a sinistra
				if(abs((*fapvector)[i].GetFAP(49))>MAX_ORIZONTAL)  //la testa è troppo a destra o troppo a sinistra
					headX=i;
			}

			for(i=0;i<2;i++)
			{
				headShake[i]=-1;
				headNod[i]=-1;
				headExprX[i]=-1;
				headExprY[i]=-1;
			}
			if((headY!=-1)||(headX!=-1))
			{
				flag=1;
				//cerco i key frame che causano il movimento eccessivo della testa
				if((headY!=-1)&&(headY!=FramesTotalNumber))
				{
					i=headY;
					while((i>=0)&&(fapmask_head_expr[i][0]==0))
						i--;
					headExprY[0]=i;

					i=headY;
					while((i<FramesTotalNumber-1)&&(fapmask_head_ns[i][0]==0))
						i++;
					headExprY[1]=i;

					i=headY;
					while((i>=0)&&(fapmask_head_expr[i][0]==0))
						i--;
					headNod[0]=i;
					//Il fap ha senso solo se è diverso da 0 e se è nella stessa direzione
					//del punto esagerato della testa
					if(headNod[0]!=-1)
						if((faps_head_ns[headNod[0]][0]==0)||((abs(faps_head_ns[headNod[0]][0])/faps_head_ns[headNod[0]][0])!=(abs((*fapvector)[headY].GetFAP(48))/(*fapvector)[headY].GetFAP(48)))) 
							headNod[0]=-1;

					i=headY;
					while((fapmask_head_ns[i][0]==0)&&(i<FramesTotalNumber-1))
						i++;
					headNod[1]=i;
					//Il fap ha senso solo se è diverso da 0 e se è nella stessa direzione
					//del punto esagerato della testa
					if(headNod[1]!=-1)
						if((faps_head_ns[headNod[1]][0]==0)||((abs(faps_head_ns[headNod[1]][0])/faps_head_ns[headNod[1]][0])!=(abs((*fapvector)[headY].GetFAP(48))/(*fapvector)[headY].GetFAP(48)))) 
							headNod[1]=-1;

					//Il nod intervalla sempre tra un fap a 0 e uno con un valore
					//tanto vale considerarne subito solo uno
					if(headNod[1]!=-1)
						headNod[0]=headNod[1];
				}

				if((headX!=-1)&&(headX!=FramesTotalNumber))
				{
					i=headX;
					while((i>=0)&&(fapmask_head_expr[i][1]==0))
						i--;
					headExprX[0]=i;

					i=headX;
					while((i<FramesTotalNumber-1)&&(fapmask_head_ns[i][1]==0))
						i++;
					headExprX[1]=i;

					i=headX;
					while((i>=0)&&(fapmask_head_expr[i][1]==0))
						i--;
					headShake[0]=i;
					//Il fap ha senso solo se è diverso da 0 e se è nella stessa direzione
					//del punto esagerato della testa
					if(headShake[0]!=-1)
						if((faps_head_ns[headShake[0]][1]==0)||((abs(faps_head_ns[headShake[0]][1])/faps_head_ns[headShake[0]][1])!=(abs((*fapvector)[headX].GetFAP(48))/(*fapvector)[headX].GetFAP(48)))) 

							i=headX;
					while((fapmask_head_ns[i][1]==0)&&(i<FramesTotalNumber-1))
						i++;
					headShake[1]=i;
					if(headShake[1]!=-1)
						if((faps_head_ns[headShake[1]][1]==0)||((abs(faps_head_ns[headShake[1]][1])/faps_head_ns[headShake[1]][1])!=(abs((*fapvector)[headX].GetFAP(48))/(*fapvector)[headX].GetFAP(48)))) 
							headShake[1]=-1;

					//Lo shake intervalla sempre tra un fap a 0 e uno con un valore
					//tanto vale considerarne subito solo uno
					if(headShake[1]!=-1)
						headShake[0]=headShake[1];
				}

				if(headNod[0]!=-1)
				{
					if((headExprY[0]==-1)&&(headExprY[1]==-1))
					{
						head_direction=faps_head_ns[headNod[0]][0]/abs(faps_head_ns[headNod[0]][0]);
						faps_head_ns[headNod[0]][0]=head_direction*MAX_VERTICAL;
					}
					else
					{
						if((headExprY[0]==-1) && (faps_head_expr[headExprY[0]][0]!=0))
						{
							head_direction=faps_head_expr[headExprY[0]][0]/abs(faps_head_expr[headExprY[0]][0]);
							y1=(faps_head_expr[headExprY[0]][0]*head_direction*MAX_VERTICAL)/(faps_head_expr[headExprY[0]][0]+faps_head_ns[headNod[0]][0]);
							y2=head_direction*MAX_VERTICAL-y1;
							faps_head_expr[headExprY[0]][0]=y1;
							faps_head_ns[headNod[0]][0]=y2;
						}

						if((headExprY[1]==-1) && (faps_head_expr[headExprY[1]][0]!=0))
						{
							head_direction=faps_head_expr[headExprY[1]][0]/abs(faps_head_expr[headExprY[1]][0]);
							y1=(faps_head_expr[headExprY[1]][0]*head_direction*MAX_VERTICAL)/(faps_head_expr[headExprY[1]][0]+faps_head_ns[headNod[0]][0]);
							y2=head_direction*MAX_VERTICAL-y1;
							faps_head_expr[headExprY[1]][0]=y1;
							faps_head_ns[headNod[0]][0]=y2;
						}
					}
				}
				else
				{
					if((headExprY[0]!=-1) && (faps_head_expr[headExprY[0]][0]!=0))
					{
						head_direction=faps_head_expr[headExprY[0]][0]/abs(faps_head_expr[headExprY[0]][0]);
						faps_head_expr[headExprY[0]][0]=head_direction*MAX_VERTICAL;
					}
					if((headExprY[1]!=-1) && (faps_head_expr[headExprY[1]][0]!=0))
					{
						head_direction=faps_head_expr[headExprY[1]][0]/abs(faps_head_expr[headExprY[1]][0]);
						faps_head_expr[headExprY[1]][0]=head_direction*MAX_VERTICAL;
					}
				}


				if(headShake[0]!=-1)
				{
					if((headExprX[0]==-1)&&(headExprX[1]==-1))
					{
						head_direction=faps_head_ns[headShake[0]][1]/abs(faps_head_ns[headShake[0]][1]);
						faps_head_ns[headShake[0]][1]=head_direction*MAX_ORIZONTAL;
					}
					else
					{
						if((headExprX[0]!=-1) && (faps_head_expr[headExprX[0]][1]!=0))
						{
							head_direction=faps_head_expr[headExprX[0]][1]/abs(faps_head_expr[headExprX[0]][1]);
							y1=(faps_head_expr[headExprX[0]][1]*head_direction*MAX_ORIZONTAL)/(faps_head_expr[headExprX[0]][1]+faps_head_ns[headShake[0]][1]);
							y2=head_direction*MAX_ORIZONTAL-y1;
							faps_head_expr[headExprX[0]][1]=y1;
							faps_head_ns[headShake[0]][1]=y2;
						}

						if((headExprX[1]!=-1) && (faps_head_expr[headExprX[1]][1]!=0))
						{
							head_direction=faps_head_expr[headExprX[1]][1]/abs(faps_head_expr[headExprX[1]][1]);
							y1=(faps_head_expr[headExprX[1]][1]*head_direction*MAX_ORIZONTAL)/(faps_head_expr[headExprX[1]][1]+faps_head_ns[headShake[0]][1]);
							y2=head_direction*MAX_ORIZONTAL-y1;
							faps_head_expr[headExprX[1]][1]=y1;
							faps_head_ns[headShake[0]][1]=y2;
						}
					}
				}
				else
				{
					if((headExprX[0]!=-1) && (faps_head_expr[headExprX[0]][1]!=0))
					{
						head_direction=faps_head_expr[headExprX[0]][1]/abs(faps_head_expr[headExprX[0]][1]);
						faps_head_expr[headExprX[0]][1]=head_direction*MAX_ORIZONTAL;
					}
					if((headExprX[1]!=-1) && (faps_head_expr[headExprX[1]][1]!=0))
					{
						head_direction=faps_head_expr[headExprX[1]][1]/abs(faps_head_expr[headExprX[1]][1]);
						faps_head_expr[headExprX[0]][1]=head_direction*MAX_ORIZONTAL;
					}
				}
			}

			NextState(&speaker_state);
			state=GetState(speaker_state);
			start_sec=end_sec;
		}
		else
		{
			end_sec+=gaze_resolution;	
			NextState(&speaker_state);
			state=GetState(speaker_state);
			start_sec=end_sec;
		}
	}
	return(flag);
}

void FaceGazeSimulator::GazeGeneratorSpeaker(phoneme *phonemes)
{
	//	exprPtr comm_act;
	phoneme *phon;
	float start_sec, end_sec;
	//	i;
	int state;
	//	int start_frame, end_frame;


#ifdef OUTPUT
	fprintf(face_log,"\nSpeaker Gaze...\n\n");
#endif


	speaker_state=first_speaker_state;
	start_sec=0;
	end_sec=0;
	phon=phonemes;

	while (phon!=NULL)
	{			
		state=GetState(speaker_state);
		end_sec=start_sec;
		if (state==LOOK_AT)
		{
			while (phon!=NULL && state==LOOK_AT)
			{
				end_sec+=phon->dur;	
				NextState(&speaker_state);
				state=GetState(speaker_state);
				phon=(phoneme*)phon->next;	
			}
			fprintf(face_log,"LOOK_AT da %d a %d\n", (int)(start_sec*FRAMERATE), (int)(end_sec*FRAMERATE));
			((FaceAnimationGenerator*)animationcalculator)->LookAt(end_sec,start_sec);
		}
		else
			if (state==LOOK_AWAY)
			{
				while (phon!=NULL && state==LOOK_AWAY)
				{
					end_sec+=phon->dur;
					NextState(&speaker_state);
					state=GetState(speaker_state);
					phon=(phoneme*)phon->next;	
				}
				fprintf(face_log,"LOOK_AWAY da %d a %d\n", (int)(start_sec*FRAMERATE), (int)(end_sec*FRAMERATE));
				((FaceAnimationGenerator*)animationcalculator)->LookAway(end_sec,start_sec);
			}
			start_sec=end_sec;
	}
	((FaceAnimationGenerator*)animationcalculator)->eyes_key(0,0,end_sec*FRAMERATE);
}

void FaceGazeSimulator::GazeGeneratorSpeakerBML()
{
	//	exprPtr comm_act;
	float start_sec, end_sec;
	//	i;
	int state;
	//	int start_frame, end_frame;


#ifdef OUTPUT
	fprintf(face_log,"\nSpeaker Gaze...\n\n");
#endif


	speaker_state=first_speaker_state;
	start_sec=0;
	end_sec=0;

	for(int i=0;i<num_of_transitions;i++)
	{
		state=GetState(speaker_state);
		end_sec=start_sec;
		if (state==LOOK_AT)
		{
			while ((i<num_of_transitions)&&(state==LOOK_AT))
			{
				i++;
				end_sec+=gaze_resolution;	
				NextState(&speaker_state);
				state=GetState(speaker_state);
			}
			((FaceAnimationGenerator*)animationcalculator)->LookAt(end_sec,start_sec);
		}
		else
			if (state==LOOK_AWAY)
			{
				while ((i<num_of_transitions)&&(state==LOOK_AWAY))
				{
					i++;
					end_sec+=gaze_resolution;
					NextState(&speaker_state);
					state=GetState(speaker_state);
				}
				((FaceAnimationGenerator*)animationcalculator)->LookAway(end_sec,start_sec);
			}
			start_sec=end_sec;
	}
	((FaceAnimationGenerator*)animationcalculator)->eyes_key(0,0,end_sec*FRAMERATE);
}


//! Compute the eye gaze animation for the listener agent
void FaceGazeSimulator::GazeGeneratorListener(phoneme *phonemes)
{
	phoneme *phon;
	float start_sec, end_sec;
	int state;

#ifdef OUTPUT
	fprintf(face_log,"\nListener Gaze...\n\n");
#endif

	listener_state=first_listener_state;
	start_sec=0;
	end_sec=0;

	phon=phonemes;

	while (phon!=NULL)
	{			
		state=GetState(listener_state);
		end_sec=start_sec;
		if (state==LOOK_AT)
		{
			while (phon!=NULL && state==LOOK_AT)
			{
				end_sec+=phon->dur;	
				NextState(&listener_state);
				state=GetState(listener_state);
				phon=(phoneme*)phon->next;	
			}
			((FaceAnimationGenerator*)animationcalculator)->LookAt(end_sec,start_sec);
		}
		else
			if (state==LOOK_AWAY)
			{
				while (phon!=NULL && state==LOOK_AWAY)
				{
					end_sec+=phon->dur;
					NextState(&listener_state);
					state=GetState(listener_state);
					phon=(phoneme*)phon->next;	
				}
				((FaceAnimationGenerator*)animationcalculator)->LookAway(end_sec,start_sec);
			}
			start_sec=end_sec;
	}
}

//! Given the start time of a phoneme returns
//! the expression which contains the phoneme

CommunicativeAct *FaceGazeSimulator::FindCommActFromPhoneme(float phoneme_start)
{
	CommunicativeAct *comm_act;

	comm_act=((FaceEngine*)engine)->last_comm_act;
	//	comm_act=first_expr->next;

	while (comm_act)
	{
		if (strcmp(comm_act->function,"text")&&
			comm_act->start_time<=phoneme_start&&
			phoneme_start<=comm_act->start_time+comm_act->dur_time)
			return comm_act;
		comm_act=(CommunicativeAct*)comm_act->prev;
	}

	return NULL;
}

//hese fucntions are copied from the belief.cpp file

int FaceGazeSimulator::GetGazeState( int prev_state_speaker,int prev_state_listener,float LIMIT,float SCALE)
{

	int i,j;
	gaze_candidate *set;
	double r,o;
	int prev_state;

	//THE PREVOIUS VERSION
	//k=GetNumberOfStates(d,node);
	//array=GetArrayState(d,node);

	//IT WORKS ONLY IF THE NODE HAS 4 STATES (S0L0,S1L1,S2L2,S3L3)
	const int k=4;
	double array[k];

	//S1L1
	array[0] = gpf.getValue(0);
	//S0L1
	array[1] = gpf.getValue(1);
	//S1L0
	array[2] = gpf.getValue(2);
	//S0L0
	array[3] = gpf.getValue(3);

	gpf.clear();

	set=(gaze_candidate*)malloc(sizeof(gaze_candidate)*k);

	if (prev_state_speaker==LOOK_AT&&prev_state_listener==LOOK_AT)
		prev_state=S1L1;
	else
		if (prev_state_speaker==LOOK_AWAY&&prev_state_listener==LOOK_AT)
			prev_state=S0L1;
		else
			if (prev_state_speaker==LOOK_AT&&prev_state_listener==LOOK_AWAY)
				prev_state=S1L0;
			else
				if (prev_state_speaker==LOOK_AWAY&&prev_state_listener==LOOK_AWAY)
					prev_state=S0L0;

#ifdef DEBUG_GAZE
	fprintf(face_log,"Stato precedente:%s\n",stringstate(prev_state));
	fprintf(face_log,"Probabilita' BN:\n");
	for (i=0;i<k;i++)
		fprintf(face_log,"%s:%f\n",stringstate(i),array[i]);
#endif

	for (i=0,j=0;i<k;i++)
	{
		if (array[i]>LIMIT||i==prev_state)
		{
			set[j].id=i;
			set[j].p=array[i];
			j++;
		}
	}

	for (i=0;i<j;i++)
	{
		if (set[i].id==prev_state)
			set[i].p*=SCALE;
	}

	for (i=0,o=0;i<j;i++)
		o+=set[i].p;

	for (i=0;i<j;i++)
		set[i].p/=o;
#ifdef DEBUG_GAZE
	fprintf(face_log,"Probabilita' finali:\n");
	for (i=0;i<j;i++)
		fprintf(face_log,"%s:%f\n",stringstate(set[i].id),set[i].p);
#endif
	r=randomgen->GetRand01();
#ifdef DEBUG_GAZE
	fprintf(face_log,"valore casuale: %f\n",r);
#endif
	o=set[0].p;
	for (i=0;i<j;i++)
	{
		if (r<=o)
		{
#ifdef DEBUG_GAZE
			fprintf(face_log,"Nuovo stato: %s\n",stringstate(set[i].id));
#endif
			return set[i].id;
		}
		o=o+set[i+1].p;
	}

	return 1;
}

//! Set current gaze state for speaker or listener
void FaceGazeSimulator::SetState(unsigned char *speaker_or_listener,int new_state)
{
	*speaker_or_listener=new_state;
}

//! Get current gaze state for speaker or listener
int FaceGazeSimulator::GetState(unsigned char *speaker_or_listener)
{
	return (int)(*speaker_or_listener);
}

void FaceGazeSimulator::CreateStateTransitions(int n, int state)
{
	int i;

	speaker_state=(unsigned char*)malloc(sizeof(unsigned char)*num_of_transitions);
	listener_state=(unsigned char*)malloc(sizeof(unsigned char)*num_of_transitions);

	if (!speaker_state||!listener_state)
	{
		printf("out of memory\n");
		exit(2);
	}
	// commentando la linea successiva ho sempre
	// la stessa sequenza di numeri casuali
	//init_genrand(clock());
	for (i=0;i<n;i++)
	{
		speaker_state[i]=state;
		listener_state[i]=state;
	}

	num_of_transitions=n;

	first_speaker_state=speaker_state;
	first_listener_state=listener_state;


}

//! Skip to next gaze state for speaker or listener
void FaceGazeSimulator::NextState(unsigned char **speaker_or_listener)
{
	*speaker_or_listener+=1;
}
