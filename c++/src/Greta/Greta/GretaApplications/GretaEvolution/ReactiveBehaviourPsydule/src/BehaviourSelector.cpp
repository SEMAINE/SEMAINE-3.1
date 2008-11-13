//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


// ReactiveBehaviour.cpp: implementation of the ReactiveBehaviour class.
//
//////////////////////////////////////////////////////////////////////



#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include "BehaviourSelector.h"
#include "ReactiveBehaviourPsydule.h"
#include "XMLGenericParser.h"

#define MAX_DELAY 3000
#define MIN_DELAY 1500


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BehaviourSelector::BehaviourSelector():JThread()
{
}

BehaviourSelector::BehaviourSelector(ReactiveBehaviourPsydule *rbp, CentralClock *pc, AgentState *as):JThread()
{
	this->rbp=rbp;
	this->agentstate=as;
	this->pc=pc;
	randgen = new RandomGen();
	GretaName=rbp->getGretaName();

	//InitSelector();
	mimicry=new DataBackchannel("MIMICRY");
	reactive=new DataBackchannel("REACTIVE");
	cognitive=new DataBackchannel("COGNITIVE");
	lastBackchannelSent=new DataBackchannel("LAST_BACKCHANNEL_SENT");
}

BehaviourSelector::~BehaviourSelector()
{
	delete randgen;
}

int BehaviourSelector::updateBackchannelRequest(DataBackchannel *db)
{
	DataBackchannel *databackchannel;

	databackchannel=FindRequest(db->type);
	if(databackchannel==NULL)
	{
		printf("Backchannel type %s not found", db->type.c_str());
		return 0;
	}

	if(db->priority<databackchannel->priority)
		databackchannel->CopyDataBackchannel(db);

	return 1;
}

DataBackchannel* BehaviourSelector::FindRequest(std::string type)
{
	if(type=="MIMICRY")
		return mimicry;
	if(type=="REACTIVE")
		return reactive;
	if(type=="COGNITIVE")
		return cognitive;
	return NULL;
}

std::string BehaviourSelector::selectBackchannelType()
{
	float randomvalue=randgen->GetRand01();
	if(randomvalue>=0 && randomvalue<=mimicryLevel)
		return(retrieveBackchannelrequest("MIMICRY"));
	else
		return(retrieveBackchannelrequest("REACTIVE"));
}

std::string BehaviourSelector::retrieveBackchannelrequest(std::string type)
{
	std::string behaviour="";
	DataBackchannel *db=FindRequest(type);

	if(type=="MIMICRY")
	{
		behaviour=WriteBML(db);
		if(behaviour!="")
		{
			printf("\nMIMICRY for event: %s\n\n", db->zone.c_str());
			lastBackchannelSent->CopyDataBackchannel(db);
		}
	}
	if(type=="REACTIVE")
	{
		behaviour=WriteFML(db);
		if(behaviour!="")
		{
			printf("\nBACKCHANNEL for event: %s\n\n", db->zone.c_str());
			lastBackchannelSent->CopyDataBackchannel(db);
		}
	}

	if(type=="COGNITIVE")
		; //da definire

	return behaviour;
}


void BehaviourSelector::run()
{
	std::string behaviour="";
	int il=0;

	while(true)
	{
		Sleep(5);
		behaviour=retrieveBackchannelrequest("COGNITIVE");
		if(behaviour=="")
			behaviour=selectBackchannelType();

	//	if(behaviour=="")
	//		printf("Non ci sono behaviour\n");//, behaviour.c_str());
		

		if(behaviour!="")
		{
			if(behaviour.substr(0,behaviour.find_first_of(";"))=="bml")
			{
				std::string bml=behaviour.substr(behaviour.find_first_of(";")+1);
				rbp->SendBehaviour(bml, GretaName+PSYCLONE_BML_MESSAGE_HEADER);
				il=(int)(MAX_DELAY-(MAX_DELAY-MIN_DELAY)*agentstate->getUserInterestLevel());
				if(il<0) il=1300;
				Sleep(1000+il);
				mimicry->CleanDataBackchannel();
				reactive->CleanDataBackchannel();
			}
			else
			{
				std::string fml=behaviour.substr(behaviour.find_first_of(";")+1);
				rbp->SendBehaviour(fml, GretaName+PSYCLONE_FML_MESSAGE_HEADER);
				il=(int)(MAX_DELAY-(MAX_DELAY-MIN_DELAY)*agentstate->getUserInterestLevel());
				if(il<0) il=1300;
				Sleep(1000+il);
				mimicry->CleanDataBackchannel();
				reactive->CleanDataBackchannel();
			}
		}
	}
}

std::string BehaviourSelector::WriteBML(DataBackchannel *db)
{
	std::string bml;
	int time=pc->GetTime();
	int index=1;
	
	if(db->referencesMap.size()<1)
		return("");

	bml="bml;";
	bml+="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	bml+="<!DOCTYPE bml SYSTEM \"bml/bml.dtd\" []>\n";
	bml+="<bml>\n";

	std::map<std::string, std::string>::iterator refiter;
	for(refiter=db->referencesMap.begin();refiter!=db->referencesMap.end();refiter++)
	{
		bml+=WriteSignalInBML((*refiter).first, (*refiter).second, time, index);
		index+=1;
	}
	bml+="</bml>\n";
	return(bml);
}

//std::string BehaviourSelector::WriteSignalInBML(DataBackchannel *db)
std::string BehaviourSelector::WriteSignalInBML(std::string modality, std::string reference, int time, int index)
{
	//Create BML for mimicry behaviour
	std::string bml="";
	char s[256];

	if(reference=="" || modality=="")
		return("");
	else
	{
		sprintf(s,"%d",index);
		bml+="<" + modality + " id=\"sig" + (std::string)s + "\"  ";
		//sprintf(s,"%.3f",(float)(time+1500)/1000);
		sprintf(s,"%.3f",0.0);
		
		bml+="start=\"" + (std::string)s + "\" ";//>\n";
		//bml+="end=\"" + db->end + "\" stroke=\"1.0\">\n";
		bml+="end=\"1.5\" stroke=\"1.0\">\n";
		//
		bml+="<description level=\"1\" type=\"gretabml\">\n";
		bml+="<reference>" + reference + "</reference>\n";
		bml+="<intensity>1.00</intensity>\n";
		bml+="<FLD.value>0.70</FLD.value>\n";
		bml+="<PWR.value>0.20</PWR.value>\n";
		bml+="<REP.value>0.00</REP.value>\n";
		bml+="<SPC.value>0.00</SPC.value>\n";
		bml+="<TMP.value>0.40</TMP.value>\n";
		bml+="</description>\n";
		bml+="</" + modality + ">\n";
		
		return(bml);
	}
}

std::string BehaviourSelector::WriteFML(DataBackchannel *db)
{
	int i=0;
	char s[256];
	std::string fml="";
	std::vector<std::string>::iterator itermp;

	if(db->communicativefunction.size()==0)
		return("");

	//sprintf(s,"%d",db->time);
	//fml=(std::string)s + ";fml;";
	//std::string check=(std::string)s + ";fml;";
	fml="fml;";
	
	fml+="<?xml version=\"1.0\"?>\n";
	fml+="<!DOCTYPE fml-apml SYSTEM \"apml/fml-apml.dtd\" []>\n";
	fml+="<fml-apml>\n";

	fml+="<fml>\n";

	for(itermp=db->communicativefunction.begin(); itermp!=db->communicativefunction.end(); itermp++)
	{
//			check+=(*itermp) + " ";
			i+=1;
			sprintf(s,"%d",i);
			fml+="<backchannel id=\"p" + (std::string)s + "\" ";
			fml+="type=\"" + (*itermp) + "\" ";
			//sprintf(s,"%.3f",(float)(pc->GetTime()+1500)/1000);
			sprintf(s,"%.3f",(float)0.0);
			//DEFINE END!!
			fml+="start=\"" + (std::string)s + "\" end=\"1.5\" ";
			//fml+="importance=\"" + 1.0/(float)vecCommFun.size() + "\"/>\n";
			fml+="importance=\"1.0\"/>\n";
	}
	
//	fml+="<emotion id=\"e\" type=\"" + db->emotionalstate + "\" regulation=\"felt\" start=\"0.0\" end=\"2.0\" importance=\"1.0\"/>\n";
	fml+="</fml>\n";
	fml+="</fml-apml>\n";

	//if(check!="")
	//	printf("%s\n", check.c_str());
	return(fml);
}



/*std::string BehaviourSelector::WriteSignalInBML(DataBackchannel *db)
{
	//Create BML for mimicry behaviour
	std::string bml="";
	char s[256];

	if(db->reference=="")
		return("");
	else
	{
		bml="bml;";
		//sprintf(s,"%d",db->time);
		//bml=(std::string)s + ";bml;";// + headdirection;
		//printf("%s\n",headdirection.c_str());

		bml+="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
		bml+="<!DOCTYPE bml SYSTEM \"bml/bml.dtd\" []>\n";
		bml+="<bml>\n";
		//DA DEFINIRE!!!!
		bml+="<" + db->modality + " id=\"h1\"  ";
		sprintf(s,"%.3f",(float)(pc->GetTime()+1500)/1000);
		
		bml+="start=\"" + (std::string)s + "\" ";//>\n";
		//bml+="end=\"" + db->end + "\" stroke=\"1.0\">\n";
		bml+="end=\"1.5\" stroke=\"1.0\">\n";
		//
		bml+="<description level=\"1\" type=\"gretabml\">\n";
		bml+="<reference>" + db->reference + "</reference>\n";
		bml+="<intensity>1.00</intensity>\n";
		bml+="<FLD.value>1.00</FLD.value>\n";
		bml+="<PWR.value>0.50</PWR.value>\n";
		bml+="<REP.value>0.00</REP.value>\n";
		bml+="<SPC.value>1.00</SPC.value>\n";
		bml+="<TMP.value>0.40</TMP.value>\n";
		bml+="</description>\n";
		bml+="</" + db->modality + ">\n";
		bml+="</bml>\n";

		return(bml);
	}
}
*/