#include "GretaReactiveBehaviour.h"
#include "DataContainer.h"
#include "RandomGen.h"
#include "ReactiveBehaviourTrigger.h"
#include "Wm4Matrix3.h"
#include "Wm4Vector3.h"

#define PI 3.141592653589793
#define MINIMUM_ANGULAR_SPEED 0.005
#define TEMPORAL_WINDOW_INTEREST_LEVEL 3000.0

extern RandomGen *randomgen;

ReactiveBehaviourTrigger::ReactiveBehaviourTrigger()//:JThread()
{
}

ReactiveBehaviourTrigger::ReactiveBehaviourTrigger(GretaReactiveBehaviour *grb)//:JThread()
{
	int i;

	this->grb=grb;

	//EA=EventAnalyser();
	aiPersonMemory = new AIPersonMemory;

	//initialise head postion
	for(i=0;i<3;i++)
	{
		lastheadDeg=0;
		lastangularspeed=0.0;
	}
	lastheadrotationtime=grb->getpCentralClock()->GetTime();
}


ReactiveBehaviourTrigger::~ReactiveBehaviourTrigger()
{
}

//void ReactiveBehaviourTrigger::setActiveThread() { activethread=1; }
//void ReactiveBehaviourTrigger::setContent(JString cont) { content=cont; }

// Add message to the Message Buffer
int ReactiveBehaviourTrigger::addMessage(std::string mes)
{
	messageList.push_back(mes);
	return 1;
}

/*
void ReactiveBehaviourTrigger::run()
{
	Sleep(5);
}*/

void ReactiveBehaviourTrigger::messages()
{
	InputData *newinputdata;
	double p;

	//while(true)
	//{
		while(!messageList.empty())
		//if(!messageList.empty())
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
				newinputdata->SetTime(grb->getpCentralClock()->GetTime());
				newinputdata->SetExpirationTime(grb->getpCentralClock()->GetTime()+4000);
				for(int i=1;i<tokens.size();i++)
				{
					pos=tokens[i].find_first_of(':',0);
					newinputdata->AddParameter(tokens[i].substr(0,pos), tokens[i].substr(pos+1,tokens[i].length()-1));
				}

				CheckAdaptRules(newinputdata);

				std::string ok="";
				
				if(newinputdata->name=="POSES")
					ok=HeadMovement(newinputdata);
				else
					ok=CheckBCRules(newinputdata);

				if(newinputdata->name=="NOD" || newinputdata->name=="SHAKE" || (newinputdata->name=="POSES" && newinputdata->zone!=""))
				{
					grb->getMimicrySender()->addInputData(*newinputdata);
				}

				/*if(randomgen==0)
					randomgen = new RandomGen();
				p = randomgen->GetRand01();
				
				if(newinputdata->bcprobability>p)
				{
					printf("richiedo un bc, generato da: %s  zone: %s  prob:%.2f\n", newinputdata->name.c_str(), newinputdata->zone.c_str(), newinputdata->bcprobability);
					if(grb->agentstate.getAgentTurn()==0) //send backchannel only if the agent is listener
						grb->getReactiveBackchannelSender()->execute(); //ping for backchannel
					
					// if head movement then send also ping for mimicry
					grb->getMimicrySender()->addInputData(*newinputdata);
					//setInputData(*newinputdata); 
					//grb->getMimicrySender().execute(); 
				}*/

				newinputdata->CleanData();
				delete(newinputdata);
			}
			messageList.pop_front();
		}
		grb->getMimicrySender()->running();
		
		// Don't overload processor
	//	Sleep(5);
	//}
}


void ReactiveBehaviourTrigger::CheckAdaptRules(InputData *data)
{
	std::map<std::string, AdaptRules>::iterator iterrule;
	std::vector<Implication>::iterator iterimp;
	std::vector<Operation>::iterator itercons;

	iterrule = grb->listenerdata->mapAdaptRules.find(data->name);

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

std::string ReactiveBehaviourTrigger::CheckBCRules(InputData *data)
{
	std::map<std::string, BCRules>::iterator iterrule;
	for(iterrule=grb->listenerdata->mapBackchannelRules.begin();iterrule!=grb->listenerdata->mapBackchannelRules.end();iterrule++)
	{
		if((*iterrule).first==data->name)
		{
			data->bcprobability=(*iterrule).second.mapParameters["probability"];
			data->bcpriority=(*iterrule).second.mapParameters["priority"];
			//ComputeBCprobability(data->name);
		}
	}

	if(data->bcprobability==1)
		return("OK");
	else
		return("");
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
	Wm4::Vector3f vec1(0,0,1);
	Wm4::Vector3f vec2(0,0,0);
	float angularspeed=0.0;
	std::map<std::string, std::string>::iterator iterpar;
	std::string zone="";

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

	/*INTEREST LEVEL
	aiPersonMemory->addRecord(bodyDeg, headDeg, eyeDeg, timeB);

	if(data->time-TEMPORAL_WINDOW_INTEREST_LEVEL<0)
		timeA=0.0;
	else
		timeA=(float)data->time-TEMPORAL_WINDOW_INTEREST_LEVEL;

	grb->getpAgentState()->setUserInterestLevel(aiPersonMemory->getInterestLevel(timeA,timeB));
	//printf("int lev: %.3f\n", grb->getpAgentState()->getUserInterestLevel());
	*/

	/*
	if(headDeg<20) printf("ZONE A\n");
	else if(headDeg>=20 && headDeg<50) printf("ZONE B\n");
	else if(headDeg>=50) printf("ZONE C\n");
	*/

	//compute angular speed for rotation around x, y and z
	angularspeed=fabs(headDeg-lastheadDeg)/(float)(data->time-lastheadrotationtime);

	if(headDeg<20 && lastheadDeg>=20)
		zone="BA";
	else
	if(headDeg>=20 && lastheadDeg<20)
		zone="AB";
	else
	if(headDeg<50 && lastheadDeg>=50)
		zone="CB";
	else
	if(headDeg>=50 && lastheadDeg<50)
		zone="BC";
	/*else
	if(headDeg>=50 && lastheadDeg>=50)
		zone="C";
	else
	if(headDeg<20 && lastheadDeg<20)
		zone="A";
	else
	if(headDeg>=20 && lastheadDeg>=20 && lastheadDeg<50)
		zone="B";
	*/
	//data->bcprobability=ComputeBCprobability(zone);
	data->zone=zone;
	
	
	//to compute angular speed I need to keep track of this data:
	lastheadDeg=headDeg;
	lastangularspeed=angularspeed;	
	lastheadrotationtime=data->time;
	
	return(zone);
}

float ReactiveBehaviourTrigger::ComputeBCprobability(std::string event)
{
	//time thresholds depend on the speaker's interest level t1[1,3]  t2[3,5]
	int t1, t2;
	int lbct=grb->getLastBCTime();

	float uil=grb->getpAgentState()->getUserInterestLevel();

	t1=(int)(3000-2000*uil);
	t2=(int)(5000-2000*uil);

	//from zone to zone
	if(event=="BA")
	{
		if(lbct<=t1) return(0.5);
		if(lbct>t1 && lbct<=t2) return(0.7);
		if(lbct>t2) return(1.0);
	}
	if(event=="AB")
	{
		if(lbct<=t1) return(0.2);
		if(lbct>t1 && lbct<=t2) return(0.5);
		if(lbct>t2) return(0.7);
	}
	if(event=="BC")
	{
		if(lbct<=t1) return(0.1);
		if(lbct>t1 && lbct<=t2) return(0.3);
		if(lbct>t2) return(0.5);
	}
	if(event=="CB")
	{
		if(lbct<=t1) return(0.4);
		if(lbct>t1 && lbct<=t2) return(0.6);
		if(lbct>t2) return(0.9);
	}

	if(event=="")
		return(0.0);

	//staying in a zone
/*	if(event=="A")
	{
		if(lbct<=t1) return(0.5);
		if(lbct>t1 && lbct<=t2) return(0.7);
		if(lbct>t2) return(1.0);
	}
	if(event=="B")
	{
		if(lbct<=t1) return(0.3);
		if(lbct>t1 && lbct<=t2) return(0.5);
		if(lbct>t2) return(0.7);
	}
	if(event=="C")
	{
		if(lbct<=t1) return(0.1);
		if(lbct>t1 && lbct<=t2) return(0.3);
		if(lbct>t2) return(0.5);
	}
*/

	//la probabilità di un back channel dipende da: 
	//	- livello d'attenzione dell'agente
	//	- tempo trascorso dall'ultimo backchannel
	//	- tipo dell'ultimo backchannel visualizzato (due bc uguali hanno meno prob di ripetersi)
	//	- direzione dello sguardo dell'agente (se ha notato il comportamento dello speaker)

	// for all the other event (nod, shake, pitch variation, pauses)
	if(lbct<=t1) return(0.5);
	if(lbct>t1 && lbct<=t2) return(0.7);
	if(lbct>t2) return(1.0);
	
	return 0.0;
}