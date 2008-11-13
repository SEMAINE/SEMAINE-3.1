// GretaBMLRealtimePsydule.cpp: implementation of the GretaBMLRealtimePsydule class.
//
//////////////////////////////////////////////////////////////////////

#include "RealtimeBMLPsydule.h"
#include "FAPwriter.h"
#include "RandomGen.h"
#include "FileNames.h"
#include "IdleMovements.h"

#include <string>
#include <vector>
#include <math.h>
#include <time.h>

#include <iostream>
#include <fstream>

#define MAXFRAMESENT 20

BAPConverter *GestureGlobalBAPconverter;
extern IniManager inimanager;
extern FileNames filenames;
RandomGen *randogen;
RandomGen *rando;

//timeat
int DELAY = 1000;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RealtimeBMLPsydule::RealtimeBMLPsydule(JString name, JString host, int port, JString GretaName) : Psydule(name, host, port)
{
	randogen = new RandomGen();

	std::list<JString> datatypes;
	fapframes=0;
	bapframes=0;
	//all_fapframes=0;
	fapframeswithoutnoise=0;
 	filenames.Base_File="output/RealtimeBMLPsydule";
	this->name=name;
	this->host=host;
	this->port=port;
	this->GretaName=GretaName;
	datatypes.push_back(GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER);
	printf(PSYCLONE_CLOCK_MESSAGE_HEADER );
	datatypes.push_back(GretaName+PSYCLONE_BML_MESSAGE_HEADER);
			
	if(Register(GretaName+PSYCLONE_GRETA_WHITEBOARD,datatypes)!=0)
	{
		//printf("registered to Psyclone\n");
	}
	else
	{
		printf("could not register to Psyclone\n");
		exit(0);
	}

	FPS=25.0f;

	NoEndFaceSignals = false;

	ReadMessage(ReceiveMessage(2200));

	last_fap_frame_time=pc.GetTime();
	last_bap_frame_time=pc.GetTime();

	GestureGlobalBAPconverter = new BAPConverter();

	if(inimanager.GetValueInt("REALTIME_GESTUREENGINE")==0)
		last_bap_frame=0;
	else
		last_bap_frame=new BAPFrame(GestureGlobalBAPconverter);
}

RealtimeBMLPsydule::~RealtimeBMLPsydule()
{

}

void RealtimeBMLPsydule::MainLoop()
{
	printf("\nRealtimeBMLPsydule running...\n");

	while(true)
	{
		Sleep(5);
		Message *msg_last;
		Message msg_bml, msg_clock;
		msg_last=ReceiveMessage(5);
		msg_bml=NULL;
		msg_clock=NULL;

		while(msg_last!=NULL)
		{
			if(strcmp(msg_last->type.c_str(),GretaName+PSYCLONE_BML_MESSAGE_HEADER)==0)
			{
				msg_bml=*msg_last;
			}
			if(strcmp(msg_last->type.c_str(),GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
			{
				msg_clock=*msg_last;
			}
			msg_last=ReceiveMessage(5);
		}
		ReadMessage(&msg_bml);
		ReadMessage(&msg_clock);
		SendIdle();
	}
}

int RealtimeBMLPsydule::ReadMessage(Message *msg)
{
	std::string content;

	if (msg!=0)
	{
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
		{
			int time;
			time=atoi(msg->getContent().c_str());
			pc.SetTime(time);
			return 1;
		}
		
		if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_BML_MESSAGE_HEADER)==0)
		{
						
			filenames.Phonemes_File="";
			content=msg->getContent();

			//timeat=clock();		
					
			findCurrentFrame(fapframes);		

			if(bmlengine.Execute("",(char *)content.c_str(),1,last_bap_frame,&last_fap_frame,true)!=0)
			{
				if(inimanager.GetValueInt("REALTIME_GESTUREENGINE")==1)
					bmlengine.GetMPlanner()->GetCopyLastBAPFrame(last_bap_frame);
				//is an infinite signal?
				NoEndFaceSignals=false;
				NoEndFaceSignals=bmlengine.hasNoEndFaceSignals();

				faceengine=bmlengine.GetFaceEngine();
				fapframes=faceengine->GetAnimation();
				fapframeswithoutnoise=faceengine->GetAnimationWithoutNoise();
		

				mplanner=bmlengine.GetMPlanner();
				bapframes=mplanner->GetAnimation();			

				if (NoEndFaceSignals==true)
				{	
					//cut the animation
					int size=fapframes->size();
					fapframes->resize(floor((double)(size/2)));
					fapframeswithoutnoise->resize(floor((double)(size/2)));
					//if (bapframes->size()==size) bapframes->resize(floor((double)(size/2)));
				}
				
				//et=clock();
				//printf("%i\n", et-st);
	
				int start_time = pc.GetTime()+DELAY;
								
				if (bmlengine.speech!=0) 
				{				
					//sound_file_name.copy(iter->reference.c_str());													 				

					char str[20];
					std::string command;
					//int f = DELAY+ pc.GetTime() +  (OFFSET/FPS*1000) + 1000.0f * iter->start;				
					
					sprintf(str,"%d",start_time+ (int) (bmlengine.speech_delay*1000));
					//printf("audio starts at %s ", str );
										
					command="PLAYWAVAT ";
					command+=str;
					
					this->PostString(command.c_str(), GretaName+PSYCLONE_GRETA_WHITEBOARD, GretaName+PSYCLONE_REALTIME_PLAYER_COMMAND);
					//this->SendBinaryFile(filenames.Wav_File, GretaName+PSYCLONE_GRETA_WHITEBOARD, GretaName+PSYCLONE_DATA_WAV);					
					this->PostSpeechFromChar(bmlengine.content_of_speech, GretaName+PSYCLONE_GRETA_WHITEBOARD, GretaName+PSYCLONE_DATA_WAV,&bmlengine.speech_size);																	
					//free (bmlengine.content_of_speech);
					
				}

				SendFAPsBAPs(start_time,last_fap_frame_time,last_bap_frame_time);			
			}
		}
		return 1;
	}
	else
		return 0;
}

int RealtimeBMLPsydule::SendIdle()
{

	if (pc.GetTime() > last_fap_frame_time) 
	
	{
		
		//if ((fapframes!=0)&&(fapframes->empty()==false)) lastframe=(*((fapframes->end())-1)).clone();
		findCurrentFrame(fapframeswithoutnoise);

		IdleMovements *idlemovements = new IdleMovements();
		//generate five seconds of continuos expressions
		idlemovements->generate(last_fap_frame,5.0);

		if (fapframes!=0)
			fapframes->clear();
		
		fapframes=idlemovements->GetAnimation();
		fapframeswithoutnoise=idlemovements->GetAnimationWithoutNoise();

		//bapframes=new BAPFrameVector (0, &BAPFrame(GestureGlobalBAPconverter));		
		//if ((bapframes!=0) && (bapframes->empty()==false))  bapframes->clear();

		/*std::vector<FAPFrame>::iterator iter;
		for(iter=fapframes->begin();iter!=fapframes->end();iter++)  {
		printf( "lastframe 48 %i, 49 %i, 50 %i /n",lastframe.FAPs[48].value,lastframe.FAPs[49].value,lastframe.FAPs[50].value);

		BAPFrame* AnimationFrame = new BAPFrame();				
				bapframes->push_back(AnimationFrame);
			}
		*/

		int start_time = 0;

		if (last_fap_frame_time==0.0f)
			start_time=pc.GetTime() + DELAY;
		else
			start_time = last_fap_frame_time;
	
		//printf("last_frame_time %i ", last_frame_time);

		if(bapframes!=0)
			bapframes->clear();
		SendFAPsBAPs(start_time,last_fap_frame_time,last_bap_frame_time);
		
	}
	
	return 1;
}


void RealtimeBMLPsydule::findCurrentFrame(FAPFrameVector *frames)
{

	last_fap_frame.ActivateAndSetAllFAPsTo(0);

	//if something was displayed before find a start frame of new animation
	if	((frames!=0) && (!frames->empty())) 
	{
								
		//it can be last frame of the prevoius animation				
		last_fap_frame=(*((frames->end())-3)).clone();
	

		//add condition if the animation is not finished yet
		//if (last_frame_time > pc.GetTime() + DELAY) {
								
		//or it can be the frame that is displayed at the moment
		int  index = (int)(last_fap_frame_time - pc.GetTime() -DELAY)*FPS/1000;				
						
		if (((*frames).size()>index)&&(index > (int)0 ))  
			last_fap_frame=(*frames)[(*frames).size()-index].clone();															
		
	}
				
}



/*
void GretaBMLRealtimePsydule::SendFAP()
{
std::string msg;
int frameind;
char tm[256];
int timeat=20000;

FAPwriter fw;
//fw.WriteToFile(fapframes, "provafap.fap", 25);

for(frameind=0;frameind<(*fapframes).size();frameind++) 
{
msg="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
msg+="<greta-object>\n";
msg+="<timestamp>\n";
msg+="<sent at=\"" + (std::string)(itoa(time, tm, 10)) + "\"/>\n";
msg+="<expire at=\"\"/>\n";
msg+="<use at=\"" + (std::string)(itoa(timeat, tm, 10)) + "\" as_received=\"0\"/>\n";
msg+="</timestamp>\n";
msg+="<body>\n";

msg+=fw.FapFrameToString(fapframes, frameind);

msg+="</body>\n";
msg+="</greta-object>\n";

//printf("%s\n", msg.c_str()); 

this->PostString(msg, GretaName+PSYCLONE_GRETA_WHITEBOARD, GretaName+PSYCLONE_FAP_FRAME);
timeat+=40;
}
}
*/

//returns the time of last frame in a block

void RealtimeBMLPsydule::SendFAPsBAPs(int start_frame_time,int &lastfapframetime,int &lastbapframetime)
{
	//int last_frame_number;
	std::string msg,label;
	int frameind;
	char tm[256];
	int i;
	int maxlength;
	clock_t faptimeat,baptimeat;


	//FAPwriter fw;

	//BAPwriter bw;
	//bw.WriteToFile(bapframes, "provabap.bap");

	if (start_frame_time==-1) faptimeat=pc.GetTime()+DELAY;
	else faptimeat=start_frame_time;

	baptimeat=faptimeat;

	sprintf(tm,"%d",pc.GetTime());

	label=tm;

	msg="DELETEDIFF ";

	msg+=label;

	msg+=" ";

	sprintf(tm,"%d",faptimeat-1);

	msg+=tm;


	this->PostString(msg, GretaName+PSYCLONE_GRETA_WHITEBOARD, GretaName+PSYCLONE_REALTIME_PLAYER_COMMAND);

	msg="";

	frameind=0;

	maxlength=0;

	if((bapframes!=0)&&(fapframes!=0))
	{
		if((*bapframes).size()>(*fapframes).size())
			maxlength=(*bapframes).size();
		else
			maxlength=(*fapframes).size();
	}
	else
	{
		if(bapframes!=0)
			maxlength=(*bapframes).size();
		if(fapframes!=0)
			maxlength=(*fapframes).size();
	}

	while(frameind<maxlength)
	{
		if(fapframes!=0)
		{
			msg="";

			msg+=label+"\n";

			for(i=0;i<MAXFRAMESENT;i++)
			{
				if ((i+frameind)<(*fapframes).size())
				{
					(*fapframes)[i+frameind].framenumber=faptimeat;
					msg+=(*fapframes)[i+frameind].toString();
					//last_frame_number=(i+frameind);
				}
				else
					break;
				faptimeat+=40;
			}

			this->PostString(msg, GretaName+PSYCLONE_GRETA_WHITEBOARD, GretaName+PSYCLONE_FAP_FRAME);
		}

		if(bapframes!=0)
		{
			msg="";

			msg+=label+"\n";

			for(i=0;i<MAXFRAMESENT;i++)
			{

				if ((i+frameind)<(*bapframes).size())
				{
					(*bapframes)[i+frameind]->SetFrameNumber(baptimeat);
					msg+=(*bapframes)[i+frameind]->WriteBAP();
				}
				else
					break;
				baptimeat+=40;
			}

			this->PostString(msg, GretaName+PSYCLONE_GRETA_WHITEBOARD, GretaName+PSYCLONE_BAP_FRAME);
		}

		frameind+=MAXFRAMESENT;
	}

	//return last_frame_number/FPS*1000+faptimeat;
	lastfapframetime=faptimeat-40;
	lastbapframetime=baptimeat-40;

}

