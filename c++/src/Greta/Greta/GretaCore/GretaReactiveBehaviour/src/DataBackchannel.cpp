#include "DataBackchannel.h"

DataBackchannel::DataBackchannel()
{
}

// constructor to use
DataBackchannel::DataBackchannel(std::string type) 
{
	this->type=type; 
	this->priority=1000;
	this->zone="";
}

// destructor
DataBackchannel::~DataBackchannel()
{
}

void DataBackchannel::CopyDataBackchannel(DataBackchannel *db)
{
	std::vector<std::string>::iterator iter;
	std::map<std::string, std::string>::iterator itermap;
	time=db->time;
	priority=db->priority;

	modality=db->modality;
	reference=db->reference;
	for(itermap=db->referencesMap.begin();itermap!=db->referencesMap.end();itermap++)
		referencesMap[(*itermap).first]=(*itermap).second;

	end=db->end;
	zone=db->zone;

	emotionalstate=db->emotionalstate;
	for(iter=db->communicativefunction.begin();iter!=db->communicativefunction.end();iter++)
		communicativefunction.push_back((*iter));
}

void DataBackchannel::CleanDataBackchannel()
{
	time=0;
	priority=1000;

	modality="";
	reference="";
	end="";
	zone="";

	emotionalstate="";
	communicativefunction.clear();
	referencesMap.clear();
}

void DataBackchannel::Print()
{
	std::vector<std::string>::iterator iter;
	printf("%s at %d with priority %d\n", type.c_str(), time, priority);

	if(type=="MIMICRY")
		printf("Modality: %s    Ref: %s   End: %s\n", modality.c_str(), reference.c_str(), end.c_str());
	else
	{
		printf("Emo: %s\nCommunicative intention(s): ", emotionalstate.c_str());
		for(iter=communicativefunction.begin();iter!=communicativefunction.end();iter++)
			printf("%s  ", (*iter).c_str());
		printf("\n");
	}
}