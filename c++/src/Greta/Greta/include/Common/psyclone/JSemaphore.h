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


// JThread.h: interface for the JThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JSEMAPHORE_H__7D7254A3_4145_4D83_94DE_1343EB810C61__INCLUDED_)
#define AFX_JSEMAPHORE_H__7D7254A3_4145_4D83_94DE_1343EB810C61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WIN32
#ifdef _WIN32
#define WIN32
#endif
#endif

#ifdef WIN32
	#include <windows.h>
	#if !defined(_WIN32_WCE)
		#include <process.h>
	#endif
#else
	#include <pthread.h>
	#include <signal.h>
	#include <sys/time.h>
	#include <unistd.h>
	#undef HANDLE
	#define HANDLE pthread_cond_t*
#endif

#include "Object.h"
#include "JString.h"
// #include <thread.h>

namespace cmlabs {

class JThread;

class JSemaphore : public Object
{
public:
	JSemaphore(JString semName = "");
	~JSemaphore();
	Object* clone() const { return NULL; }

	int count;
	JString name;

	bool wait(long ms = -1);
	bool post();
	bool tryWait();
	bool unitTest();

private:
	
	HANDLE hSemaphore;
	#ifndef WIN32
		pthread_mutex_t* mutex;
	#endif
};

class JSingleSemaphore : public Object
{
public:
	JSingleSemaphore();
	~JSingleSemaphore();
	Object* clone() const { return NULL; }

	bool wait(long ms = -1);
	bool postToAll();
	bool tryWait();

private:
	
	HANDLE hSemaphore;
	#ifndef WIN32
		pthread_mutex_t* mutex;
	#endif
};

class JMutex : public Object
{
public:
	JMutex();
	~JMutex();
	Object* clone() const { return NULL; }

	// Try to lock the mutex
	bool enterMutex(long ms = -1) const;
	// Try to unlock the mutex
	bool leaveMutex() const;
	// For backward compatibility
	bool EnterMutex(long ms = -1) const;
	// For backward compatibility
	bool LeaveMutex() const;

private:

	#ifdef WIN32
		HANDLE  mutex;
	#else
		pthread_cond_t* semaphore;
		pthread_mutex_t* semaphoremutex;
		pthread_mutex_t* mutex;
	#endif
};

class WriteAccessMutex : public Object
{
public:
	WriteAccessMutex();
	~WriteAccessMutex();
	Object* clone() const { return NULL; }

	bool gainReadAccess(long ms = -1);
	bool releaseReadAccess();

	bool gainWriteAccess(long ms = -1);
	bool releaseWriteAccess();

private:

	int currentReadCount;
	JMutex accessMutex;
};

#ifndef WIN32
	bool calcTimeout(struct timespec &timeout, struct timeval &now, long ms);
#endif

} // namespace cmlabs

#include "JThread.h"




#endif // !defined(AFX_JSEMAPHORE_H__7D7254A3_4145_4D83_94DE_1343EB810C61__INCLUDED_)
