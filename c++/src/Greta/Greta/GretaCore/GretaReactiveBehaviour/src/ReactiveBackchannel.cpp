#include "ReactiveBackchannel.h"
#include "EngineParameterSet.h"
#include "EngineParameter.h"
#include "RandomGen.h"

#include <map>
#include <string>

extern CentralClock *pc;
extern RandomGen *ran;

ReactiveBackchannel::ReactiveBackchannel()
{
}

ReactiveBackchannel::ReactiveBackchannel(AgentState *agentstate)
{
	this->agentstate=agentstate;
	nodtime=0;
	shaketime=0;
}

ReactiveBackchannel::~ReactiveBackchannel()
{
}

void ReactiveBackchannel::GetCommunicativeFunctions(std::vector<std::string> *vecResultCommFun)
{
	float incr=0;
	int numcommfun=1;
	int ispresent=0;
	int j, k, i=0;
	int dim=0;
	float index=0.0;
	float sum=0;
	float randomvalue;
	float total=0.0;
	std::string::size_type loc;
	std::vector<CommFun> vecCommFun;
	std::vector<CommFun>::iterator iter;
	MMSystemSpace::EngineParameterSet* listenerfunctions;
	std::map<std::string,EngineParameter>::iterator itermp;

	listenerfunctions=agentstate->internalstate.GetEngineParameterSet("listener_fuction");

	for(itermp=listenerfunctions->mp.begin(); itermp!=listenerfunctions->mp.end(); itermp++)
	{
		loc = (*itermp).first.find("value",0);
		if((*itermp).second.GetValue()>0 && loc!=std::string::npos)
		{
			CommFun cf;
			cf.name=(*itermp).first.substr(0,loc-1);
			cf.importance=(*itermp).second.GetValue();
			total+=cf.importance;
			vecCommFun.push_back(cf);
		}
	}

	dim=(int)vecCommFun.size();
	randomvalue=ran->GetRand01();
	if(randomvalue==1) 
		numcommfun=dim;
	else
		numcommfun=(int)((float)dim*randomvalue)+1;

	for(i=0; i<numcommfun; i++)
	{
		k=0;
		index=0;
		randomvalue=total*ran->GetRand01();
		while(index<=total)
		{
			incr=vecCommFun[k].importance;
			if(randomvalue>=index && randomvalue<=index+incr)
				break;
            k+=1;
			index+=incr;
		}

		total-=vecCommFun[k].importance;
		vecResultCommFun->push_back(vecCommFun[k].name);
		vecCommFun[k].name=vecCommFun[dim-1].name;
		vecCommFun[k].importance=vecCommFun[dim-1].importance;
		vecCommFun.pop_back();
		dim-=1;
	}
}

DataBackchannel *ReactiveBackchannel::GenerateBackchannelBehaviour(InputData data)
{
	DataBackchannel *db=NULL;

	std::string fml="";
	int time;

	time=pc->GetTime();

/*	if(data.name=="head_nod")
	{
		if(time>=nodtime+3000)
		{
			db=selectCommunicativeIntentions(time, data.priority);
			db->zone="head_nod";
			nodtime=time;
		}
	}
	else
	if(data.name=="head_shake")
	{
		if(time>=shaketime+3000)
		{
			db=selectCommunicativeIntentions(time, data.priority);
			db->zone="head_shake";
			shaketime=time;
		}
	}
	else*/
		if(data.name=="POSES" && (data.zone=="BA" || data.zone=="CB" || data.zone=="AB" || data.zone=="LR" || data.zone=="LRB"))
		{
			db=selectCommunicativeIntentions(time, data.priority);
			if(data.zone=="BA")
				db->zone="from zone B to A";
			else
			if(data.zone=="CB")
				db->zone="from zone C to B";
			else
			if(data.zone=="AB")
				db->zone="from zone A to B";
			else
			if(data.zone=="LR")
				db->zone="tilt";
			else
			if(data.zone=="LRB")
				db->zone="tilt back";
		}
		else
		{
			db=selectCommunicativeIntentions(time, data.priority);
			db->zone=data.name;
		}

	return(db);
}

DataBackchannel *ReactiveBackchannel::selectCommunicativeIntentions(int time, int priority)
{
	DataBackchannel *db;
	std::vector<std::string> vecCommFun;
	std::vector<std::string>::iterator itermp;
	std::string::size_type loc;
	std::string fml;
	int i=0;
	char s[255];

	db=new DataBackchannel("REACTIVE");
	db->time=time;
	db->priority=priority;
	
	GetCommunicativeFunctions(&vecCommFun);

	db->communicativefunction=vecCommFun;
	db->emotionalstate=agentstate->getEmotionState();

	return(db);
}