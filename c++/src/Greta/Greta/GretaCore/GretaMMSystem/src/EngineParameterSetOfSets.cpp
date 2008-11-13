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

#include ".\EngineParameterSetOfSets.h"
#include "XMLGenericParser.h"
#include "EngineParameter.h"

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

EngineParameterSetOfSets::EngineParameterSetOfSets(void)
{
}

EngineParameterSetOfSets::~EngineParameterSetOfSets(void)
{
}

int EngineParameterSetOfSets::LoadFromBuffer(std::string buffer)
{
	XMLGenericParser p;
	XMLGenericTree *t;
	t=p.ParseBufferWithXSD((char*)buffer.c_str(),"mmsystem/xsd/profiles.xsd");
	if(t==0)
	{
		printf("EngineParameterSetOfSets: error parsing buffer\n");
		return 0;
	}

	std::list<XMLGenericTree*>::iterator mod,modpar;
	
	for(mod=(t->child).begin();mod!=(t->child).end();mod++)
	{
		EngineParameterSet m;
		m.name=(*mod)->GetAttribute("name");

		for(modpar=((*mod)->child).begin();modpar!=((*mod)->child).end();modpar++)
		{
			EngineParameter mp;
			mp.name=(*modpar)->GetAttribute("name");
			mp.SetValue((*modpar)->GetAttributef("value"));
			(m.mp)[mp.name]=mp;
		}
		(*this)[m.name]=m;
	}

	delete t;
	return 1;
}

int EngineParameterSetOfSets::Load(std::string filepath,char *buffer)
{
	XMLGenericParser p;
	XMLGenericTree *t;	
	if(filepath!="")
	{		
		t=p.ParseFileWithXSD(filepath,"mmsystem/xsd/profiles.xsd");
	}
	else
	{
		t=p.ParseBufferWithXSD(buffer,"mmsystem/xsd/profiles.xsd");
	}
	if(t==0)
	{
		printf("EngineParameterSetOfSets error parsing \n");
		return 0;
	}

	std::list<XMLGenericTree*>::iterator mod,modpar;
	
	for(mod=(t->child).begin();mod!=(t->child).end();mod++)
	{
		EngineParameterSet m;
		m.name=(*mod)->GetAttribute("name");

		for(modpar=((*mod)->child).begin();modpar!=((*mod)->child).end();modpar++)
		{
			EngineParameter mp;
			mp.name=(*modpar)->GetAttribute("name");
			mp.SetValue((*modpar)->GetAttributef("value"));
			(m.mp)[mp.name]=mp;
		}
		(*this)[m.name]=m;
	}

	delete t;
	return 1;
}
/*
int EngineParameterSetOfSets::LoadFromFile(std::string filepath)
{
	std::string buffer,line;
	if(filepath=="")
	{
		printf("EngineParameterSetOfSets: no filename given\n");
		return 0;
	}
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			buffer=buffer+line+"\n";
		}
		inputfile.close();
		if(LoadFromBuffer(buffer)==0)
			return 0;
		else
			return 1;
	}
	return 0;
}
*/
EngineParameterSet *EngineParameterSetOfSets::GetEngineParameterSet(std::string name)
{
	//this->Print();
	if(((*this).find(name)==(*this).end()))
		return 0;
	return &((*this)[name]);
}


float EngineParameterSetOfSets::GetAttrValue(std::string name)
{
	std::string opname=name.substr(0,name.find_first_of("."));
	std::string opattr=name.substr(name.find_first_of(".")+1);

	if((GetEngineParameterSet(opname)==0)
		||((GetEngineParameterSet(opname)!=0)
		&&(GetEngineParameterSet(opname)->GetAttrAddress(opattr)==0)))
	{
		if(name.find_first_not_of("0123456789-.")!=std::string::npos)
		{
			return 0;
		}
		return (float)atof(name.c_str());
	}
	else
		return *GetEngineParameterSet(opname)->GetAttrAddress(opattr);
}

int EngineParameterSetOfSets::SetAttrValue(std::string name, std::string value)
{
	std::string opname=name.substr(0,name.find_first_of("."));
	std::string opattr=name.substr(name.find_first_of(".")+1);

	if((GetEngineParameterSet(opname)==0)
		||((GetEngineParameterSet(opname)!=0)
		&&(GetEngineParameterSet(opname)->GetAttrAddress(opattr)==0)))
		return 0;
	else 
		GetEngineParameterSet(opname)->mp[opattr].SetValue(atof(value.c_str()));
}

std::string EngineParameterSetOfSets::PrintXML()
{
	std::string xmlstring;
	char value[255];

	xmlstring="<?xml version=\"1.0\"?>\n";
	xmlstring+="<profile\n";
	xmlstring+="xmlns=\"http://www.w3schools.com\"\n";
	xmlstring+="xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
	xmlstring+="xsi:schemaLocation=\"profiles.xsd\">\n";


	std::map<std::string,EngineParameterSet>::iterator iter;
	for(iter=(*this).begin();iter!=(*this).end();iter++)
	{
		xmlstring+="<parameter-set name=\"" + (*iter).first + "\">\n";
		std::map<std::string,EngineParameter>::iterator iter2;
		for(iter2=(((*iter).second).mp).begin();iter2!=(((*iter).second).mp).end();iter2++)
		{
			sprintf(value,"%.2f",(*iter2).second.GetValue());
			xmlstring+="<Parameter name=\"" + (*iter2).first + "\" value=\"" + (std::string)value + "\"/>\n";		
		}
		xmlstring+="</parameter-set>\n";
	}
	xmlstring+="</profile>\n";
	return xmlstring;
}

void EngineParameterSetOfSets::Print()
{
	printf("******************************************\n");
	std::map<std::string,EngineParameterSet>::iterator iter;
	for(iter=(*this).begin();iter!=(*this).end();iter++)
	{
		printf("parameter-set (id=%s): %s\n",(*iter).first.c_str(),(*iter).second.name.c_str());
		std::map<std::string,EngineParameter>::iterator iter2;
		for(iter2=(((*iter).second).mp).begin();iter2!=(((*iter).second).mp).end();iter2++)
		{
			printf("\tmodparam (id=%s): %s=%.2f\n",(*iter2).first.c_str(),
				(*iter2).second.name.c_str(),(*iter2).second.GetValue());
				//,(*iter2).second.min,(*iter2).second.max);
		
		}
	}
}

void EngineParameterSetOfSets::SaveCSV(float time,FILE *preferencefile,FILE *gesturefile,FILE *torsofile,FILE *headfile,FILE *facefile,FILE *gazefile)
{
	FILE *f;
	float gestp,torsp,facep,headp,gazep;
	char store[255];
	
	std::map<std::string,EngineParameterSet>::iterator iter;
	for(iter=(*this).begin();iter!=(*this).end();iter++)
	{
		if((*iter).second.name=="gesture")
		{
			f=gesturefile;
			gestp=(*iter).second.mp["preference.value"].GetValue();
		}
		if((*iter).second.name=="torso")
		{
			f=torsofile;
			torsp=(*iter).second.mp["preference.value"].GetValue();
		}
		if((*iter).second.name=="head")
		{
			f=headfile;
			headp=(*iter).second.mp["preference.value"].GetValue();
		}
		if((*iter).second.name=="face")
		{
			f=facefile;
			facep=(*iter).second.mp["preference.value"].GetValue();
		}
		if((*iter).second.name=="gaze")
		{
			f=gazefile;
			gazep=(*iter).second.mp["preference.value"].GetValue();
		}

		sprintf(store,"%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
			time,
			(*iter).second.mp["preference.value"].GetValue(),
			(*iter).second.mp["OAC.value"].GetValue(),
			(*iter).second.mp["SPC.value"].GetValue(),
			(*iter).second.mp["TMP.value"].GetValue(),
			(*iter).second.mp["FLD.value"].GetValue(),
			(*iter).second.mp["PWR.value"].GetValue(),
			(*iter).second.mp["REP.value"].GetValue());
		fprintf(f,"%s",store);
	}

	sprintf(store,"%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
		time,gestp,torsp,headp,facep,gazep);

	fprintf(preferencefile,"%s",store);
}
