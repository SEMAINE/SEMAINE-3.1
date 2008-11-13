#include "ReactiveBehaviourPsydule.h"
#include "DataContainer.h"
#include "IniManager.h"
#include "Timer.h"

extern DataContainer *datacontainer;
extern IniManager inimanager;
extern CentralClock *pc;

ReactiveBehaviourPsydule::ReactiveBehaviourPsydule()
{	
	initPsycloneConnection();

	listenerdata=datacontainer->getListenerData();

	agentstate=new AgentState(listenerdata->agentstate);
	behaviourselector=new BehaviourSelector(this, pc, agentstate);
	behaviourselector->mimicryLevel=agentstate->getValue("mimicry_level.mimicry.value");
	behaviourselector->start();
	reactivebehaviourtrigger=new ReactiveBehaviourTrigger(agentstate);
	
	while(true)
	{
		ReadInputMessages();
		ReadMessage(psydule->ReceiveMessage(100));
	}
}

ReactiveBehaviourPsydule::~ReactiveBehaviourPsydule()
{
}


// Load the application parameter from the inimanager
int ReactiveBehaviourPsydule::initPsycloneConnection()
{	
	// Register Psyclone module adress
	psyduleHost=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	// Register Psyclone module port
	psydulePort=inimanager.GetValueInt("PSYCLONE_PORT");
	// Get Greta's Name
	GretaName=(JString)inimanager.GetValueString("GRETA_NAME").c_str();

	// Creation of the Psyclone inteface
	psydule=new Psydule(GretaName+INPUTGRETA_MODULE_NAME,psyduleHost,psydulePort);
	

	// registered packet list
	std::list<JString> registeredGretaDatatypes;
	// register to Greta Clock messages.
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER);
	// register to Caracterisation Baseline Message.
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_AGENTSTATE_MESSAGE_HEADER);
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_UPDATEAGENTSTATE_MESSAGE_HEADER);
	// register to System Feedback
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_SYSTEM_FEEDBACK_MESSAGE_HEADER);
	// register to Cognitive Backchannel
	registeredGretaDatatypes.push_back(GretaName+PSYCLONE_COGNITIVE_FML_MESSAGE_HEADER);

	
	// registration GretaWiteboard
	if(psydule->Register(GretaName+PSYCLONE_GRETA_WHITEBOARD,registeredGretaDatatypes)!=0)
	{
		std::cout << GretaName+INPUTGRETA_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_OK << std::endl;		
	}
	else
	{
		std::cout << GretaName+INPUTGRETA_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_FAILED<< std::endl;
		psydule=0;
		return 0;
	}
	std::cout << "Registration succesful" << std::endl;

	// Creation of the Psyclone input module if it does not have already been created (by another instanciation of ReactiveBehaviour)
	psyduleInput=new Psydule(GretaName+INPUTWATSON_MODULE_NAME,psyduleHost,psydulePort);

		std::list<JString> registeredInputDatatypes;
		// register to Input messages, Watson
		registeredInputDatatypes.push_back(PSYCLONE_INPUT_MESSAGE_HEADER);

		// registration InputWiteboard
		if(psyduleInput->Register(PSYCLONE_INPUT_WHITEBOARD,registeredInputDatatypes)!=0)
		{
			std::cout << GretaName+INPUTWATSON_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_OK << std::endl;		
		}
		else
		{
			std::cout << GretaName+INPUTWATSON_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_FAILED<< std::endl;
			
			return 0;
		}
		std::cout << "Registration succesful" << std::endl;
	
	return 1;
}

int ReactiveBehaviourPsydule::ReadMessage(Message *msg)
{
	JString content;
	int time;
	if(msg==NULL) return(0);

	content=msg->getContent();
	if(strcmp(content.c_str(),"")!=0)
	{
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
		{
			time=atoi(msg->getContent().c_str());
			pc->SetTime(time);
		}
		else
		{
			printf("message : %s /n",content.c_str());
			//if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_COGNITIVE_FML_MESSAGE_HEADER)==0)
		//	behaviourselector->updateBackchannelRequest((std::string)content);
		//else
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_SYSTEM_FEEDBACK_MESSAGE_HEADER)==0)
		{
			;//da modificare rispettando la forma usata per inviare feedback (indica se un backchannel è stato eseguito)
		}
		else
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_AGENTSTATE_MESSAGE_HEADER)==0)
		{
			agentstate->internalstate.LoadFromBuffer((std::string)content);
			behaviourselector->mimicryLevel=agentstate->getValue("mimicry_level.mimicry.value");
		}
		else
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_UPDATEAGENTSTATE_MESSAGE_HEADER)==0)
		{
			std::string app=(std::string)content;
			if(app.substr(0,app.find_first_of("="))=="emotion_state.emotion.name")
			{
					agentstate->setEmotionState(app.substr(app.find_first_of("=")+1));
			}
			else
			if(app.substr(0,app.find_first_of("="))=="mimicry_level.mimicry.value")
			{
				agentstate->internalstate.SetAttrValue("mimicry_level.mimicry.value", app.substr(app.find_first_of("=")+1));
				behaviourselector->mimicryLevel=agentstate->getValue("mimicry_level.mimicry.value");
//				behaviourselector->setMimicryPercentage(agentstate->getValue("mimicry_level.mimicry.value"));
			}
			if(app.substr(0,app.find_first_of("="))=="agent_identity.agent.name")
			{
				agentstate->setAgentName(app.substr(app.find_first_of("=")+1));
			}
			else
				agentstate->setSingle(app, "");
		}
		}
		return 1;
	}
	return 0;
}

int ReactiveBehaviourPsydule::ReadInputMessages()
{
	Message *msg;
	JString content="";
	char v[255];
	int time=0;
	int end_time=0;
	msg=psyduleInput->ReceiveMessage(10);
	std::map<std::string,DataBackchannel*> bmlfml;

	time=(int)(((float)clock()/CLOCKS_PER_SEC)*1000);
	end_time=(int)(((float)clock()/CLOCKS_PER_SEC)*1000)+10;
	
	//while(msg!=NULL)
	while(end_time>time)
	{
		if(msg!=NULL)
			if(strcmp(msg->type.c_str(),PSYCLONE_INPUT_MESSAGE_HEADER)==0)
			{
				content=msg->getContent();
				reactivebehaviourtrigger->addMessage((std::string)content);
			}
		msg=psyduleInput->ReceiveMessage(10);
		time=(int)(((float)clock()/CLOCKS_PER_SEC)*1000);
	}
	
	if(reactivebehaviourtrigger->messageList.size()>0)
		bmlfml=reactivebehaviourtrigger->ReceiveMessages();


	if(bmlfml["bml"]!=NULL)
		behaviourselector->updateBackchannelRequest(bmlfml["bml"]);

	if(bmlfml["fml"]!=NULL)
		behaviourselector->updateBackchannelRequest(bmlfml["fml"]);

	return(1);
}

void ReactiveBehaviourPsydule::SendBehaviour(std::string bmlfml, JString messagetype)
{
	psydule->PostString(bmlfml,GretaName+PSYCLONE_GRETA_WHITEBOARD, messagetype);
}
JString ReactiveBehaviourPsydule::getGretaName()
{	
	return GretaName;
}