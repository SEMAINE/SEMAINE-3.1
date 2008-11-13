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

#include "behaviorqualitycomputation.h"
#include "XMLGenericParser.h"
#include "BehaviorQualifier.h"
#include "Modulation.h"

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

BehaviorQualityComputation::BehaviorQualityComputation()
{
	logfile=fopen("logs/beahviorqc_log.html","w");
	fprintf(logfile,"<html>\n<head>\n</head>\n<body>\n");
}

BehaviorQualityComputation::BehaviorQualityComputation(std::string filepath)
{
	logfile=fopen("logs/beahviorqc_log.html","w");
	fprintf(logfile,"<html>\n<head>\n</head>\n<body>\n");

	this->LoadQualifiersList(filepath,"");
}

BehaviorQualityComputation::~BehaviorQualityComputation(void)
{
	fprintf(logfile,"</body>\n</html>\n");
	fclose(logfile);
}

int BehaviorQualityComputation::LoadQualifiersList(std::string filepath,char *buffer)
{
	XMLGenericParser p;
	XMLGenericTree *t;	
	if(filepath!="")
	{		
		t=p.ParseFileWithXSD(filepath,"mmsystem/xsd/qualifiers.xsd");
	}
	else
	{
		t=p.ParseBufferWithXSD(buffer,"mmsystem/xsd/qualifiers.xsd");
	}
	if(t==0)
	{
		printf("BehaviorQualityComputation: error parsing %s\n",filepath.c_str());
		fprintf(logfile,"BehaviorQualityComputation: error parsing %s\n",filepath.c_str());
		return 0;
	}
	//every child of t is a set of rules for a communicative act.
	//in the xml file all the rules for a communicative act are in the
	//same tag called "rule"
	std::list<XMLGenericTree*>::iterator rule;
	
	for(rule=(t->child).begin();rule!=(t->child).end();rule++)
	{
		BehaviorQualifier qualifier;

		qualifier.name=(*rule)->GetAttribute("name");
		fprintf(logfile,"List of the modulations found for the CA:&nbsp;<b>%s</b><br/>\n",qualifier.name.c_str());
		//every child of the rule is a modulation on an attribute of the expressivity or
		//on the preference of a modality
		std::list<XMLGenericTree*>::iterator op;
		fprintf(logfile,"<table border='1'>\n");
		fprintf(logfile,"<tr><td>destination</td><td>operand</td><td>op1</td><td>op2</td><td>op3</td></tr>\n");
		for(op=((*rule)->child).begin();op!=((*rule)->child).end();op++)
		{
			Modulation modulation;
			//the destination of the modulation must always be there
			XMLGenericTree *dest=(*op)->FindNodeCalled("destination");
			dest=dest->FindNodeCalled("text");
			RemoveSpacesAndNewlines(dest->value);
			modulation.destination=dest->value;

			//and the attribute of the destination too
			XMLGenericTree *attr=(*op)->FindNodeCalled("parameter");
			attr=attr->FindNodeCalled("text");
			RemoveSpacesAndNewlines(attr->value);
			modulation.destination+="."+attr->value;

			//the operator also must always be there
			XMLGenericTree *operat=(*op)->FindNodeCalled("operator");
			operat=operat->FindNodeCalled("text");
			RemoveSpacesAndNewlines(operat->value);
			modulation.Operator=operat->value;

			//operand1 must to be there but can have different forms
			XMLGenericTree *oper1=(*op)->FindNodeCalled("op1_name");
			if(oper1==0)
				oper1=(*op)->FindNodeCalled("op1_value");
			oper1=oper1->FindNodeCalled("text");
			RemoveSpacesAndNewlines(oper1->value);
			modulation.Operand1=oper1->value;
			
			//the attribute of operand1 can be there
			XMLGenericTree *oper1_attr=(*op)->FindNodeCalled("op1_attribute");
			if(oper1_attr!=0)
			{
				oper1_attr=oper1_attr->FindNodeCalled("text");
				RemoveSpacesAndNewlines(oper1_attr->value);
				modulation.Operand1+="."+oper1_attr->value;
			}

			//operand2 is optional
			XMLGenericTree *oper2=(*op)->FindNodeCalled("op2_name");
			if(oper2==0)
				oper2=(*op)->FindNodeCalled("op2_value");
			if(oper2!=0)
			{
				oper2=oper2->FindNodeCalled("text");
				RemoveSpacesAndNewlines(oper2->value);
				modulation.Operand2=oper2->value;
				//the attribute of operand2 is optional
				XMLGenericTree *oper2_attr=(*op)->FindNodeCalled("op2_attribute");
				if(oper2_attr!=0)
				{
					oper2_attr=oper2_attr->FindNodeCalled("text");
					RemoveSpacesAndNewlines(oper2_attr->value);
					modulation.Operand2+="."+oper2_attr->value;
				}
			}
			
			//operand3 is optional and anyway it can be only a value
			XMLGenericTree *oper3=(*op)->FindNodeCalled("op3_value");
			if(oper3!=0)
			{
				oper3=oper3->FindNodeCalled("text");
				RemoveSpacesAndNewlines(oper3->value);
				modulation.Operand3=oper3->value;
			}

			
			fprintf(logfile,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
				modulation.destination.c_str(),modulation.Operator.c_str(),
				modulation.Operand1.c_str(),modulation.Operand2.c_str(),modulation.Operand3.c_str());

			qualifier.AddModulation(modulation);
			
		}
		this->AddQualifier(qualifier);

		fprintf(logfile,"</table><br/>\n");
	}

	//this->PrintQualifiers();

	delete t;

	return 1;
}

int BehaviorQualityComputation::AddQualifier(BehaviorQualifier qual)
{
	if(this->qualifiers.find(qual.name)!=this->qualifiers.end())
	{
		printf("BehaviorQualityComputation: could not add duplicated qualifier %s\n",qual.name.c_str());
		fprintf(logfile,"BehaviorQualityComputation: could not add duplicated qualifier %s\n",qual.name.c_str());
	}
	else
		this->qualifiers[qual.name]=qual;
	return 1;
}

Dynamicline BehaviorQualityComputation::ComputeDynamicline(MMSystemSpace::CommunicativeIntention *ca,Baseline bl)
{
	bool found;

	Dynamicline dl;

	dl=bl;


	BehaviorQualifier *qual;

	found=true;

	qual=FromCAtoQualifier(*ca);

	if(qual==0)
		found=false;

	if(found==true)
	{

		std::map<std::string,ModulationList>::iterator iter;
		for(iter=(qual->modulations).begin();iter!=(qual->modulations).end();iter++)
		{
			std::list<Modulation>::iterator iter2;
			for(iter2=(*iter).second.begin();iter2!=(*iter).second.end();iter2++)
			{
				std::string destname=(*iter2).destination.substr(0,(*iter2).destination.find_first_of("."));
				std::string destattr=(*iter2).destination.substr((*iter2).destination.find_first_of(".")+1);

				float *destination;

				destination=dl.GetEngineParameterSet(destname)->GetAttrAddress(destattr);

				if(destination==0)
				{
					printf("BehaviorQualityComputation: illegal modulation on attribute %s\n",destattr.c_str());
					fprintf(logfile,"BehaviorQualityComputation: illegal modulation on attribute %s<br>\n",destattr.c_str());
					break;
				}
				
				float op1;
				op1=dl.GetAttrValue((*iter2).Operand1);

				if(&op1==0)
				{
					printf("BehaviorQualityComputation: illegal modulation on attribute %s\n",(*iter2).Operand1.c_str());
					fprintf(logfile,"BehaviorQualityComputation: illegal modulation on attribute %s<br>\n",(*iter2).Operand1.c_str());
					break;
				}

				std::string op2name=(*iter2).Operand2.substr(0,(*iter2).Operand2.find_first_of("."));
				std::string op2attr=(*iter2).Operand2.substr((*iter2).Operand2.find_first_of(".")+1);

				float op2;

				op2=dl.GetAttrValue((*iter2).Operand2);

				if(&op2==0)
				{
					printf("BehaviorQualityComputation: illegal modulation on attribute %s\n",(*iter2).Operand2.c_str());
					fprintf(logfile,"BehaviorQualityComputation: illegal modulation on attribute %s<br>\n",(*iter2).Operand2.c_str());
					break;
				}

				std::string op3name=(*iter2).Operand3.substr(0,(*iter2).Operand3.find_first_of("."));
				std::string op3attr=(*iter2).Operand3.substr((*iter2).Operand3.find_first_of(".")+1);

				float op3;

				op3=dl.GetAttrValue((*iter2).Operand3);

				if(&op3==0)
				{
					printf("BehaviorQualityComputation: illegal modulation on attribute %s\n",(*iter2).Operand3.c_str());
					fprintf(logfile,"BehaviorQualityComputation: illegal modulation on attribute %s<br>\n",(*iter2).Operand3.c_str());
					break;
				}

				ComputeOperation(destination,(*iter2).Operator,op1,op2,op3);

			}
		}
	}



	return dl;
}

void BehaviorQualityComputation::ComputeOperation(float *destination,std::string operat,float op1,float op2,float op3)
{
	if(operat=="VAL")
		*destination=op1;
	if(operat=="ADD")
		*destination=op1+op2;
	if(operat=="SUB")
		*destination=op1-op2;
	if(operat=="MUL")
		*destination=op1*op2;
	if(operat=="DIV")
		*destination=op1/op2;
	if(operat=="REL")
		*destination=op1+(op2-op1)*op3;
	if(operat=="LIM")
		*destination=Limit(*destination,op1,op2);
}

void BehaviorQualityComputation::PrintQualifiers()
{
	std::map<std::string,BehaviorQualifier>::iterator iter;
	for(iter=qualifiers.begin();iter!=qualifiers.end();iter++)
	{
		printf("\nqualifier name %s (id=%s):\n",
			(*iter).second.name.c_str(),(*iter).first.c_str());
		std::map<std::string,ModulationList>::iterator iter2;
		for(iter2=(((*iter).second).modulations).begin();iter2!=(((*iter).second).modulations).end();iter2++)
		{
			printf("modulation (id=%s):\n",(*iter2).first.c_str());
			std::list<Modulation>::iterator iter3;
			for(iter3=(*iter2).second.begin();iter3!=(*iter2).second.end();iter3++)
			{
				printf("\t\t%s %s %s %s %s\n",(*iter3).destination.c_str(),(*iter3).Operator.c_str(),
				(*iter3).Operand1.c_str(),(*iter3).Operand2.c_str(),(*iter3).Operand3.c_str());
			}
		}
	}
}

float BehaviorQualityComputation::Limit(float val,float min,float max)
{
	if(val>max)
		return max;
	if(val<min)
		return min;
	return val;
}

void BehaviorQualityComputation::RemoveSpacesAndNewlines(std::string &s)
{
	unsigned int i,j;
	for(i=0;i<s.length();i++)
		if((s[i]=='\n')||(s[i]=='\r')||(s[i]==9))
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
				s1=s.substr(0,i);
				s2=s.substr(j,s.length()-j+1);
				s=s1+s2;
			}
			else
				s=s.substr(0,i);
		}
	}
}

BehaviorQualifier *BehaviorQualityComputation::FromCAtoQualifier(MMSystemSpace::CommunicativeIntention ca)
{
	std::string lookfor;

	lookfor=ca.name+"-"+ca.type;

	if(ca.target!="")
		lookfor+="-"+ca.target;

	if(qualifiers.find(lookfor)==qualifiers.end())
	{
		lookfor=ca.name+"-"+ca.ref_type+"-"+ca.ref_id+"-"+ca.prop_type+"-"+ca.prop_value;
		if(qualifiers.find(lookfor)==qualifiers.end())
		{
			lookfor=ca.name+"-"+ca.ref_type+"-"+ca.ref_id;
			if(qualifiers.find(lookfor)==qualifiers.end())
			{
				lookfor=ca.name+"-*";
				if(qualifiers.find(lookfor)==qualifiers.end())
				{
					fprintf(logfile,"BehaviorQualityComputation: not found any qualifier for %s<br>\n",(ca.name+"-"+ca.type).c_str());
					return 0;
				}
			}
		}
	}

	//printf("BehaviorQualityComputation: found the qualifier of %s\n",lookfor.c_str());
	
	return &(qualifiers[lookfor]);
}