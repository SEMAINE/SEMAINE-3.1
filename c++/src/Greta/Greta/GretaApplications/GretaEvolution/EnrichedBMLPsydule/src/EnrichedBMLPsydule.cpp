#include "EnrichedBMLPsydule.h"
#include "FileNames.h"
#include "Collection.h"


extern FileNames filenames;

EnrichedBMLPsydule::EnrichedBMLPsydule(JString name, JString host, int port) : Psydule(name, host, port)
{
	this->name=name;
	this->host=host;
	this->port=port;
	markers.clear();
	phonemes.clear();
	behaviortags.clear();
	globaltime=0;
}

EnrichedBMLPsydule::~EnrichedBMLPsydule()
{
}
/*
int EnrichedBMLPsydule::Register()
{
	if (!this->init()) {
		printf("Could not connect to Psyclone for registering EnrichedBMLPsydule...\n\n");
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
					  <trigger from=\"Greta.Whiteboard\" type=\"Greta.Data.EnrichedBMLCode\"/>\
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
		printf("Could not connect to Psyclone for registering EnrichedBMLPsydule...\n\n");
		return 0;
	}
	return 1;
}*/

int EnrichedBMLPsydule::ReadMessage(Message *triggerMessage)
{
	/*Message* triggerMessage;
	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering EnrichedBMLPsydule...\n\n");
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

		if(strcmp(triggerMessage->type.c_str(),"Greta.Data.EnrichedBMLCode")==0)
			this->ComputeEnrichedBML(content.c_str());

			/*
			//printf("received:\n%s",content->c_str());
			BML_AgentEngine *ae;
			ae=new BML_AgentEngine();
			if(ae->Execute("",(char *)content.c_str())==1)
			{
				//Collection coll;
				SendFAPs(filenames.Fap_File);
				SendBAPs(filenames.Bap_File);
				this->postMessage("Greta.Whiteboard","Greta.PlayerPsydule.Command.Start","","","");
				//Message* tosend = new Message(this->name, "Greta.Whiteboard", "Greta.Data.FAPBAPdata");
				//tosend->setObject((Object *)(&coll));
				//tosend->object=&coll;
				//this->postMessage("Greta.Whiteboard",((Message *)tosend),"");
//				delete tosend;

			}
			else
			{
				//rID=triggerMessage->getID()+"_failure";
				//msg = new Message(this->name, "Greta.Whiteboard", rtype, rID, "");
				//delete ae;
			}
			*/
			//rID=triggerMessage->getID()+"_success";
			//msg = new Message(this->name, "Greta.Whiteboard", rtype, rID, "");
			//delete ae;
		//}
		//this->addOutputMessage(msg);
		//this->sendOutputMessages();
		return 1;
	}
	else
	{
		//printf("no messages for EnrichedBMLPsydule\n");
	}
	return 1;
}


int EnrichedBMLPsydule::ComputeEnrichedBML(std::string enrichedbml)
{
	XMLGenericParser p;
	p.SetValidating(false);
	XMLGenericTree *t=p.ParseBuffer((char*)enrichedbml.c_str());
	if(t==0)
		return 0;
	markers.clear();
	phonemes.clear();
	phonemesstruct offset;
	offset.duration=0.05;
	offset.ph="pau";
	phonemes.push_back(offset);
	this->globaltime=offset.duration;
	behaviortags.clear();
	PreVistTree(t);
	SavePhonemes();
	JString tosend=PrepareInternalBML(t);
	SendInternalBML(tosend);
	return 1;
}

void EnrichedBMLPsydule::PreVistTree(XMLGenericTree *t)
{
	if(t->name=="mark")
	{
		markerstruct mk;
		mk.name=t->GetAttribute("name");
		mk.time=globaltime;
		markers.push_back(mk);
	}
	if(t->name=="ph")
	{
		phonemesstruct ph;
		ph.ph=t->GetAttribute("p");
		if(ph.ph=="_")
			ph.ph="pau";
		ph.duration=t->GetAttributef("dur")/1000.0f;
		phonemes.push_back(ph);
		globaltime+=t->GetAttributef("dur")/1000.0f;
	}
	if((t->name=="face")||(t->name=="head")||(t->name=="gesture"))
	{
		behaviortagsstruct b;
		b.channel=t->name;
		b.signalname=t->GetAttribute("gretabml:signalname");
		b.start=t->GetAttribute("start");
		b.stroke=t->GetAttribute("stroke");
		b.end=t->GetAttribute("end");
		b.id=t->GetAttribute("id");
		behaviortags.push_back(b);
	}

	std::list<XMLGenericTree*>::iterator iter;
	if(!t->child.empty())
		for(iter=t->child.begin();iter!=t->child.end();iter++)
			PreVistTree((*iter));
}

void EnrichedBMLPsydule::SavePhonemes()
{
	FILE *f;
	f=fopen("tmp/bmlpsydulephonemes.txt","w");
	if(f!=0)
	{
		std::vector<phonemesstruct>::iterator iter;
		if(!phonemes.empty())
			for(iter=phonemes.begin();iter!=phonemes.end();iter++)
				fprintf(f,"%s %.3f\n",(*iter).ph.c_str(),(*iter).duration);
		fclose(f);
	}
}

std::string EnrichedBMLPsydule::ComputeTimings()
{
	std::string tosend;
	tosend="";
	std::vector<behaviortagsstruct>::iterator iter;
		if(!behaviortags.empty())
			for(iter=behaviortags.begin();iter!=behaviortags.end();iter++)
			{
				tosend+="<";
				tosend+=(*iter).channel;
				tosend+=" ";

				tosend+="id=\""+(*iter).id+"\" ";
				tosend+="type=\""+(*iter).signalname+"\" ";

				char buf[20];
				float time;

				time=GetMarkerTime(ExtractMarkerName((*iter).start));
				sprintf(buf,"%.3f",time);
				(*iter).start=std::string(buf);

				time=GetMarkerTime(ExtractMarkerName((*iter).end));
				sprintf(buf,"%.3f",time);
				(*iter).end=std::string(buf);

				time=GetMarkerTime(ExtractMarkerName((*iter).stroke));
				sprintf(buf,"%.3f",time);
				(*iter).stroke=std::string(buf);

				if((*iter).start!="-1.000")
					tosend+="start =\""+(*iter).start+"\" ";
				if((*iter).end!="-1.000")
					tosend+="end =\""+(*iter).end+"\" ";
				if((*iter).stroke!="-1.000")
					tosend+="stroke =\""+(*iter).stroke+"\" ";

				tosend+="/>\n";
			}
	return tosend;
}

std::string EnrichedBMLPsydule::ExtractMarkerName(std::string s)
{
	if(s.find_first_of(":")!=std::string::npos)
	{
		std::string markname;
		markname=s.substr(s.find_first_of(":")+1);
		return markname;
	}
	return "";
}

float EnrichedBMLPsydule::GetMarkerTime(std::string s)
{
	std::vector<markerstruct>::iterator iter;
	if(!markers.empty())
		for(iter=markers.begin();iter!=markers.end();iter++)
			if((*iter).name==s)
				return (*iter).time;
	return -1;
}

JString EnrichedBMLPsydule::PrepareInternalBML(XMLGenericTree *t)
{
	JString tosend;
	tosend="";

	tosend+="<?xml version=\"1.0\" ?>\n";
	tosend+="<!DOCTYPE score SYSTEM \"\" []>\n";
	tosend+="<bml>\n";
	
	tosend+=ComputeTimings().c_str();


	tosend+="<lip filename=\"tmp/bmlpsydulephonemes.txt\"/>\n";

	tosend+="</bml>\n";

	return tosend;
}

void EnrichedBMLPsydule::SendInternalBML(JString tosend)
{
	/*
	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone\n\n");
		return;
	}
	this->postMessage("Greta.Whiteboard","Greta.Data.InternalBMLCode",tosend,"","");
	*/
	this->PostString(tosend.c_str(),"Greta.Whiteboard","Greta.Data.InternalBMLCode");
}
