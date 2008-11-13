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


// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "JThread.h"
// #include <thread.h>
#include "JTime.h"

#if !defined(AFX_TIMER_H__23E586ED_E9EF_4089_9235_B98DDD7C1BA6__INCLUDED_)
#define AFX_TIMER_H__23E586ED_E9EF_4089_9235_B98DDD7C1BA6__INCLUDED_

namespace cmlabs {

class TimerReceiver
{
public:
	virtual bool receiveTimerString(JString str) = 0;
};
class TimerObjectReceiver
{
public:
	virtual bool receiveTimerObject(Object* obj) = 0;
};


class Timer : public JThread, public Object
{
public:
	Timer(JString strData, long ms);
	Timer(Object* obj, long ms);
	Timer(JString strData, long ms, void (*function)(JString string));
	Timer(JString strData, long ms, TimerReceiver* receiver);
	Timer(Object* obj, long ms, void (*function)(Object* object));
	Timer(Object* obj, long ms, TimerObjectReceiver* receiver);
	virtual ~Timer();

	Object* clone() const {return NULL;}
	void run();
	void cancel();
	long msLeft();
	long waitFor();
	bool hasEnded();
	long msSinceEnded();

	Object* getAndRemoveObject();


protected:

	JString data;
	Object* object;
	JTime startedAt;
	JTime endedAt;

	void (*stringCallback)(JString strData);
	void (*objectCallback)(Object* obj);
	TimerReceiver* timerReceiver;
	TimerObjectReceiver* timerObjectReceiver;

	JSemaphore extSem;
	JSemaphore timerSem;
	bool shouldContinue;
	long msRuntime;

	void OnTimer();
};

} // namespace cmlabs



#endif // !defined(AFX_TIMER_H__23E586ED_E9EF_4089_9235_B98DDD7C1BA6__INCLUDED_)
