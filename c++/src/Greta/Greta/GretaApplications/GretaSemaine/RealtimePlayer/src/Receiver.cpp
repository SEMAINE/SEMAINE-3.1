#include "Receiver.h"
#include "IniManager.h"

//extern	"C" int load_wave(char *,char*);
extern IniManager inimanager;

Receiver::Receiver(Psydule *psydule, FramesManager* fm, CommandsManager* cm,CentralClock *pc, JString GretaName):JThread()
{
	this->psydule=psydule;
	this->fm=fm;
	this->pc=pc;
	this->cm=cm;
	this->GretaName=GretaName;
	sound_time=0;
}


Receiver::~Receiver()
{
}

void Receiver::run()
{

	while(true)
	{
		Message *msg;
		int time;
		if(psydule==0)
			break;
		msg=psydule->ReceiveMessage(5);
		if(msg!=0)
		{
			if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER)==0)
			{		
				time=atoi(msg->getContent().c_str());
				pc->SetTime(time);
			}
			if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_FAP_FRAME)==0)
			{
				fm->AddFAPFrame(msg->content);
			}
			if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_BAP_FRAME)==0)
			{
				fm->AddBAPFrame(msg->content);
			}

			if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_REALTIME_PLAYER_COMMAND)==0)
			{
				std::string command;
				command=msg->content.c_str();
				cm->AddCommand(command.substr(0,command.find_first_of(" ")),
					command.substr(command.find_first_of(" ")+1));
			}
			
			if(strcmp(msg->type.c_str(),GretaName+PSYCLONE_DATA_WAV)==0)
			{
				std::string filename;
				char number[12];
				sprintf(number,"%d", pc->GetTime());
				filename=inimanager.Program_Path+"output/rtplayer.wav";
				this->psydule->WriteBinaryFile(filename,msg);								
			}

		}
	}
}