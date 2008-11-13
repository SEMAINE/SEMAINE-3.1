#include ".\APMLPsydule.h"
#include "FileNames.h"
#include "Collection.h"

#include <sys/stat.h>
//#include "XMLGenericParser.h"

extern FileNames filenames;

APMLPsydule::APMLPsydule(JString name, JString host, int port) : Psydule(name, host, port)
{
	this->name=name;
	this->host=host;
	this->port=port;
}

APMLPsydule::~APMLPsydule()
{
}
/*
int APMLPsydule::Register()
{
	if (!this->init()) {
		printf("Could not connect to Psyclone for registering APMLPsydule...\n\n");
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
	                  <trigger from=\"Greta.Whiteboard\" type=\"Greta.Data.APMLCode\"/>\
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
		printf("Could not connect to Psyclone for registering APMLPsydule...\n\n");
		return 0;
	}
	return 1;
}
*/
int APMLPsydule::ReadMessage(Message *triggerMessage)
{
/*	Message* triggerMessage;
	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering APMLPsydule...\n\n");
		return 0;
	}
	if ((triggerMessage = this->waitForNewMessage(100)) != NULL)*/
	if(triggerMessage!=NULL)
	{
		Message* msg;
		JString rtype;
		JString rID;
		JString content;
		rtype=this->name;
		rtype+=".Answer";
		content=triggerMessage->getContent();
		if(strcmp(content.c_str(),"")!=0)
		{
			//printf("received:  %s\n",content.c_str());
			APML_AgentEngine *aae;
			aae=new APML_AgentEngine();
			if(aae->CalculateTurnFromBuffer(content.c_str())==1)
			{
				printf("FAPfile:  %s\n",filenames.Fap_File.c_str());
				//Collection coll;
				this->SendBinaryFile(filenames.Wav_File, "Greta.Whiteboard", "Greta.Data.Audio");
				this->SendFAPs(filenames.Fap_File, "Greta.Whiteboard", "Greta.Data.FAPdata");
				this->SendBAPs(filenames.Bap_File, "Greta.Whiteboard", "Greta.Data.BAPdata");
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
				delete aae;
			}
			rID=triggerMessage->getID()+"_success";
			msg = new Message(this->name, "Greta.Whiteboard", rtype, rID, "");
			//delete aae;
		}
		this->addOutputMessage(msg);
		this->sendOutputMessages();
		return 1;
	}
	else
	{
		//printf("no messages for APMLPsydule\n");
	}
	return 1;
}

/*
int APMLPsydule::SendFAPs(std::string fapfilename)
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

int APMLPsydule::SendBAPs(std::string bapfilename)
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
}

int APMLPsydule::SendAudio(std::string audiofilename)
{
	std::string line;
	int size;

	FILE *inputfile;

	inputfile=fopen(audiofilename.c_str(),"rb");

	DataSample *ds;

	int i;
	i=0;

	if(inputfile!=0)
	{
		fseek(inputfile, 0, SEEK_END);
		size = ftell(inputfile);
		fseek(inputfile, 0, SEEK_SET);
		ds=new DataSample("Raw","");
		ds->data=(char*)malloc(size+1);

		while(!feof(inputfile))
		{
			ds->data[i]=(char)fgetc(inputfile);
			i++;
		}
	}
	else
		return 0;

	fclose(inputfile);
	
	ds->size=i;

	Message* msg = new Message(this->name, "Greta.Whiteboard", "Greta.Data.Audio");
	msg->object=ds;
	this->postMessage("Greta.Whiteboard",msg,"");
	return 1;
}*/


