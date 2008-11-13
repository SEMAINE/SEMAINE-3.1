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

// XMLGenericTree.cpp: implementation of the XMLGenericTree class.
//
//////////////////////////////////////////////////////////////////////

#include "XMLGenericTree.h"

//for general comments refer to the header file

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XMLGenericTree::XMLGenericTree()
{
	parent=0;
	child.clear();
	name="";
	value="";
	attributes.clear();
	outputfile=0;
}

XMLGenericTree::~XMLGenericTree()
{
	
	std::list<XMLGenericTree*>::iterator iter;
	if(!this->child.empty())
	{
		for(iter=this->child.begin();iter!=this->child.end();iter++)
		{
			delete (*iter);
		}
		child.clear();
	}
	if(!attributes.empty())
	{
		attributes.clear();
	}
	

}

void XMLGenericTree::addChild(XMLGenericTree *e)
{
	if(e!=0)
	{
		e->parent=this;
		child.push_back(e);	
	}
}

void XMLGenericTree::PrintWithAttributes(std::string s)
{
	printf("%s n:%s v:%s\n",s.c_str(),name.c_str(),value.c_str());
	std::list<XMLAttribute>::iterator att_iter;
	if(!attributes.empty())
	{
		printf("%s attributes:\n",s.c_str());
		for(att_iter=attributes.begin();att_iter!=attributes.end();att_iter++)
			printf("%s   n:%s v:%s\n",s.c_str(),(*att_iter).name.c_str(),(*att_iter).value.c_str());
	}		
	std::list<XMLGenericTree*>::iterator iter;
	if(!child.empty())
		for(iter=child.begin();iter!=child.end();iter++)
			(*iter)->PrintWithAttributes(s+"-");
	
}

void XMLGenericTree::addAttribute(std::string n, std::string v)
{
	XMLAttribute a;
	a.name=n;
	a.value=v;
	attributes.push_back(a);
}

XMLGenericTree* XMLGenericTree::FindNodeCalled(std::string n)
{
	if(this->name==n)
		return this;
	else
	{
		std::list<XMLGenericTree*>::iterator iter;
		if(!this->child.empty())
			for(iter=this->child.begin();iter!=this->child.end();iter++)
			{
				XMLGenericTree *res;
				res=(*iter)->FindNodeCalled(n);
				if(res!=0)
					return res;
			}
		return 0;
	}
}

std::string XMLGenericTree::GetAttribute(std::string n)
{
	std::list<XMLAttribute>::iterator att_iter;
	if(!this->attributes.empty())
	{
		for(att_iter=this->attributes.begin();att_iter!=this->attributes.end();att_iter++)
			if((*att_iter).name==n)
			{
				return (*att_iter).value;
			}
	}
	return "";
}

float XMLGenericTree::GetAttributef(std::string n)
{
	std::string a=GetAttribute(n);
	if(a!="")
	{
		return atof(a.c_str());
	}
	return 0;
}

void XMLGenericTree::SetAttribute(std::string n, std::string value)
{
	std::list<XMLAttribute>::iterator att_iter;
	if(!this->attributes.empty())
	{
		for(att_iter=this->attributes.begin();att_iter!=this->attributes.end();att_iter++)
			if((*att_iter).name==n)
			{
				(*att_iter).value=value;
			}
	}
}

bool XMLGenericTree::HasAttribute(std::string n)
{
	std::string a=GetAttribute(n);
	if(a!="")
	{
		return true;
	}
	return false;
}

int XMLGenericTree::GetNumberOfChildren()
{
	int i;
	i=0;
	std::list<XMLGenericTree*>::iterator iter;
	if(!this->child.empty())
		for(iter=this->child.begin();iter!=this->child.end();iter++)
		{
			i++;
		}
	return i;
}

void XMLGenericTree::SaveToFile(std::string s,FILE *outputfile)
{
	fprintf(outputfile,"%s n:%s v:%s\n",s.c_str(),name.c_str(),value.c_str());
	std::list<XMLAttribute>::iterator att_iter;
	if(!attributes.empty())
	{
		fprintf(outputfile,"%s attributes:\n",s.c_str());
		for(att_iter=attributes.begin();att_iter!=attributes.end();att_iter++)
			fprintf(outputfile,"%s   n:%s v:%s\n",s.c_str(),(*att_iter).name.c_str(),(*att_iter).value.c_str());
	}		
	std::list<XMLGenericTree*>::iterator iter;
	if(!child.empty())
		for(iter=child.begin();iter!=child.end();iter++)
			(*iter)->SaveToFile(s+"-",outputfile);
	
}

std::string XMLGenericTree::ToString()
{
	std::string s;
	if((name=="text")&&(value==""))
		return "";
	if(name=="text")
		return value+"\n";
	s=s+"<"+name;
	std::list<XMLAttribute>::iterator att_iter;
	if(!attributes.empty())
	{
		for(att_iter=attributes.begin();att_iter!=attributes.end();att_iter++)
			s=s+" "+(*att_iter).name+"='"+(*att_iter).value+"'";
	}

	if(!child.empty())
	{
		s=s+">\n";
		std::list<XMLGenericTree*>::iterator iter;
		for(iter=child.begin();iter!=child.end();iter++)
			s=s+(*iter)->ToString();
		s=s+"</"+name+">\n";
	}
	else
		s=s+"/>\n";

	return s;
}

void XMLGenericTree::SaveWithAttributes(std::string filename)
{
	outputfile=fopen(filename.c_str(),"w");
	if(outputfile==0)
		return;
	SaveToFile("",outputfile);
	fclose(outputfile);
	outputfile=0;
}