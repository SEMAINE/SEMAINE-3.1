// Clock.cpp: implementation of the Clock class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <time.h>
#include "ClockPsydule.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ClockPsydule::ClockPsydule(JString name, JString host, int port,JString GretaName) : Psydule(name, host, port)
{
	this->name=name;
	this->host=host;
	this->port=port;
	this->GretaName=GretaName;
	Reset();
}

ClockPsydule::~ClockPsydule()
{

}


void ClockPsydule::SendTime()
{
	int clock;

	clock=pc.GetTime();

	char msg[250];
	sprintf(msg,"%d", clock);
	
	clockstring=msg;

	this->PostString(clockstring,GretaName+PSYCLONE_GRETA_WHITEBOARD,GretaName+PSYCLONE_CLOCK_MESSAGE_HEADER); 

	sentat=clock;
}

void ClockPsydule::Reset()
{
	pc.SetTime(0);
	sentat=0;
	SendTime();
}

