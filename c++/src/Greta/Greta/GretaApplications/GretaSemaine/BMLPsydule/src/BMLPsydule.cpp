#include ".\BMLPsydule.h"
#include "FileNames.h"
#include "Collection.h"
//#include "XMLGenericParser.h"

extern FileNames filenames;

BMLPsydule::BMLPsydule(JString name, JString host, int port) : Psydule(name, host, port)
{
	this->name=name;
	this->host=host;
	this->port=port;
}

BMLPsydule::~BMLPsydule()
{
}
/*
int BMLPsydule::BMLPsyduleRegister()
{
	return(this->Register("Greta.Whiteboard", "Greta.Data.InternalBMLCode"));
	/*
	if (!this->init()) {
		printf("Could not connect to Psyclone for registering BMLPsydule...\n\n");
		return 0;
	}

	JString xml = "\
	   <module name=\"";
	   xml+=name;
	   xml+="\">\
	      <spec>\
	         <context name=\"Psyclone.System.Ready\">\
	            <phase id=\"";
	   xml+=name;
	   xml+=".MainPhase\">\
					<triggers>\
	                  <trigger from=\"Greta.Whiteboard\" type=\"Greta.Data.InternalBMLCode\"/>\
	               </triggers>\
	            </phase>\
	         </context>\
	      </spec>\
	   </module>\
	";

	if (this->isServerAvailable()) {
		this->sendRegistration(xml);
		this->start();
	}
	else {
		printf("Could not connect to Psyclone for registering BMLPsydule...\n\n");
		return 0;
	}
	return 1;*/
//}

int BMLPsydule::ReadMessage(Message *triggerMessage)
{
	Message* multMessages;
	
	/*Message* triggerMessage;
	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for receiving messages...\n\n");
		return 0;
	}*/


	//if ((triggerMessage = this->waitForNewMessage(100)) != NULL)
	
	if(triggerMessage!=NULL)
	{
		int count=0;
		while((multMessages = this->waitForNewMessage(10))!=NULL)
		{
			triggerMessage=multMessages;
			count++;
		}
		printf("DISCARDED %d messages\n",count);

		Message* msg;
		JString rtype;
		JString rID;
		JString content;
		rtype=this->name;
		rtype+=".Answer";
		content=triggerMessage->getContent();
		if(strcmp(content.c_str(),"")!=0)
		{
//			printf("received: %s\n",(char *)content.c_str());
			BML_AgentEngine *ae;
			ae=new BML_AgentEngine();
			if(ae->Execute("",(char *)content.c_str(),0)==1)
			{
				//Collection coll;
				SendFAPs(filenames.Fap_File, "Greta.Whiteboard", "Greta.Data.FAPdata");
				SendBAPs(filenames.Bap_File, "Greta.Whiteboard", "Greta.Data.BAPdata");
				this->postMessage("Greta.Whiteboard","Greta.PlayerPsydule.Command.Start","","","");
				//Message* tosend = new Message(this->name, "Greta.Whiteboard", "Greta.Data.FAPBAPdata");
				//tosend->setObject((Object *)(&coll));
				//tosend->object=&coll;
				//this->postMessage("Greta.Whiteboard",((Message *)tosend),"");
//				delete tosend;
			}
			else
			{
				rID=triggerMessage->getID()+"_failure";
				msg = new Message(this->name, "Greta.Whiteboard", rtype, rID, "");
				delete ae;
			}
			rID=triggerMessage->getID()+"_success";
			msg = new Message(this->name, "Greta.Whiteboard", rtype, rID, "");
			delete ae;
		}
		this->addOutputMessage(msg);
		this->sendOutputMessages();
		return 1;
	}
	else
	{
		//printf("no messages for BMLPsydule\n");
	}
	return 1;
}

/*
int BMLPsydule::SendFAPs(std::string fapfilename)
{
	std::string line;
	JString tosend="";

	std::ifstream inputfile(fapfilename.c_str());
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
	this->postMessage("Greta.Whiteboard","Greta.Data.FAPdata",tosend,"","");
	return 1;
}

int BMLPsydule::SendBAPs(std::string bapfilename)
{
	std::string line;
	JString tosend="";

	std::ifstream inputfile(bapfilename.c_str());
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
	this->postMessage("Greta.Whiteboard","Greta.Data.BAPdata",tosend,"","");
	return 1;
}*/
