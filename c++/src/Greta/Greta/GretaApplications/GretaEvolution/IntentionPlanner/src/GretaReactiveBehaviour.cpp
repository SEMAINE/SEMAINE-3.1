#include "GretaReactiveBehaviour.h"
#include "IniManager.h"
#include "listener\ListenerData.h"

extern DataContainer *datacontainer;
extern IniManager inimanager;

GretaReactiveBehaviour::GretaReactiveBehaviour()
{
	//provvisorio
	lastbctime=1;
}

GretaReactiveBehaviour::~GretaReactiveBehaviour()
{
}

CentralClock* GretaReactiveBehaviour::getpCentralClock() { return &pc; }
AgentState* GretaReactiveBehaviour::getpAgentState() { return(&agentstate); }
int GretaReactiveBehaviour::getLastBCTime() { return(lastbctime); }
ReactiveBackchannelSender* GretaReactiveBehaviour::getReactiveBackchannelSender() { return(&reactivebackchannelsender);}
MimicrySender* GretaReactiveBehaviour::getMimicrySender() { return(&mimicrysender); }

int GretaReactiveBehaviour::run()
{
	//find data
	listenerdata=datacontainer->getListenerData();
	initApplication();
	initRegistration();
	
	initAgentState();
	initReactiveBackchannelSender();
	initMimicrySender();
	initReactiveBehaviourTrigger();
	initTimeController();
	
	while(true)
	{
		ReadInputMessages();
		ReadMessage(psydule->ReceiveMessage(100));
		//ReadMessage(psyduleInput->ReceiveMessage(1000));
	}

	return 1;
}

// Load the application parameter from the inimanager
int GretaReactiveBehaviour::initApplication()
{	
	// Register Psyclone module adress
	psyduleHost=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	// Register Psyclone module port
	psydulePort=inimanager.GetValueInt("PSYCLONE_PORT");
	// Register Greta's name
	GretaName=(JString)inimanager.GetValueString("GRETA_NAME").c_str();
	// Creation of the Psyclone inteface

	psydule=new Psydule(GretaName+INPUTGRETA_MODULE_NAME,psyduleHost,psydulePort);
	psyduleInput=new Psydule(INPUTWATSON_MODULE_NAME,psyduleHost,psydulePort);

	return 1;
}

int GretaReactiveBehaviour::initReactiveBackchannelSender()
{	
	// creation of the ftbSender interface
	reactivebackchannelsender = ReactiveBackchannelSender(psydule, this);
	std::cout << "FML Sender Construction succesful" << std::endl;
	return 1;
}

int GretaReactiveBehaviour::initMimicrySender()
{	
	// creation of the ftbSender interface
	mimicrysender = MimicrySender(psydule, this);
	std::cout << "Mimicry signal Sender Construction succesful" << std::endl;
	//mimicrysender.start();
	return 1;
}

int GretaReactiveBehaviour::initRegistration()
{	
	// registered packet list
	std::list<JString> registeredGretaDatatypes;
	// register to Greta Clock messages.
	registeredGretaDatatypes.push_back(PSYCLONE_CLOCK_MESSAGE_HEADER);
	// register to Caracterisation Baseline Message.
	registeredGretaDatatypes.push_back(PSYCLONE_AGENTSTATE_MESSAGE_HEADER);
	// register to System Feedback
	registeredGretaDatatypes.push_back(PSYCLONE_SYSTEM_FEEDBACK_MESSAGE_HEADER);

	
	// registration GretaWiteboard
	if(psydule->Register(PSYCLONE_GRETA_WHITEBOARD,registeredGretaDatatypes)!=0)
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


	std::list<JString> registeredInputDatatypes;
	// register to Input messages, Watson
	registeredInputDatatypes.push_back(PSYCLONE_INPUT_MESSAGE_HEADER);

	// registration InputWiteboard
	if(psyduleInput->Register(PSYCLONE_INPUT_WHITEBOARD,registeredInputDatatypes)!=0)
	{
		std::cout << GretaName+INPUTGRETA_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_OK << std::endl;		
	}
	else
	{
		std::cout << GretaName+INPUTGRETA_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_FAILED<< std::endl;
		psyduleInput=0;
		return 0;
	}
	std::cout << "Registration succesful" << std::endl;
	
	
	return 1;
}

int GretaReactiveBehaviour::initReactiveBehaviourTrigger()
{	
	reactivebehaviourtrigger=ReactiveBehaviourTrigger(this);
	//reactivebehaviourtrigger.start();
	return 1;
}

int GretaReactiveBehaviour::initTimeController()
{	
	timecontroller=TimeController(this);
	timecontroller.start();
	return 1;
}

int GretaReactiveBehaviour::initAgentState()
{	
	agentstate=AgentState(listenerdata->agentstate);
	agentstate.start();
	return 1;
}

int GretaReactiveBehaviour::ReadMessage(Message *msg)
{
	JString content;
	int time;
	if(msg==NULL) return(0);

	content=msg->getContent();
	if(strcmp(content.c_str(),"")!=0)
	{
		if(strcmp(msg->type.c_str(),PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
		{
			time=atoi(msg->getContent().c_str());
			pc.SetTime(time);
		//	printf("aggiorno il clock %d   %d\n", time, pc.GetTime());
		}
		else
		if(strcmp(msg->type.c_str(),PSYCLONE_SYSTEM_FEEDBACK_MESSAGE_HEADER)==0)
		{
			;//da modificare rispettando la forma usata per inviare feedback (indica se un backchannel è stato eseguito)
			//this->lastbctime=atoi(msg->getContent().c_str());
		}
		else
		if(strcmp(msg->type.c_str(),PSYCLONE_AGENTSTATE_MESSAGE_HEADER)==0)
		{
			agentstate.addMessage((std::string)content);
		}
		return 1;
	}
	return 0;
}

int GretaReactiveBehaviour::ReadInputMessages()
{
	Message *msg;
	JString content;
	msg=psyduleInput->ReceiveMessage(0);

	while(msg!=NULL)
	{
		content=msg->getContent();
		if(strcmp(msg->type.c_str(),PSYCLONE_INPUT_MESSAGE_HEADER)==0)
		{
			//printf("Watson\n");
			reactivebehaviourtrigger.addMessage((std::string)content);
		}
		msg=psyduleInput->ReceiveMessage(0);
	}
	reactivebehaviourtrigger.messages();
	return(1);
}