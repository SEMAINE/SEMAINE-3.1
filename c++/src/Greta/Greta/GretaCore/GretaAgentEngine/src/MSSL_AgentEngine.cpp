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

// 
//
//////////////////////////////////////////////////////////////////////
//MMSL Engine
//from a Multimodal Signal Score Language file
//to fap+bap+wav files

#include ".\mssl_agentengine.h"
#include "IniManager.h"
#include "GesturePlanner.h"
#include "MotorPlanner.h"
#include "FaceEngine.h"
#include "RandomGen.h"
#include "FileNames.h"
#include "XMLGenericParser.h"

extern IniManager inimanager;

extern FileNames filenames;

extern FILE* gesture_log;
extern FILE* face_log;
extern  RandomGen *randomgen;
extern std::string ini_filename;


MSSL_AgentEngine::MSSL_AgentEngine(void)
{
	facesignals.clear();
	gesturesignals.clear();
	inimanager.ReadIniFile(ini_filename);
}

MSSL_AgentEngine::~MSSL_AgentEngine(void)
{
}

int MSSL_AgentEngine::Execute(std::string MSSLfilename)
{
	std::string fapfilename,bapfilename;

	fapfilename=MSSLfilename.substr(MSSLfilename.find_last_of("/\\")+1);
	fapfilename=fapfilename.substr(0,fapfilename.find_last_of("."));
	fapfilename=inimanager.Program_Path+"output/"+fapfilename+".fap";

	bapfilename=MSSLfilename.substr(MSSLfilename.find_last_of("/\\")+1);
	bapfilename=bapfilename.substr(0,bapfilename.find_last_of("."));
	bapfilename=inimanager.Program_Path+"output/"+bapfilename+".bap";

	if(LoadMSSL(MSSLfilename)==0)
		return 0;
	if(GenerateFace(fapfilename)==0)
		return 0;
	if(GenerateGestures(bapfilename)==0)
		return 0;
	return 1;
}

int MSSL_AgentEngine::GenerateGestures(std::string bapfilename)
{
	GestureSpace::MotorPlanner *mMPlanner;
	GestureSpace::GesturePlanner *mGPlanner;

	gesture_log=fopen("logs/gesture_engine_log.txt","w");
	
	float start_time,end_time;

	start_time=0.0f;
	end_time=0.0f;

	//printf("gesture engine started\n");

	mGPlanner=new GestureSpace::GesturePlanner();

	mMPlanner=new GestureSpace::MotorPlanner();

	mGPlanner->Init();

	mGPlanner->Request("REST=LEGS_BOTH", -1.0, -0.5, 0.0, -2, -2, -2, -2, -2);

	std::vector<singlesignal>::iterator iter;
	for(iter=gesturesignals.begin();iter!=gesturesignals.end();iter++)
	{
	//	printf("gesture %s timings %f,%f,%f\n",(*iter).name.c_str(),(*iter).start,((*iter).start+(*iter).end)/2.0f,(*iter).end);
		mGPlanner->Request(ToUpper(((*iter).classname+"="+(*iter).instancename)).c_str(),(*iter).start,((*iter).start+(*iter).end)/2.0f,(*iter).end, -2, -2, -2, -2, -2);
		if((*iter).end>end_time)
			end_time=(*iter).end;
	}

	mGPlanner->Request("REST=LEGS_BOTH", end_time+1.50,end_time+1.60,end_time+1.75, -2, -2, -2, -2, -2);
	
	mGPlanner->InsertRestKeys();

	mMPlanner->Init(mGPlanner->GetGestureVector(),int(start_time*GLOBAL_FPS),int((end_time+1.75)*GLOBAL_FPS));

	mMPlanner->GenerateAnimation();

	mMPlanner->WriteAnimation(bapfilename);

	//printf("gesture engine ended\n");

	fclose(gesture_log);

	delete mMPlanner;
	delete mGPlanner;

	return 1;
}

int MSSL_AgentEngine::GenerateFace(std::string fapfilename)
{
	CommunicativeAct* first_comm_act,*comm_act;
	float end_time;

	face_log=fopen("logs/face_engine_log.txt","w");

	FaceEngine *faceengine;

	//printf("face engine started\n");

	filenames.Phonemes_File="";
	filenames.Fap_File=fapfilename;
	//filenames.Phonemes_File=inimanager.Program_Path+"tmp/complexemotion2.pho";

	comm_act=new CommunicativeAct();

    comm_act->function=(char*)malloc(5*sizeof(char));
	sprintf(comm_act->function,"APML");
	comm_act->value=(char*)malloc(1*sizeof(char));
	sprintf(comm_act->value,"");

	first_comm_act=comm_act;

	end_time=0;

	std::vector<singlesignal>::iterator iter;
	for(iter=facesignals.begin();iter!=facesignals.end();iter++)
	{
		comm_act->next=new CommunicativeAct();
		comm_act=(CommunicativeAct*)comm_act->next;
		comm_act->function=(char*)malloc(((*iter).classname.length()+1)*sizeof(char));
		sprintf(comm_act->function,"%s",(*iter).classname.c_str());
		comm_act->value=(char*)malloc(((*iter).instancename.length()+1)*sizeof(char));
		sprintf(comm_act->value,"%s",(*iter).instancename.c_str());
		comm_act->start_time=(*iter).start;
		comm_act->dur_time=(*iter).end-(*iter).start;
		comm_act->AddPhase(caphase_attack,comm_act->dur_time*0.2f);
		comm_act->AddPhase(caphase_decay,comm_act->dur_time*0.1f);
		comm_act->AddPhase(caphase_sustain,comm_act->dur_time*0.5f);
		comm_act->AddPhase(caphase_release,comm_act->dur_time*0.2f);

		//Expressivity parameters 
		if(((*iter).expressivitySPC>=-1)&&((*iter).expressivitySPC<=1))
			comm_act->SetExpressivitySPC((*iter).expressivitySPC);
		else
			comm_act->SetExpressivitySPC(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));

		if(((*iter).expressivityTMP>=-1)&&((*iter).expressivityTMP<=1))
			comm_act->SetExpressivityTMP((*iter).expressivityTMP);
		else
			comm_act->SetExpressivityTMP(inimanager.GetValueFloat("ENGINE_EXPR_TMP"));

		if(((*iter).expressivityFLD>=-1)&&((*iter).expressivityFLD<=1))
			comm_act->SetExpressivityFLD((*iter).expressivityFLD);
		else
			comm_act->SetExpressivityFLD(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));

		if(((*iter).expressivityPWR>=-1)&&((*iter).expressivityPWR<=1))
			comm_act->SetExpressivityPWR((*iter).expressivityPWR);
		else
			comm_act->SetExpressivityPWR(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));

		if(((*iter).expressivityREP>=-1)&&((*iter).expressivityREP<=1))
			comm_act->SetExpressivityREP((*iter).expressivityREP);
		else
			comm_act->SetExpressivityREP(inimanager.GetValueFloat("ENGINE_EXPR_REP"));



		comm_act->conflict_comm_act_list=0;
		comm_act->next=0;

		if((*iter).end>end_time)
			end_time=(*iter).end;
	}
	faceengine=new FaceEngine(first_comm_act,comm_act,false);

	if(faceengine->CalculateTurn("",end_time+1.0f,0,0, 0)==0)
	{
		delete faceengine;
		printf("face engine ended with errors\n");
		fclose(face_log);
		return 0;
	}

	delete faceengine;

	//printf("face engine ended\n");

	fclose(face_log);

	return 1;
}

std::string MSSL_AgentEngine::ToUpper(std::string s)
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

int MSSL_AgentEngine::LoadMSSL(std::string MSSLfilename)
{
	XMLGenericParser *p;
	XMLGenericTree *t;
	std::list<XMLGenericTree*>::iterator iterch;
	std::list<XMLGenericTree*>::iterator iterbl;

	p=new XMLGenericParser();
	//p->SetValidating(false);
	t=p->ParseFile(MSSLfilename);
	
	if(t!=0)
	{
		if(!t->child.empty())
			for(iterch=t->child.begin();iterch!=t->child.end();iterch++)
			{
				if((*iterch)->name=="channel")
				{
					if((*iterch)->GetAttribute("type")=="face")
					{
						for(iterbl=(*iterch)->child.begin();iterbl!=(*iterch)->child.end();iterbl++)
						{
							singlesignal *s;
							s=new singlesignal;
							s->classname=(*iterbl)->GetAttribute("class");
							s->instancename=(*iterbl)->GetAttribute("instance");
							s->start=(*iterbl)->GetAttributef("start");
							s->end=(*iterbl)->GetAttributef("end");
							facesignals.push_back(*s);
						}
					}
					if((*iterch)->GetAttribute("type")=="gesture")
					{
						for(iterbl=(*iterch)->child.begin();iterbl!=(*iterch)->child.end();iterbl++)
						{
							singlesignal *s;
							s=new singlesignal;
							s->classname=(*iterbl)->GetAttribute("class");
							s->instancename=(*iterbl)->GetAttribute("instance");
							s->start=(*iterbl)->GetAttributef("start");
							s->end=(*iterbl)->GetAttributef("end");
							gesturesignals.push_back(*s);
						}
					}
				}
			}
	}
	else
		return 0;

	delete p;
	delete t;
	return 1;
}