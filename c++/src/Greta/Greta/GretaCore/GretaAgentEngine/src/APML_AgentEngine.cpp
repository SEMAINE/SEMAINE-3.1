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

// APML_AgentEngine.cpp: implementation of the APML_AgentEngine class.
//
//////////////////////////////////////////////////////////////////////
/**
* APML_AgentEngine class: calculates agent's animation (body+face)
*
* @see GestureEngine, FaceEngine
* @author Maurizio Mancini, manmau@yahoo.com
* @version 1.0
*/
#include <stdio.h>
#include "APML_AgentEngine.h"
#include "MaryInterface.h"
#include "FestivalInterface.h"
#include "EulerInterface.h"
#include "RealSpeechInterface.h"
#include "XMLGenericParser.h"
#include "GretaXMLParser.h"
#include "RandomGen.h"
#include "FileNames.h"
#include <direct.h>
#include <process.h>
#include <crtdbg.h>


using namespace GestureSpace;

FILE* gesture_log=0;
FILE* face_log=0;
FILE* agent_log=0;

extern IniManager inimanager;
extern std::string ini_filename;
FileNames filenames;
RandomGen *randomgen=0;

//like all the other files involved in animation calculations it will
//send its output to a common face_log if symbol LOGFILE is defined

//constructor, does nothing
APML_AgentEngine::APML_AgentEngine()
{
	if(inimanager.initialized==false)
		inimanager.ReadIniFile(ini_filename);
	if(randomgen==0)
		randomgen = new RandomGen();
}

APML_AgentEngine::~APML_AgentEngine()
{
	if(randomgen!=0)
	{
		delete randomgen;
		randomgen=0;
	}
}

int APML_AgentEngine::CallMary()
{
	MaryInterface *mi;
	mi=new MaryInterface(this->first_comm_act);

	if(inimanager.GetValueString("MARY_LANGUAGE")=="ENGLISH_MALE1")
	{
		mi->SetEnglish();
		mi->SetMale(0);
	}
	else
	if(inimanager.GetValueString("MARY_LANGUAGE")=="ENGLISH_FEMALE1")
	{
		mi->SetEnglish();
		mi->SetFemale(0);
	}
	else
	if(inimanager.GetValueString("MARY_LANGUAGE")=="GERMAN_FEMALE1")
	{
		mi->SetGerman();
		mi->SetFemale(0);
	}
	else
	if(inimanager.GetValueString("MARY_LANGUAGE")=="GERMAN_MALE1")
	{
		mi->SetGerman();
		mi->SetMale(0);
	}
	else
	{
		printf("The specified voice for Mary is unknown.\n");
		return 0;
	}


	if(mi->Run(filenames.APML_File,"tmp/mary.xml",filenames.Wav_File)==1)
	{
		if(mi->CalculateTimings(this->first_comm_act,calculated_animation_time,number_of_comm_act)==1)
		{
			this->all_phonemes_list=mi->timingscalculator->all_phonemes_list;
			this->PhoVector=&(mi->timingscalculator->PhoVector);
			delete mi;
			return 1;
		}
	}

	delete mi;

	return 0;
}

int APML_AgentEngine::CallFestival()
{
	FestivalInterface *fi;
	fi=new FestivalInterface(this->first_comm_act);
	if(fi->PrepareScript()==1)
	{
		if(fi->RunScript()==1)
		{
			if(fi->CalculateTimings(this->first_comm_act,calculated_animation_time,number_of_comm_act)==1)
			{
				this->all_phonemes_list=fi->timingscalculator->all_phonemes_list;
				this->PhoVector=&(fi->timingscalculator->PhoVector);
				return 1;
			}
		}
	}
	return 0;
}

int APML_AgentEngine::CallEuler()
{
	EulerInterface *ei;
	ei=new EulerInterface(this->first_comm_act);

	if(ei->Prepare()==1)

	{

		if(ei->Run()==1)

		{
			if(ei->CalculateTimings(this->first_comm_act,calculated_animation_time,number_of_comm_act)==1)
			{
				this->all_phonemes_list=ei->timingscalculator->all_phonemes_list;
				this->PhoVector=&(ei->timingscalculator->PhoVector);
				return 1;
			}
		}
	}
	return 0;
}

int APML_AgentEngine::CallRealSpeech()
{
	RealSpeechInterface *rsi;
	rsi=new RealSpeechInterface(this->first_comm_act);
	if(rsi->Prepare()==1)
	{
		if(rsi->Run()==1)
		{
			if(rsi->CalculateTimings(this->first_comm_act,calculated_animation_time,number_of_comm_act)==1)
			{
				this->all_phonemes_list=rsi->timingscalculator->all_phonemes_list;
				this->PhoVector=&(rsi->timingscalculator->PhoVector);
				return 1;
			}
		}
	}
	return 0;
}

int APML_AgentEngine::CalculateTurnFromBuffer(std::string buffer)
{
	std::string line;

	std::ofstream outputfile("apml/buffer.xml");

	if(outputfile.is_open())
	{
			outputfile << buffer << "\n";
		outputfile.close();
	}
	else
		return 0;

	this->CalculateTurn("apml/buffer.xml");

	return 1;
}

//the main and only method of this class, it executes body and face engines
//INPUT: lst file
int APML_AgentEngine::CalculateTurn(std::string xmlfile)
{
	if(!inimanager.initialized)
	{
		printf("can't find greta.ini, exting the engine\n");
		return 0;
	}

	chdir(inimanager.Program_Path.c_str());

	//opens logfile
#ifdef LOGFILE
	std::string logname;
	logname=inimanager.Program_Path+"logs/gesture_engine_log.txt";
	gesture_log=fopen(logname.c_str(),"w");
	logname=inimanager.Program_Path+"logs/face_engine_log.txt";
	face_log=fopen(logname.c_str(),"w");
	logname=inimanager.Program_Path+"logs/agent_engine_log.txt";
	agent_log=fopen(logname.c_str(),"w");

	if(face_log==NULL)
	{
		printf("cannot creat face log file (probably the log folder is missing)\n");
		return 0;
	}

	if(gesture_log==NULL)
	{
		printf("cannot creat gesture log file (probably the log folder is missing)\n");
		return 0;
	}

	if(agent_log==NULL)
	{
		printf("cannot creat agent log file (probably the log folder is missing)\n");
		return 0;
	}

	//logfile=fopen("logs/gesture_engine_log.txt","w");
#else
	gesture_log=stderr;
	face_log=stderr;
	agent_log=stderr;
	//logfile=stderr;
#endif

	filenames.BuildFileNames(xmlfile);

	GretaXMLParser *xmlparser;
	xmlparser=new GretaXMLParser();
	
	/*
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag( tmpFlag );
	*/

	Copy_DTD_in_C_TMP();

	if(xmlparser->ParseXMLFile(filenames.APML_File,apmltreeroot,&first_comm_act,&last_comm_act)==0)
	{
		delete xmlparser;
		return 0;
	}

	delete xmlparser;

	if(CallSpeechSynthesizer()==0)
		return 0;


	FaceEngine* FE;
	GestureEngine* GE;
	FE=new FaceEngine(first_comm_act,last_comm_act,false);
	

	//printf("\nstarting face engine...\n");
	if(FE->CalculateTurn(filenames.Fap_File,this->calculated_animation_time+1,this->PhoVector,this->all_phonemes_list, 0)!=0)
	{
		delete FE;
		//printf("face engine ended!\n");
		//printf("\nstarting gesture engine...\n");
		GE=new GestureEngine();
		GE->CalculateTurn(filenames.Bap_File,filenames.Turns_File);
		//printf("gesture engine ended!\n");
		delete GE;
	}
	else
	{
		delete FE;
		printf(" !errors in the face engine!\n");
		return 0;
	}

	//closes logfiles
	if (gesture_log!=stderr)
		fclose(gesture_log);
	if (face_log!=stderr)
		fclose(face_log);
	if (agent_log!=stderr)
		fclose(agent_log);

	CommunicativeAct *iter,*todelete;
	iter=first_comm_act;
	while(iter!=0)
	{
		todelete=iter;
		iter=(CommunicativeAct *)iter->next;
		delete todelete;
	}

	

	return 1;
}


int APML_AgentEngine::CallSpeechSynthesizer()
{
	//TODO: speech synthesizers will be called here
	
		if(inimanager.GetValueInt("MARY")==1)
		{
			if(this->CallMary()==1)
				return 1;
		 }
	
	if(inimanager.GetValueInt("FESTIVAL")==1)
	{
		if(this->CallFestival()==1)
			return 1;
	}

	if(inimanager.GetValueInt("EULER")==1)
	{
		if(this->CallEuler()==1)
			return 1;
	}

	if(inimanager.GetValueInt("REALSPEECH")==1)
	{
		if(this->CallRealSpeech()==1)
			return 1;
	}

	return 0;
}

void APML_AgentEngine::CreateNewTxtFileWithAccents(XMLGenericTree *root)
{
	string nameFile;
	FILE *EulerX;
	std::list<XMLGenericTree*>::iterator iter;

//	XMLGenericTree* p;
	iter=root->child.begin();
	
	if((*root).name=="text")
	{
		//salvare in file di testo singolo (accentsX.txt) il root->GetAttribute("value")
		nameFile="";
		ostringstream num;
		num.clear();
		num << this->numText;
		nameFile = inimanager.Program_Path + "tmp/euler_" + num.str() + ".txt";

		EulerX=fopen(nameFile.c_str(),"w");
		fprintf(EulerX,"%s",root->value);
		fclose(EulerX);

		//incrementa contatore X
		this->numText+=1;
	}

	while((iter!=root->child.end()))
	{
		CreateNewTxtFileWithAccents(*iter);
		iter++;
	}

	/*
	while(expr)
	{
		if (strcmp(expr->function,"text")==0) printf("%s\n", expr->value);
		expr=(CommunicativeAct*)expr->next;
	}*/
}

char* APML_AgentEngine::RewriteXMLFile(char *xmlfile)
{
	std::string line;
	std::string pho;
	int position;

	std::ifstream inputfile(xmlfile);
	if(inputfile.is_open())
	{
		std::ofstream outputfile("tmp/noaccents.xml");
		if(outputfile.is_open())
		{
			
			while((inputfile.rdstate()&ios::eofbit)==0)
			{
				std::getline(inputfile,line,'\n');
				if(line!="")
				{	
					while(1)
					{
						position = (int)line.find_first_of('è');
						if (position!=-1)
							line.replace(position, 1, "e");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('é');
						if (position!=-1)
							line.replace(position, 1, "e");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('ê');
						if (position!=-1)
							line.replace(position, 1, "e");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('ì');
						if (position!=-1)
							line.replace(position, 1, "i");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('ò');
						if (position!=-1)
							line.replace(position, 1, "o");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('à');
						if (position!=-1)
							line.replace(position, 1, "a");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('ù');
						if (position!=-1)
							line.replace(position, 1, "u");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('ï');
						if (position!=-1)
							line.replace(position, 1, "i");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('î');
						if (position!=-1)
							line.replace(position, 1, "i");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('û');
						if (position!=-1)
							line.replace(position, 1, "u");
						else break;
					}
					while(1)
					{
						position = (int)line.find_first_of('ô');
						if (position!=-1)
							line.replace(position, 1, "o");
						else break;
					}
					
					/*while(1)
					{
						position = line.find_first_of("-");
						if (position!=-1)
							line.replace(position, 1, " ");
						else break;
					}
					while(1)
					{
						position = line.find_first_of('\'');
						if (position!=-1)
							line.replace(position, 1, "");
						else break;
					}*/
				}
				outputfile << line.c_str() << "\n";
			}
		}
		else
		{
			inputfile.close();
			return("");
		}
		outputfile.close();
	}
	else
	{
		printf("File %s not found!\n", xmlfile);
		return("");
	}
	
	inputfile.close();

	//new file XML
	return("tmp/noaccents.xml");

}

int APML_AgentEngine::Copy_DTD_in_C_TMP()
{
	FILE *dtd,*dtd2,*dtd3;
	std::string path,path2,path3;
	char *dtd_text;
	int dtd_size;

	dtd=0;
	dtd2=0;
	dtd3=0;

	path="C:/tmp/apml.dtd";
	path2=inimanager.Program_Path[0];
	path2+=":/tmp/apml.dtd";
	path3=inimanager.Program_Path+"apml/apml.dtd";

	dtd3=fopen(path3.c_str(),"rb");
	if (!dtd3)
	{
		printf("\nCan't open %s for reading!\n",path3.c_str());
		return 0;
	}

	dtd=fopen(path.c_str(),"wb");
	if (!dtd)
	{
		if(mkdir("c:/tmp")==0)
		{
			printf("WARNING: folder C:/tmp created\n"); 
			dtd=fopen(path.c_str(),"wb");
		}
		else
		{
			printf("\nCan't create %s!\n",path.c_str());
			printf("!!!->You need create the C:\\tmp folder\n");
			return 0;
		}
	}

	if((inimanager.Program_Path[0]!='c')&&(inimanager.Program_Path[0]!='C'))
	{
		dtd2=fopen(path2.c_str(),"wb");
		if (!dtd2)
		{
			std::string dirpath;
			dirpath=inimanager.Program_Path[0]+":/tmp";
			if(mkdir(dirpath.c_str())==0)
			{
				printf("WARNING: folder %s created\n",dirpath.c_str()); 
				dtd2=fopen(path2.c_str(),"wb");
			}
			else
			{
				printf("\nCan't create %s!\n",dirpath.c_str());
				printf("!!!->You need create the %s folder\n",dirpath.c_str());
				return 0;
			}
		}
	}

	fseek(dtd3,0L,SEEK_END);
	dtd_size=ftell(dtd3);
	fseek(dtd3,0L,SEEK_SET);
	dtd_text=(char*)malloc(dtd_size);
	if (dtd_text==0)
	{
		printf("\nMalloc error for copying dtd.\n");
		return 0;
	}
	memset(dtd_text,0,dtd_size);
	fread(dtd_text,dtd_size,1,dtd3);
	fwrite(dtd_text,dtd_size,1,dtd);
	if(dtd2!=0)
		fwrite(dtd_text,dtd_size,1,dtd2);
	fclose(dtd);
	if(dtd2!=0)
		fclose(dtd2);
	fclose(dtd3);

	return 1;
}

