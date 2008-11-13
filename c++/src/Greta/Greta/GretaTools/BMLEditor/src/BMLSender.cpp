// BMLSenderPsydule.cpp: implementation of the BMLSenderPsydule class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include "BMLSender.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BMLSender::BMLSender(JString name, JString host, int port) : CppAIRPlug(name, host, port)
{
	this->name=name;
	this->host=host;
	this->port=port;
}

BMLSender::~BMLSender()
{

}

int BMLSender::Register()
{

	if (!this->init()) {
		int msg = MessageBox(NULL, "Could not connect to Psyclone for registering BML Editor...", "BML Editor", MB_OK|MB_ICONINFORMATION);
		printf("Could not connect to Psyclone for registering BMLEditorPsydule...\n\n");
		return 0;
	}

	JString xml = "\
	   <module name=\"";
	   xml+=this->name;
	   xml+="\">\
	      <spec>\
	         <context name=\"Psyclone.System.Ready\">\
	            <phase id=\"";
	   xml+=this->name;
	   xml+=".MainPhase\">\
					<triggers>\
	                  <trigger from=\"Greta.Whiteboard\" type=\"Greta.Data.IDontKnow\"/>\
	               </triggers>\
	            </phase>\
	         </context>\
	      </spec>\
	   </module>\
	";

	if (this->isServerAvailable())
	{
		this->sendRegistration(xml);
		this->start();
	}
	else {
		int msg = MessageBox(NULL, "Could not connect to Psyclone for registering BLMEditorPsydule...", "BML Editor", MB_OK|MB_ICONINFORMATION);
		printf("Could not connect to Psyclone for registering BMLEditorPsydule...\n\n");
		return 0;
	}
	return 1;

}

int BMLSender::PostFile(std::string BMLFileName)
{
	std::string line;
	JString tosend="";

	std::ifstream inputfile(BMLFileName.c_str());
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			if(line!="")
			{
				tosend+=line.c_str();
				tosend+="\n";
			}
		}
	}
	else
		return 0;
	inputfile.close();

	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering BMLEditorPsydule...\n\n");
		return 0;
	}

	this->postMessage("Greta.Whiteboard","Greta.Data.InternalBMLCode",tosend,"","");

	return 1;
}

int BMLSender::PostString(std::string BML)
{

	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering BMLEditorPsydule...\n\n");
		return 0;
	}
	
	this->postMessage("Greta.Whiteboard","Greta.Data.InternalBMLCode",BML.c_str(),"","");

	return 1;
}

int BMLSender::ReceiveMessage()
{
	Message* triggerMessage;
	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering BMLEditorPsydule...\n\n");
		return 0;
	}
	if ((triggerMessage = this->waitForNewMessage(100)) != NULL)
	{
		JString content;
		content=triggerMessage->getContent();
		if(strcmp(content.c_str(),"")!=0)
		{

		}
		return 1;
	}
	else
	{
		return 0;
		//printf("no messages for BMLSender\n");
	}
	return 1;
}
