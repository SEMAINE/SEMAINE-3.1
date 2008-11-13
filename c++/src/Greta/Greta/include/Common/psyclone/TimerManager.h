/***************************** License **********************************

 Copyright (C) 2006 by Communicative Machines
 http://www.cmlabs.com   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

************************************************************************/


// TimerManager.h: interface for the TimerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERMANAGER_H__FD665152_A2CC_41FF_B61D_7E192A47B82D__INCLUDED_)
#define AFX_TIMERMANAGER_H__FD665152_A2CC_41FF_B61D_7E192A47B82D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "ObjectDictionary.h"
#include "Timer.h"
#include "JTime.h"

namespace cmlabs {

class TimerManager : public Object, public TimerObjectReceiver
{
public:
	TimerManager();
	virtual ~TimerManager();

	Object* clone() const {return NULL;}
	bool unitTest();
	
	//! Create a String Timer without callback
	JString createTimer(JString strData, long ms);
	//! Create an Object Timer without callback
	JString createTimer(Object* obj, long ms);
	//! Create a String Timer with a function callback
	JString createTimer(JString strData, long ms, void (*function)(JString id));
	//! Create an Object Timer with a function callback
	JString createTimer(Object* obj, long ms, void (*function)(Object* object));
	//! Create a String Timer with a TimerReceiver callback
	JString createTimer(JString strData, long ms, TimerReceiver* receiver);
	//! Create an Object Timer with a TimerObjectReceiver callback
	JString createTimer(Object* obj, long ms, TimerObjectReceiver* receiver);

	void doMaintenance();

	//! Cancel Timer
	bool cancel(JString id);
	//! Cancel all Timers
	bool cancelAll();

	//! Wait for Timer
	long waitFor(JString id);
	//! How long left of Timer
	long msLeft(JString id);

	//! Active Timer count
	int getActiveTimerCount();

	//! Get the active Timer IDs
	Collection getActiveTimerIDs();

	//! For testing only
	bool receiveTimerObject(Object* obj);

protected:
	ObjectDictionary* timerList;

	Timer* getTimer(JString id);
};

} // namespace cmlabs

#endif // !defined(AFX_TIMERMANAGER_H__FD665152_A2CC_41FF_B61D_7E192A47B82D__INCLUDED_)
