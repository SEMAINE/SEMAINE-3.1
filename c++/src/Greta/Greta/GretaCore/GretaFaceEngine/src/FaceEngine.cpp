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

// FaceEngine.cpp: implementation of the FaceEngine class.
//
//////////////////////////////////////////////////////////////////////
/**
* FaceEngine class: calculates agent's face animation
*
* @see GestureEngine, APML_AgentEngine
* @see greta.c
* @author Maurizio Mancini, manmau@yahoo.com
* @version 1.0
*/

#include "IntensityFilter.h"
//#include "NewExpressionContainer.h" 
//#include "ComplexEmotionManager.h"

#include "DataContainer.h"
#include "FaceEngine.h"
#include "IniManager.h"
#include "FileNames.h"
#include "ComplexExpression.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <direct.h>
#include "RandomGen.h"
#include "GazePseudoBelief.h"

extern RandomGen *randomgen;
extern DataContainer *datacontainer;

//at the palce of the gaze belief we use it
GazePseudoBelief gpf;

//extern "C" int** get_fap_id(char*,float);

extern IniManager inimanager;

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <process.h>

extern FileNames filenames;
extern FILE *face_log;

#define OUTPUT

//constructor, does nothing
FaceEngine::FaceEngine(CommunicativeAct *first_comm_act,CommunicativeAct *last_comm_act, bool bml)
{
	this->first_comm_act=first_comm_act;
	this->last_comm_act=last_comm_act;

	tmp_file_index=1;

	FAPFileGenerator = new FaceAnimationGenerator(this,first_comm_act, bml);


	/* DEBUG VERSION HEAP CORRUPTION CHECK 
	added by bjoern 2004-04-21          */
	// Get current flag
	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

	// Turn on check function at every alloc/dealloc
	//tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;

	// Set flag to the new value
	//_CrtSetDbgFlag( tmpFlag );
}
// add the first (not neutral) frame for interpolation

/*
* in the realtime version is used to define the first frame of animation
*
*/

void FaceEngine::setFirstFrame(FAPFrame *frame, bool yes)
{
	if(yes==true) 
	{
		FAPFileGenerator->first_frame=(*frame).clone();
		FAPFileGenerator->first_frame_flag=true;
	}
	else
		FAPFileGenerator->first_frame_flag=false;
}

void FaceEngine::InvertSlashes(char* target)
{
	int i,l;
	l=strlen(target);
	for(i=0;i<l;i++)
		if(target[i]=='\\')target[i]='/';
}

int FaceEngine::CalculateTurn(std::string fapfilename,float animationlength,PhonemeVector *PhoVector,phoneme *phonemes, int bml, float speech_delay)
{
	//std::string facialdisplayslibrarypath;
	FaceExpression *fe;
	CommunicativeAct* comm_act;
	int number_of_comm_act;

	number_of_comm_act=0;

	//not used
	//init_belief_networks();

	FAPFileGenerator->setFaceExpressionDictionary(datacontainer->getFaceExpressionDictionary());
	//if(FAPFileGenerator->Init()==0)	return 0;
	
	//JAVA VERSION is not used	
	//ComplexEmotionManager *complexemotionmanager = datacontainer->getComplexEmotionManager();
	
	NewExpressionContainer *exppar = datacontainer -> getNewExpressionContainer();
	
	IntensityFilter * intfilt = datacontainer ->getIntensityFilter();

	int licznik=0;

	comm_act=first_comm_act;
	while(comm_act!=NULL)
	{
		stringToLower(comm_act->function);
		std::string function=comm_act->function;
		stringToLower(comm_act->value);
		std::string value=comm_act->value;
		if(function!="deictic")
		{
			CommunicativeAct* prev_comm_act;
			bool found;
			found=false;
			if((function=="affect")&&(value.substr(0,8)=="complex:"))
			{
								
				prev_comm_act=first_comm_act;
				while(prev_comm_act!=comm_act)
				{
					if(strcmp(prev_comm_act->function,comm_act->function)==0)
					{ 
						if(strcmp(prev_comm_act->value,comm_act->value)==0)
						{ 
							/* here there is a bug:

							the values of face_expr_faps and active_channels
							have to be copied by value and not by reference

							there is exception in a destructor of comm_act,
							becouse face_expr_faps is destroyed two times if it is copied by reference 

							// copy channels 
							comm_act->face_expr_faps=prev_comm_act->face_expr_faps;
							found=true;
							*/

						}
					}
					prev_comm_act=(CommunicativeAct*)prev_comm_act->next;
				}
			}
			if(found==false)
			{
				if((function=="affect")&&(value.substr(0,8)=="complex:")) //if it is complex then...
				{
					
					licznik++;				
					//find faps
					value=value.substr(8);
					
					
					/*
					//use the extended java version of complex facial expressions

					int** temp_frame = complexemotionmanager->GetExpression(function,value);										
					comm_act->face_expr_faps=temp_frame;
					*/
					

					//a new complex facial expression algorithm
					
					ComplexExpression ce;
					ce.setLibrary(datacontainer->getComplexFacialExpressionLibrary());
				
					//int **result;

					int **temp_frame=ce.generate(value);
					
					/*
					for (int i=0;i<69;i++)
					{
						if ((result[1][i]!=temp_frame[1][i])) 
						{
								if ( (i!=23)&&(i!=24)&&(i!=25)&&(i!=26)) printf(" problem in %i with %s on position %i (%i VS %i) \n", licznik, value.c_str(),i,temp_frame[1][i],result[1][i]);
						}

					}
					*/				
					

					comm_act->face_expr_faps=temp_frame;
				
					//find channels

					//default ones
					comm_act->active_channels.eyebrows=1;
					comm_act->active_channels.gaze=NEUTRAL;
					comm_act->active_channels.mouth=1;
					comm_act->active_channels.head_direction=NEUTRAL;
					comm_act->active_channels.head_movement=NEUTRAL;

					// find channels automatically					

					//this piece of code is redundant 
					//see FaceExpressionDictionary.cpp
					
					if (temp_frame!=0) 
					{

						//attention: in some cases the value for a channel will remain unchanged -> any channel will not be activated	

						//RAISING
						if ((temp_frame[1][31]>0)&&
							(temp_frame[1][32]>0)&&
							(temp_frame[1][33]>0)&&
							(temp_frame[1][34]>0)&&
							(temp_frame[1][35]>0)&&
							(temp_frame[1][36]>0)&&
							(temp_frame[1][33]>temp_frame[1][31])&&
							(temp_frame[1][34]>temp_frame[1][32])&&
							(temp_frame[1][33]>temp_frame[1][35])&&
							(temp_frame[1][34]>temp_frame[1][36])						
							)
						{
							comm_act->active_channels.eyebrows=RAISING;
						}

						//UP_DOWN -> like in sadness
						if ((temp_frame[1][31]>0)&&
							(temp_frame[1][32]>0)&&
							(temp_frame[1][33]>0)&&
							(temp_frame[1][34]>0)&&
							(temp_frame[1][35]>0)&&
							(temp_frame[1][36]>0)&&
							(temp_frame[1][37]>0)&&
							(temp_frame[1][38]>0)				
							)
						{
							comm_act->active_channels.eyebrows=UP_DOWN;
						}

						//FROWN -> 
						if ((temp_frame[1][31]<0)&&
							(temp_frame[1][32]<0)&&
							(temp_frame[1][33]<0)&&
							(temp_frame[1][34]<0)&&
							(temp_frame[1][35]<0)&&
							(temp_frame[1][36]<0)&&
							(temp_frame[1][37]>0)&&
							(temp_frame[1][38]>0)				
							)
						{
							comm_act->active_channels.eyebrows=FROWN;

						}
						//LOOK_AT
						if ((abs(temp_frame[1][23])<=3000)&&
							(abs(temp_frame[1][24])<=3000)&&
							(abs(temp_frame[1][25])<=3000)&&
							(abs(temp_frame[1][26])<=3000)
							)
						{
							comm_act->active_channels.gaze=LOOK_AT;
						}
						//LOOK_AWAY
						if ((abs(temp_frame[1][23])>3000)&&
							(abs(temp_frame[1][24])>3000)&&
							(abs(temp_frame[1][25])>3000)&&
							(abs(temp_frame[1][26])>3000)
							)
						{
							comm_act->active_channels.gaze=LOOK_AWAY;

						}
						//LIP_CORNER_UP
						if ((temp_frame[1][59]>0)&&
							(temp_frame[1][60]>0)
							)
						{
							comm_act->active_channels.mouth=LIP_CORNER_UP;

						}
						//LIP_CORNER_DOWN
						if ((temp_frame[1][59]<0)&&
							(temp_frame[1][60]<0)
							)
						{
							comm_act->active_channels.mouth=LIP_CORNER_DOWN;

						}
						//TENSE
						if ((temp_frame[1][51]>0)&&
							(temp_frame[1][52]>0)&&
							(temp_frame[1][55]>0)&&
							(temp_frame[1][56]>0)&&
							(temp_frame[1][57]>0)&&
							(temp_frame[1][58]>0))
						{
							comm_act->active_channels.mouth=TENSE;

						}
						//ASIDE
						if (abs((temp_frame[1][50])>1000))
						{
							comm_act->active_channels.head_direction=ASIDE;

						}
						//LOOK_DOWN
						if ((temp_frame[1][48]<-3000))
						{
							comm_act->active_channels.head_direction=LOOK_DOWN;

						}
						//LOOK_UP
						if (temp_frame[1][48]>3000)
						{
							comm_act->active_channels.head_direction=LOOK_UP;

						}
						//HEAD_NOD
						if (abs((temp_frame[1][48])<3000)&&
							((temp_frame[1][48]>1000)||
							(temp_frame[1][48]<-1000))
							)
						{
							comm_act->active_channels.head_movement=HEAD_NOD;
						}

						//HEAD_SHAKE
						if (abs(temp_frame[1][49])>1000)
						{
							comm_act->active_channels.head_movement=HEAD_SHAKE;
						}

						//till here is redundant					

					} // else means that cfe was not found - all channels should be neutral???

				}
				else
					if((function=="affect")&&(value.substr(0,8)=="fapfile:"))
					{
						value=value.substr(8);
						comm_act->face_expr_faps=this->LoadExpressionFromFAPFile(value);
					}
					else
					{

						//if the signal is neither "complex:" nor "fapfile:" search it in facelibrary or in EFE

						FaceExpression *fe;

						std::string function =  comm_act->function;
						std::string emotion_label =  comm_act->value;
						float emotion_intensity = comm_act->getIntensity();

						if  (function=="affect")
						{


							//TO DO : choose intensity type

							//decrease intensity or use partial expressions (see: Face relevead, Paul Ekman page 220)
							
							//partial expressions

							if (inimanager.GetValueInt("INTENSITY")==1) 
							{ //use INTENSITY_

								Expression *temp_expression = exppar->findExpressionByEmotionAndIntensity(emotion_label,emotion_intensity);

								float random = randomgen->GetRand01();

								if (temp_expression==0)
								{
									//if this expresion is not present in EFE...

									//search in facelibrary
									fe=this->FAPFileGenerator->FaceDictionary->GetExpression(comm_act->function,comm_act->value);

									if (!fe==0) 
									{  //if this expression is in facelibrary..

										//modify its intensity by facial parts
										//fe = intfilt.modify(fe,emotion_intensity);

										fe = intfilt->modify(fe,emotion_intensity,random);

									} 
									else
									{
										//the expression is not present in facelibrary

										printf("Expression unknown, %s=%s\n", comm_act->function,comm_act->value);
										comm_act->fe=0;
									}

								} 
								else
								{
									//expression is present in EFE...

									//use this definition					

									if (inimanager.GetValueFloat("INTENSITY_PROB")>random) 
									{

										fe=temp_expression->getFaceExpression();

									}
									else
									{
										//error
										temp_expression = exppar->findExpressionByEmotionAndIntensity(emotion_label,1);
										// rescale intensity
										//temp_expression -> rescaleIntensity(emotion_intensity);
										fe=temp_expression->getFaceExpression();
										fe->rescaleIntensity(emotion_intensity);

									}//End if-else random

								}//end of else

								//add expression to ca 

								if (fe!=0)  
								{
									comm_act->face_expr_faps=fe->GetRawFaps(comm_act->prob);
									comm_act->fe=fe;
									comm_act->fe->GetMovement(comm_act->prob,&comm_act->head_m_type,&comm_act->head_m_amplitude,&comm_act->head_m_period,comm_act->expressivityREP,comm_act->expressivitySPC);
								}
								else
								{							
									comm_act->fe=0;
								}


							} 
							else
							{ //OLD CODE - NO INTENSITY + Facelibrary

								fe=this->FAPFileGenerator->FaceDictionary->GetExpression(comm_act->function,comm_act->value);
								if(fe!=0)
								{

									comm_act->face_expr_faps=fe->GetRawFaps(comm_act->prob);
									comm_act->fe=fe;
									comm_act->fe->GetMovement(comm_act->prob,&comm_act->head_m_type,&comm_act->head_m_amplitude,&comm_act->head_m_period,comm_act->expressivityREP,comm_act->expressivitySPC);
								}
								else
								{
									//printf("FaceEngine: could not find the expression %s=%s\n", comm_act->function,comm_act->value);
									printf("Expression unknown: %s=%s\n", comm_act->function,comm_act->value);
									comm_act->fe=0;
								}

							}//end of else (intensity)

						} 
						else
						{ // no affect

							fe=this->FAPFileGenerator->FaceDictionary->GetExpression(comm_act->function,comm_act->value);
							if(fe!=0)
							{

								comm_act->face_expr_faps=fe->GetRawFaps(comm_act->prob);
								comm_act->fe=fe;
								comm_act->fe->GetMovement(comm_act->prob,&comm_act->head_m_type,&comm_act->head_m_amplitude,&comm_act->head_m_period,comm_act->expressivityREP,comm_act->expressivitySPC);
							}
							else
							{
								//printf("FaceEngine: could not find the expression %s=%s\n",
								//	comm_act->function,comm_act->value);
								comm_act->fe=0;
							}
						}

					}					

			}
		}
		//comm_act->Print();
		comm_act=(CommunicativeAct*)(comm_act->next);
		number_of_comm_act++;
	}

	CommunicativeAct* comm_actx=(CommunicativeAct*)first_comm_act->next;

	FAPFileGenerator->BuildChannels();

	/*
	while (comm_actx!=NULL)
	{
	printf("expression %s :\n",comm_actx->value);
	printf("Eyebrows %d \n",comm_actx->active_channels.eyebrows);
	printf("Gaze %d \n",comm_actx->active_channels.gaze);
	printf("MOuth %d \n",comm_actx->active_channels.mouth);		
	//printf("fap 31 %d :\n",comm_actx->face_expr_faps[1][31]);

	comm_actx=(CommunicativeAct*)(comm_actx->next);
	}
	*/

	//printf("generating speaker\n");
	if(FAPFileGenerator->BuildSpeakerFAPFile(fapfilename,animationlength,number_of_comm_act,PhoVector,phonemes,speech_delay)==0)
	{
		delete FAPFileGenerator;
		return 0;
	}

	//printf("generating listener\n");
	if(bml==0)
	{
		if(FAPFileGenerator->BuildListenerFAPFile(number_of_comm_act,phonemes)==0)
		{
			delete FAPFileGenerator;
			return 0;
		}
	}

	//DO NOT CANCEL exppar - WE WANT TO USE THE SAME OBJECT ALL THE TIME
	//IT WILL BE CANCELLED BY the DATACONTAINER AT THE END

	//delete exppar;

	//DO NOT CANCEL intfilt - WE WANT TO USE THE SAME OBJECT ALL THE TIME
	//IT WILL BE CANCELLED BY the DATACONTAINER AT THE END

	//delete intfilt;

	//DO NOT CANCEL COMPLEXEMOTIONMANAGER - WE WANT TO USE THE SAME CONNECTION ALL THE TIME
	//IT WILL BE CANCELLED BY the DATACONTAINER AT THE END

	//delete complexemotionmanager;

	//belief network not used
	//CloseBeliefNetworks();

	//	delete FAPFileGenerator;
	//	delete timingscalculator;

	return 1;
}

//destructor
FaceEngine::~FaceEngine()
{
	if(this->FAPFileGenerator!=0)
		delete this->FAPFileGenerator;
	this->FAPFileGenerator=0;
	fprintf(face_log,"\nface_log closed correctly\n");
	fclose(face_log);
}

int FaceEngine::WriteFapFile(std::string fapfilename)
{
	FAPwriter fw;
	fw.WriteToFile(FAPFileGenerator->GetAnimation(),fapfilename,25);
	return 1;
}

void FaceEngine::stringToLower(std::string &s)
{
	for(int i=0;i<s.length();i++)
		s[i]=tolower(s[i]);
}

void FaceEngine::stringToLower(char *s)
{
	for(int i=0;i<strlen(s);i++)
		s[i]=tolower(s[i]);
}

int **FaceEngine::LoadExpressionFromFAPFile(std::string filename)
{
	float version;
	int fps,numberofframes,fapnum,i,framenumber;
	char basename[200];
	char cbuf[1000];
	FILE *fapfile;

	int **faps;

	faps=(int**)malloc(sizeof(int*)*2);
	for(i=0;i<2;i++)
	{
		faps[i]=(int*)malloc(sizeof(int)*69);
		for(int j=0;j<69;j++)
			faps[i][j]=0;
	}

	if(!(fapfile = fopen(filename.c_str(),"r")))
	{
		printf("WARNING: can't read %s\n",filename.c_str());
		return 0;
	}
	while(1)
	{/*read header line*/
		if(!fscanf(fapfile,"%s",cbuf))
		{
			printf("ERROR: bad fapfile: %s\n",cbuf);
			return 0;
		}
		if(cbuf[0] == '#')
			while(getc(fapfile) != '\n');
		else
		{
			sscanf(cbuf,"%f ",&version);
			fscanf(fapfile,"%s %d %d",&basename,&fps,&numberofframes);
			if(fabs(version - 2.1) > 0.001)
				printf("WARNING: Expecting FAP file version 2.1 not %f\n",version);
			while(getc(fapfile) != '\n');
			break;
		}
	}

	while(1)
	{
		for(fapnum=1;fapnum<68;fapnum++)
		{
			fscanf(fapfile,"%d",&faps[0][fapnum]);
		}
		while(getc(fapfile) != '\n');
		break;
	}

	if(cbuf[0] == '#') while(getc(fapfile) != '\n');
	else
	{
		fscanf(fapfile,"%d",&framenumber);
		for(fapnum=0;fapnum<68;fapnum++)
		{
			if(faps[0][fapnum]==1)
			{
				fscanf(fapfile,"%d",&faps[1][fapnum]);
			}
		}
	}

	return faps;
}