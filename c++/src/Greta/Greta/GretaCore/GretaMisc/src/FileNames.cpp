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

#include "fileNames.h"
#include "IniManager.h"

extern IniManager inimanager;

FileNames::FileNames(void)
{
	Base_File="";
	APML_File="";
	Fap_File="";
	Bap_File="";
	Fap_File_Listener="";
	Wav_File="";
	Phonemes_File="";
	Script_File="";
	Text_File="";
	Turns_File="";
}

FileNames::~FileNames(void)
{
}

void FileNames::BuildFileNames(std::string filename)
{
//	int i;
	std::string basename;

	APML_File=filename;

	Base_File=filename.substr(filename.find_last_of("/\\")+1);
	Base_File=Base_File.substr(0,Base_File.find_last_of("."));

	FMLAPML_to_BML_File=inimanager.Program_Path+"bml/"+Base_File+".xml";

	Fap_File=inimanager.Program_Path+"output/"+Base_File+".fap";
	Bap_File=inimanager.Program_Path+"output/"+Base_File+".bap";
	Fap_File_Listener=inimanager.Program_Path+"output/"+Base_File+"_listener.fap";
	Wav_File=inimanager.Program_Path+"output/"+Base_File+".wav";
	Phonemes_File=inimanager.Program_Path+"tmp/"+Base_File+".pho";
	Script_File=inimanager.Program_Path+"tmp/"+Base_File+".scm";
	Text_File=inimanager.Program_Path+"tmp/"+Base_File+".txt";
	Turns_File=inimanager.Program_Path+"tmp/"+Base_File+"_turns.txt";

}
