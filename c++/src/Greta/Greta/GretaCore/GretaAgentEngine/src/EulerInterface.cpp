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

// EulerInterface.cpp: implementation of the EulerInterface class.
//
//////////////////////////////////////////////////////////////////////

#include ".\EulerInterface.h"
#include "IniManager.h"
#include "FileNames.h"
#include <process.h>
#include <iostream>     
#include <sstream>   
#include <math.h>

extern IniManager inimanager;
extern FILE *agent_log;
extern FILE *face_log;
extern FileNames filenames;

#define OUTPUT

EulerInterface::EulerInterface(CommunicativeAct* first_comm_act):SpeechInterface(first_comm_act)
{
}

int EulerInterface::Prepare()
{
	CommunicativeAct* appCommAct;
	FILE *EulerAll;
	FILE *EulerX;
	string nameFileX;
	int i=1;

	appCommAct=first_comm_act;
	EulerAll=fopen("tmp/euler_all.txt","w");
	while(appCommAct!=NULL)
	{
		if(strcmp(appCommAct->function,"text")==0)
		{
			std::string result;
			result=utfToAscii(appCommAct->value);
			nameFileX="";
			ostringstream num;
			num.clear();
			num << i;
			nameFileX = inimanager.Program_Path + "tmp/euler_" + num.str() + ".txt";
			EulerX=fopen(nameFileX.c_str(),"w");
			fprintf(EulerX,"%s",result.c_str());
			fclose(EulerX);
			i+=1;
			fprintf(EulerAll, "%s",result.c_str());
		}
		else 
			if (strcmp(appCommAct->function,"boundary")==0)
				fprintf(EulerAll, ", ");
		appCommAct=(CommunicativeAct*)appCommAct->next;
	}
	TotFileX=i;
	fclose(EulerAll);
	return 1;
}

std::string EulerInterface::utfToAscii(std::string s)
{
	std::string t;
	for(int i=0;i<s.length();i++)
	{
		if((s[i]=='Ã')&&(s[i+1]=='¨'))
		{
			t=t+"è";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='©'))
		{
			t=t+"é";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='ª'))
		{
			t=t+"ê";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='¹'))
		{
			t=t+"ù";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='§'))
		{
			t=t+"ç";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]==-96))
		{
			t=t+"à";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='¬'))
		{
			t=t+"ì";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='²'))
		{
			t=t+"ò";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='®'))
		{
			t=t+"î";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='´'))
		{
			t=t+"ô";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='»'))
		{
			t=t+"û";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='¢'))
		{
			t=t+"â";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='¯'))
		{
			t=t+"ï";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='«'))
		{
			t=t+"ë";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='¶'))
		{
			t=t+"ö";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='¼'))
		{
			t=t+"ü";
			i++;
		}
		else
		if((s[i]=='Ã')&&(s[i+1]=='¤'))
		{
			t=t+"ä";
			i++;
		}
		else
			t=t+s[i];
	}
	return t;
}

int EulerInterface::Run()
{
	CommunicativeAct* appCommAct;
	int i=1;
	string pathIn, pathOut, pathOut2, pathscript;
	string speed;
	string dictionary;
	ostringstream speechRate;
	speechRate << inimanager.GetValueFloat("SPEECH_STRETCH");
	speed = speechRate.str();
	
	//printf("\n* Euler started *\n");
	
	pathIn= inimanager.Program_Path+"tmp/euler_all.txt";
	pathOut= inimanager.Program_Path+"tmp/AppEuler_all.pho";
	pathscript= inimanager.Program_Path+"tmp/eulerscript.txt";
	chdir(inimanager.GetValueString("EULER_PATH").c_str());

	FILE *scripteuler=fopen(pathscript.c_str(),"w");
	fprintf(scripteuler,"-iniFile euler.ini -phoFileOut %s -textFile %s\n",pathOut.c_str(),pathIn.c_str());
//	_spawnl( _P_WAIT,"cmdeuler.exe"," ","-iniFile","euler.ini",pathscript.c_str(),NULL);
//	_spawnl( _P_WAIT,"cmdeuler.exe"," ","-iniFile","euler.ini","-phoFileOut",pathOut.c_str(),"-textFile",pathIn.c_str(),NULL);
//	pathOut2= inimanager.Program_Path+"tmp/AppEuler_all2.pho";
//	ChangeTimeFilePho(pathOut, pathOut2, 1.0/*inimanager.GetValueFloat("SPEECH_STRETCH")*/);
//	ConvertFilePho(pathOut, filenames.Phonemes_File, 1.0/*inimanager.GetValueFloat("SPEECH_STRETCH")*/);
	
	appCommAct=first_comm_act;
	while(appCommAct!=NULL)
	{
		//metti contatore (locale) a 1
		if(strcmp(appCommAct->function,"text")==0)
		{
			//non creare più i file di testo singoli ma chiama solo Euler passando come path accentsX.txt
			//incrementa contatore X
			pathIn="";
			ostringstream num;
			num.clear();
			num << i;
			pathIn = inimanager.Program_Path + "tmp/euler_" + num.str() + ".txt";
			pathOut="";
			pathOut = inimanager.Program_Path + "tmp/AppEuler_" + num.str() + ".pho";
			i+=1;
			fprintf(scripteuler,"-iniFile euler.ini -phoFileOut %s -textFile %s\n",pathOut.c_str(),pathIn.c_str());
			//_spawnl( _P_WAIT,"cmdeuler.exe"," ","-iniFile","euler.ini","-phoFileOut",pathOut.c_str(),"-textFile",pathIn.c_str(),NULL);
			//ConvertFilePho(pathOut, appCommAct->phonemes_file, 1.0/*inimanager.GetValueFloat("SPEECH_STRETCH")*/);
		}
		appCommAct=(CommunicativeAct*)appCommAct->next;
	}
	fclose(scripteuler);
	_spawnl( _P_WAIT,"cmdeuler.exe"," ","-iniFile","euler.ini",pathscript.c_str(),NULL);

	//printf("*  Euler ended  *\n");

	pathOut2= inimanager.Program_Path+"tmp/AppEuler_all2.pho";
	pathOut= inimanager.Program_Path+"tmp/AppEuler_all.pho";
	ChangeTimeFilePho(pathOut, pathOut2, inimanager.GetValueFloat("SPEECH_STRETCH"));
	ConvertFilePho(pathOut, filenames.Phonemes_File, inimanager.GetValueFloat("SPEECH_STRETCH"));


	appCommAct=first_comm_act;
	i=1;
	while(appCommAct!=NULL)
	{
		//metti contatore (locale) a 1
		if(strcmp(appCommAct->function,"text")==0)
		{
			ostringstream num;
			num.clear();
			num << i;
			pathOut="";
			pathOut = inimanager.Program_Path + "tmp/AppEuler_" + num.str() + ".pho";
			ConvertFilePho(pathOut, appCommAct->phonemes_file, inimanager.GetValueFloat("SPEECH_STRETCH"));
			i+=1;
		}
		appCommAct=(CommunicativeAct*)appCommAct->next;
	}

	chdir(inimanager.Program_Path.c_str());
	if(inimanager.GetValueString("EULER_LANGUAGE")=="FRENCH_FEMALE1")
		dictionary=inimanager.Program_Path +"fr4";
	else
		dictionary=inimanager.Program_Path +"fr3";
	_spawnl( _P_WAIT, "mbrola"," ","-t","1.0",dictionary.c_str(),"tmp/AppEuler_all2.pho",filenames.Wav_File.c_str(),NULL);

	return 1;
}

int EulerInterface::ConvertFilePho(string nameFile, string finalFile, float speed)
{
	string line;
	string pho;
	string previousPho="";
	float time;

	ifstream inputfile(nameFile.c_str());
	if(inputfile.is_open())
	{
		ofstream outputfile(finalFile.c_str());
		if(outputfile.is_open())
		{
			
			while((inputfile.rdstate()&ios::eofbit)==0)
			{
				getline(inputfile,line,'\n');
				if(line!="")
				{	
					pho = line.substr(0,line.find_first_of(" ")).c_str();
					if (pho != "_")
						outputfile << pho.c_str() << " ";
					else
						if (previousPho!="_")
							outputfile << "#" << " ";
					line = line.substr(line.find_first_of(" ")+1,line.length()-line.find_first_of(" ")-1);
					time = (atof(line.substr(0,line.find_first_of(" ")).c_str())*speed)/1000;
					if (previousPho=="_" && pho=="_")
						;
					else
						outputfile << time << "\n";
					previousPho=pho;
				}
			}
		}
		else
		{
			inputfile.close();
			return 0;
		}
		outputfile.close();
	}
	else
	{
		printf("File %s not found!\n", nameFile.c_str());
		return 0;
	}
	
	inputfile.close();
	return 1;
}

int EulerInterface::ChangeTimeFilePho(string nameFile, string finalFile, float speed)
{
	string line;
	string pho;
	int time;

	ifstream inputfile(nameFile.c_str());
	if(inputfile.is_open())
	{
		ofstream outputfile(finalFile.c_str());
		if(outputfile.is_open())
		{
			
			while((inputfile.rdstate()&ios::eofbit)==0)
			{
				getline(inputfile,line,'\n');
				if(line!="")
				{	
					pho = line.substr(0,line.find_first_of(" ")).c_str();
					outputfile << pho.c_str() << " ";
					line = line.substr(line.find_first_of(" ")+1,line.length()-line.find_first_of(" ")-1);
					time = floor((atof(line.substr(0,line.find_first_of(" ")).c_str()))*speed);
					outputfile << time;
					line = line.substr(line.find_first_of(" ")+1,line.length()-line.find_first_of(" ")-1);
					outputfile << " " << line << "\n";
				}
			}
		}
		else
		{
			inputfile.close();
			return 0;
		}
		outputfile.close();
	}
	else
	{
		printf("File %s not found!\n", nameFile.c_str());
		return 0;
	}
	
	inputfile.close();
	return 1;
}

void EulerInterface::InvertSlashes(char* target)
{
	int i,l;
	l=strlen(target);
	for(i=0;i<l;i++)
		if(target[i]=='\\')target[i]='/';
}


EulerInterface::~EulerInterface(void)
{
}
