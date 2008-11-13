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

// /BML Engine.cpp: implementation of the /BML Engine class.
//
//////////////////////////////////////////////////////////////////////

//BML Engine for Greta
//http://twiki.isi.edu/Public/BMLSpecification

#include ".\BML_AgentEngine.h"
#include "APMLCommunicativeActTimingCalc.h"
#include "IniManager.h"
#include "HeadEngine.h"
#include "TorsoEngine.h"
#include "RandomGen.h"
#include "FileNames.h"
#include "bapanimationjoiner.h"
#include "FAPAnimationJoiner.h"
#include "BAPwriter.h"
#include "XMLGenericParser.h"
//#include <crtdbg.h>
//#include <math.h>

extern IniManager inimanager;
extern std::string ini_filename;
extern FileNames filenames;

extern FILE* gesture_log;
extern FILE* face_log;
extern RandomGen *randomgen;


BML_AgentEngine::BML_AgentEngine(void)
{
	signals.clear();
	this->phofile="";
	
	if(inimanager.initialized==false)
	{
		if (ini_filename!="")
			inimanager.ReadIniFile(ini_filename);
		else
			inimanager.ReadIniFile("greta.ini");
	}

	gesturetorsolength=0.0f;
	if(randomgen==0)
		randomgen = new RandomGen();

	te = new TorsoSpace::TorsoEngine();
	he = new HeadSpace::HeadEngine();
	mMPlanner=0;
	faceengine=0;


	speech_size=0;
}

BML_AgentEngine::~BML_AgentEngine(void)
{
	if(randomgen!=0)
	{
		delete randomgen;
		randomgen=0;
	}
	if(te!=0)
	{
		delete te;
		te=0;
	}
	if(he!=0)
	{
		delete he;
		he=0;
	}
	if(mMPlanner!=0)
	{
		delete mMPlanner;
		mMPlanner=0;
	}
	if(faceengine!=0)
	{
		delete faceengine;
		faceengine=0;
	}
}

bool BML_AgentEngine::hasNoEndFaceSignals() 
{ 
	return noend_facesignals;
}


int BML_AgentEngine::Execute(std::string BMLfilename,char *buffer,bool writetofile,BAPFrame* startbf,FAPFrame* startff,bool isrealtime)
{
	std::string fapfilename,bapfilename;

	
	if(BMLfilename=="")
	{
		fapfilename="buffer";
		filenames.Base_File=inimanager.Program_Path+"output/"+fapfilename;
		fapfilename=inimanager.Program_Path+"output/"+fapfilename+".fap";

		bapfilename="buffer";
		bapfilename=inimanager.Program_Path+"output/"+bapfilename+".bap";

		filenames.Fap_File=fapfilename;
		filenames.Bap_File=bapfilename;
	}
	else
	{
		filenames.BuildFileNames(BMLfilename);
	}

	

	if(LoadBML(BMLfilename,buffer,isrealtime)==0)
		return 0;

	

			noend_facesignals=false;
			
			//very special case:
			//check if the signal is infinite
			//TO DO: what if there is an infinitive signal and the finite one?
			std::vector<MMSystemSpace::Signal>::iterator iter;
			for(iter=signals.begin();iter!=signals.end();iter++) 
			{			
				if((iter->noend==true)&&(((*iter).modality=="head")||((*iter).modality=="face")||((*iter).modality=="gaze")))
					noend_facesignals=true;			          
			}			
		

	faceheadlength=0;


	bool useheadengine;

	useheadengine=inimanager.GetValueInt("USE_HEADENGINE");

	if(useheadengine==true)
	{
		if(GenerateHead("")==0)
		{
			this->signals.clear();
			return 0;
		}
	}

	FAPAnimationJoiner faj;

	if(GenerateFace("",startff,isrealtime)==0)
	{
		this->signals.clear();
		return 0;
	}

	
	if(useheadengine==true)
	{
		faj.Join(faceengine->GetAnimation(),he->GetAnimation());
	}

	
	if(writetofile)
	{
		FAPwriter fw;
		fw.WriteToFile(faceengine->GetAnimation(),filenames.Fap_File,25);
	}

	if(GenerateTorso(filenames.Bap_File)==0)
	{
		this->signals.clear();
		return 0;
	}

	
	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	//tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	//_CrtSetDbgFlag( tmpFlag );

	if(GenerateGestures(filenames.Bap_File,startbf)==0)
	{
		this->signals.clear();
		return 0;
	}

	
	BAPAnimationJoiner baj;

	if(writetofile)
	{
		BAPWriter bw;
		bw.WriteToFile(baj.Join(mMPlanner->GetAnimation(),te->GetAnimation()),filenames.Bap_File);
	}
	else
	{
		baj.Join(mMPlanner->GetAnimation(),te->GetAnimation());
	}


	this->signals.clear();

	return 1;
}

FaceEngine* BML_AgentEngine::GetFaceEngine()
{
	return this->faceengine;
}

GestureSpace::MotorPlanner* BML_AgentEngine::GetMPlanner()
{
	return this->mMPlanner;
}

int BML_AgentEngine::GenerateGestures(std::string bapfilename,BAPFrame* startbf)
{
	int realtime_gestureengine;

	realtime_gestureengine=inimanager.GetValueInt("REALTIME_GESTUREENGINE");

	if((realtime_gestureengine!=0)&&(realtime_gestureengine!=1))
		realtime_gestureengine=false;

	gesture_log=fopen("logs/gesture_engine_log.txt","w");

	float start_time;

	start_time=0.0f;
	gesturetorsolength+=1.0f;

	//printf("gesture engine started\n");

	mGPlanner=new GestureSpace::GesturePlanner();

	mMPlanner=new GestureSpace::MotorPlanner();

	mGPlanner->Init();

	if(!realtime_gestureengine)
	{
		mGPlanner->Request("REST=LEGS_BOTH", -1.0, -0.5, 0.0, -2, -2, -2, -2, -2);
	}

	std::vector<MMSystemSpace::Signal>::iterator iter;
	for(iter=signals.begin();iter!=signals.end();iter++)
	{
		if((*iter).modality=="gesture")
		{
			//printf("gesture %s timings %f,%f,%f\n",(*iter).name.c_str(),(*iter).start,((*iter).start+(*iter).end)/2.0f,(*iter).end);
			if((*iter).strokes.empty())
			{
				(*iter).strokes.push_back((*iter).duration/2.0f);
			}
			if((*iter).strokes.size()==1)
				mGPlanner->Request(
				ToUpper((*iter).reference).c_str(),
				(*iter).start,
				(*iter).start+(*iter).strokes[0],
				(*iter).start+(*iter).duration,
				(*iter).GetParamValuef("SPC.value"),
				(*iter).GetParamValuef("TMP.value"),
				(*iter).GetParamValuef("FLD.value"),
				(*iter).GetParamValuef("PWR.value"),
				(*iter).GetParamValuef("REP.value")
				);
			else
			{
				std::vector<float>::iterator strokiter;
				for(strokiter=(*iter).strokes.begin();strokiter!=(*iter).strokes.end();strokiter++)
				{
					(*strokiter)+=(*iter).start;
				}
				float str=(*iter).strokes[0];
				(*iter).strokes.erase((*iter).strokes.begin());
				mGPlanner->RequestWithRepetition(
					ToUpper((*iter).reference).c_str(),
					(*iter).start,
					str,
					(*iter).start+(*iter).duration,
					(*iter).GetParamValuef("SPC.value"),
					(*iter).GetParamValuef("TMP.value"),
					(*iter).GetParamValuef("FLD.value"),
					(*iter).GetParamValuef("PWR.value"),
					(*iter).GetParamValuef("REP.value"),
					(*iter).strokes);
			}
			if(((*iter).start+(*iter).duration+2)>gesturetorsolength)
				gesturetorsolength=(*iter).start+(*iter).duration+2;
		}

	}

	if(!realtime_gestureengine)
	{
		mGPlanner->Request("REST=LEGS_BOTH", gesturetorsolength+1.50,gesturetorsolength+1.60,gesturetorsolength+1.75,-2,-2, -2, -2, -2);
		mGPlanner->InsertRestKeys(0);
	}
	else
	{
		mGPlanner->InsertRestKeys(0);
	}

	mMPlanner->Init(mGPlanner->GetGestureVector(),int(start_time*GLOBAL_FPS),int((gesturetorsolength+1.75)*GLOBAL_FPS));

	if(realtime_gestureengine)
		if(startbf!=0)
			mMPlanner->AddBAPFrame(startbf,0);

	mMPlanner->GenerateAnimation();

	//mMPlanner->WriteAnimation(bapfilename);

	//printf("gesture engine ended\n");

	fclose(gesture_log);

	return 1;
}

int BML_AgentEngine::GenerateFace(std::string fapfilename,FAPFrame* startff,bool isrealtime)
{
	float attack,decay,sustain,release;
	CommunicativeAct* first_comm_act,*comm_act;

	CommunicativeActTimingCalc timingscalculator(this);

	face_log=fopen("logs/face_engine_log.txt","w");

	bool useheadengine;

	useheadengine=inimanager.GetValueInt("USE_HEADENGINE");

	//printf("face engine started\n");

	comm_act=new CommunicativeAct();

	comm_act->function=(char*)malloc(5*sizeof(char));
	sprintf(comm_act->function,"APML");
	comm_act->value=(char*)malloc(1*sizeof(char));
	sprintf(comm_act->value,"");

	first_comm_act=comm_act;

	faceengine=new FaceEngine(first_comm_act,comm_act,true);

	//add a first (non neutral) frame of interpolation if it is neccessary
	if (startff!=0)
		faceengine->setFirstFrame(startff,true);

	//it is supposed that there is only one speech act in each bml	
	speech_delay=0.0f;

	std::vector<MMSystemSpace::Signal>::iterator iter;

	for(iter=signals.begin();iter!=signals.end();iter++)
	{
		if(((*iter).modality=="face")||((*iter).modality=="gaze")||((*iter).modality=="head"))
		{
			if(useheadengine==true)
				continue;
			std::string classname,instancename;
			if((*iter).reference!="")
			{
				classname=(*iter).reference.substr(0,(*iter).reference.find_first_of("="));
				instancename=(*iter).reference.substr((*iter).reference.find_first_of("=")+1);
			}
			else
			{
				classname=(*iter).type;
				if(classname=="NOD")
				{
					classname="HEAD";
					instancename="NOD";
				}
				if(classname=="SHAKE")
				{
					classname="HEAD";
					instancename="SHAKE";
				}
				if(classname=="TOSS")
				{
					classname="HEAD";
					instancename="TOSS";
				}
				if(classname=="ORIENT")
				{
					classname="DEICTIC";
					instancename=(*iter).direction;
				}

			}
			comm_act->next=new CommunicativeAct();
			comm_act=(CommunicativeAct*)comm_act->next;
			comm_act->function=(char*)malloc((classname.length()+1)*sizeof(char));
			sprintf(comm_act->function,"%s",classname.c_str());
			comm_act->value=(char*)malloc((instancename.length()+1)*sizeof(char));
			sprintf(comm_act->value,"%s",instancename.c_str());
			comm_act->start_time=(*iter).start;
			comm_act->dur_time=(*iter).duration;//-(*iter).start;
			comm_act->activation=1;

			comm_act->setIntensity((*iter).GetIntensity());

			comm_act->SetExpressivitySPC((*iter).GetParamValuef("SPC.value"));
			comm_act->SetExpressivityTMP((*iter).GetParamValuef("TMP.value"));
			comm_act->SetExpressivityFLD((*iter).GetParamValuef("FLD.value"));
			comm_act->SetExpressivityPWR((*iter).GetParamValuef("PWR.value"));
			comm_act->SetExpressivityREP((*iter).GetParamValuef("REP.value"));

			timingscalculator.CalcAttackDecaySustainRelease(comm_act,&attack,&decay,&sustain,&release);
			comm_act->AddPhase(caphase_attack,attack);
			comm_act->AddPhase(caphase_decay,decay);
			comm_act->AddPhase(caphase_sustain,sustain);
			comm_act->AddPhase(caphase_release,release);

			comm_act->type=(*iter).modality;

			comm_act->conflict_comm_act_list=0;
			comm_act->next=0;

			if(((*iter).start+(*iter).duration)>faceheadlength)
				faceheadlength=(*iter).start+(*iter).duration;
		}

		//it is supposed that there is only one sppech act in each bml	

		if((*iter).modality=="speech")
		{
			if((*iter).reference!="")
				filenames.Phonemes_File=(*iter).reference;
			if(
				( (*iter).start!=0 ) &&( speech_delay==0.0f ) &&( isrealtime ) 
				)
				speech_delay=(*iter).start;
		}
	}


//	if(this->hasSpeech()==false)
//		filenames.Phonemes_File="";

	if(faceengine->CalculateTurn(fapfilename,faceheadlength+1.0f,0,0, 1,speech_delay)==0)
	{
		delete faceengine;
		//printf("face engine ended with errors\n");
		fclose(face_log);
		return 0;
	}

	//printf ("faceheadlength %d : \n", faceheadlength);

	//printf("face engine ended\n");

	fclose(face_log);

	return 1;
}

int BML_AgentEngine::GenerateTorso(std::string bapfilename)
{
	te->Cleanup();

	float start_time;

	start_time=0.0f;
	gesturetorsolength=0.0f;

	//printf("torso engine started\n");

	if(!signals.empty())
	{
		std::vector<MMSystemSpace::Signal>::iterator iter;
		for(iter=signals.begin();iter!=signals.end();iter++)
		{
			if((*iter).modality=="torso")
			{
				if((*iter).strokes.empty())
				{
					(*iter).strokes.push_back((*iter).duration/2.0f);
				}

				te->ScheduleTorsoGesture((*iter),(*iter).reference);

				if(((*iter).start+(*iter).duration)>gesturetorsolength)
					gesturetorsolength=(*iter).start+(*iter).duration;
			}
		}

		te->RenderAnimation(0,gesturetorsolength+1,25);
	}


	//printf("torso engine ended\n");

	return 1;
}

int BML_AgentEngine::GenerateHead(std::string fapfilename)
{
	he->Cleanup();

	float start_time;

	start_time=0.0f;

	//printf("head engine started\n");

	if(!signals.empty())
	{
		std::vector<MMSystemSpace::Signal>::iterator iter;
		for(iter=signals.begin();iter!=signals.end();iter++)
		{
			if((*iter).modality=="head")
			{
				if((*iter).strokes.empty())
				{
					(*iter).strokes.push_back((*iter).duration/2.0f);
				}

				he->ScheduleHeadGesture((*iter),(*iter).reference);

				if(((*iter).start+(*iter).duration)>faceheadlength)
					faceheadlength=(*iter).start+(*iter).duration;
			}
		}

		he->RenderAnimation(0,faceheadlength+1,25);
	}


	//printf("head engine ended\n");

	return 1;
}


std::string BML_AgentEngine::ToUpper(std::string s)
{
	std::string r;
	int c;
	r="";
	for(int i=0; i<s.length(); i++)
	{
		c=toupper(s[i]);
		std::string character(1,c);
		r=r+character;
	}
	return r;
}

int BML_AgentEngine::LoadBML(std::string BMLfilename,char *buffer,bool isrealtime)
{
	XMLGenericParser *p;
	XMLGenericTree *t;
	std::list<XMLGenericTree*>::iterator iterch;
	std::list<XMLGenericTree*>::iterator iterbl;

	speech=0;

	
	
	p=new XMLGenericParser();

	p->SetValidating(true);
	if(BMLfilename!="")
		t=p->ParseFile(BMLfilename);
	else
		t=p->ParseBuffer(buffer);

	signals.clear();

	
	
	if(t!=0)
	{
		for(iterch=t->child.begin();iterch!=t->child.end();iterch++)
		{			
			MMSystemSpace::Signal s;
			//decode bml-to signal
			if(s.StoreBML(*iterch,inimanager)==0)
			{ 
				delete t;
				return 0;
			}
		 	signals.push_back(s);
		}
	}
	else
	{
		printf("Error parsing %s\n", BMLfilename.c_str());
		return 0;
	}

	
	
	std::sort(signals.begin(),signals.end());

	
	
	int  ln;

	speech=0;
	std::vector<MMSystemSpace::Signal>::iterator itersig;
	for(itersig=signals.begin();itersig!=signals.end();itersig++)
	{

		if((*itersig).modality=="speech")
		{
			speech=&(*itersig);
			if(speech->TemporizeTimeMarkers((*itersig).start)==0)
			{
				delete t;
				return 0;
			}
			if(filenames.Wav_File=="")
				filenames.Wav_File=filenames.Base_File+".wav";

			//if engine is real time
			//use "the speech without wav file" procedure
			if(isrealtime)
			{
				//initial size of the speech is 0
				speech_size=0;
				content_of_speech=speech->GenerateSpeechToChar(&speech_size);				
				if(content_of_speech==0)
				{
					speech_size=0;
					delete t;
					return 0;
				}
			} 
			else
			{
			if(speech->GenerateSpeech(filenames.Wav_File)==0)
			{
				delete t;
				return 0;
			}
			}

		}
	}

	
	
	for(itersig=signals.begin();itersig!=signals.end();itersig++)
	{
		ln=0;
		(*itersig).Temporize(signals, speech, &ln);
	}
	


	delete p;
	delete t;	
	return 1;
}

bool BML_AgentEngine::hasSpeech()
{
	std::vector<MMSystemSpace::Signal>::iterator itersig;
	for(itersig=signals.begin();itersig!=signals.end();itersig++)
	{
		if((*itersig).modality=="speech")
		{
			return true;
		}
	}
	return false;
}
