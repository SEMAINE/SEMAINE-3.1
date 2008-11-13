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

#include "InputData.h"
#include "XMLGenericTree.h"
#include "XMLGenericParser.h"


InputData::InputData()
{
}

InputData::InputData(std::string modality, std::string name)
{
	int i;
	this->modality=ToLower(modality);
	this->name=ToUpper(name);
	this->priority=-1;
	this->probability=0.0;
	this->zone="";
	this->time=-1;
	this->triggerBC=0;
	for(i=0; i<3; i++)
	{
		radiant_head_rotation[i]=0.0;
		mm_head_translation[i]=0.0;
	}
}

InputData::~InputData(void)
{
}

void InputData::AddParameter(std::string name, std::string value)
{
	mapParameters[ToUpper(name)] = value;
}

std::string InputData::ToUpper(std::string s)
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

std::string InputData::ToLower(std::string s)
{
	std::string r;
	int c;
	r="";
	for(int i=0; i<s.length(); i++)
	{
		c=tolower(s[i]);
		std::string character(1,c);
		r=r+character;
	}
	return r;
}

void InputData::CleanData()
{
	mapParameters.clear();
}

void InputData::PrintData()
{
	std::map<std::string, std::string>::iterator iterdata;

	printf("NAME: %s  TIME: %d   PRIORITY: %d   PROBABILITY: %.2f\n", this->name.c_str(), this->time, this->priority, this->probability);	
	printf("PARAMETERS:\n");

	for(iterdata=mapParameters.begin();iterdata!=mapParameters.end();iterdata++)
		printf("PAR_NAME: %s  PAR_VALUE: %s\n", (*iterdata).first.c_str(), (*iterdata).second.c_str());
}