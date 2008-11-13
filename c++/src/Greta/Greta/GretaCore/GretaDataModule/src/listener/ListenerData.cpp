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

// ListenerData.cpp: implementation of the ListenerData class.
// to upload all data needed by the listener model (in Intention Planner)
//////////////////////////////////////////////////////////////////////

#include "ListenerData.h"
#include "XMLGenericParser.h"

extern IniManager inimanager;

extern FILE *data_log;


ListenerData::ListenerData()
{

}

ListenerData::~ListenerData()
{

}

void ListenerData::LoadAll(std::string data_repertory)
{
	std::string listener;
	if (data_repertory=="")
	{
		listener="listener";
	} else {
		listener="listener/";
	}
	std::string analysisrules="/analysisrules.xml";
	std::string rules="/rules.xml";
	std::string rulesfile="/rulesfile.xml";
	std::string initialstate="/initialstate.xml";

	LoadInputAdaptRules(listener+data_repertory+analysisrules);
	LoadBackchannelRules(listener+data_repertory+rules);
	LoadBackchannelComplexRules(listener+data_repertory+rulesfile);
	GenerateCombinationRules();
	LoadAgentState(listener+data_repertory+initialstate);
}

int ListenerData::LoadInputAdaptRules(std::string filepath)
{
	if(filepath=="")
	{
		printf("ListenerData: no filename given\n");
		fprintf(data_log,"ListenerData: no filename given\n");
		return 0;
	}
	XMLGenericParser *xmlparser;
	xmlparser=new XMLGenericParser();
	XMLGenericTree *xmltree;
	xmltree=xmlparser->ParseFile(filepath);
	if(xmltree==0)
	{
		printf("ListenerData: error parsing %s\n",filepath.c_str());
		fprintf(data_log,"ListenerData: error parsing %s\n",filepath.c_str());
		return 0;
	}
	
	std::list<XMLGenericTree*>::iterator rule;
		
	if(!(xmltree->child).empty())
		for(rule=(xmltree->child).begin();rule!=(xmltree->child).end();rule++)
		{
			if((*rule)->name=="rule")
			{
				AdaptRules *newrule;
				newrule=new AdaptRules();

				newrule->name=(*rule)->GetAttribute("name");

				std::list<XMLGenericTree*>::iterator children;
				children=((*rule)->child).begin();

				while(children!=(*rule)->child.end())
				{
					if((*children)->name=="modify")
					{
						Operations *newoperation;
						newoperation=new Operations();

						LoadOperation(children, newoperation);
						newrule->vecModify.push_back(*newoperation);
					}
					else
					if((*children)->name=="ifthen")
					{
						Implications *newimplication;
						newimplication=new Implications();

						std::list<XMLGenericTree*>::iterator childimp;
						childimp=((*children)->child).begin();

						while(childimp!=(*children)->child.end())
						{
							if((*childimp)->name=="condition")
							{
								Operations *newoperation1;
								newoperation1=new Operations();

								LoadOperation(childimp, newoperation1);
								newimplication->vecCondiction.push_back(*newoperation1);
							}
							else
							if((*childimp)->name=="modify")
							{
								Operations *newoperation2;
								newoperation2=new Operations();

								LoadOperation(childimp, newoperation2);
								newimplication->vecConsequence.push_back(*newoperation2);
							}
							childimp++;
						}
						newrule->vecImplication.push_back(*newimplication);
					}
					children++;
				}

				//newrule->PrintRule();
				mapAdaptRules[(*rule)->GetAttribute("name")]=(*newrule);
			}
		}

	delete xmltree;
	delete xmlparser;

	return 1;
}

int ListenerData::LoadOperation(std::list<XMLGenericTree*>::iterator children, Operations *oper)
{
	//the parameter must always be there
	XMLGenericTree *par=(*children)->FindNodeCalled("parameter");
	if(par==NULL) return 0;
	par=par->FindNodeCalled("text");
	oper->parameter=ToUpper(par->value);

	//and the operator too
	XMLGenericTree *operat=(*children)->FindNodeCalled("operator");
	if(operat==NULL) return 0;
	operat=operat->FindNodeCalled("text");
	oper->op=ToUpper(operat->value);

	//the value also must always be there
	XMLGenericTree *value=(*children)->FindNodeCalled("value");
	if(value==NULL) return 0;
	value=value->FindNodeCalled("text");
	oper->value=value->value;

	return 1;
}

int ListenerData::LoadBackchannelRules(std::string filepath)
{
	if(filepath=="")
	{
		printf("ListenerData: no filename given\n");
		fprintf(data_log,"ListenerData: no filename given\n");
		return 0;
	}
	XMLGenericParser *xmlparser;
	xmlparser=new XMLGenericParser();
	XMLGenericTree *xmltree;
	xmltree=xmlparser->ParseFile(filepath);
	if(xmltree==0)
	{
		printf("ListenerData: error parsing %s\n",filepath.c_str());
		fprintf(data_log,"ListenerData: error parsing %s\n",filepath.c_str());
		return 0;
	}
	
	std::list<XMLGenericTree*>::iterator rule;
		
	if(!(xmltree->child).empty())
		for(rule=(xmltree->child).begin();rule!=(xmltree->child).end();rule++)
		{
			if((*rule)->name=="rule")
			{
				BCRules *newrule;
				newrule=new BCRules();

				newrule->name=(*rule)->GetAttribute("name");

				std::list<XMLGenericTree*>::iterator children;
				children=((*rule)->child).begin();

				while(children!=(*rule)->child.end())
				{
					if((*children)->name=="parameter")
					{
						//newrule->mapParameters[(*children)->GetAttribute("name")]=atof((*children)->GetAttribute("value").c_str());				
						newrule->mapParameters[(*children)->GetAttribute("name")]=(*children)->GetAttribute("value");				
					}
					children++;
				}

				//newrule->PrintRule();
				mapBackchannelRules[(*rule)->GetAttribute("name")]=(*newrule);
			}
		}

	delete xmltree;
	delete xmlparser;

	return 1;
}


int ListenerData::LoadAgentState(std::string filepath)
{
	std::string line;
	if(filepath=="")
	{
		printf("ListenerData: no filename given\n");
		fprintf(data_log,"ListenerData: error parsing %s\n",filepath.c_str());
		return 0;
	}
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			agentstate=agentstate+line+"\n";
		}
		inputfile.close();
	}
	else 
		return 0;
	return 1;
}

std::string ListenerData::ToUpper(std::string s)
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





int ListenerData::LoadBackchannelComplexRules(std::string filepath)
{
	if(filepath=="")
	{
		printf("ListenerData: no filename given\n");
		fprintf(data_log,"ListenerData: no filename given\n");
		return 0;
	}
	XMLGenericParser *xmlparser;
	xmlparser=new XMLGenericParser();
	XMLGenericTree *xmltree;
	xmltree=xmlparser->ParseFile(filepath);
	if(xmltree==0)
	{
		printf("ListenerData: error parsing %s\n",filepath.c_str());
		fprintf(data_log,"ListenerData: error parsing %s\n",filepath.c_str());
		return 0;
	}
	
	std::list<XMLGenericTree*>::iterator rule;
		
	if(!(xmltree->child).empty())
		for(rule=(xmltree->child).begin();rule!=(xmltree->child).end();rule++)
		{
			if((*rule)->name=="rule")
			{
				BCComplexRules *newrule;
				newrule=new BCComplexRules();

				newrule->name=(*rule)->GetAttribute("name");

				std::list<XMLGenericTree*>::iterator children_rule;
				children_rule=((*rule)->child).begin();



				while(children_rule!=(*rule)->child.end())
				{
					if((*children_rule)->name=="usersignals")
					{
						Sig *newusersig=new Sig();
						std::list<XMLGenericTree*>::iterator children_usersignals;
						children_usersignals=((*children_rule)->child).begin();
						while(children_usersignals!=(*children_rule)->child.end())
						{
							newusersig->id=atoi((*children_usersignals)->GetAttribute("id").c_str());
							newusersig->name=(*children_usersignals)->GetAttribute("name");
							newusersig->modality=(*children_usersignals)->GetAttribute("modality");
							newrule->usersignalsVec.push_back((*newusersig));

							//newrule->usersignalsVec[(*children_usersignals)->GetAttribute("name")]=(*children_usersignals)->GetAttribute("id");
							children_usersignals++;
						}

					}
					else
					if((*children_rule)->name=="response")
					{
						newrule->response_probability=atof((*children_rule)->GetAttribute("probability").c_str());
						newrule->response_priority=atoi((*children_rule)->GetAttribute("priority").c_str());
						std::list<XMLGenericTree*>::iterator children_response;
						children_response=((*children_rule)->child).begin();

						while(children_response!=(*children_rule)->child.end())
						{
							if((*children_response)->name=="signals")
							{
								newrule->signals_probability=atof((*children_response)->GetAttribute("probability").c_str());
								std::list<XMLGenericTree*>::iterator children_signals;
								children_signals=((*children_response)->child).begin();
								while(children_signals!=(*children_response)->child.end())
								{
									Sig *newsig=new Sig();
									newsig->name=(*children_signals)->GetAttribute("name");
									newsig->modality=(*children_signals)->GetAttribute("modality");
									newrule->signalVec.push_back(*newsig);
									children_signals++;
								}
							}
							else
							if((*children_response)->name=="cognitive")
								newrule->reactive_probability=atof((*children_response)->GetAttribute("probability").c_str());

							children_response++;
						}
					}


					children_rule++;
				}

				//newrule->PrintRule();
				mapBackchannelComplexRules[(*rule)->GetAttribute("name")]=(*newrule);
			}
		}

	delete xmltree;
	delete xmlparser;

	return 1;
}

void ListenerData::GenerateCombinationRules()
{
	std::map<std::string, BCComplexRules>::iterator iterRule1;
	std::map<std::string, BCComplexRules>::iterator iterRule2;
	

	//std::map<std::string, std::string>::iterator iterUser1;
	//std::map<std::string, std::string>::iterator iterUser2;
	std::vector<Sig>::iterator iterUser1;
	std::vector<Sig>::iterator iterUser2;
	
	std::vector<Sig>::iterator iterSig1;
	std::vector<Sig>::iterator iterSig2;

	BCComplexRules *newrule;

	for(iterRule1=mapBackchannelComplexRules.begin(); iterRule1!=mapBackchannelComplexRules.end(); iterRule1++)
	{
		for(iterRule2=mapBackchannelComplexRules.begin(); iterRule2!=mapBackchannelComplexRules.end(); iterRule2++)
		{
			//check if all the user signal that trigger these two rules are on different modalities
			for(iterUser1=(*iterRule1).second.usersignalsVec.begin(); iterUser1!=(*iterRule1).second.usersignalsVec.end(); iterUser1++)
			{
				for(iterUser2=(*iterRule2).second.usersignalsVec.begin(); iterUser2!=(*iterRule2).second.usersignalsVec.end(); iterUser2++)
					if((*iterUser1).modality==(*iterUser2).modality)// && ((*iterUser1).name==(*iterUser2).name))
						break;
				if(iterUser2!=(*iterRule2).second.usersignalsVec.end())
					break;
			}

			if(iterUser1>=(*iterRule1).second.usersignalsVec.end())
			{
				//I can combine only if they ask for signals on different modality or the same signal on the same modality
				for(iterSig1=(*iterRule1).second.signalVec.begin(); iterSig1!=(*iterRule1).second.signalVec.end(); iterSig1++)
				{
					for(iterSig2=(*iterRule2).second.signalVec.begin(); iterSig2!=(*iterRule2).second.signalVec.end(); iterSig2++)
						if(((*iterSig1).modality==(*iterSig2).modality) && ((*iterSig1).name!=(*iterSig2).name))
							break;
					if(iterSig2!=(*iterRule2).second.signalVec.end())
						break;
				}
				if(iterSig1>=(*iterRule1).second.signalVec.end())
				{	
					//Combination of two rules
					newrule=new BCComplexRules();

					newrule->response_priority=(*iterRule1).second.response_priority+(*iterRule2).second.response_priority;
					newrule->response_probability=((*iterRule1).second.response_probability+(*iterRule2).second.response_probability)/2;

					newrule->reactive_probability=((*iterRule1).second.reactive_probability+(*iterRule2).second.reactive_probability)/2;
					newrule->signals_probability=((*iterRule1).second.signals_probability+(*iterRule2).second.signals_probability)/2;

					for(iterSig1=(*iterRule1).second.signalVec.begin(); iterSig1!=(*iterRule1).second.signalVec.end(); iterSig1++)
						newrule->signalVec.push_back((*iterSig1));
					for(iterSig2=(*iterRule2).second.signalVec.begin(); iterSig2!=(*iterRule2).second.signalVec.end(); iterSig2++)
					{
						for(iterSig1=(*iterRule1).second.signalVec.begin(); iterSig1!=(*iterRule1).second.signalVec.end(); iterSig1++)
							if(((*iterSig2).modality==(*iterSig1).modality) && ((*iterSig2).name==(*iterSig1).name))
								break;
						if(iterSig1>=(*iterRule1).second.signalVec.end())
							newrule->signalVec.push_back((*iterSig2));
					}

					for(iterUser1=(*iterRule1).second.usersignalsVec.begin(); iterUser1!=(*iterRule1).second.usersignalsVec.end(); iterUser1++)
						newrule->usersignalsVec.push_back((*iterUser1));

					for(iterUser2=(*iterRule2).second.usersignalsVec.begin(); iterUser2!=(*iterRule2).second.usersignalsVec.end(); iterUser2++)
						newrule->usersignalsVec.push_back((*iterUser2));

					newrule->name=(*iterRule1).second.name + "_" + (*iterRule2).second.name;
					
					
					//Now... does a combination of these two rules already exist?!?
					//if not... add new rule... finally! :)

					if(isRulePresent(newrule)==false)
					{
						mapBackchannelComplexRules[newrule->name]=*newrule;
						//newrule->PrintRule();
					}
				}
			}
		}
	}
}

bool ListenerData::isRulePresent(BCComplexRules *rule)
{
	std::map<std::string, BCComplexRules>::iterator iterRule;
	std::vector<Sig>::iterator iterUser1;
	std::vector<Sig>::iterator iterUser2;

	bool found=false;

	for(iterRule=mapBackchannelComplexRules.begin(); iterRule!=mapBackchannelComplexRules.end(); iterRule++)
	{
		if((*iterRule).second.usersignalsVec.size()==rule->usersignalsVec.size())
		{
			int signals=0;
			for(iterUser1=(*iterRule).second.usersignalsVec.begin(); iterUser1!=(*iterRule).second.usersignalsVec.end(); iterUser1++)
			{
				for(iterUser2=rule->usersignalsVec.begin(); iterUser2!=rule->usersignalsVec.end(); iterUser2++)
				{
					if(((*iterUser2).modality==(*iterUser1).modality) && ((*iterUser2).name==(*iterUser1).name))
							signals+=1;
				}
			}

			if(signals==(int)rule->usersignalsVec.size())
			{
				found=true;
			}
		}
	}
	return found;
}
