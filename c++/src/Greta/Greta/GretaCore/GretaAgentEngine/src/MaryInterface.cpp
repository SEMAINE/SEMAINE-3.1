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

// MaryInterface.cpp: implementation of the MaryInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "MaryInterface.h"
#include "IniManager.h"
#include "XMLGenericParser.h"
#include "SDL_net.h"
#include "FileNames.h"
#include <iostream>
#include <fstream>
#include <sstream>

extern IniManager inimanager;
extern FILE *agent_log;
extern FileNames filenames;

int very_long;
char* very_long_speech;

TCPsocket GretaMaryInfoSocket;
TCPsocket GretaMaryDataSocket;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MaryInterface::MaryInterface(CommunicativeAct* first_comm_act):SpeechInterface(first_comm_act)
{
	//host = "cling.dfki.uni-sb.de";
	//host = "hopfen";

	host = inimanager.GetValueString("MARY_ADDRESS");

	port = 59125;

	//in  = "TEXT_EN";
	in  = "SSML";

	//out  = "POSTPROCESSED_DE";
	//out = "AUDIO";
	//out = "DURATIONS_DE";
	out = "ACOUSTPARAMS";

	audiotype = "MP3";
	//audiotype = "WAVE";
	
	voice = "us1";
	//voice = "slt-arctic";

	sex="female";

	english=true;
	german=false;
	french=false;


}

MaryInterface::MaryInterface():SpeechInterface(0)
{
	//host = "cling.dfki.uni-sb.de";
	//host = "hopfen";

	host = inimanager.GetValueString("MARY_ADDRESS");

	port = 59125;

	//in  = "TEXT_EN";
	in  = "SSML";

	//out  = "POSTPROCESSED_DE";
	//out = "AUDIO";
	//out = "DURATIONS_DE";
	out = "ACOUSTPARAMS";

	audiotype = "MP3";
	//audiotype = "WAVE";
	
	voice = "us1";
	//voice = "slt-arctic";

	sex="female";

	english=true;
	german=false;
	french=false;


}

MaryInterface::~MaryInterface()
{
	Deconnect();
}

int MaryInterface::Connect()
{
	if(SDL_Init(0)==-1)
	{
		if(agent_log!=0)
			fprintf(agent_log,"SDL_Init: %s\n", SDL_GetError());
		initialized=false;
		return 0;
	}
	else
		if(SDLNet_Init()==-1)
		{
			if(agent_log!=0)
				fprintf(agent_log,"SDLNet_Init: %s\n", SDLNet_GetError());
			initialized=false;
			return 0;
		}
		else
			initialized=true;

	IPaddress ip;
	if(agent_log!=0)
		fprintf(agent_log,"MARY: trying to connect\n");
	if(SDLNet_ResolveHost(&ip,host.c_str(),port)==-1)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_ResolveHost: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_ResolveHost error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		return 0;
	}

	GretaMaryInfoSocket=SDLNet_TCP_Open(&ip);
	if(!GretaMaryInfoSocket)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_TCP_Open: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_TCP_Open error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		return 0;
	}
	if(agent_log!=0)
		fprintf(agent_log,"MARY: info socket connected\n");

	std::string message="MARY IN="+in+" OUT="+out+" AUDIO="+audiotype+" VOICE="+voice+"\15\12";
	if(agent_log!=0)
		fprintf(agent_log,"MARY: sending %s",message.c_str());
	int len,result;
	len=message.length(); // add one for the terminating NULL
	result=SDLNet_TCP_Send(GretaMaryInfoSocket,(void*)message.c_str(),len);
	if(result<len)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_TCP_Send error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		Deconnect();
		return 0;
		// It may be good to disconnect sock because it is likely invalid now.
	}


	#define MAXLEN 10240
	char msg[MAXLEN];
	result=1;

	result=SDLNet_TCP_Recv(GretaMaryInfoSocket,msg,MAXLEN-1);
	if(result<=0)
	{
		Deconnect();
		return 0;
	}

	msg[result-1]=0;

	if(result!=len)
	{
		//printf("%s\n",msg);
	}
		
	id=msg;
	if(agent_log!=0)
		fprintf(agent_log,"MARY: received ID  %s\n",id.c_str());

	
	if(SDLNet_ResolveHost(&ip,host.c_str(),port)==-1)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_ResolveHost: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_ResolveHost error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		return 0;
	}

	GretaMaryDataSocket=SDLNet_TCP_Open(&ip);
	if(!GretaMaryDataSocket)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_TCP_Open: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_TCP_Open error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		Deconnect();
		return 0;
	}
	if(agent_log!=0)
		fprintf(agent_log,"MARY: data socket connected\n");

	std::string datasend=id+"\n";
	if(agent_log!=0)
		fprintf(agent_log,"MARY: sending %s",datasend.c_str());
	len=datasend.length(); // add one for the terminating NULL
	result=SDLNet_TCP_Send(GretaMaryDataSocket,(void*)datasend.c_str(),len);
	if(result<len)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_TCP_Send error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		Deconnect();
		return 0;
		// It may be good to disconnect sock because it is likely invalid now.
	}


	return 1;
}

void MaryInterface::Deconnect()
{
	SDLNet_Quit();
	SDL_Quit();
	if(agent_log!=0)
		fprintf(agent_log,"MARY: deconnected.\n");
	initialized=false;
}

int MaryInterface::SSMLToPhonemes(std::string input,std::string outputfilename)
{
	in  = "SSML";
	out = "ACOUSTPARAMS";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::SSMLToWave(std::string input,std::string outputfilename)
{
	in  = "SSML";
	out = "AUDIO";
	audiotype = "WAVE";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::SSMLToMp3(std::string input,std::string outputfilename)
{
	in  = "SSML";
	out = "AUDIO";
	audiotype = "MP3";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::MaryXMLToPhonemes(std::string input,std::string outputfilename)
{
	in  = "RAWMARYXML";
	out = "ACOUSTPARAMS";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::MaryXMLToWave(std::string input,std::string outputfilename)
{
	in  = "RAWMARYXML";
	out = "AUDIO";
	audiotype = "WAVE";

	
	if(Connect()==0)
		return 0;

	
	//TO DO:
	//receive a char* isted a file for a realtimeversion

	if(SendAndReceive(input,outputfilename)==0)
		return 0;

	

	Deconnect();
	return 1;
}

char * MaryInterface::MaryXMLToWaveInChar(std::string input, int *size)
{
	in  = "RAWMARYXML";
	out = "AUDIO";
	audiotype = "WAVE";
	char * output;

	if(Connect()==0)
		return 0;
	
	output=SendAndReceiveNoFile(input,size);
	
	Deconnect();
	return output;
}


int MaryInterface::MaryXMLToMp3(std::string input,std::string outputfilename)
{
	in  = "RAWMARYXML";
	out = "AUDIO";
	audiotype = "MP3";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::APMLToPhonemes(std::string input,std::string outputfilename)
{
	in  = "APML";
	out = "ACOUSTPARAMS";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::APMLToWave(std::string input,std::string outputfilename)
{
	in  = "APML";
	out = "AUDIO";
	audiotype = "WAVE";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::APMLToMp3(std::string input,std::string outputfilename)
{
	in  = "APML";
	out = "AUDIO";
	audiotype = "MP3";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::TextToPhonemes(std::string input,std::string outputfilename)
{
	if((voice=="slt-arctic")||(voice=="us2")||(voice=="us1"))
		in  = "TEXT_EN";
	else
		in  = "TEXT_DE";

	out = "ACOUSTPARAMS";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::TextToWave(std::string input,std::string outputfilename)
{
	if((voice=="slt-arctic")||(voice=="us2")||(voice=="us1"))
		in  = "TEXT_EN";
	else
		in  = "TEXT_DE";
	
	out = "AUDIO";
	audiotype = "WAVE";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

int MaryInterface::TextToMp3(std::string input,std::string outputfilename)
{
	if((voice=="slt-arctic")||(voice=="us2")||(voice=="us1"))
		in  = "TEXT_EN";
	else
		in  = "TEXT_DE";
	
	out = "AUDIO";
	audiotype = "MP3";
	if(Connect()==0)
		return 0;
	if(SendAndReceive(input,outputfilename)==0)
		return 0;
	Deconnect();
	return 1;
}

void MaryInterface::SetEnglish()
{
	if(sex=="female")
		//voice = "slt-arctic";
		voice = "us1";
	else
		voice = "us2";

	english=true;
	german=false;
	french=false;

}

void MaryInterface::SetGerman()
{
	if(sex=="female")
		voice = "de1";
	else
		voice = "de2";
	english=false;
	german=true;
	french=false;
}

void MaryInterface::SetFrench()
{
	if(sex=="female")
		voice = "french";
	else
		voice = "french";
	english=false;
	german=false;
	french=true;
}


void MaryInterface::SetFemale(int n)
{
	sex="female";
	if(voice=="us2")
		//voice="slt-arctic";
		voice = "us1";
	if(voice=="de2")
		voice="de1";
}

void MaryInterface::SetMale(int n)
{
	sex="male";
	if((voice=="slt-arctic")||(voice=="us1"))
		voice="us2";
	if(voice=="de1")
		voice="de2";
}

//ELISABETTA
int MaryInterface::SendAndReceive(std::string input,std::string outputfilename)
{


	int len,result;
	if(agent_log!=0)
		fprintf(agent_log,"MARY: sending %s",input.c_str());
	len=input.length(); // add one for the terminating NULL


	result=SDLNet_TCP_Send(GretaMaryDataSocket,(void*)input.c_str(),len);


	if(result<len)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_TCP_Send error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		Deconnect();
		return 0;
		// It may be good to disconnect sock because it is likely invalid now.
	}

	
	SDLNet_TCP_Shutdown(GretaMaryDataSocket);

	
	//if(Send(tosend)==1)
	//{
	std::string received;
	received="";
	result=1;
	char singlechar[1];

	FILE *outputfile;

	
	if(out!="AUDIO")
		outputfile=fopen(outputfilename.c_str(),"w"); //dichiaro una stringa
	else
		outputfile=fopen(outputfilename.c_str(),"wb"); //allocare spazio in memoria per binary (provare con una stringa)
	
	
	char msg[MAXLEN];

	//TO DO:
	//do not save in the file but memorize in char*

	while(result>0)
	{
		if(out!="AUDIO")
		{
			result=SDLNet_TCP_Recv(GretaMaryDataSocket,msg,MAXLEN-1);
			msg[result]=0;
			received+=msg;
			fprintf(outputfile,"%s",msg);
			// qui è meglio scrivere in una stringa e non in un file
		}
		else
		{
			result=SDLNet_TCP_Recv(GretaMaryDataSocket,singlechar,1);
			if(outputfile!=NULL)
				fputc(singlechar[0],outputfile);
			// qui è meglio aggiungere in coda il carattere alla stringa o allo spazio in memoria per binary
		}
	}
	
	//if(out!="AUDIO")
	//	printf("MARY: received\n %s\n",received.c_str());
	//SDLNet_TCP_Shutdown(GretaMaryDataSocket);

	 if(outputfile!=NULL)		fclose(outputfile);

	//}

	return 1;
}

//openmary output is memorized in char* instead of using wav files
/*
char * MaryInterface::SendAndReceiveNoFile(std::string input,int *licznik)
{

	int len,result;
	if(agent_log!=0)
		fprintf(agent_log,"MARY: sending %s",input.c_str());
	
	len=input.length(); // add one for the terminating NULL

	result=SDLNet_TCP_Send(GretaMaryDataSocket,(void*)input.c_str(),len);


	if(result<len)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_TCP_Send error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		Deconnect();
		return 0;
		// It may be good to disconnect sock because it is likely invalid now.
	}

	
	SDLNet_TCP_Shutdown(GretaMaryDataSocket);

	
	//if(Send(tosend)==1)
	//{
	std::string received;
	received="";
	result=1;
	char *singlechar;
	//char singlechar[1];

	singlechar = (char*) malloc (sizeof(char));				
	
	char msg[MAXLEN];
	
	char *output;
	
	output = (char*) malloc ( sizeof(char));
	
	while(result>0)
	{
		if(out!="AUDIO")
		{
			result=SDLNet_TCP_Recv(GretaMaryDataSocket,msg,MAXLEN-1);
			msg[result]=0;
			received+=msg;				
		}
		else
		{
			 result=SDLNet_TCP_Recv(GretaMaryDataSocket,singlechar,1);
			 output = (char*) realloc (output, (++(*licznik)) * sizeof(char));
			 output[(*licznik)-1]=(*singlechar);
			 
			 //output->append(1,singlechar[0]);			 					
		}
	}
	
		output = (char*) realloc (output, (++(*licznik)) * sizeof(char));
		output[(*licznik)-1]=EOF;
		
	return output;
}
*/


char * MaryInterface::SendAndReceiveNoFile(std::string input,int *licznik)
{
	int len,result;
	if(agent_log!=0)
		fprintf(agent_log,"MARY: sending %s",input.c_str());
	
	len=input.length(); // add one for the terminating NULL

	result=SDLNet_TCP_Send(GretaMaryDataSocket,(void*)input.c_str(),len);

	if(result<len)
	{
		if(agent_log!=0)
		{
			fprintf(agent_log,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			fprintf(agent_log,"SDLNet_TCP_Send error:\n");
			fprintf(agent_log,SDLNet_GetError());
			fprintf(agent_log,"\n\n");
		}
		Deconnect();
		return 0;
		// It may be good to disconnect sock because it is likely invalid now.
	}

	SDLNet_TCP_Shutdown(GretaMaryDataSocket);
	
	//if(Send(tosend)==1)
	//{
	std::string received;
	received="";
	result=1;
	
	char *singlechar;	
	singlechar = (char*) malloc (sizeof(char));				
	
	char msg[MAXLEN];
	
	char *output;
	
	output = (char*) malloc ( sizeof(char));
		
	//time_t seconds;
	//seconds = time (NULL);	 
	 
	
	//the old way of receiving the data from the openmary
/*

	 std::string tymczas="";	

	 while(result>0)
	{
		//this should not happen normally
		if(out!="AUDIO")
		{
			result=SDLNet_TCP_Recv(GretaMaryDataSocket,msg,MAXLEN-1);
			msg[result]=0;
			received+=msg;				
		}
		else
		{
			 result=SDLNet_TCP_Recv(GretaMaryDataSocket,singlechar,1);
			 output = (char*) realloc (output, (++(*licznik)) * sizeof(char));
			 output[(*licznik)-1]=(*singlechar);
			 
			 //output->append(1,singlechar[0]);			 					
		}
	}
	
		output = (char*) realloc (output, (++(*licznik)) * sizeof(char));
		output[(*licznik)-1]=EOF;
		very_long=*licznik;
	*/

	//questo e il nuovo modo di leggere audio
	//e molto veloce
	
	char *millechar;

	//you can try with higher values if you need it working faster
		
	int wynik=0;

	very_long_speech = (char*) malloc ( 1000 * sizeof(char));	
	millechar = (char*) malloc (1000 * sizeof(char));				

	while(result>0)
	{
		//this should not happen normally
		if(out!="AUDIO")
		{
			result=SDLNet_TCP_Recv(GretaMaryDataSocket,msg,MAXLEN-1);
			msg[result]=0;
			received+=msg;				
		}
		else
		{
			
			result=SDLNet_TCP_Recv(GretaMaryDataSocket,millechar,1000);			 
			 		
			int old_res=wynik;
			wynik = wynik+result;			
			very_long=wynik;
	
			very_long_speech = (char*) realloc (very_long_speech, very_long * sizeof(char));
	
			for (int i=old_res;i<wynik;i++)			 
				very_long_speech[i]=millechar[i-old_res];			 
		}
	}

	
	very_long_speech = (char*) realloc (very_long_speech, (very_long+2) * sizeof(char));
		
	very_long_speech[++very_long]=EOF;
		
	(*licznik)=very_long;	
	
	output=very_long_speech;
	

	return output;

	//TO use OUTPUT INSTEAD
}



void MaryInterface::APMLToSSML(XMLGenericTree *t)
{
	if(t!=0)
		if(SSMLfile!=0)
		{
			if(t->name!="text")
			{
					fprintf(SSMLfile,"<mark name=\"%s\"/>\n",t->name.c_str());
			}
			else
				fprintf(SSMLfile,"%s\n",t->value.c_str());
		}

	if(t!=0)
	{
		std::list<XMLGenericTree*>::iterator iter;
		if(!(t->child).empty())
			for(iter=t->child.begin();iter!=t->child.end();iter++)
				APMLToSSML((*iter));
	}

	if(t!=0)
		if(SSMLfile!=0)
		{
			if((t->name!="text")&&(t->name!="boundary"))
				fprintf(SSMLfile,"<mark name=\"/%s\"/>\n",t->name.c_str());
		}

}

int MaryInterface::Run(std::string inputfilename,std::string maryfilename,std::string wavefilename)
{
	std::string buffer;
	buffer="";
	std::string line;
	std::ifstream inputfile(inputfilename.c_str());
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			buffer=buffer+line;
		}
	}


	//printf("\n* Mary started *\n");
	//if you use preregistered file
	if(inimanager.GetValueInt("MARY_NATURAL_SPEECH")!=1)
	{
		if(this->APMLToPhonemes(buffer,maryfilename)==0)
		{
			printf("* Mary error! *\n");
			return 0;
		}
		if(this->APMLToWave(buffer,wavefilename)==0)
		{
			printf("* Mary error! *\n");
			return 0;
		}
	}//MARY_NATURAL_VOICE
	//printf("* Mary ended *\n");

	XMLGenericParser *p;
	XMLGenericTree *t;

	p=new XMLGenericParser();
	p->SetValidating(false);

	t=p->ParseFile(maryfilename);

	delete p;

	if(t!=0)
	{
		if(ExtractPhonemesFromMaryOutput(t)==0)
		{
			delete t;
			return 0;
		}
	}
	else
		return 0;

	ExtractAllPhonemes(filenames.Phonemes_File,t,0,0,0);


	delete t;

	return 1;
}

int MaryInterface::ExtractPhonemesFromMaryOutput(XMLGenericTree *t)
{
	FILE *phonemesfile;
	std::string phonemesfilename;
	t=t->FindNodeCalled("s");
	std::list<XMLGenericTree*>::iterator iter;
	int filecounter;
	filecounter=1;
	phonemesfilename="";
	phonemesfilename = "tmp/tmp";
	std::stringstream streamOut;
    streamOut << filecounter;
	phonemesfilename+=streamOut.str();
	phonemesfilename+=".pho";
	phonemesfile=fopen(phonemesfilename.c_str(),"w");
	if(english==true)
		fprintf(phonemesfile,"pau 0.200\n");
	else
		fprintf(phonemesfile,"_ 0.200\n");
	if(!t->child.empty())
	{
		if((*(t->child.begin()))->name=="prosody")
			t=*(t->child.begin());
		for(iter=t->child.begin();iter!=t->child.end();iter++)
		{
			if((*iter)->name!="phrase")
			{
				printf("error in the file returned by Mary\n");
				return 0;
			}
			std::list<XMLGenericTree*>::iterator tag;
			for(tag=(*iter)->child.begin();tag!=(*iter)->child.end();tag++)
			{
				if((*tag)->name=="mark")
				{
					filecounter++;
					fclose(phonemesfile);
					std::stringstream streamOut;
					streamOut << filecounter;
					phonemesfilename = "tmp/tmp";
					phonemesfilename+=streamOut.str();
					phonemesfilename+=".pho";
					phonemesfile=fopen(phonemesfilename.c_str(),"w");
					if(english==true)
						fprintf(phonemesfile,"pau 0.200\n");
					else
						fprintf(phonemesfile,"_ 0.200\n");
					while((tag!=(*iter)->child.end())&&((*tag)->name=="mark"))
						tag++;
					tag--;
				}
				else
				{
					ExtractAllPhonemes(phonemesfile,(*tag),0,0,0);
				}
			}
		}
		fclose(phonemesfile);
		printf("there are %d segments\n",filecounter);
	}
	return 1;
}

void MaryInterface::ExtractAllPhonemes(std::string filename,XMLGenericTree *t,std::list<MMSystemSpace::TimeMarker> *tml,std::vector<float> *stressingpoints,float *time)
{
	FILE *phonemesfile;
	phonemesfile=fopen(filename.c_str(),"w");
	if(english==true)
		fprintf(phonemesfile,"pau 0.001\n");
	if(french==true)
		fprintf(phonemesfile,"# 0.001\n");
	if(german==true)
		fprintf(phonemesfile,"_ 0.001\n");
		

	ExtractAllPhonemes(phonemesfile,t,tml,stressingpoints,time);
	fclose(phonemesfile);
}

void MaryInterface::ExtractAllPhonemes(FILE* f,XMLGenericTree *t,std::list<MMSystemSpace::TimeMarker> *tml,std::vector<float> *stressingpoints,float *time)
{
	if(t->name=="ph")
	{
		std::string finalphoneme;
		float duration;
		duration=t->GetAttributef("d");
		duration=duration/1000.0f;

		if(english==true)
			finalphoneme=SampaToFestival(t->GetAttribute("p"));
		else
			finalphoneme=t->GetAttribute("p");

		if((t->parent!=0)&&(stressingpoints!=0))
			if(t->parent->name=="syllable")
				if(t->parent->HasAttribute("stress"))
					if(t->parent->GetAttributef("stress")==1)
					{
						stressingpoints->push_back(*time+duration/2.0f);
						t->parent->SetAttribute("stress","0");
					}

		fprintf(f,"%s %.4f\n",finalphoneme.c_str(),duration);

		if(time!=0)
			*time=*time+duration;
	}
	if(t->name=="boundary")
	{
		if(t->GetAttribute("duration")!="")
		{
			float duration;
			duration=atof(t->GetAttribute("duration").c_str());
			duration=duration/1000.0f;
			if(english==true)
				fprintf(f,"pau %.4f\n",duration);
			if(french==true)
				fprintf(f,"# %.4f\n",duration);
			if(german==true)
				fprintf(f,"_ %.4f\n",duration);
			if(time!=0)
				*time=*time+duration;
		}
	}
	if(t->name=="mark")
	{
		if(tml!=0)
		{
			MMSystemSpace::TimeMarker tm;
			tm.id=t->GetAttribute("name");
			tm.time=*time;
			tm.concretized=true;
			tml->push_back(tm);
		}
	}
	std::list<XMLGenericTree*>::iterator iter;
	if(!t->child.empty())
		for(iter=t->child.begin();iter!=t->child.end();iter++)
		{
			ExtractAllPhonemes(f,(*iter),tml,stressingpoints,time);
		}
}


std::string MaryInterface::SampaToFestival(std::string p)
{
	if(p=="E")
		return "eh";
	if(p=="A")
		return "aa";
	if(p=="EI")
		return "ey";
	if(p=="V")
		return "ah";
	if(p=="@U")
		return "ow";
	if(p=="U")
		return "uh";
	if(p=="O")
		return "ao";
	if(p=="r=")
		return "er";
	if(p=="j")
		return "y";
	if(p=="u")
		return "uw";
	if(p=="T")
		return "th";
	if(p=="{")
		return "ae";
	if(p=="i")
		return "iy";

	if(p=="D")
		return "dh";
	if(p=="dZ")
		return "jh";
	if(p=="AI")
		return "ay";

	if(p=="aU")
		return "aw";
	if(p=="I")
		return "ih";
	if(p=="@")
		return "ax";
	if(p=="S")
		return "sh";
	if(p=="N")
		return "ng";


	return p;
}

