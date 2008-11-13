#include "GBPReceiver.h"
// Don't suppress this include : use to allow cross references
#include "GBPEngine.h"


// default constructor
GBPReceiver::GBPReceiver():JThread()
{
}

// constructor to use
GBPReceiver::GBPReceiver(Psydule *psydule,GBPEngine *gbpe):JThread()
{
	this->gbpengine=gbpe;
	this->psydule=psydule;
}

// Destructor
GBPReceiver::~GBPReceiver()
{
}



// Getters
GBPEngine  *GBPReceiver::getpGBPEngine()
{
	return gbpengine;
}

// The running method (use start() to execute in thread mode)
void GBPReceiver::run()
{
	// Todo : add a stop condition
	while(true)
	{
		Message *msg;
		int time;
		msg= psydule->ReceiveMessage(1);
		if(msg!=0)
		{
			// Case of the clock adjustement message
			if(strcmp(msg->type.c_str(),PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
			{		
				time=atoi(msg->getContent().c_str());
				getpGBPEngine()->getpCentralClock()->SetTime(time);
			}
			// Case of the Function Markup Language Message
			if(strcmp(msg->type.c_str(),PSYCLONE_FML_MESSAGE_HEADER)==0)
			{	
				std::string test = (std::string)msg->getContent().c_str();
				getpGBPEngine()->getpGBPTranscoderThread()->addMessage(test);
			}
			// Case of the Baseline initialization
			if(strcmp(msg->type.c_str(),PSYCLONE_CARACTERISATION_BASELINE_MESSAGE_HEADER)==0)
			{		
				std::cout << "Baseline loading" << std::endl;
				int i = getpGBPEngine()->getpGBPTranscoderThread()->loadBaseline("",msg->getContent());
				if (i==1)
				{					
					std::cout << "Baseline loading successful" << std::endl;
				}
				else
				{
					std::cout << "Error during the Baseline loading" << std::endl;
				}
			}
			//Case of the Behavior Quality Computation System initialization
			if(strcmp(msg->type.c_str(),PSYCLONE_CARACTERISATION_BQC_MESSAGE_HEADER)==0)
			{		
				std::cout << "BQC loading" << std::endl;
				int i = getpGBPEngine()->getpGBPTranscoderThread()->loadBQC("",msg->getContent());
				if (i==1)
				{
					std::cout << "BQC loading successful" << std::endl;
				}
				else
				{
					std::cout << "Error during BQC loading" << std::endl;
				}
			}
			// Case of the Lexicon initialization
			if(strcmp(msg->type.c_str(),PSYCLONE_CARACTERISATION_MSS_MESSAGE_HEADER)==0)
			{	
				std::cout << "Lexicon loading" << std::endl;
				int i = getpGBPEngine()->getpGBPTranscoderThread()->loadMMS("",msg->getContent());
				if (i==1)
				{
					std::cout << "Lexicon loading successful" << std::endl;
				}
				else
				{
					std::cout << "Error during lexicon loading" << std::endl;
				}
			}
		}
		// Don't overload processor
		Sleep(5);
	}
}