// ReactiveListenerPsydule.cpp: implementation of the ReactiveListenerPsydule class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <math.h>
#include "IniManager.h"
#include "XMLGenericParser.h"
#include "GretaReactiveBehaviour.h"

extern IniManager inimanager;
extern RandomGen *randomgen;

EventAnalyser::EventAnalyser(GretaReactiveBehaviour *grb)
{
	int i;
	this->grb=grb;

	grb->listenerdata->mapBackchannelRules
	//this->LoadRulesList("listener/rules.xml");

	for(i=0;i<3;i++)
		last_head_rotation[i]=0;
	
	time_head_still=0;

	last_rotation_time=-1;
}

EventAnalyser::~EventAnalyser()
{

}

int EventAnalyser::LoadRulesList(std::string filepath)
{
	if(filepath=="")
	{
		printf("ReactiveListenerPsydule: no filename given\n");
		//fprintf(logfile,"ReactiveListenerPsydule: no filename given\n");
		return 0;
	}
	XMLGenericParser *xmlparser;
	xmlparser=new XMLGenericParser();
	XMLGenericTree *xmltree;
	xmltree=xmlparser->ParseFile(filepath);
	if(xmltree==0)
	{
		printf("ReactiveListenerPsydule: error parsing %s\n",filepath.c_str());
		//fprintf(logfile,"ReactiveListenerPsydule: error parsing %s\n",filepath.c_str());
		return 0;
	}
	
	std::list<XMLGenericTree*>::iterator rule;
		
	if(!(xmltree->child).empty())
		for(rule=(xmltree->child).begin();rule!=(xmltree->child).end();rule++)
		{
			if((*rule)->name=="rule")
			{
				Rules *newrule;
				newrule=new Rules();

				newrule->name=(*rule)->GetAttribute("name");

				std::list<XMLGenericTree*>::iterator children;
				children=((*rule)->child).begin();

				while(children!=(*rule)->child.end())
				{
					if((*children)->name=="parameter")
						newrule->mapParameters[(*children)->GetAttribute("name")]=atof((*children)->GetAttribute("value").c_str());				
					children++;
				}

				newrule->PrintRule();
				mapBackchannelRules[(*rule)->GetAttribute("name")]=(*newrule);
			}
		}

	delete xmltree;
	delete xmlparser;

	return 1;
}


InputData* EventAnalyser::RulesAnalysis(InputData *data)
{
	//analizzare l'evento in base ad un file di regole
	
	/* DEBUG
	std::map<std::string, std::string>::iterator iterpar;
	printf("%s %s\n",data->name.c_str(), (char*)data->time.printTimeMS());

	for(iterpar=data->mapParameters.begin();iterpar!=data->mapParameters.end();iterpar++)
		printf("	%s: %s\n",(*iterpar).first.c_str(), (*iterpar).second.c_str());
	printf("\n");*/



	std::map<std::string, Rules>::iterator iterrule;
	for(iterrule=mapBackchannelRules.begin();iterrule!=mapBackchannelRules.end();iterrule++)
	{
		if((*iterrule).first==data->name)
		{
			data->bcprobability=(*iterrule).second.mapParameters["probability"];
			data->bcpriority=(*iterrule).second.mapParameters["priority"];
			ComputeBackchannelProbability(data);
			return(data);
		}
	}

	
	

	/*

	if(data->name=="NODS")
	{
		std::map<std::string, std::string>::iterator iterpar;
		iterpar=data->mapParameters.find("STATE");
		if((*iterpar).second=="1")
			data->value="nod";
		else
			if((*iterpar).second=="-1")
				data->value="shake";
		data->bcprobability=1;
		ComputeBackchannelProbability(data);
		return(data);
	}
	
	if(data->name=="POSES")
	{
		if(last_rotation_time==-1)
			if(AnalyseHeadPosition(data)==1)
			{
				data->bcprobability=1;
				ComputeBackchannelProbability(data);
				return(data);
			}
		else
			last_rotation_time=data->executiontime;
	}*/
	return(NULL);
}

int EventAnalyser::AnalyseHeadPosition(InputData *data)
{
	int i;
	bool headstill=0;
	std::map<std::string, std::string>::iterator iterpar;

	//find new head position
	iterpar=data->mapParameters.find("RX");
	if(iterpar!=NULL)
		new_head_rotation[0]=atof((*iterpar).second.c_str());
	iterpar=data->mapParameters.find("RY");
	if(iterpar!=NULL)
		new_head_rotation[1]=atof((*iterpar).second.c_str());	
	iterpar=data->mapParameters.find("RZ");
	if(iterpar!=NULL)
		new_head_rotation[2]=atof((*iterpar).second.c_str());
		
	//check if head is moving
	if(abs(new_head_rotation[0]-last_head_rotation[0])<0.01 &&
		abs(new_head_rotation[1]-last_head_rotation[1])<0.01 &&
		abs(new_head_rotation[2]-last_head_rotation[2])<0.01)
	{
		headstill=1;
		time_head_still+=data->executiontime-last_rotation_time;
	}
	else
	{
		headstill=0;
		time_head_still=0;
	}

	last_rotation_time=data->executiontime;
	for(i=0;i<3;i++)
		last_head_rotation[i]=new_head_rotation[i];

	if(time_head_still>500)
	{
		//la testa è ferma, calcolarne qui la direzione?!
		//tra -pi/4 e 1/4*pi UP
		//tra 1/4*p e 3/4*pi UP_LEFT
		//tra 3/4*pi e 5/4*pi LEFT
		//tra 5/4*pi e 7/4*pi DOWN_LEFT
		//tra 7/4*pi e 9/4*pi DOWN
		//tra 9/4*p e 11/4*pi DOWN_RIGHT
		//tra 11/4*pi e 13/4*pi RIGHT
		//tra 13/4*pi e 15/4*pi UP_RIGHT
		time_head_still=0;
		return(1);
	}
	return(0);
}


float EventAnalyser::ComputeBackchannelProbability(InputData *data)
{
	//la probabilità di un back channel dipende da: 
	//	- livello d'attenzione dell'agente
	//	- tempo trascorso dall'ultimo backchannel
	//	- tipo dell'ultimo backchannel visualizzato (due bc uguali hanno meno prob di ripetersi)
	//	- direzione dello sguardo dell'agente (se ha notato il comportamento dello speaker)

	return 1;
}