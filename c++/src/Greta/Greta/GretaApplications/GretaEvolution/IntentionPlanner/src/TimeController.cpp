#include "GretaReactiveBehaviour.h"
#include "DataContainer.h"
#include "TimeController.h"


TimeController::TimeController():JThread()
{
}

TimeController::TimeController(GretaReactiveBehaviour *grb):JThread()
{
	this->grb=grb;
	timelimit=4000;
	bcrequest=0;
}


TimeController::~TimeController()
{
}

void TimeController::run()
{
	int starttime, currenttime;

	starttime=grb->getpCentralClock()->GetTime();

	while(1)
	{
		currenttime=grb->getpCentralClock()->GetTime();
		if(bcrequest==1)
		{
			//printf("manda bc richiesto\n");
			starttime=currenttime;
			bcrequest=0;
		}

	//	if (currenttime-starttime>=timelimit)
	//		grb->getReactiveBackchannelSender().execute();
			//calcola probabilità per mandare un ping
	 
		// Don't overload processor
		Sleep(5);
	}
}

void TimeController::SetBCrequest()
{
	bcrequest=1;
}
