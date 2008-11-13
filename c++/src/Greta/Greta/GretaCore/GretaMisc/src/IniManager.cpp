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
// IniManager.cpp: implementation of the IniManager class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <direct.h>

#include "IniManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IniManager::IniManager()
{
	parameters.clear();
	char *path;
	path=(char*)malloc(10000*sizeof(char));
	GetModuleFileName(NULL,path,10000);
	Program_Path=path;
	if(Program_Path.find('\\')!=std::string::npos)
		Program_Path=Program_Path.substr(0,Program_Path.find_last_of("\\")+1);
	delete path;
	_chdir(Program_Path.c_str());
	InvertSlashes(Program_Path);
	initialized=false;
}

IniManager::~IniManager()
{

}

int calculParentDir(std::string * path)
{
int i=0;
while( path->find( "../", 0 ) != std::string::npos ) {
       path->erase(0,3);
	   i++;
   }
   return i;
}

int remParentDir(std::string * path,int i)
{
	while(i!=0)
	{
		i--;
		// erase the last "/"
 		path->erase(path->length()-1);
		path->erase(path->rfind("/")+1);
	}
	return 1;
}

int IniManager::ReadIniFile(std::string IniFileName)
{
	std::string line;
	std::string path;
	parameters.clear();	

	path = Program_Path;
	remParentDir(&path,calculParentDir(&IniFileName));
	IniFileName=path+IniFileName;

	printf("reading %s\n",IniFileName.c_str());

	std::ifstream inputfile(IniFileName.c_str());
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			IniParameter *p;
			std::getline(inputfile,line,'\n');
			if(line!="")
			{
				p=new IniParameter();
				p->name=line.substr(0,line.find_first_of("="));
				ToUppercase(p->name);
				p->value=line.substr(line.find_first_of("=")+1,line.length()-line.find_first_of("=")-1);
				parameters.push_back(p);
			}
		}
	}
	else
		return 0;
	inputfile.close();
	initialized=true;
	return 1;

}

std::string IniManager::GetValueString(std::string name)
{
	std::list<IniParameter*>::iterator iter;
	ToUppercase(name);
	if(!parameters.empty())
	{
		for(iter=parameters.begin();iter!=parameters.end();iter++)
		{
			if((*iter)->name==name)
				return (*iter)->value;	
		}
	}
	printf("IniManager warning: requested unknown IniParameter %s\n",name.c_str());
	return "";
}

float IniManager::GetValueFloat(std::string name)
{
	std::list<IniParameter*>::iterator iter;
	ToUppercase(name);
	if(!parameters.empty())
	{
		for(iter=parameters.begin();iter!=parameters.end();iter++)
		{
			if((*iter)->name==name)
				return atof((*iter)->value.c_str());
		}
	}
	printf("IniManager warning: requested unknown IniParameter %s\n",name.c_str());
	return -999999999.0f;
}

int IniManager::WriteIniFile(std::string IniFileName)
{
	std::string line;

	IniFileName=Program_Path+IniFileName;
	std::ofstream outputfile(IniFileName.c_str());

	printf("writing %s\n",IniFileName.c_str());

	if(outputfile.is_open())
	{
		std::list<IniParameter*>::iterator iter;
		for(iter=parameters.begin();iter!=parameters.end();iter++)
		{
			outputfile << (*iter)->name << "=" << (*iter)->value << "\n";
		}
		outputfile.close();
	}
	else
		return 0;

	return 1;
}

std::string IniManager::ExtractValueString(std::string s)
{
	if(s.find('=',0)==std::string::npos)
		return "";
	return s.substr(s.find('=',0)+1,s.length()-s.find('=')+1);
}

float IniManager::ExtractValueFloat(std::string s)
{
	if(s.find('=',0)==std::string::npos)
		return 0;
	return atof(s.substr(s.find('=',0)+1,s.length()-s.find('=')+1).c_str());
}

void IniManager::InvertSlashes(std::string &s)
{
	for(int i=0; i<s.length(); i++)
		if(s[i]=='\\')
			s[i]='/';
}

int IniManager::GetValueInt(std::string name)
{
	std::list<IniParameter*>::iterator iter;
	ToUppercase(name);
	if(!parameters.empty())
	{
		for(iter=parameters.begin();iter!=parameters.end();iter++)
		{
			if((*iter)->name==name)
				return atoi((*iter)->value.c_str());
		}
	}
	printf("IniManager warning: requested unknown IniParameter %s\n",name.c_str());
	return -999999999;
}

void IniManager::ToUppercase(std::string &s)
{
	for(int i=0; i<s.length(); i++)
			s[i]=toupper(s[i]);
}

void IniManager::SetValueInt(std::string name, int value)
{
	std::list<IniParameter*>::iterator iter;
	char v[255];
	if(!parameters.empty())
	{
		for(iter=parameters.begin();iter!=parameters.end();iter++)
		{
			if((*iter)->name==name)
			{
				sprintf(v,"%d",value);
				(*iter)->value=std::string(v);
				return;
			}
		}
	}
	printf("IniManager warning: requested unknown IniParameter %s (for writing)\n",name.c_str());
}

void IniManager::SetValueFloat(std::string name, float value)
{
	std::list<IniParameter*>::iterator iter;
	char v[255];
	if(!parameters.empty())
	{
		for(iter=parameters.begin();iter!=parameters.end();iter++)
		{
			if((*iter)->name==name)
			{
				sprintf(v,"%.2f",value);
				(*iter)->value=std::string(v);
				return;
			}
		}
	}
	printf("IniManager warning: requested unknown IniParameter %s (for writing)\n",name.c_str());
}

void IniManager::SetValueString(std::string name, std::string value)
{
	std::list<IniParameter*>::iterator iter;
	if(!parameters.empty())
	{
		for(iter=parameters.begin();iter!=parameters.end();iter++)
		{
			if((*iter)->name==name)
			{
				(*iter)->value=value;
				return;
			}
		}
	}
	printf("IniManager warning: requested unknown IniParameter %s (for writing)\n",name.c_str());
}
