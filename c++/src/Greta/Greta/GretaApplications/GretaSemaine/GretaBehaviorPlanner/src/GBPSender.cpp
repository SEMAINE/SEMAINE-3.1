#include "GBPSender.h"


// Default constructor
GBPSender::GBPSender():JThread()
{

}

// Constructor to use
GBPSender::GBPSender(Psydule *psydule):JThread()
{
	this->psydule=psydule;
}

// Destructor
GBPSender::~GBPSender()
{

}

// Execution methods
void GBPSender::run()
{
	while (1)
	{
		if(!messageList.empty())
		{
			std::string toto =(std::string) messageList.front();
			JString * mes = new JString(toto.c_str());	
			//std::cout <<"Envoie : " << mes->c_str() <<std::endl;
			// send the message to the whiteboard
			psydule->PostString(mes->c_str(),PSYCLONE_GRETA_WHITEBOARD,PSYCLONE_BML_MESSAGE_HEADER);
			// Remove the sent message from the input buffer
			messageList.pop_front();
		}
		// Don't overload the processor
		Sleep(5);
	}
}

// Add message to the Message Buffer
int GBPSender::addMessage(std::string mes)
{
	messageList.push_back(mes);
	return 1;
}