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


// Psydule.cpp: implementation of the Psydule class.
//
//////////////////////////////////////////////////////////////////////

#include "Psydule.h"

//extern FileNames filenames;
//extern Uint32 wav_length;
//extern Uint8 *wav_buffer;
//extern int very_long;
//extern char* very_long_speech;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Psydule::Psydule(JString name, JString host, int port) : CppAIRPlug(name, host, port)
{
	this->name=name;
	this->port=port;
}

Psydule::~Psydule()
{
}

int Psydule::Register(JString whiteboard, JString datatype)
{
	std::list<JString> types;
	types.push_back(datatype);
	return Register(whiteboard,types);
}

int Psydule::RawRegister(JString rawregistrationstring)
{
	if (!this->init())
	{
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name.c_str());
		return 0;
	}
	if (this->isServerAvailable())
	{
		this->sendRegistration(rawregistrationstring);
		this->start();
	}
	else {
		printf("Could not connect to Psyclone for registering %s...\n\n", name.c_str());
		return 0;
	}
	printf("Psyclone: Connected to whiteboard\n");
	return 1;
}

int Psydule::Register(JString whiteboard, std::list<JString> datatypes)
{

	if (!this->init()) {
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name.c_str());
		return 0;
	}

	JString xml = "\
	   <module name=\"";
	   xml+=this->name;
	   xml+="\">\
	      <spec>\
	         <context name=\"Psyclone.System.Ready\">\
	            <phase id=\"";
	   xml+=this->name;
	   xml+=".MainPhase\">\
					<triggers>\
					";
					std::list<JString>::iterator iter;	
					for(iter=datatypes.begin();iter!=datatypes.end();iter++)
					{
						xml+="<trigger from=\"";
						xml+=whiteboard;
						xml=xml+"\" type=\"" + (*iter) + "\"/>\
							";
					}
					xml=xml+"</triggers>\
				</phase>\
	         </context>\
	      </spec>\
	   </module>\
	";

	if (this->isServerAvailable())
	{
		this->sendRegistration(xml);
		this->start();
	}
	else {
		printf("Could not connect to Psyclone for registering %s...\n\n", name.c_str());
		return 0;
	}
	printf("Psyclone: Connected to %s\n", whiteboard.c_str());
	return 1;

}

Message* Psydule::ReceiveMessage(int timeout)
{
	Message* triggerMessage;

	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name);
		return 0;
	}
	if ((triggerMessage = this->waitForNewMessage(timeout)) != NULL)
	{
		return triggerMessage;
	}
	else
		return 0;
}

/*
ObjectCollection* Psydule::ReceiveAllMessages()
{
	ObjectCollection* triggerMessages;

	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name);
		return 0;
	}
	if ((triggerMessages = this->getAllRetrievedMessages()) != NULL)
	{
		return triggerMessages;
	}
	else
		return 0;
}*/

JString Psydule::ReceiveString(int timeout)
{
	Message* triggerMessage;


	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name);
		return 0;
	}
	if ((triggerMessage = this->waitForNewMessage(timeout)) != NULL)
	{
		JString content;
		content=triggerMessage->getContent();
		return content;
	}
	else
		return "";
}

int Psydule::PostFile(std::string filename, JString whiteboard, JString datatype)
{
	std::string line;
	JString tosend="";

	std::ifstream inputfile(filename.c_str());
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
	{
		printf("File %s not found\n", filename.c_str());
		return 0;
	}
	inputfile.close();

	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name);
		return 0;
	}

	this->postMessage(whiteboard,datatype,tosend,"","");

	return 1;
}

int Psydule::PostString(std::string tosend, JString whiteboard, JString datatype)
{
	if (!this->isServerAvailable()) {
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name);
		return 0;
	}
	
	this->postMessage(whiteboard,datatype,tosend.c_str(),"","");

	return 1;
}

int Psydule::PostSpeechFromChar(char* tosend, JString whiteboard, JString datatype, int *size)
{
	if (!this->isServerAvailable()) 
	{
		printf("Could not connect to Psyclone for registering %s...\n\n", this->name);
		return 0;
	}
	
	//if there is no speech for some reason
	//if ( (*size)==0) return 0; 

	DataSample *ds;

	ds=new DataSample("Raw","");
	
	//TO DO : CHANGE IT
	ds->size=(*size);	
	ds->data=tosend;

	//ds->data=very_long_speech;
	//ds->size=very_long;		

	Message* msg = new Message(this->name, whiteboard, datatype);

	msg->object=ds;

	this->postMessage(whiteboard,msg,"");
	
	return 1;
} 


int Psydule::SendFAPs(std::string fapfilename, JString whiteboard, JString datatype)
{
	this->PostFile(fapfilename,whiteboard,datatype);
	return 1;
}

int Psydule::SendBAPs(std::string bapfilename, JString whiteboard, JString datatype)
{
	this->PostFile(bapfilename,whiteboard,datatype);
	return 1;
}



////////////////////////////////////////////////////////////////////////

/*
int Psydule::ReceiveWAV(Message *msg)
{

	DataSample *ds;

	if(msg->object!=0)
		{
			ds=(DataSample*)msg->object;

			wav_buffer = (Uint8 *) malloc(ds->size);
			wav_length = ds->size;
			memcpy(wav_buffer, ds->data, ds->size); 
		}
		else
		{
			printf("ERROR: received empty wavefile!\n");
			return 0;
		}
		
	}

return 1;
}

*/
///////////////////////////////////////////////////////////////////////

//TO DO: ad a methof send CHAR*


int Psydule::SendBinaryFile(std::string binfilename, JString whiteboard, JString datatype)
{
	std::string line;
	JString tosend="";
	int size;
	
	FILE *inputfile;

	inputfile=fopen(binfilename.c_str(),"rb");

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
	{
		printf("Binary file %s not found\n", binfilename.c_str());
		return 0;
	}

	fclose(inputfile);
	
	ds->size=i;

	Message* msg = new Message(this->name, whiteboard, datatype);
	
	msg->object=ds;
	
	this->postMessage(whiteboard,msg,"");

	//delete ds->data;
	
	return 1;
}

int Psydule::WriteBinaryFile(std::string filename, Message *msg)
{
	//save data to a wave file
	FILE *f;
	f=fopen(filename.c_str(),"wb");
	if(f!=0)
	{
		DataSample *ds;
		if(msg->object!=0)
		{
			ds=(DataSample*)msg->object;
			for(int i=0;i<ds->size;i++)
				fputc(ds->data[i],f);
			//sprintf((char*)agent->wav_filename.c_str(),filename.c_str());
		}
		else
		{
			printf("ERROR: received empty wavefile!\n");
			return 0;
		}
		fclose(f);
	}
	else
		return 0;
	return 1;
}
