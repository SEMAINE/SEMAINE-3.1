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


#include "CentralClock.h"
#include "Timer.h"

CentralClock::CentralClock()
{
	SetTime(0);
}


CentralClock::~CentralClock()
{
}

void CentralClock::SetTime(int t)
{
	synchronizedat=(int)(((float)clock()/CLOCKS_PER_SEC)*1000);
	externaltime=t;
	time=(int)(((float)clock()/CLOCKS_PER_SEC)*1000)-synchronizedat+externaltime;
	time=(time/40)*40;
}
	

int CentralClock::GetTime()
{
	time=(int)(((float)clock()/CLOCKS_PER_SEC)*1000)-synchronizedat+externaltime;
	time=(time/40)*40;
	return time;
}