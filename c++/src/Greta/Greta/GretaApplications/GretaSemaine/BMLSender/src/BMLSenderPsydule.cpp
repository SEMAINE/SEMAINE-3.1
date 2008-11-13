// BMLSenderPsydule.cpp: implementation of the BMLSenderPsydule class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include "BMLSenderPsydule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BMLSenderPsydule::BMLSenderPsydule(JString name, JString host, int port) : Psydule(name, host, port)
{
	Acc=0;
	Vel=0;
	DI=0;
	CI=0;
	SPC=0;
	TMP=0;
	FLD=0;
	PWR=0;

	//b01
	/*
	AccMin=-15.253600;
	AccMax=17.616000;
	VelMin=-0.757922;
	VelMax=0.636820;
	DIMin=0.136507;
	DIMax=1.000000;
	CIMin=0.307608;
	CIMax=0.655996;
	*/
	/*
	//b06
	AccMin=-18.334200;
	AccMax=20.588900;
	VelMin=-0.478905;
	VelMax=1.286910;
	DIMin=0.043195;
	DIMax=1.000000;
	CIMin=0.275925;
	CIMax=0.586569;
	*/
	AccMin=20;
	AccMax=80;
	VelMin=0;
	VelMax=4;
	DIMin=0;
	DIMax=1.000000;
	CIMin=0.2;
	CIMax=0.5;
	this->name=name;
	this->host=host;
	this->port=port;

	logfile=fopen("logs/bmlpsydulelog.txt","w");
}

BMLSenderPsydule::~BMLSenderPsydule()
{
	fclose(logfile);

}

int BMLSenderPsydule::ReadMessage(Message *triggerMessage)
{
	int i;

	if(triggerMessage!=NULL)
	{
		JString content;
		content=triggerMessage->getContent();
		if(strcmp(content.c_str(),"")!=0)
		{
			fprintf(logfile,"%s\n",content.c_str());
			std::string tempString;
			std::string c;
			c=std::string(content.c_str());
			std::vector<std::string> tokens;
			std::string::size_type pos=0, prev_pos=0; 
			tokens.clear();
			while ((pos=c.find_first_of(';',prev_pos))!=std::string::npos)
			{
				tempString = c.substr(prev_pos, pos-prev_pos);
				tempString = std::string(_strupr((char *)tempString.c_str()));
				//if (tempString.length() != 0) {
				tokens.push_back(tempString);
				//}
				prev_pos=pos+1;
			}
			this->emotionname="";

			this->Vel=atof(tokens[0].c_str());
			this->Acc=atof(tokens[1].c_str());
			this->DI=atof(tokens[2].c_str());
			this->CI=atof(tokens[3].c_str());
			
			std::vector<struct emot> emots;

			for(i=4;i<tokens.size();)
			{
				struct emot e;
				e.name=tokens[i];
				if((i+1)<tokens.size())
					e.priority=atof(tokens[i+1].c_str());
				if((i+2)<tokens.size())
					e.vote=atof(tokens[i+2].c_str());
				emots.push_back(e);
				i=i+3;
			}

			int maxvote=0;
			int maxpriority=0;

			for(i=0;i<emots.size();i++)
			{
				if(emots[i].vote>maxvote)
				{
					maxvote=emots[i].vote;
					maxpriority=emots[i].priority;
					this->emotionname=emots[i].name;
				}
				if(emots[i].vote==maxvote)
					if(emots[i].priority>maxpriority)
					{
						maxvote=emots[i].vote;
						maxpriority=emots[i].priority;
						this->emotionname=emots[i].name;
					}
			}

			//printf("prevalent emotion: %s\n",this->emotionname.c_str());
			fprintf(logfile,"prevalent emotion: %s\n\n",this->emotionname.c_str());


			MapParameters();

			char SPCvalue[20];
			sprintf(SPCvalue,"%.3f",SPC);
			char TMPvalue[20];
			sprintf(TMPvalue,"%.3f",TMP);
			char FLDvalue[20];
			sprintf(FLDvalue,"%.3f",FLD);
			char PWRvalue[20];
			sprintf(PWRvalue,"%.3f",PWR);

			std::string bml;
			bml="";
			bml+="<?xml version=\"1.0\" ?>\n";
			bml+="<!DOCTYPE score SYSTEM \"\" []>\n";
			bml+="<bml>\n";
			bml+="<face id='face_expression1' type='affect=anger' start='0.0' end='5.0'/>\n";
		/*	if(this->emotionname=="")
				bml+="<gesture id='gesture1' type='ADJECTIVAL=GENOVA2' start='1.0' end='4.0'/>\n";
			if(this->emotionname=="ANGER")
				bml+="<gesture id='gesture1' type='EMOTION=ANGER' start='1.0' end='4.0'/>\n";
			if(this->emotionname=="JOY")
				bml+="<gesture id='gesture1' type='ADJECTIVAL=LARGE' start='1.0' end='4.0'/>\n";
			if(this->emotionname=="SADNESS")
				bml+="<gesture id='gesture1' type='ADJECTIVAL=GENOVA2' start='1.0' end='4.0'/>\n";
			if(this->emotionname=="FEAR")
				bml+="<gesture id='gesture1' type='EMOTION=FEAR' start='1.0' end='4.0'/>\n";
			if(this->emotionname=="SERENITY")
				bml+="<gesture id='gesture1' type='ADJECTIVAL=LARGE' start='1.0' end='4.0'/>\n";

			bml+="<gesture id='gesture1' stroke='2.5'/>\n";
			bml+="<gesture id='gesture1' expressivityspc='"+std::string(SPCvalue)+"'/>\n";
			bml+="<gesture id='gesture1' expressivitytmp='"+std::string(TMPvalue)+"'/>\n";
			bml+="<gesture id='gesture1' expressivityfld='"+std::string(FLDvalue)+"'/>\n";
			bml+="<gesture id='gesture1' expressivitypwr='"+std::string(PWRvalue)+"'/>\n";*/
			bml+="</bml>";
			this->PostString(bml, "Greta.Whiteboard", "Greta.Data.InternalBMLCode");
		}
		return 1;
	}
	else
	{
		return 0;
		//printf("no messages for BMLSenderPsydule\n");
	}
	return 1;
}

void BMLSenderPsydule::MapParameters()
{

	SPC=((1-((CI-CIMin)/(CIMax-CIMin)))*2.0f)-1;
	TMP=(((Vel-VelMin)/(VelMax-VelMin))*2.0f)-1;
	FLD=((1-((DI-DIMin)/(DIMax-DIMin)))*2.0f)-1;
	PWR=(((Acc-AccMin)/(AccMax-AccMin))*2.0f)-1;
	if(SPC>1)
		SPC=1;
	if(SPC<-1)
		SPC=-1;
	if(TMP>1)
		TMP=1;
	if(TMP<-1)
		TMP=-1;
	if(FLD>1)
		FLD=1;
	if(FLD<-1)
		FLD=-1;
	if(PWR>1)
		PWR=1;
	if(PWR<-1)
		PWR=-1;

	printf("CI=%.2f -> SPC=%.2f\n",CI,SPC);
	printf("Vel=%.2f -> TMP=%.2f\n",Vel,TMP);
	printf("DI=%.2f -> FLD=%.2f\n",DI,FLD);
	printf("Acc=%.2f -> PWR=%.2f\n",Acc,PWR);
	if(this->emotionname!="")
		printf("emotion=%s\n\n",this->emotionname.c_str());
}