//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// AdaptRules.cpp: implementation of the AdaptRules class.
//
//////////////////////////////////////////////////////////////////////

#include "AdaptRules.h"


Operations::Operations(void){}
Operations::~Operations(void){}

void Operations::ComputeOperation(InputData *data)
{
	std::string second;
	std::map<std::string, std::string>::iterator iterdata;
	iterdata=data->mapParameters.find(parameter);
	if(iterdata!=data->mapParameters.end() )
	{
		second=(*iterdata).second;
		data->mapParameters.erase(iterdata);
		data->mapParameters[value]=second;
	}
	else
		if(parameter=="NAME")
			data->name=value;
	data->triggerBC=1;
}

Implications::Implications(void){}
Implications::~Implications(void){}

bool Implications::EvaluateImplication(InputData *data)
{
	bool result=1;
	std::vector<Operations>::iterator itercond;
	
	for(itercond=vecCondiction.begin();itercond!=vecCondiction.end(); itercond++)
	{
		std::map<std::string, std::string>::iterator iterpar;
		iterpar=data->mapParameters.find((*itercond).parameter);
		result = result && ((*iterpar).second==(*itercond).value);
	}
	return (result);
}

AdaptRules::AdaptRules(){}
AdaptRules::~AdaptRules(){}

void AdaptRules::PrintRule()
{
	std::vector<Implications>::iterator iterimp; vecImplication;
	std::vector<Operations>::iterator itermod; vecModify;

	printf("NAME: %s\n", this->name.c_str());	

	printf("MODIFICATION:\n");
	for(itermod=vecModify.begin();itermod!=vecModify.end();itermod++)
		printf("%s  %s  %s\n", (*itermod).parameter.c_str(), (*itermod).op.c_str(), (*itermod).value.c_str());

	for(iterimp=vecImplication.begin();iterimp!=vecImplication.end();iterimp++)
	{
		printf("CONDITION:\n");
		std::vector<Operations>::iterator itercond;
		for(itercond=(*iterimp).vecCondiction.begin();itercond!=(*iterimp).vecCondiction.end();itercond++)
			printf("%s  %s  %s\n", (*itercond).parameter.c_str(), (*itercond).op.c_str(), (*itercond).value.c_str());

		printf("CONSEQUENCE:\n");
		for(itercond=(*iterimp).vecConsequence.begin();itercond!=(*iterimp).vecConsequence.end();itercond++)
			printf("%s  %s  %s\n", (*itercond).parameter.c_str(), (*itercond).op.c_str(), (*itercond).value.c_str());
	}
}