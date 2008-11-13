#include "DataContainer.h"
#include "RandomGen.h"
#include "CentralClock.h"
#include "ReactiveBehaviourTrigger.h"
#include "EngineParameterSet.h"
#include "EngineParameter.h"
#include "Wm4Matrix3.h"
#include "Wm4Vector3.h"

#define PI 3.141592653589793
#define ZONE_AB_DEGREE 30
#define ZONE_BC_DEGREE 60
#define TILT_DEGREE 8
#define TEMPORAL_WINDOW_INTEREST_LEVEL 3000.0

#define ROTATION_ANGLE 0.15

#define WATSON 1

extern DataContainer *datacontainer;
extern RandomGen *ran;
extern CentralClock *pc;

ReactiveBehaviourTrigger::ReactiveBehaviourTrigger()
{
}

ReactiveBehaviourTrigger::ReactiveBehaviourTrigger(AgentState *agentstate)
{
	int i;
	//find data
	this->agentstate=agentstate;
	listenerdata=datacontainer->getListenerData();
	
	mimicry=new Mimicry();
	reactivebackchannel=new ReactiveBackchannel(agentstate);

	//EA=EventAnalyser();
	aiPersonMemory = new AIPersonMemory;

	//initialise head postion
	lastheadDeg=0;
	lasttiltDegree=0;

	nodtime=0;
	shaketime=0;
}


ReactiveBehaviourTrigger::~ReactiveBehaviourTrigger()
{
}

// Add message to the Message Buffer
int ReactiveBehaviourTrigger::addMessage(std::string mes)
{
	messageList.push_back(mes);
	return 1;
}

std::map<std::string,DataBackchannel*> ReactiveBehaviourTrigger::ReceiveMessages()
{
	InputData *newinputdata;
	DataBackchannel *bmlfml=NULL;
	double p;
	std::vector<std::string> usersignalsVec;
	
	messageMap.clear();
	messageMap["bml"]=NULL;
	messageMap["fml"]=NULL;

	while(!messageList.empty())
	{
		std::string content =(std::string) messageList.front();

		if(strcmp(content.c_str(),"")!=0)
		{
			std::string tempString;
			std::string c;
			std::string name="";
			c=std::string(content.c_str());
			std::vector<std::string> tokens;
			std::string::size_type pos=0, pos1=0, prev_pos=0; 
			tokens.clear();
			while ((pos=c.find_first_of(';',prev_pos))!=std::string::npos)
			{
				tempString = c.substr(prev_pos, pos-prev_pos);
				tempString = std::string(_strupr((char *)tempString.c_str()));
				//if (tempString.length() != 0) {
				tokens.push_back(tempString);
				//}
				prev_pos=pos+1;
			}
			pos=tokens[0].find_first_of(':',0);

			newinputdata=new InputData(tokens[0].substr(0,pos), tokens[0].substr(pos+1,tokens[0].length()-1));
			newinputdata->SetTime(pc->GetTime());
			for(int i=1;i<tokens.size();i++)
			{
				pos=tokens[i].find_first_of(':',0);
				newinputdata->AddParameter(tokens[i].substr(0,pos), tokens[i].substr(pos+1,tokens[i].length()-1));
			}

			CheckAdaptRules(newinputdata);

			//if(newinputdata->name=="POSES")
			//	HeadMovement(newinputdata);
			
			if(WATSON==1)
				newinputdata=AnalyseWatsonData(newinputdata);

			if(newinputdata!=NULL)
			{
				//NUOVA PROVA
				usersignalsVec.push_back(newinputdata->name);
				//printf("aggiungo %s\n", newinputdata->name.c_str());

				//FINE DELLA NUOVA PROVA

				/* CODICE ORIGINALE
				if(CheckBCRules(newinputdata)>0)
				{
					//newinputdata->PrintData();
					if(newinputdata->modality!="SPEECH")
					{
						bmlfml=NULL;
						bmlfml=mimicry->GenerateMimicryBehaviour(*newinputdata);
						if(bmlfml!=NULL)
							messageMap["bml"]=bmlfml;
						//else
						//	messageMap["bml"]=NULL;
					}
					
					if(agentstate->getAgentTurn()==0)  //send reactive behaviour only if listener
					{
						bmlfml=NULL;
						bmlfml=reactivebackchannel->GenerateBackchannelBehaviour(*newinputdata);

						if(bmlfml!=NULL)
							messageMap["fml"]=bmlfml;
						//else
						//	messageMap["fml"]=NULL;
					}
				}*/

				newinputdata->CleanData();
				delete(newinputdata);
			}
		}
		messageList.pop_front();
	}
	messageList.clear();

	//NUOVA PROVA
	return(CheckComplexRule(&usersignalsVec));
	

	//return(messageMap);
}

InputData *ReactiveBehaviourTrigger::AnalyseWatsonData(InputData *data)
{
	int time=(int)(((float)clock()/CLOCKS_PER_SEC)*1000);

	if(data->name=="POSES")
	{
		HeadMovement(data);
		if(data->name!="")
			return data;
		else
		{
			data->CleanData();
			delete(data);
			return NULL;
		}
	}
	else
	if(data->name=="head_nod")
	{
		if(time>=nodtime+2000)
		{
			nodtime=time;
			return data;
		}
		else
		{
			data->CleanData();
			delete(data);
			return NULL;
		}
	}
	else
	if(data->name=="head_shake")
	{
		if(time>=shaketime+2000)
		{
			shaketime=time;
			return data;
		}
		else
		{
			data->CleanData();
			delete(data);
			return NULL;
		}
	}
	else
		return data;
}

std::map<std::string,DataBackchannel*> ReactiveBehaviourTrigger::CheckComplexRule(std::vector<std::string> *usersigVec)
{
	DataBackchannel *bmlfml=NULL;
	BCComplexRules *rule=NULL;
	bmlfmlMap.clear();
	bmlfmlMap["bml"]=NULL;
	bmlfmlMap["fml"]=NULL;

	int priority_rule=0;

	std::map<std::string, BCComplexRules>::iterator rulesiter;
	for(rulesiter=listenerdata->mapBackchannelComplexRules.begin(); rulesiter!=listenerdata->mapBackchannelComplexRules.end();rulesiter++)
	{
		int find=0;
		std::vector<std::string>::iterator useriter;
		for(useriter=usersigVec->begin(); useriter!=usersigVec->end(); useriter++)
		{
			//std::map<std::string,std::string>::iterator iter;
			std::vector<Sig>::iterator iter;
			for(iter=(*rulesiter).second.usersignalsVec.begin();iter!=(*rulesiter).second.usersignalsVec.end();iter++)
				if((*iter).name==(*useriter))
					break;
//				iter = (*rulesiter).second.usersignalsVec.find((*useriter)); 
			if (iter != (*rulesiter).second.usersignalsVec.end())
				find+=1;
		}
		if(find!=0 && find==(int)((*rulesiter).second.usersignalsVec.size())) //the rule is satisfied
		{
			//combinare i segnali su diverse modalità per il mimicry??!
			
			if((*rulesiter).second.response_priority>=priority_rule)
			{
				rule=&(*rulesiter).second;
				priority_rule=(*rulesiter).second.response_priority;
			}
		}
	}

	if(rule!=NULL)
	{
		float prob=ran->GetRand01();
		if(prob<=rule->response_probability)
		{
			if(rule->signalVec.size()>0) //mimicry or specific signals
			{
				bmlfml=NULL;
				bmlfml=DataForBML(rule);
				if(bmlfml!=NULL)
				{
					bmlfml->zone=rule->name;
					bmlfmlMap["bml"]=bmlfml;
				}
			}

			if(rule->reactive_probability>0)
			{
				bmlfml=DataForFML(rule->response_priority);
				if(bmlfml!=NULL)
				{
					bmlfml->zone=rule->name;
					bmlfmlMap["fml"]=bmlfml;
				}
			}
 
			//printf("\n\nScatta questa regola:   %s\n\n", rule->name.c_str());
		}
	}
	return(bmlfmlMap);
}


void ReactiveBehaviourTrigger::CheckAdaptRules(InputData *data)
{
	std::map<std::string, AdaptRules>::iterator iterrule;
	std::vector<Implications>::iterator iterimp;
	std::vector<Operations>::iterator itercons;

	iterrule = listenerdata->mapAdaptRules.find(data->name);

	//check all implication in the rule
	for(iterimp=(*iterrule).second.vecImplication.begin();iterimp!=(*iterrule).second.vecImplication.end();iterimp++)
	{
		if((*iterimp).EvaluateImplication(data)==1)
		{
			for(itercons=(*iterimp).vecConsequence.begin();itercons!=(*iterimp).vecConsequence.end(); itercons++)
				(*itercons).ComputeOperation(data);
		}
	}
	
	for(itercons=(*iterrule).second.vecModify.begin();itercons!=(*iterrule).second.vecModify.end(); itercons++)
		(*itercons).ComputeOperation(data);
}

int ReactiveBehaviourTrigger::CheckBCRules(InputData *data)
{
	std::map<std::string, BCRules>::iterator iterrule;
	for(iterrule=listenerdata->mapBackchannelRules.begin();iterrule!=listenerdata->mapBackchannelRules.end();iterrule++)
	{
		if((*iterrule).first==data->name)
		{
			data->setPriority(atoi((*iterrule).second.mapParameters["priority"].c_str()));
			data->setProbability(atof((*iterrule).second.mapParameters["probability"].c_str()));
		}
	}

	if(data->name=="POSES" && data->zone=="")
		return (-1);
	else
	{
		float prob=ran->GetRand01();
		if(prob<=data->getProbability())
			return data->getPriority();
		else
			return (-1);
	}
}


std::string ReactiveBehaviourTrigger::HeadMovement(InputData *data)
{
	int i;
	float headDeg=0;
	float eyeDeg;
	float bodyDeg=0;
	float timeA, timeB;
	float userinterestlevel;
	float cosin;
	char uil[255];
	Wm4::Vector3f vec1(0,0,1);
	Wm4::Vector3f vec2(0,0,0);
	float angularspeed=0.0;
	std::map<std::string, std::string>::iterator iterpar;
	std::string zone="";
	float tiltDegree=0.0;

	if(data==NULL)
		return "";

	//find new head rotation in radiants
	iterpar=data->mapParameters.find("rotation_x");
	if(iterpar!=NULL)
		data->radiant_head_rotation[0]=atof((*iterpar).second.c_str());
	iterpar=data->mapParameters.find("rotation_y");
	if(iterpar!=NULL)
		data->radiant_head_rotation[1]=atof((*iterpar).second.c_str());
	iterpar=data->mapParameters.find("rotation_z");
	if(iterpar!=NULL)
		data->radiant_head_rotation[2]=atof((*iterpar).second.c_str());

	//find new head translation
	iterpar=data->mapParameters.find("traslation_x");
	if(iterpar!=NULL)
		data->mm_head_translation[0]=atof((*iterpar).second.c_str());
	iterpar=data->mapParameters.find("traslation_y");
	if(iterpar!=NULL)
		data->mm_head_translation[1]=atof((*iterpar).second.c_str());	
	iterpar=data->mapParameters.find("traslation_z");
	if(iterpar!=NULL)
		data->mm_head_translation[2]=atof((*iterpar).second.c_str());
	
	
	
	Wm4::Matrix3f matX(1, 0, 0, 
				0, cos(data->radiant_head_rotation[0]), sin(data->radiant_head_rotation[0]),
				0, -sin(data->radiant_head_rotation[0]), cos(data->radiant_head_rotation[0]));

	Wm4::Matrix3f matY(cos(data->radiant_head_rotation[1]), 0, -sin(data->radiant_head_rotation[1]), 
					0, 1, 0, 
					sin(data->radiant_head_rotation[1]), 0, cos(data->radiant_head_rotation[1]));

	Wm4::Matrix3f matZ(cos(data->radiant_head_rotation[2]), sin(data->radiant_head_rotation[2]), 0,
					-sin(data->radiant_head_rotation[2]), cos(data->radiant_head_rotation[2]), 0,
					0, 0, 1);

	vec2=vec1*matY*matZ*matX;		

	//cos(a)=(vxux + vyuy + vzuz)/|v||u|
	headDeg=(2*(acos(vec1.X()*vec2.X() + vec1.Y()*vec2.Y() + vec1.Z()*vec2.Z())))*180.0/PI;

	eyeDeg=headDeg;
	timeB=(float)data->time;

	/*INTEREST LEVEL*/
	aiPersonMemory->addRecord(bodyDeg, headDeg, eyeDeg, timeB);

	if(data->time-TEMPORAL_WINDOW_INTEREST_LEVEL<0)
		timeA=0.0;
	else
		timeA=(float)data->time-TEMPORAL_WINDOW_INTEREST_LEVEL;

	agentstate->setUserInterestLevel(aiPersonMemory->getInterestLevel(timeA,timeB));
	

	/*
	if(headDeg<20) printf("ZONE A\n");
	else if(headDeg>=20 && headDeg<50) printf("ZONE B\n");
	else if(headDeg>=50) printf("ZONE C\n");
	*/
	tiltDegree=abs(data->radiant_head_rotation[2])*180.0/PI;

	if(headDeg<ZONE_AB_DEGREE && lastheadDeg>=ZONE_AB_DEGREE)
		zone="BA";
	else
	if(headDeg>=ZONE_AB_DEGREE && lastheadDeg<ZONE_AB_DEGREE)
		zone="AB";
	else
	if(headDeg<ZONE_BC_DEGREE && lastheadDeg>=ZONE_BC_DEGREE)
		zone="CB";
	else
	if(headDeg>=ZONE_BC_DEGREE && lastheadDeg<ZONE_BC_DEGREE)
		zone="BC";
	else
	if(tiltDegree>TILT_DEGREE && lasttiltDegree<=TILT_DEGREE)
		zone="LR";  //lateral rotation
	else
	if(tiltDegree<=TILT_DEGREE && lasttiltDegree>TILT_DEGREE)
		zone="LRB";  //lateral rotation back
	/*else
	if(headDeg>=ZONE_BC_DEGREE && lastheadDeg>=ZONE_BC_DEGREE)
		zone="C";
	else
	if(headDeg<ZONE_AB_DEGREE && lastheadDeg<ZONE_AB_DEGREE)
		zone="A";
	else
	if(headDeg>=ZONE_AB_DEGREE && lastheadDeg>=ZONE_AB_DEGREE && lastheadDeg<ZONE_BC_DEGREE)
		zone="B";
	*/
	data->zone=zone;
	
	std::string headdirection="head";

	if(zone!="")
	{
		if(data->zone=="BA" || data->zone=="CB")
			headdirection="head_infront";
		else
		{
			if(data->radiant_head_rotation[0]>ROTATION_ANGLE)
				headdirection+="_down";
			if(data->radiant_head_rotation[0]<-ROTATION_ANGLE)
				headdirection+="_up";

			if(data->radiant_head_rotation[1]<-ROTATION_ANGLE)
				headdirection+="_left";
			if(data->radiant_head_rotation[1]>ROTATION_ANGLE)
				headdirection+="_right";

			if(data->radiant_head_rotation[2]<-ROTATION_ANGLE)
				headdirection+="_aside_l";
			if(data->radiant_head_rotation[2]>ROTATION_ANGLE)
				headdirection+="_aside_r";
		}

		if(headdirection!="head")
		{
			data->name=headdirection;
		}
		else
			data->name="";
	}
	else
		data->name="";

	//to compute angular speed I need to keep track of this data:
	lastheadDeg=headDeg;
	lasttiltDegree=tiltDegree;
	
	return(zone);
}

DataBackchannel *ReactiveBehaviourTrigger::DataForBML(BCComplexRules *rule)
{
	//Create BML for mimicry behaviour
	DataBackchannel *db;

	char s[255];

	std::string end="1.2";
	
	db=new DataBackchannel("MIMICRY");

	std::vector<Sig>::iterator iterSig;
	for(iterSig=rule->signalVec.begin();iterSig!=rule->signalVec.end();iterSig++)
		db->referencesMap[(*iterSig).modality]=(*iterSig).name;
	
	db->time=pc->GetTime();
	db->priority=rule->response_priority;
	db->end=end;

	return(db);
}

void ReactiveBehaviourTrigger::GetCommunicativeFunctions(std::vector<std::string> *vecResultCommFun)
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
	std::vector<CommFunction> vecCommFun;
	std::vector<CommFunction>::iterator iter;
	MMSystemSpace::EngineParameterSet* listenerfunctions;
	std::map<std::string,EngineParameter>::iterator itermp;

	listenerfunctions=agentstate->internalstate.GetEngineParameterSet("listener_fuction");

	for(itermp=listenerfunctions->mp.begin(); itermp!=listenerfunctions->mp.end(); itermp++)
	{
		loc = (*itermp).first.find("value",0);
		if((*itermp).second.GetValue()>0 && loc!=std::string::npos)
		{
			CommFunction cf;
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

DataBackchannel *ReactiveBehaviourTrigger::DataForFML(int priority)
{
	DataBackchannel *db;
	std::vector<std::string> vecCommFun;
	std::vector<std::string>::iterator itermp;
	std::string::size_type loc;
	std::string fml;
	int i=0;
	char s[255];
	int time=pc->GetTime();

	db=new DataBackchannel("REACTIVE");
	db->time=time;
	db->priority=priority;
	
	GetCommunicativeFunctions(&vecCommFun);

	db->communicativefunction=vecCommFun;
	db->emotionalstate=agentstate->getEmotionState();

	return(db);
}