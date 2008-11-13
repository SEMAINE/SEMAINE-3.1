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

#include ".\RealSpeechInterface.h"
#include "IniManager.h"
#include "FileNames.h"
#include <process.h>
#include <sstream>
#include <math.h>

extern IniManager inimanager;
extern FILE *agent_log;
extern FILE *face_log;
extern FileNames filenames;

#define OUTPUT

RealSpeechInterface::RealSpeechInterface(CommunicativeAct* first_comm_act):SpeechInterface(first_comm_act)
{

}

int RealSpeechInterface::Prepare()
{
	CommunicativeAct* appCommAct;
	FILE *EulerAll;
	FILE *EulerX;
	string nameFileX;
	int i=1;
	//un file generico tmp/euler_all.txt dove mettere tutto il testo (concatena i first_comm_act->value)
	//un file nel while con il nome a piacere (in tmp/euler_x.txt) e contenuto first_comm_act->value

	appCommAct=first_comm_act;
	EulerAll=fopen("tmp/euler_all.txt","w");
	while(appCommAct!=NULL)
	{
		if(strcmp(appCommAct->function,"text")==0)
		{
			nameFileX="";
			ostringstream num;
			num.clear();
			num << i;
			nameFileX = inimanager.Program_Path + "tmp/euler_" + num.str() + ".txt";
			EulerX=fopen(nameFileX.c_str(),"w");
			fprintf(EulerX,"%s",appCommAct->value);
			fclose(EulerX);
			i+=1;
			fprintf(EulerAll, "%s", appCommAct->value);
		}
		else 
			if (strcmp(appCommAct->function,"boundary")==0)
				fprintf(EulerAll, ", ");
			//fprintf(script_for_festival,"(Phdur_save_text \"%s\" \"%s\")\n",first_comm_act->value,first_comm_act->phonemes_file);
		appCommAct=(CommunicativeAct*)appCommAct->next;
	}
	TotFileX=i;
	fclose(EulerAll);
	return 1;
}

int RealSpeechInterface::Run()
{
	//chiama euler su euler_all.txt e ciascun euler_x.txt
	CommunicativeAct* appCommAct;
	int i=1;
	string pathIn, pathOut, pathOut2;
	string speed="1.2";//inimanager.GetValueFloat("SPEECH_STRETCH")
	float speechrate=1.0;
	//printf("* Euler started *\n");

	//inimanager.Program_Path+"tmp/";
	
	pathIn= inimanager.Program_Path+"tmp/euler_all.txt";
	pathOut= inimanager.Program_Path+"tmp/AppEuler_all.pho";
	//cmdeuler -FileText tmp/euler_all.txt -phofileout filenames.Phomenes_File
	chdir("C:/Euler");
	_spawnl( _P_WAIT,inimanager.GetValueString("EULER_PATH").c_str()," ","-iniFile","C:/Euler/euler.ini","-phoFileOut",pathOut.c_str(),"-textFile",pathIn.c_str(),NULL);
	pathOut2= inimanager.Program_Path+"tmp/AppEuler_all2.pho";
	ChangeTimeFilePho(pathOut, pathOut2, speechrate);
	ConvertFilePho(pathOut2, filenames.Phonemes_File);
	
	appCommAct=first_comm_act;
	//per ogni expr: cmdeuler -FileText tmp/euler_x.txt -phofileout expr->phonemes_file
	while(appCommAct!=NULL)
	{
		if(strcmp(appCommAct->function,"text")==0)
		{
			pathIn="";
			ostringstream num;
			num.clear();
			num << i;
			pathIn = inimanager.Program_Path + "tmp/euler_" + num.str() + ".txt";
			pathOut="";
			pathOut = inimanager.Program_Path + "tmp/AppEuler_" + num.str() + ".pho";
			//printf("%s\n", pathOut.c_str());
			i+=1;
			//“cmdEuler -iniFile euler.ini -phoFileOut output.pho -text bonjour”
			//chdir("C:/Euler");
			_spawnl( _P_WAIT,inimanager.GetValueString("EULER_PATH").c_str()," ","-iniFile","C:/Euler/euler.ini","-phoFileOut",pathOut.c_str(),"-textFile",pathIn.c_str(),NULL);
			//fprintf(script_for_festival,"(Phdur_save_text \"%s\" \"%s\")\n",first_comm_act->value,first_comm_act->phonemes_file);
			ConvertFilePho(pathOut, appCommAct->phonemes_file );
		}
		appCommAct=(CommunicativeAct*)appCommAct->next;
	}
	//sprintf(Script_File_Modified,"\"%s\"",filenames.Script_File);
	//disable festival here if using prerecorded voice
	//printf("*  Euler ended  *\n");
	chdir(inimanager.Program_Path.c_str());
	pathOut = inimanager.Program_Path + "mbrola";
	//printf("mbrola -t %f Languages/French/fr1/fr1 tmp/AppEuler_all.pho %s\n", inimanager.GetValueFloat("SPEECH_STRETCH"),inimanager.Program_Path.c_str());
	//devo cambiare la velocità nel file AppEuler_all.pho e modificare il valore del tempo molt per lo speech rate
	_spawnl( _P_WAIT, "mbrola","-t","1.0","Languages/French/fr4/fr4","tmp/AppEuler_all2.pho",filenames.Wav_File.c_str(),NULL);

	return 1;
}

int RealSpeechInterface::ConvertFilePho(string nameFile, string finalFile)
{
	string line;
	string pho;
	float time;

	//printf("%s	%s\n", nameFile.c_str(), finalFile.c_str());
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
					time = (atof(line.substr(0,line.find_first_of(" ")).c_str())/1000);
					outputfile << time << "\n";
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
		printf("File emotion.txt not found!\n");
		return 0;
	}
	
	inputfile.close();
	return 1;
}

int RealSpeechInterface::ChangeTimeFilePho(string nameFile, string finalFile, float speed)
{
	string line;
	string pho;
	int time;

	//printf("%s	%s\n", nameFile.c_str(), finalFile.c_str());
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
		printf("File emotion.txt not found!\n");
		return 0;
	}
	
	inputfile.close();
	return 1;
}

void RealSpeechInterface::InvertSlashes(char* target)
{
	int i,l;
	l=strlen(target);
	for(i=0;i<l;i++)
		if(target[i]=='\\')target[i]='/';
}

RealSpeechInterface::~RealSpeechInterface(void)
{
}
