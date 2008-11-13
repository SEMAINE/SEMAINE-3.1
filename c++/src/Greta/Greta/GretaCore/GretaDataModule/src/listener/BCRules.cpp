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

// BCRules.cpp: implementation of the BCRules class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <math.h>
#include "BCRules.h"


BCRules::BCRules(){}

BCRules::~BCRules(){}

void BCRules::PrintRule()
{
	std::map<std::string, std::string>::iterator iter;

	printf("%s\n", this->name.c_str());	

	for(iter=mapParameters.begin();iter!=mapParameters.end();iter++)
		printf("%s  %s\n", (*iter).first.c_str(), (*iter).second.c_str());
}

BCComplexRules::BCComplexRules()
{
	reactive_probability=0.0;
	signals_probability=0.0;
}

BCComplexRules::~BCComplexRules(){}

void BCComplexRules::PrintRule()
{
	FILE *fid;

	fid=fopen("NewRules.txt","w");
	fprintf(fid, "RULE NAME: %s\n", this->name.c_str());

	//std::map<std::string, std::string>::iterator useriter;
	std::vector<Sig>::iterator useriter;
	fprintf(fid, "USER SIGNALS:\n");
	for(useriter=usersignalsVec.begin(); useriter!=usersignalsVec.end(); useriter++)
		fprintf(fid, "\tSIGNAL: %s    %s\n", (*useriter).modality.c_str(), (*useriter).name.c_str());
	fprintf(fid, "RESPONSE: probability: %.2f    priority: %d\n", this->response_probability, this->response_priority);
	if(this->signalVec.size()>0)
	{
		std::vector<Sig>::iterator signaliter;
		fprintf(fid, "SIGNALS    prob: %.2f\n", this->signals_probability);
		for(signaliter=signalVec.begin(); signaliter!=signalVec.end(); signaliter++)
			fprintf(fid, "\tSIGNAL: %s   %s\n", (*signaliter).name.c_str(), (*signaliter).modality.c_str());
	}

	fprintf(fid, "REACTIVE probability: %.2f\n\n\n", this->reactive_probability);
	fclose(fid);
}
