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

#include ".\festivalinterface.h"
#include "IniManager.h"
#include "FileNames.h"
#include <process.h>

extern IniManager inimanager;
extern FILE *agent_log;
extern FILE *face_log;
extern FileNames filenames;

#define OUTPUT

FestivalInterface::FestivalInterface(CommunicativeAct* first_comm_act):SpeechInterface(first_comm_act)
{

}

int FestivalInterface::PrepareScript()
{
	if((inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_FEMALE1")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE1")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE2")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="ITALIAN")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="POLISH")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="SWEDISH"))
	{
		script_for_festival=fopen(filenames.Script_File.c_str(),"w");
		if (!script_for_festival)
		{
#ifdef OUTPUT
			fprintf(agent_log,"FACE ENGINE ERROR: couldn't open script file for Festival!\n");
#endif
			return 0;
		}
		if (inimanager.GetValueString("FESTIVAL_LANGUAGE")=="ITALIAN")
		{
			fprintf(script_for_festival,"(require 'it_define-animation)\n");
		}
		else 
		if (inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_FEMALE1")
		{
			fprintf(script_for_festival,"(require 'us_define-animation)\n");
		}
		else
		if (inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE1")
		{
			fprintf(script_for_festival,"(require 'us_define-animation_male1)\n");
		}
		else
		if (inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE2")
		{
			fprintf(script_for_festival,"(require 'us_define-animation_male2)\n");
		}
		else
		if (inimanager.GetValueString("FESTIVAL_LANGUAGE")=="POLISH")
		{
			fprintf(script_for_festival,"(require 'pl_define-animation)\n");
		}
		else
		if (inimanager.GetValueString("FESTIVAL_LANGUAGE")=="SWEDISH")
		{
			fprintf(script_for_festival,"(require 'sw_define-animation)\n");
		}
		else
		{
			// Error: Language must be defined for the SCRIPT_FILE!
#ifdef OUTPUT
			fprintf(agent_log,"FACE ENGINE ERROR: language must be defined (ITALIAN, US_ENGLISH, POLISH, SWEDISH)\n");
#endif
			return 0;
		}
		fprintf(script_for_festival,"(Param.set 'Duration_Stretch %f)\n",inimanager.GetValueFloat("SPEECH_STRETCH"));

		while(first_comm_act!=NULL)
		{
			if(strcmp(first_comm_act->function,"text")==0)
				fprintf(script_for_festival,"(Phdur_save_text \"%s\" \"%s\")\n",first_comm_act->value,first_comm_act->phonemes_file.c_str());
			first_comm_act=(CommunicativeAct*)first_comm_act->next;
		}

		if (inimanager.GetValueInt("FESTIVAL")==1)
		{
			InvertSlashes(filenames.APML_File);
			InvertSlashes(filenames.Phonemes_File);
			InvertSlashes(filenames.Wav_File);
			//fprintf(script_file,"(APML_PhdurWav_save '%s '%s '%s)\n",XML_FILE,PHONEMES_FILE,WAV_FILE);
			fprintf(script_for_festival,"(APML_PhdurWav_save \"%s\" \"%s\" \"%s\")\n",filenames.APML_File.c_str(),filenames.Phonemes_File.c_str(),filenames.Wav_File.c_str());
		}
		else
		{
			InvertSlashes(filenames.APML_File);
			InvertSlashes(filenames.Phonemes_File);
			fprintf(script_for_festival,"(APML_Phdur_save \"%s\" \"%s\" \"%s\")\n",filenames.APML_File.c_str(),filenames.Phonemes_File.c_str());
		}

		fprintf(script_for_festival,"(quit)\n");

		fclose(script_for_festival);

	}//END IF ITALIAN OR ENGLISH
	return 1;
}

int FestivalInterface::RunScript()
{
	std::string Script_File_Modified;
	FILE *tmp_file;

	if((inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_FEMALE1")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE1")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE2")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="ITALIAN")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="POLISH")
		||(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="SWEDISH"))
	{
		tmp_file=fopen(inimanager.GetValueString("FESTIVAL_PATH").c_str(),"r");

		if (!tmp_file)
		{
	#ifdef OUTPUT
			fprintf(agent_log,"\nError: Festival not found!\n");
	#endif
			return 0;
		}
		else
		{
			fclose(tmp_file);
	#ifdef OUTPUT
			fprintf(agent_log,"\nRunning Festival script...\n\n");	
	#endif


		//printf("\n* Festival started *\n");
		Script_File_Modified="\""+filenames.Script_File+"\"";
		//disable festival here if using prerecorded voice
		_spawnl( _P_WAIT,inimanager.GetValueString("FESTIVAL_PATH").c_str()," ","-b",Script_File_Modified.c_str(),NULL);
		printf("%s -b %s\n",inimanager.GetValueString("FESTIVAL_PATH").c_str(),Script_File_Modified.c_str());
		//printf("*  Festival ended  *\n");

		}
	}
	return 1;
}

void FestivalInterface::InvertSlashes(std::string target)
{
	int i,l;
	l=target.length();
	for(i=0;i<l;i++)
		if(target[i]=='\\')target[i]='/';
}


FestivalInterface::~FestivalInterface(void)
{
}
