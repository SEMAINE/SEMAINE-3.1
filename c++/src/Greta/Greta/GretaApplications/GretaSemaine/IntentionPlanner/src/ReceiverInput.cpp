#include "GretaReactiveBehaviour.h"
#include "ReceiverInput.h"
#include "IniManager.h"

extern IniManager inimanager;


ReceiverInput::ReceiverInput()
{
}

ReceiverInput::ReceiverInput(GretaReactiveBehaviour *grb)
{
	this->grb=grb;
	// Register Psyclone module adress
	psyduleHost=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	// Register Psyclone module port
	psydulePort=inimanager.GetValueInt("PSYCLONE_PORT");
	// Creation of the Psyclone inteface
	psyduleInput=new Psydule(INPUTRECEIVER_MODULE_NAME,psyduleHost,psydulePort);

	initRegistrationInputWhiteboard();
}

ReceiverInput::~ReceiverInput()
{
}

int ReceiverInput::initRegistrationInputWhiteboard()
{	
	std::list<JString> registeredInputDatatypes;
	// register to Input messages, Watson
	registeredInputDatatypes.push_back(PSYCLONE_INPUT_MESSAGE_HEADER);

	// registration InputWiteboard
	if(psyduleInput->Register(PSYCLONE_INPUT_WHITEBOARD,registeredInputDatatypes)!=0)
	{
		std::cout << INPUTRECEIVER_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_OK << std::endl;		
	}
	else
	{
		std::cout << INPUTRECEIVER_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_FAILED<< std::endl;
		psyduleInput=0;
		return 0;
	}
	std::cout << "Registration succesful" << std::endl;

	return 1;
}

int ReceiverInput::ReadMessage()
{
	JString content;
	int time;
	Message *msg;

	msg=psyduleInput->ReceiveMessage(100);
	if(msg==NULL) return(0);

	content=msg->getContent();
	if(strcmp(content.c_str(),"")!=0)
	{
		if(strcmp(msg->type.c_str(),PSYCLONE_INPUT_MESSAGE_HEADER)==0)
		{
			printf("Watson\n");
			grb->getpReactiveBehaviourTrigger()->addMessage((std::string)content);
		}
		return 1;
	}
	return 0;
}