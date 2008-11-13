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

// APMLTree.cpp: implementation of the APMLTree class.
//
//////////////////////////////////////////////////////////////////////

#include "APMLTree.h"

//for general comments refer to the header file

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned int APMLTree::phonemesfilecount=0;

//not commented - just copies a generic tree into an APML tree
APMLTree* APMLTree::ConvertGenericToAPMLTree(XMLGenericTree *generic)
{
	std::list<XMLGenericTree*>::iterator iter;
	APMLTree *element;
	if(generic==0)
		return 0;
	element=new APMLTree();
	element->name=generic->name;
	if(element->name=="text")
		element->RemoveSpacesAndNewlines(generic->value);
	element->value=generic->value;
	std::list<XMLAttribute>::iterator att_iter;
	if(!generic->attributes.empty())
	{
		for(att_iter=(generic->attributes.begin());att_iter!=generic->attributes.end();att_iter++)
			element->addAttribute((*att_iter).name,(*att_iter).name);
	}		
	if(!generic->child.empty())
	for(iter=generic->child.begin();iter!=generic->child.end();iter++)
		element->addChild(APMLTree::ConvertGenericToAPMLTree(*iter));
	return element;
}


//constructor
APMLTree::APMLTree():XMLGenericTree()
{
	start=0.0f;
	duration=0.0f;
}

APMLTree::~APMLTree()
{

}

//adds a child
void APMLTree::addChild(APMLTree *e)
{
	if(e!=0)
	{
		e->parent=this;
		child.push_back(e);	
	}
}

//just to print out the tree
void APMLTree::PrintWithDurations(std::string s)
{
	std::list<APMLTree*>::iterator iter;
	if(!child.empty())
		for(iter=child.begin();iter!=child.end();iter++)
			(*iter)->PrintWithDurations(s+"-");
	
}

void APMLTree::setValue(std::string s)
{
	RemoveSpacesAndNewlines(s);
	value=s;
}

//useful function
void APMLTree::RemoveSpacesAndNewlines(std::string &s)
{
	unsigned int i,j;
	for(i=0;i<s.length();i++)
		if((s[i]=='\n')||(s[i]=='\r'))
			s[i]=' ';
	for(i=0;i<s.length();i++)
	{
		if((s[i]==' ')&&(i<(s.length()-1)))
		{
			j=i+1;
			while((s[j]==' ')&&(j<s.length()))
				j++;
			if(j<s.length())
			{
				std::string s1,s2;
				s1=s.substr(0,i+1);
				s2=s.substr(j,s.length()-j+1);
				s=s1+s2;
			}
			else
				s=s.substr(0,i+1);
		}
	}
}

//this function writes the line corresponding to "this" node into the script file
//for Festival
void APMLTree::WriteLinesToScriptFile(FILE *f)
{
	std::list<APMLTree*>::iterator iter;

	if(name=="text")
		fprintf(f,"(Phdur_save_text \"%s\" \"%s\")\n",value.c_str(),PhonemesFileName.c_str());
			
	if(!child.empty())
		for(iter=child.begin();iter!=child.end();iter++)
			(*iter)->WriteLinesToScriptFile(f);
}

void APMLTree::MatchPhonemesTimingsForText(std::ifstream &globalphonemesfile,float &time)
{
	std::list<APMLTree*>::iterator iter;
	std::string linefromtagfile,linefromglobalfile,aux;
	float tagtime;
	bool found;

	//if it's a text node
	if(name=="text")
	{
		//opens the phonemes file corresponding to the node
		std::ifstream tagphonemesfile(PhonemesFileName.c_str());
		tagtime=0;
		//until there is something in the file
		while(tagphonemesfile.good())
		{
			//reads a line
			std::getline(tagphonemesfile,linefromtagfile,'\n');
			//if it's not empty
			if(linefromtagfile=="")
				continue;
			//if it's not a pause
			if((linefromtagfile.find("pau",0)!=std::string::npos)||
				(linefromtagfile.find('#',0)!=std::string::npos)||
				(linefromtagfile.find("_",0)!=std::string::npos))
			{
				if(start!=0)
				{
					std::streampos pos=globalphonemesfile.tellg();
					std::getline(globalphonemesfile,linefromglobalfile,'\n');
					if((linefromglobalfile.find("pau",0)!=std::string::npos)||
						(linefromglobalfile.find("_",0)!=std::string::npos)||
					(linefromglobalfile.find('#',0)!=std::string::npos))
					{
						aux=linefromglobalfile.substr(linefromglobalfile.find(' ',0),linefromglobalfile.length()-linefromglobalfile.find(' ',0)+1);
						time=time+(float)atof(aux.c_str());
					}
					else
						globalphonemesfile.seekg(pos);
					
					duration=time-start;
				}
				continue;
			}
			found=false;
			linefromtagfile=linefromtagfile.substr(0,linefromtagfile.find(' ',0));
			while((globalphonemesfile.good())&&(found==false))
			{
				std::getline(globalphonemesfile,linefromglobalfile,'\n');


				aux=linefromglobalfile.substr(0,linefromglobalfile.find(' ',0));
				if(aux==linefromtagfile)
				{
					found=true;
					if(start==0)
						start=time;
				}

				aux=linefromglobalfile.substr(linefromglobalfile.find(' ',0),linefromglobalfile.length()-linefromglobalfile.find(' ',0)+1);
				time=time+(float)atof(aux.c_str());
/*
				if((linefromglobalfile.find("pau",0)!=std::string::npos)||
				(linefromglobalfile.find('#',0)!=std::string::npos))
				{
					continue;
				}
				*/
			}
		}
		tagphonemesfile.close();
	}

	if(!child.empty())
		for(iter=child.begin();iter!=child.end();iter++)
			(*iter)->MatchPhonemesTimingsForText(globalphonemesfile,time);

}

void APMLTree::AssignTimings()
{
	std::list<APMLTree*>::iterator iter;
	if(!child.empty())
		for(iter=child.begin();iter!=child.end();iter++)
			(*iter)->AssignTimings();
	if((name!="text")&&(name!="boundary")&&(!child.empty()))
	{
		this->start=(*child.begin())->start;
		float d;
		d=0;
		for(iter=child.begin();iter!=child.end();iter++)
			d=d+(*iter)->duration;
		this->duration=d;
	}
}

void APMLTree::AssignPhonemesFileNames(std::string &programpath)
{
	char c_path[30];
	std::string path;
	
	if(name=="text")
	{
		_itoa(phonemesfilecount++,c_path,10);
		path=c_path;
		path="tmp"+path;
		path=path+".pho";
		path=""+path;
		path=programpath+path;
		PhonemesFileName=path;
	}

	std::list<APMLTree*>::iterator iter;
	if(!child.empty())
		for(iter=child.begin();iter!=child.end();iter++)
			(*iter)->AssignPhonemesFileNames(programpath);

	
}


