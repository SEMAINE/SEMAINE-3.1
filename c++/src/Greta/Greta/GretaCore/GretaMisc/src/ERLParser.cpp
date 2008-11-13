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

// ERLParser.cpp: implementation of the ERLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "ERLParser.h"
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

ERLParser::ERLParser():XMLGenericParser()
{
	//reads the configuration of the parser
	//inimanager.ReadIniFile("ERLParser.ini");
	//default value for timing calculation is false
}

ERLTree *ERLParser::ParseFile(std::string name)
{
	XMLGenericTree *generictree;
	//parses the file using a generic parser
	generictree=XMLGenericParser::ParseFile(name);
	//converts the result to an APML tree
	result_tree=ERLTree::ConvertGenericToERLTree(generictree);
	//deletes the generic tree
	delete generictree;
	//if there is something in the tree
	if(result_tree!=0)
	{
		CalculateTimings();
	}
	return result_tree;
}

int ERLParser::CalculateTimings()
{
	//assigns filenames to the text nodes of the tree
	result_tree->AssignPhonemesFileNames(inimanager.Program_Path+"tmp/");
//	if(calculate_timings)
//	{
		//calls festival
		ERLParser::CalculateTextNodesTimings();
		result_tree->AssignTimings();
//	}
	return 1;
}


ERLParser::~ERLParser()
{

}


void ERLParser::CalculateTextNodesTimings(void)
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

