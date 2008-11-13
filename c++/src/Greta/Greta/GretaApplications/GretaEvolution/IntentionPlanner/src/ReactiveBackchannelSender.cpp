#include "GretaReactiveBehaviour.h"
#include "ReactiveBackchannelSender.h"
#include "EngineParameterSet.h"
#include "EngineParameter.h"

#include <map>
#include <string>


ReactiveBackchannelSender::ReactiveBackchannelSender()
{

}

ReactiveBackchannelSender::ReactiveBackchannelSender(Psydule *psydule, GretaReactiveBehaviour *grb)
{
	this->psydule=psydule;
	this->grb=grb;
}

ReactiveBackchannelSender::~ReactiveBackchannelSender()
{

}

void ReactiveBackchannelSender::execute()
{
	int starttime, currenttime;
	std::string fml;

	starttime=grb->getpCentralClock()->GetTime();

	fml=selectCommunicativeIntentions();
	if(fml=="")
		printf("Warning: listener's communicative intentions indefined!\n");
	else
	{
		printf("manda FML richiesto a %d\n", starttime);
		SendBackchannelFML(fml);
	}
}

std::string ReactiveBackchannelSender::selectCommunicativeIntentions()
{
	MMSystemSpace::EngineParameterSet* listenerfunctions;
	std::map<std::string,EngineParameter>::iterator itermp;
	std::string::size_type loc;
	std::string fml;
	int i=0;
	int starttime;
	char s[10];

	fml="<?xml version=\"1.0\"?>\n";
	fml+="<!DOCTYPE fml-apml SYSTEM \"fml-apml.dtd\" []>\n";
	fml+="<fml-apml>\n";
	fml+="<fml>\n";

	starttime=grb->getpCentralClock()->GetTime();

	listenerfunctions=grb->agentstate.internalstate.GetEngineParameterSet("listener_fuction");

	for(itermp=listenerfunctions->mp.begin(); itermp!=listenerfunctions->mp.end(); itermp++)
	{
		loc = (*itermp).first.find("value",0);
		if((*itermp).second.GetValue()>0 &&  loc!=string::npos)
		{
			i+=1;
			sprintf(s,"%d",i);
			fml+="<backchannel id=\"p" + (std::string)s + "\" ";
			fml+="type=\"" + (*itermp).first.substr(0,loc-1) + "\" ";
			sprintf(s,"%.2f",(float)(starttime/1000));
			//DEFINE END!!
			fml+="start=\"" + (std::string)s + "\" end=\"1.0\" ";
			sprintf(s,"%.2f",(*itermp).second.GetValue());
			fml+="importance=\"" + (std::string)s + "\"/>\n";
		}
	}
	fml+="</fml>\n";
	fml+="</fml-apml>\n";

	if(i==0)
		return("");
	else
		return(fml);
}

void ReactiveBackchannelSender::SendBackchannelFML(std::string fml)
{
	psydule->PostString(fml,PSYCLONE_GRETA_WHITEBOARD,PSYCLONE_FML_MESSAGE_HEADER);
}