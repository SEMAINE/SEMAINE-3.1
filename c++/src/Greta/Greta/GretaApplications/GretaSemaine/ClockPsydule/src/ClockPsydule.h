// CLOCK.h: interface for the CLOCK class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include <iostream>
#include <fstream>
#include <string>

#include "CentralClock.h"
#include "Psydule.h"
#include "../../../include/common/definitions.h"


class ClockPsydule : public Psydule
{
public:
	ClockPsydule(JString name, JString host, int port, JString GretaName);
	virtual ~ClockPsydule();

	void SendTime();

	void Reset();

	void setGretaName(JString gretaName);

	JString name,host,GretaName;
	int port;
	std::string clockstring;
	CentralClock pc;
	int sentat;
};
