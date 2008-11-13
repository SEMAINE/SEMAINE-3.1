#include "GBPEngine.h"

/** Call to the inimanager :
* The main class must declare and initialize the inimanager.
*/
extern IniManager inimanager;

GBPEngine::GBPEngine(std::string ifn)
{
	this->iniFileName = ifn;
}

GBPEngine::~GBPEngine()
{

}

// Getters
CentralClock GBPEngine::getCentralClock()
{
	return pc;
}
CentralClock * GBPEngine::getpCentralClock()
{
	return &pc;
}

GBPSender * GBPEngine::getpGBPSender()
{
	return gbpsender;
}

GBPTranscoderThread GBPEngine::getGBPTranscoderThread()
{
	return gbptranscoderThread;
}

GBPTranscoderThread *GBPEngine::getpGBPTranscoderThread()
{
	return &gbptranscoderThread;
}

// The run methods
int GBPEngine::run()
{
	// Initialisation of the Greta Behavior Planner Engine
	if (initApplication()==0)
	{
		std::cout << GBP_ERROR_MESSAGE_GBPENGINE_INITIALIZATION_FAILED << std::endl;
		return 0;
	}	
	else
	{
		std::cout << "Application loaded" << std::endl;
	}
	if (initSender()!=1)
	{
		std::cout << GBP_ERROR_MESSAGE_GBPENGINE_SENDER_INITIALIZATION_FAILED << std::endl;
		return 0;
	}	
	else
	{
		std::cout << "Sender loaded" << std::endl;
	}
	if (initReceiver()!=1)
	{
		std::cout << GBP_ERROR_MESSAGE_GBPENGINE_RECEIVER_INITIALIZATION_FAILED << std::endl;
		return 0;
	}	
	else
	{
		std::cout << "Receiver loaded" << std::endl;
	}
	
	if (initTranscoderThread()!=1)
	{
		std::cout << GBP_ERROR_MESSAGE_GBPENGINE_TRANSCODER_INITIALIZATION_FAILED << std::endl;
		return 0;
	}	
	else
	{
		std::cout << "Transcoder loaded" << std::endl;
	}
	
	while(1)
	{
		Sleep(5);
	}
	return 1;
}

int GBPEngine::quit()
{
	return 1;
}

// Load the appplication parameter from the inimanager
int GBPEngine::initApplication()
{	
	// Register Psyclone module adress
	psyduleHost=(std::string)inimanager.GetValueString(GBP_INIT_PARAMETER_PSYDULE_HOST).c_str();
	if (psyduleHost.compare("")==0)
	{
		// The Ini file doesn't contain the psydule host
		std::cout << GBP_ERROR_MESSAGE_GBPENGINE_INI_PARAMETER_CANT_BE_READ << GBP_INIT_PARAMETER_PSYDULE_HOST << std::endl;
		return 0;
	}
	#ifdef _DEBUG
	else
	{
		std::cout << "Psydule host :"<< psyduleHost.c_str() << std::endl;		
	}
	#endif
	// Register Psyclone module port
	psydulePort=inimanager.GetValueInt(GBP_INIT_PARAMETER_PSYDULE_PORT);
	if (psydulePort==-999999999)
	{
		// The Ini file doesn't contain the psydule port
		std::cout << GBP_ERROR_MESSAGE_GBPENGINE_INI_PARAMETER_CANT_BE_READ << GBP_INIT_PARAMETER_PSYDULE_PORT << std::endl;
		return 0;
	}
	#ifdef _DEBUG
	else
	{
		std::cout << "Psydule port :"<< psydulePort << std::endl;		
	}
	#endif

	// Creation of the Psyclone inteface
	psydule=new Psydule(GBP_MODULE_PSYCLONE_NAME,psyduleHost.c_str(),psydulePort);
	return 1;
}
int GBPEngine::initSender()
{	
	// creation of the gbpSender interface
	gbpsender = new GBPSender(psydule);
	#ifdef _DEBUG
		std::cout << "BML Sender Construction succesful" << std::endl;
		std::cout << "Starting BML Sender"<< std::endl;
	#endif
	// Starting of the gbpSender interface
	if (gbpsender->start()!=1)
	{
		return 0;
	}	
	#ifdef _DEBUG
		std::cout << "Starting BML Sender succesful"<< std::endl;
	#endif
	return 1;
}

int GBPEngine::initReceiver()
{	
	// registered packet list
	std::list<JString> registeredDatatypes;
	// register to Greta Clock messages.
	registeredDatatypes.push_back(PSYCLONE_CLOCK_MESSAGE_HEADER);
	// register to Greta FML messages.
	registeredDatatypes.push_back(PSYCLONE_FML_MESSAGE_HEADER);
	// register to Caracterisation Baseline Message.
	registeredDatatypes.push_back(PSYCLONE_CARACTERISATION_BASELINE_MESSAGE_HEADER);
	// register to Caracterisation BQC Message.
	registeredDatatypes.push_back(PSYCLONE_CARACTERISATION_BQC_MESSAGE_HEADER);
	// register to Caracterisation MSS Message.
	registeredDatatypes.push_back(PSYCLONE_CARACTERISATION_MSS_MESSAGE_HEADER);	
	// registration
	if(psydule->Register(PSYCLONE_GRETA_WHITEBOARD,registeredDatatypes)!=0)
	{
		std::cout << GBP_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_OK << std::endl;		
	}
	else
	{
		std::cout << GBP_MODULE_NAME << " : " <<LOG_MESSAGE_PSYCLONE_CONNEXION_FAILED<< std::endl;
		psydule=0;
		return 0;
	}
	#ifdef _DEBUG
		std::cout << "Registration succesful" << std::endl;
		std::cout << "Begining of Receiver Construction" << std::endl;
	#endif
	// register Receiver
	fmlRec = new GBPReceiver(psydule,this);
	#ifdef _DEBUG
		std::cout << "Receiver Construction succesful" << std::endl;
		std::cout << "Starting FML Receiver" << std::endl;
	#endif
	// Starting receiver
	fmlRec->start();	
	#ifdef _DEBUG
		std::cout << "Starting FML Receiver succesful" << std::endl;
	#endif
	return 1;
}

int GBPEngine::initTranscoderThread()
{	
	// creation of the gbpTranscoder
	gbptranscoderThread = GBPTranscoderThread(this);
	#ifdef _DEBUG
		std::cout << "TranscoderThread Construction succesful" << std::endl;
		std::cout << "Starting TranscoderThread"<< std::endl;
	#endif
	// Starting of the gbpTranscoder thread
	gbptranscoderThread.run();
	#ifdef _DEBUG
		std::cout << "Starting TranscoderThread succesful"<< std::endl;
	#endif
	return 1;
}
