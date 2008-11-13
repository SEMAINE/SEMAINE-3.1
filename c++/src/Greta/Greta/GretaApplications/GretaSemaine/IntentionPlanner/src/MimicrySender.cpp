#include "GretaReactiveBehaviour.h"
#include "MimicrySender.h"

#define ROTATION_ANGLE 0.30


MimicrySender::MimicrySender()//:JThread()
{

}

MimicrySender::MimicrySender(Psydule *psydule, GretaReactiveBehaviour *grb)//:JThread()
{
	this->psydule=psydule;
	this->grb=grb;
}

MimicrySender::~MimicrySender()
{

}

//void MimicrySender::run()
//{}

int MimicrySender::running()
{
	std::string bml="";
	int time;

	std::vector<int> nods;
	std::vector<int> shakes;
	int numPoses=0;

	InputData data;

//	while(true)
//	{
		while(!inputDataList.empty())
		//if(!inputDataList.empty())
		{
			bml="";
			time=grb->getpCentralClock()->GetTime();
			if(time<senttime+5000)
				return 0;
			else
			{
				data =(InputData)inputDataList.front();
				bml=CalulateBML(data, time);
			}
		/*	if(data.name=="NOD")
			{
				if(nods.size()>1)
					if(starttime-nods[(int)(nods.size()-1)]>2000)
						nods.clear();
				if(nods.size()==3)
					bml=CalulateBML(data);
				nods.push_back(starttime);
				numPoses=0;
			}

			if(data.name=="SHAKE")
			{
				if(shakes.size()>1)
					if(starttime-shakes[shakes.size()-1]>2000)
						shakes.clear();
				if(shakes.size()==3)
					;//bml=CalulateBML(data);
				shakes.push_back(starttime);
				numPoses=0;
			}

			if(data.name=="POSES")
			{
				numPoses+=1;
				//bml=CalulateBML(data);
			}

			if(numPoses>2)
			{
				nods.clear();
				shakes.clear();
			}*/

			if(bml=="")
			{
				//printf("Warning: it's impossible to define listener's mimicry behaviour!\n");
				//printf("Warning: data name: %s   zone: %s   %.2f  %.2f  %.2f\n", data.name.c_str(), data.zone.c_str(), data.radiant_head_rotation[0], data.radiant_head_rotation[1], data.radiant_head_rotation[2]);
			}
			else
			{
				printf("name: %s    zone: %s\n", data.name.c_str(), data.zone.c_str());
				//printf("manda BML per mimicry richiesto a %d\n", starttime);
				SendMimicryBML(bml);
				senttime=time;
			}
			//inputDataList.pop_front();
			inputDataList.clear();
		}
		return 1;
		// Don't overload processor
		//Sleep(5);
	//}
}

int MimicrySender::addInputData(InputData indata)
{
	inputDataList.push_back(indata);
	return 1;
}

std::string MimicrySender::CalulateBML(InputData data, int time)
{
	//Create BML for mimicry behaviour
	std::string bml;
	std::string headdirection="head=head";
	int starttime;
	char s[10];
	
	if(data.name=="NOD")
		headdirection+="_up_right";
	if(data.name=="SHAKE")
		headdirection+="_shake";

	if(data.name=="POSES")
	{
		if(data.zone=="BA" || data.zone=="CB")
			headdirection+="_infront";
		else
		{
			if(data.radiant_head_rotation[0]>ROTATION_ANGLE)
				headdirection+="_down";
			if(data.radiant_head_rotation[0]<-ROTATION_ANGLE)
				headdirection+="_up";

			if(data.radiant_head_rotation[1]<-ROTATION_ANGLE)
				headdirection+="_left";
			if(data.radiant_head_rotation[1]>ROTATION_ANGLE)
				headdirection+="_right";

			if(data.radiant_head_rotation[2]<-ROTATION_ANGLE)
				headdirection+="_asid_l";
			if(data.radiant_head_rotation[2]>ROTATION_ANGLE)
				headdirection+="_asid_r";
		}
		
		//if(headdirection=="head")
			
	}

	if(headdirection=="head")
		return("");
	else
	{
		bml="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
		bml+="<!DOCTYPE bml SYSTEM \"bml/bml.dtd\" []>\n";
		bml+="<bml>\n";
		//DA DEFINIRE!!!!
		bml+="<head id=\"h1\"  ";
		sprintf(s,"%.2f",((float)time+2000)/1000);
		bml+="start=\"" + (std::string)s + "\" ";//>\n";
		bml+="end=\"2.00\" stroke=\"1.0\">\n";
		//
		bml+="<description level=\"1\" type=\"gretabml\">\n";
		bml+="<reference>" + headdirection + "</reference>\n";
		bml+="<intensity>1.00</intensity>\n";
		bml+="<FLD.value>0.00</FLD.value>\n";
		bml+="<PWR.value>0.00</PWR.value>\n";
		bml+="<REP.value>0.00</REP.value>\n";
		bml+="<SPC.value>0.00</SPC.value>\n";
		bml+="<TMP.value>0.00</TMP.value>\n";
		bml+="</description>\n";
		bml+="</head>\n";
		bml+="</bml>\n";

		printf("%s\n", bml.c_str());
		return(bml);
	}
}

void MimicrySender::SendMimicryBML(std::string bml)
{
	psydule->PostString(bml,PSYCLONE_GRETA_WHITEBOARD,PSYCLONE_BML_MESSAGE_HEADER);
}