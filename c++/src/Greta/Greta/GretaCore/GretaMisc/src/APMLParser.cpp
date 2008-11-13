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

// APMLParser.cpp: implementation of the APMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "APMLParser.h"
#include <stdio.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <libxml/xmlreader.h>

extern IniManager inimanager;
//for general comments refer to the header file

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

APMLParser::APMLParser():XMLGenericParser()
{
	//reads the configuration of the parser
	//inimanager.ReadIniFile("apmlparser.ini");
	//default value for timing calculation is false
	calculate_timings=false;
}

APMLTree *APMLParser::ParseFile(std::string name)
{
	XMLGenericTree *generictree;
	//error if timings have to be calculated but ini file was not found
	if((calculate_timings)&&(inimanager.parameters.empty()))
	{
		printf("apml parser: ini file not found or empty\n");
		return 0;
	}
	this->xmlfilename=name;
	//defines the file names used by festival
	MakeFilesNames(name.substr(name.find_last_of("/\\")+1));
	//parses the file using a generic parser
	generictree=XMLGenericParser::ParseFile(name);
	//converts the result to an APML tree
	result_tree=APMLTree::ConvertGenericToAPMLTree(generictree);
	//deletes the generic tree
	delete generictree;
	//if there is something in the tree
	if(result_tree!=0)
	{
		CalculateTimings();
	}
	return result_tree;
}

int APMLParser::CalculateTimings()
{
	//assigns filenames to the text nodes of the tree
	result_tree->AssignPhonemesFileNames(inimanager.Program_Path+"tmp/");
	if(calculate_timings)
	{
		//calls festival
		CallFestival();
		//calculates nodes' timings
		APMLParser::CalculateTextNodesTimings();
		result_tree->AssignTimings();
	}
	return 1;
}


APMLParser::~APMLParser()
{

}

void APMLParser::EnableTimingsCalculation()
{
	calculate_timings=true;

}

void APMLParser::DisableTimingsCalculation()
{
	calculate_timings=false;

}


int APMLParser::CallFestival()
{
	FILE *scriptfile;

	//creates an empty script file for Festival
	scriptfile=fopen(scriptfilename.c_str(),"w");
	//puts stuff in it
	fprintf(scriptfile,"(require 'us_define-animation)\n");
	fprintf(scriptfile,"(Param.set 'Duration_Stretch %f)\n",1.0f);
	
	if(result_tree!=0)
	{

		result_tree->WriteLinesToScriptFile(scriptfile);
		
		fprintf(scriptfile,"(APML_PhdurWav_save \"%s\" \"%s\" \"%s\")\n",xmlfilename.c_str(),phonemesfilename.c_str(),wavfilename.c_str());

		fclose(scriptfile);

		_spawnl( _P_WAIT,inimanager.GetValueString("FESTIVAL_PATH").c_str()," ","-b",scriptfilename.c_str(),NULL);
	}
	return 1;
}

void APMLParser::CalculateTextNodesTimings(void)
{
	float time;

	time=0;

	//opens the file containing the phonemes of the text

	std::ifstream globalphonemesfile(phonemesfilename.c_str());

	//calls the function to calculate tree's timings
	if(result_tree!=0)
		result_tree->MatchPhonemesTimingsForText(globalphonemesfile,time);
			
	//closes file
	globalphonemesfile.close();
}

void APMLParser::MakeFilesNames(std::string s)
{
	wavfilename=s.substr(0,s.find_last_of("."))+".wav";
	wavfilename=inimanager.Program_Path+"output/"+wavfilename;
	phonemesfilename=s.substr(0,s.find_last_of("."))+".pho";
	phonemesfilename=inimanager.Program_Path+"tmp/"+phonemesfilename;
	scriptfilename=s.substr(0,s.find_last_of("."))+".scm";
	scriptfilename=inimanager.Program_Path+"tmp/"+scriptfilename;
}
