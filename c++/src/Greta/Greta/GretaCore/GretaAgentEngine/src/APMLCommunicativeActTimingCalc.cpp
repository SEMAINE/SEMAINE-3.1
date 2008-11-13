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

// CommunicativeActTimingCalc.cpp: implementation of the CommunicativeActTimingCalc class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceEngine.h"
#include "APMLCommunicativeActTimingCalc.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "IniManager.h"
#include "FileNames.h"

extern IniManager inimanager;

extern FILE *face_log;
extern FileNames filenames;

#define OUTPUT


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CommunicativeActTimingCalc::CommunicativeActTimingCalc(void *e)
{
	engine=e;
	first_text_act=0;
	last_text_act=0;
	number_of_comm_act=0;
	all_phonemes_list=0;

	TMP = new EngineParameter();
	TMP->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_TMP"));
}

CommunicativeActTimingCalc::~CommunicativeActTimingCalc()
{
	delete TMP;
}

void CommunicativeActTimingCalc::CalculateTimings(CommunicativeAct* first_comm_act,float &animation_time, int &num_of_comm_act)
{
	float attack,decay,sustain,release;
	
//	int i;
	CommunicativeAct* comm_act;
	float current_time;

	// Scan for first and last phrase

	all_phonemes_list=NULL;

	comm_act=(CommunicativeAct*)(first_comm_act->next);
	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
		{
			if (!first_text_act)
				first_text_act=comm_act;
			last_text_act=comm_act;
		}
		comm_act=(CommunicativeAct*)(comm_act->next);
	}
	//printf("first_text_act:%s\n",first_text_act->value);
	//printf("last_text_act:%s\n",last_text_act->value);

	// Calculate TEXT only times (duration)

	comm_act=(CommunicativeAct*)(first_comm_act->next);
	while (comm_act)
	{
		if (strcmp(comm_act->function,"text")==0)
		{
			CalcTextDur(comm_act);
		}
		comm_act=(CommunicativeAct*)(comm_act->next);
	}

	// Fix Text phonemes duration

	if(FixTextDurPho(first_comm_act)==0)
	{
		animation_time=0;
		number_of_comm_act=0;
		return;
	}

	// Calculate EXPR only times (duration)

	comm_act=(CommunicativeAct*)(first_comm_act->next);
	while (comm_act)
	{
		if (strcmp(comm_act->function,"text")!=0)
		{
			CalcCommActDur(comm_act);
		}
		number_of_comm_act++;
		comm_act=(CommunicativeAct*)(comm_act->next);
	}

	// Calculate total time and start time for every communicative act and text

	comm_act=(CommunicativeAct*)(first_comm_act->next);
	current_time=0;	
	while (comm_act)
	{
		CommunicativeAct* prev;

		prev=(CommunicativeAct*)(comm_act->prev);
		
		if (prev&&strcmp(comm_act->function,"boundary")) // skip boundary and recalcutate them later
			comm_act->start_time=current_time;
		//else
		//if (!strcmp(comm_act->function,"boundary"))
		//	CalcBoundaryTiming(comm_act);
		else
			comm_act->start_time=0;
		
		if (!strcmp(comm_act->function,"text"))
			current_time+=comm_act->dur_time;

		//if (comm_act->start_time==(comm_act->start_time+comm_act->dur_time))
		//	printf("\n\nWarning: START_TIME=END_TIME for (%s,%s)!!!!!\n\n",comm_act->function,comm_act->value);
		comm_act=(CommunicativeAct*)(comm_act->next);
	}

	// recalculate boundary times 
	comm_act=(CommunicativeAct*)(first_comm_act->next);
	while (comm_act)
	{
		CommunicativeAct* prev;

		prev=(CommunicativeAct*)(comm_act->prev);
		
		if (!strcmp(comm_act->function,"boundary"))
			CalcBoundaryTiming(comm_act);
		comm_act=(CommunicativeAct*)(comm_act->next);
	}


	// consecutive emphasis have equal probability (then equal FD)
	comm_act=(CommunicativeAct*)(first_comm_act->next);
	while (comm_act)
	{
		if ((strcmp(comm_act->function,"emphasis")==0)||(strcmp(comm_act->function,"boundary")==0))
		{
			CommunicativeAct* comm_act2;

			comm_act2=FindConsecutiveExpr(comm_act);
			if (comm_act2&&!strcmp(comm_act->value,comm_act2->value))
				comm_act2->prob=comm_act->prob;

		}
		comm_act=(CommunicativeAct*)(comm_act->next);
	}


	// TODO: turns file generation

	comm_act=(CommunicativeAct*)(first_comm_act->next);

//TURNS FILE GENERATION - may be no more needed
	FILE* turns_file;
	turns_file=fopen(filenames.Turns_File.c_str(),"w");
	while (comm_act)
	{
		// 2004-07-11: appended comm_act->intensity to pass intensity info to gesture engine for
		// overall activation expressivity setting.
		if(turns_file&&strcmp(comm_act->function,"deictic")&&strcmp(comm_act->function,"text")) {
			
			fprintf(turns_file,"%s;%s=%s;%.3f;%.3f;%.3f;\n",
				comm_act->rheme_or_theme==PHONEME_RHEME?"rheme":(comm_act->rheme_or_theme==PHONEME_THEME?"theme":"empty"),
				comm_act->function,
				comm_act->value,
				comm_act->start_time,
				comm_act->dur_time);
		} else if (turns_file&&strcmp(comm_act->function,"text")) {
			fprintf(turns_file,"%s;%s=%s;%.3f;%.3f;%.3f;\n",
				comm_act->rheme_or_theme==PHONEME_RHEME?"rheme":(comm_act->rheme_or_theme==PHONEME_THEME?"theme":"empty"),
				comm_act->function,
				comm_act->obj,
				comm_act->start_time,
				comm_act->dur_time);
		}
		comm_act=(CommunicativeAct*)(comm_act->next);
	}
	fclose(turns_file);

	comm_act=first_comm_act;
	while(comm_act!=NULL)
	{
		CalcAttackDecaySustainRelease(comm_act,&attack,&decay,&sustain,&release);
		comm_act->AddPhase(caphase_attack,attack);
		comm_act->AddPhase(caphase_decay,decay);
		comm_act->AddPhase(caphase_sustain,sustain);
		comm_act->AddPhase(caphase_release,release);
		comm_act=(CommunicativeAct*)(comm_act->next);
		
		//comm_act->Print();
	}

	phoneme *pho_ptr;
	pho_ptr=this->all_phonemes_list;
	animation_time=0;
	while(pho_ptr)
	{
		animation_time=animation_time+pho_ptr->dur;
		pho_ptr=(phoneme*)pho_ptr->next;
	}

	//ELISABETTA END

	this->number_of_comm_act=number_of_comm_act;
}

//! Compute the duration of text communicative act 'comm_act'
//! by using the phonemes list given by Festival

int CommunicativeActTimingCalc::CalcTextDur(CommunicativeAct *comm_act)
{
	FILE *pho_file;
	float dur_text_act;
	float dur_pho;
	char phoneme_string[256];
	phoneme *pho_ptr;
	phoneme *temp;

	comm_act->phonemes_list=NULL;

	pho_file=fopen(comm_act->phonemes_file.c_str(),"r");
	if (!pho_file)
	{
#ifdef OUTPUT
		fprintf(face_log,"Error reading '%s'\n",comm_act->phonemes_file.c_str());
#endif
		
		return 0;
	}

	while (fscanf(pho_file,"%s %f",phoneme_string,&dur_pho)!=EOF)
	{
							
		pho_ptr=(phoneme*)malloc(sizeof(phoneme));

		if (pho_ptr == NULL) 
		{
#ifdef OUTPUT
			fprintf(face_log,"out of memory\n");
#endif

			return 0;
		}

     	memset(pho_ptr, 0, sizeof(phoneme));
		pho_ptr->pho=(char*)malloc(sizeof(char)*(strlen(phoneme_string)+1));
		if (pho_ptr->pho==NULL)
		{
#ifdef OUTPUT
			fprintf(face_log,"out of memory\n");
#endif

			return 0;
		}
		strcpy(pho_ptr->pho,phoneme_string);
		pho_ptr->dur=dur_pho;
		if (comm_act->phonemes_list==NULL)
			comm_act->phonemes_list=pho_ptr;
		else
		{
			temp=comm_act->phonemes_list;
			while (temp->next)
				temp=(phoneme*)(temp->next);
			temp->next=pho_ptr;
		}
	}
	

	fclose(pho_file);
#ifndef DEBUG
//	unlink(comm_act->phonemes_file);
#endif


    if (comm_act!=first_text_act)
	{
		pho_ptr=comm_act->phonemes_list;
		if (pho_ptr)
		{
			//maurizio 4-5-2004
			if((strcmp(((phoneme*)(pho_ptr))->pho,"pau")==0)||
				(strcmp(((phoneme*)(pho_ptr))->pho,"#")==0)||
				(strcmp(((phoneme*)(pho_ptr))->pho,"_")==0))
			{
				comm_act->phonemes_list=(phoneme*)(pho_ptr->next);
				free(pho_ptr);
			}
		}
	}

	if (comm_act!=last_text_act)
	{
		pho_ptr=comm_act->phonemes_list;

		if (pho_ptr)
		{
			temp=NULL;
			while (pho_ptr->next)
			{
				temp=pho_ptr;
				pho_ptr=(phoneme*)(pho_ptr->next);
			}
			//maurizio 4-5-2004
			if(temp!=NULL)
				if(strcmp(((phoneme*)(temp->next))->pho,"pau")==0||
				(strcmp(((phoneme*)(pho_ptr))->pho,"#")==0)||
				(strcmp(((phoneme*)(pho_ptr))->pho,"_")==0))
				{
					free(temp->next);
					temp->next=NULL;
				}
		}
	}

	dur_text_act=0;

	pho_ptr=comm_act->phonemes_list;
//	printf("text:%s\n",comm_act->value);
	while (pho_ptr)
	{
//		printf("Pho:%s - Dur:%f\n",pho_ptr->pho,pho_ptr->dur);
		dur_text_act+=pho_ptr->dur;	
		pho_ptr=(phoneme*)(pho_ptr->next);
	}
	comm_act->dur_time=dur_text_act;
	return 1;
}

//! Compute the duration of communicative act 'comm_act'
//! by using the list comm_act->text_list builded by the
//! function BuildTextDep.

void CommunicativeActTimingCalc::CalcCommActDur(CommunicativeAct *comm_act)
{
	text_l *text_ptr;
	CommunicativeAct *comm_act2;

	text_ptr=comm_act->text_list;
//	printf("comm_act:%s\n",comm_act->value);
	//MAJOR BUG: MOVED HERE
	comm_act2=(CommunicativeAct*)(comm_act->next);

	while (text_ptr)
	{
	//	printf("text_ptr->s:%s - ",text_ptr->s);
		//REMOVED HERE
		

		while (comm_act2)
		{
			if (!strcmp(comm_act2->function,"text"))
				if (!strcmp(comm_act2->value,text_ptr->s))
				{
				//	printf("MATCHED:%s\n",comm_act2->value);
					comm_act->dur_time+=comm_act2->dur_time;
					break;

				}
			comm_act2=(CommunicativeAct*)(comm_act2->next);
		}
		text_ptr=(text_l *)(text_ptr->next);
	}
//	printf("\n");
}

int CommunicativeActTimingCalc::FixTextDurPho(CommunicativeAct* first_communicative_act)
{
	FILE *pho_file;
	float dur_pho;
	char phoneme_string[256];
	CommunicativeAct *comm_act;
	phoneme *pho_ptr;
	phoneme *pho_ptr2;
	phoneme *pho_ptr3;
	phoneme *prev_pho_ptr;

	float totaltime=0;
	
	Phoneme *Pho;

	// build phonemes list from file

	pho_file=fopen(filenames.Phonemes_File.c_str(),"r");
	if (!pho_file)
	{
#ifdef OUTPUT
		fprintf(face_log,"Error reading '%s'\n",filenames.Phonemes_File.c_str());
#endif

		return 0;
	}

	while (fscanf(pho_file,"%s %f",phoneme_string,&dur_pho)!=EOF)
	{
		
		//printf("%s %f - ",phoneme_string,dur_pho);
	
						
		pho_ptr=(phoneme*)malloc(sizeof(phoneme));

		if (pho_ptr == NULL) 
		{
#ifdef OUTPUT
			fprintf(face_log,"out of memory\n");
#endif
			
			return 0;
		}

		memset(pho_ptr, 0, sizeof(phoneme));
		pho_ptr->pho=(char*)malloc(sizeof(char)*(strlen(phoneme_string)+1));
		if (pho_ptr->pho==NULL)
		{
#ifdef OUTPUT
			fprintf(face_log,"out of memory\n");
#endif
			
			return 0;
		}

		Pho=new Phoneme(phoneme_string,totaltime,dur_pho);
		PhoVector.push_back(*Pho);
		totaltime+=dur_pho;
		strcpy(pho_ptr->pho,phoneme_string);
		pho_ptr->dur=dur_pho;
		if (all_phonemes_list==NULL)
			all_phonemes_list=pho_ptr;
		else
		{
			pho_ptr2=all_phonemes_list;
			while (pho_ptr2->next)
				pho_ptr2=(phoneme*)(pho_ptr2->next);
			pho_ptr2->next=pho_ptr;
		}
		
	}

	fclose(pho_file);


//	printf("ALL PHONEMES LIST\n");
//	printf("-----------------\n");
//	pho_ptr=all_phonemes_list;
//	while (pho_ptr)
//	{
//		printf("pho:%s - dur:%f\n",pho_ptr->pho,pho_ptr->dur);
//		pho_ptr=pho_ptr->next;
//	}

	pho_ptr=all_phonemes_list;
	comm_act=(CommunicativeAct*)(first_communicative_act->next);

//	printf("\nCORRECT PHONEMES LIST\tWRONG PHONEMES LIST\n");
//	printf("---------------------\t-------------------\n");
	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
		{
			
			pho_ptr2=comm_act->phonemes_list;
			prev_pho_ptr=NULL;
			while (pho_ptr&&pho_ptr2)
			{
//				printf("pho:%s\t - dur:%f\tpho:%s - dur:%f\n",pho_ptr->pho,pho_ptr->dur,pho_ptr2->pho,pho_ptr2->dur);	
//				if (strcmp(pho_ptr->pho,pho_ptr2->pho)&&!strcmp(pho_ptr->pho,"pau"))
				if ((strcmp(pho_ptr->pho,pho_ptr2->pho)!=0)&&
					(!strcmp(pho_ptr->pho,"pau")||!strcmp(pho_ptr->pho,"#")||!strcmp(pho_ptr->pho,"_")))
				{
					/* block gets executed if pho_ptr!=pho_ptr2 AND pho_ptr DOES point to (pau or #) */
					/** TODO: Heap check during this malloc fails some where in the middle
					of pro0cessing the long "into" dialog. Why? */
					pho_ptr3=(phoneme*)malloc(sizeof(phoneme));
					if (pho_ptr3 == NULL) 
					{
#ifdef OUTPUT
						fprintf(face_log,"out of memory\n");
#endif
						
						return 0;
					}

					memset(pho_ptr3, 0, sizeof(phoneme));
					pho_ptr3->pho=(char*)malloc(sizeof(char)*(strlen(pho_ptr->pho)+1));
					if (pho_ptr3->pho==NULL)
					{
#ifdef OUTPUT
						fprintf(face_log,"out of memory\n");
#endif
						
						return 0;
					}
					strcpy(pho_ptr3->pho,pho_ptr->pho);
					pho_ptr3->dur=pho_ptr->dur;
					if (prev_pho_ptr)
					{
						prev_pho_ptr->next=pho_ptr3;
						pho_ptr3->next=pho_ptr2;
						pho_ptr2=pho_ptr3;
						//pho_ptr2=pho_ptr2->next;
						//printf("1!\n");
					}
					else
					{
						pho_ptr3->next=pho_ptr2;
						comm_act->phonemes_list=pho_ptr3;
						pho_ptr2=pho_ptr3;
						//pho_ptr2=pho_ptr2->next;
	
						//printf("2!\n");
					}
				}
				else { 
					if (strcmp(pho_ptr->pho,pho_ptr2->pho))
					{
						/* if pho_ptr->pho,pho_ptr2->pho are different: */
						/* BUG corrected 2004-04-21:
						 DANGER: if pho_ptr->pho is longer than pho_ptr2->pho
						 we were corrupting the heap! */
						if(strlen(pho_ptr->pho)>strlen(pho_ptr2->pho)) {
							free(pho_ptr2->pho);
							pho_ptr2->pho=(char *)malloc(sizeof(char)*(strlen(pho_ptr->pho)+1));
						}
						strcpy(pho_ptr2->pho,pho_ptr->pho);
						/**************************************************/

						pho_ptr2->dur=pho_ptr->dur;
					} else {
						pho_ptr2->dur=pho_ptr->dur;
					}
				}
				prev_pho_ptr=pho_ptr2;				
				pho_ptr2=(phoneme*)(pho_ptr2->next);
				pho_ptr=(phoneme*)(pho_ptr->next);
			}
//			printf("---------------------\t-------------------\n");
		}
		comm_act=(CommunicativeAct*)(comm_act->next);
	}


	pho_ptr=all_phonemes_list;
	comm_act=(CommunicativeAct*)(first_communicative_act->next);
//	printf("\n");
//	printf("CORRECT PHONEMES LIST\tFIXED PHONEMES LIST\n");
//	printf("---------------------\t-------------------\n");
	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
		{
			pho_ptr2=comm_act->phonemes_list;
			while (pho_ptr&&pho_ptr2)
			{
				
//				printf("pho:%s - dur:%f\tpho:%s - dur:%f\n",pho_ptr->pho,pho_ptr->dur,pho_ptr2->pho,pho_ptr2->dur);	
				if (strcmp(pho_ptr->pho,pho_ptr2->pho)||
					pho_ptr->dur!=pho_ptr2->dur)
				{
#ifdef OUTPUT
					fprintf(face_log,"Warning (1): there was a problem in with the phonemes list!\n");
					fprintf(face_log,"The audio and the video could not be synchronized...");
#endif
					//exit(2);
				}
				if ((!(pho_ptr->next)&&(pho_ptr2->next)))
					pho_ptr2->next=NULL;

				pho_ptr2=(phoneme*)(pho_ptr2->next);
				pho_ptr=(phoneme*)(pho_ptr->next);
			}
/*			if ((!pho_ptr&&pho_ptr2))
			{
				pho_ptr2->next=NULL;

				printf("Warning (2): there was a problem in with the phonemes list!\n");
				printf("The audio and the video could not be synchronized...\n");
				while (pho_ptr2)
				{
					printf("pho:%s - dur:%f\n",pho_ptr2->pho,pho_ptr2->dur);	
					pho_ptr2=pho_ptr2->next;
				}
				printf("Remaining list\n");

			}
*/	
//			printf("---------------------\t-------------------\n");
		}
		comm_act=(CommunicativeAct*)(comm_act->next);
	}


	comm_act=(CommunicativeAct*)(first_communicative_act->next);
	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
		{
			float dur_text_act;

			dur_text_act=0;
//			printf("\nTEXT:%s\n",comm_act->value);
			pho_ptr=comm_act->phonemes_list;
			while (pho_ptr)
			{
//				printf("pho:%s - dur:%f\n",pho_ptr->pho,pho_ptr->dur);
				dur_text_act+=pho_ptr->dur;
				pho_ptr=(phoneme*)(pho_ptr->next);
			}
			comm_act->dur_time=dur_text_act;		
//			printf("\nDuration:%f\n",comm_act->dur_time);
		}
		comm_act=(CommunicativeAct*)(comm_act->next);
	}
	return 1;
}

//! Compute the timing of boundary 'comm_act'
void CommunicativeActTimingCalc::CalcBoundaryTiming(CommunicativeAct *comm_act)
{
	//float onset,apex,offset;

	CommunicativeAct *comm_act2,*comm_act3;

	comm_act2=(CommunicativeAct *)comm_act->prev;

//	TODO: temp fix
	comm_act->start_time=comm_act2->start_time;
	comm_act3=FindEmphasisOverTheme(comm_act);
	if (!comm_act3)
		comm_act->dur_time=comm_act2->dur_time;
	else
	{
		if (comm_act3->start_time-(comm_act->start_time+comm_act2->dur_time)>=MIN_HEAD_OFFSET)
			comm_act->dur_time=comm_act3->start_time-comm_act->start_time;
		else
			comm_act->dur_time=comm_act2->dur_time;
	}

}

//! Returns the communicative act which 'temporally' follows the communicative act 'comm_act'
CommunicativeAct *CommunicativeActTimingCalc::FindConsecutiveExpr(CommunicativeAct *comm_act)
{
	CommunicativeAct *comm_act2;
	float k1,k2;

	k1=(comm_act->start_time+comm_act->dur_time)*FRAMERATE;

	comm_act2=(CommunicativeAct*)(comm_act->next);

	while (comm_act2)
	{
		k2=comm_act2->start_time*FRAMERATE;
		if ((k1==k2)&&(strcmp(comm_act->value,comm_act2->value)==0))
			return comm_act2;
		comm_act2=(CommunicativeAct*)(comm_act2->next);
	}
	return NULL;
}

/*
//! Compute constant 'onset', 'apex', 'offset' temporal parameters
//! give a duration 'time'
void CommunicativeActTimingCalc::CalcOnsetApexOffset(float *onset, float *apex, float *offset, float time)
{
	if (time>=0.4)
	{
		*onset=(float)0.2;
		*apex=time-(float)0.4;
		*offset=(float)0.2;
	}
	else
	{
		*onset=(float)time*(float)0.5;
		*apex=0;
		*offset=(float)time*(float)0.5;
	}

}
*/

//! Compute  'attack', 'decay', 'sustain', 'release' temporal parameters
//! for communicative act 'comm_act'

void CommunicativeActTimingCalc::CalcAttackDecaySustainRelease(CommunicativeAct *comm_act,float *attack, float *decay, float *sustain, float *release)
{
	float A, D, R;
	float comm_act_dur;

	if(comm_act->expressivityTMP>=-1 && comm_act->expressivityTMP<=1)
		TMP->SetValue(comm_act->expressivityTMP);

	comm_act_dur=comm_act->dur_time;

	if(comm_act_dur<0.04)
	{
		A=0; D=0; R=0;
	}
	else
	{
	//A,D,S,R vary according to TEMPORAL value. -1<=TMP<=1
	/* old values
		max_A=600;
		min_A=240;
		max_D=480;
		min_D=0;
		max_R=720;
		min_R=280;
	*/
	/* new values
		max_A=800;
		min_A=240;
		max_D=680;
		min_D=0;
		max_R=920;
		min_R=280;
	*/

		TMP->SetScaling(0.240f, 0.800f);
		A = TMP->GetScaledValue();

		TMP->SetScaling(0.000f, 0.680f);
		D = TMP->GetScaledValue();

		TMP->SetScaling(0.280f, 0.920f);
		//TMP->SetScaling(0.280f, 1.280f);
		R = TMP->GetScaledValue();

		if((A+D+R)>comm_act_dur)
		{
			if((A+R)>comm_act_dur)
			{
				A=comm_act_dur/3.0;
				R=comm_act_dur*(2.0/3.0);
				D=0;
			}
			else
				D=comm_act_dur-(A+R);
		}
	}

	*attack=A;
	*decay=D;
	if(A!=0)
		*sustain=comm_act_dur-(A+D+R);
	else
		*sustain=0;

	*release=R;

	//printf("%s   %f   %f  %f  %f  %f\n",comm_act->value, comm_act->dur_time, A, D,*sustain,R);
}

//! Returns a theme emphasis communicative act which follows
//! the boundary 'comm_act'
CommunicativeAct *CommunicativeActTimingCalc::FindEmphasisOverTheme(CommunicativeAct *comm_act)
{
	CommunicativeAct *comm_act2;
	
	comm_act2=(CommunicativeAct *)comm_act->next;

	while (comm_act2&&strcmp(comm_act2->function,"emphasis"))
	{
		if (!strcmp(comm_act2->function,"boundary"))
			return NULL;

		comm_act2=(CommunicativeAct *)comm_act2->next;
	}
	if (comm_act2&&!strcmp(comm_act2->function,"emphasis")&&comm_act2->rheme_or_theme==PHONEME_THEME)
		return comm_act2;
	return NULL;
}

//! Returns the first phoneme in text communicative act 'comm_act'
phoneme *CommunicativeActTimingCalc::FirstPhoneme(CommunicativeAct *comm_act)
{
	char *text;

	text=FirstText(comm_act);


	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
			if (!strcmp(comm_act->value,text))
				return comm_act->phonemes_list;	
		comm_act=(CommunicativeAct *)(comm_act->next);
	}
	return NULL;
}

//! Returns the last phoneme in text communicative act 'comm_act'
phoneme *CommunicativeActTimingCalc::LastPhoneme(CommunicativeAct *comm_act)
{
	phoneme *last_phoneme;
	char *text;

	text=LastText(comm_act);

	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
			if (!strcmp(comm_act->value,text))
			{
				last_phoneme=comm_act->phonemes_list;
				while (last_phoneme&&last_phoneme->next)
					last_phoneme=(phoneme*)(last_phoneme->next);
				return last_phoneme;
			}
		comm_act=(CommunicativeAct *)(comm_act->next);
	}
	return NULL;
}

//! Returns the phoneme which follows 'phoneme'
//! in the phonemes list 'comm_act->phonemes_list'
phoneme *CommunicativeActTimingCalc::NextPhoneme(CommunicativeAct *comm_act,phoneme *pho)
{
	phoneme *next_phoneme;
	char *text;

	text=FirstText(comm_act);

	//comm_act=comm_act->next;

	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
			if (!strcmp(comm_act->value,text))
			{
				next_phoneme=comm_act->phonemes_list;
				while (next_phoneme&&next_phoneme!=pho)
					next_phoneme=(phoneme*)(next_phoneme->next);
				if (next_phoneme)
					return (phoneme*)(next_phoneme->next);
				return NULL;
			}
		comm_act=(CommunicativeAct *)(comm_act->next);
	}
	return NULL;
}

//! Returns the phoneme which precede 'phoneme'
//! in the phonemes list 'comm_act->phonemes_list'
phoneme *CommunicativeActTimingCalc::PrevPhoneme(CommunicativeAct *comm_act,phoneme *pho)
{
	phoneme *prev_phoneme;
	char *text;

	text=LastText(comm_act);

	//comm_act=comm_act->next;

	while (comm_act)
	{
		if (!strcmp(comm_act->function,"text"))
			if (!strcmp(comm_act->value,text))
			{
				prev_phoneme=comm_act->phonemes_list;
				while (prev_phoneme&&prev_phoneme->next!=pho)
					prev_phoneme=(phoneme*)(prev_phoneme->next);
				return (phoneme*)(prev_phoneme);
			}
		comm_act=(CommunicativeAct *)(comm_act->next);
	}
	return NULL;
}

//! Returns the first text phrase nested in communicative act 'comm_act'
char *CommunicativeActTimingCalc::FirstText(CommunicativeAct *comm_act)
{

	if (!strcmp(comm_act->function,"boundary"))
		return FirstText((CommunicativeAct *)(comm_act->prev));

	if (comm_act->text_list)
		return (char*)comm_act->text_list->s;
	else
	if (!strcmp(comm_act->function,"text"))
		return (char*)comm_act->value;
	return NULL;
}

// Data un'espressione ritorna l'ultima frase
// contenuta nell'espressione

//! Returns the last text phrase nested in communicative act 'comm_act'
char *CommunicativeActTimingCalc::LastText(CommunicativeAct *comm_act)
{
	text_l *text_ptr;
	
	if (!strcmp(comm_act->function,"boundary"))
		return LastText((CommunicativeAct *)(comm_act->prev));

	if (!strcmp(comm_act->function,"text"))
		return (char*)comm_act->value;

	text_ptr=comm_act->text_list;
	
	while (text_ptr&&text_ptr->next)
		text_ptr=(text_l*)(text_ptr->next);
	
	if (text_ptr)
		return (char*)text_ptr->s;
	return NULL;
}

int CommunicativeActTimingCalc::CopyFilePho(std::string nameFile)
{
	std::string line;
	std::string pho;

	std::ifstream inputfile("tmp/newPho.pho");
	if(inputfile.is_open())
	{
		std::ofstream outputfile(nameFile.c_str());
		if(outputfile.is_open())
		{
			
			while((inputfile.rdstate()&std::ios::eofbit)==0)
			{
				std::getline(inputfile,line,'\n');
				if(line!="")
					outputfile << line.c_str() << "\n";
			}

			//sostituisci i file
		}
		else
		{
			printf("File %s not found!\n", nameFile.c_str());
			inputfile.close();
			return 0;
		}
		outputfile.close();
	}
	else
	{
		printf("File %s not found!\n", "newPho.pho");
		return 0;
	}
	
	inputfile.close();
	return 1;
}