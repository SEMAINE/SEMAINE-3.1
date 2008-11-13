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

#if !defined(AFX_JTHREAD_H__7D7254A3_4145_4D83_94DE_1343EB810C61__INCLUDED_)
#define AFX_JTHREAD_H__7D7254A3_4145_4D83_94DE_1343EB810C61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JSemaphore.h"
#include "ObjectDictionary.h"
#include "JFile.h"
#include "Debug.h"
#include "XMLParser.h"
#include <math.h>

#ifdef WIN32
	#include <windows.h>
	#ifdef _WIN32_WCE
		#include <winsock2.h>
	#else
		#include <process.h>
	#endif
	#if defined(CYGWIN)
		#include <sys/time.h>
	#endif
	#if !defined(PROCESSOR_ARCHITECTURE_AMD64)
		#define PROCESSOR_ARCHITECTURE_AMD64  9
	#endif
	#if !defined(PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
		#define PROCESSOR_ARCHITECTURE_IA32_ON_WIN64  10
	#endif
	#include "psapi.h"
#else
#ifndef Darwin
	#include <execinfo.h>
#endif // Darwin
	#include <errno.h>
	#include <pthread.h>
	#include <signal.h>
	#include <sys/times.h>
	#include <limits.h>
	#include <sys/resource.h>
	#include <unistd.h>
	#undef HANDLE
	#define HANDLE pthread_t*
#endif

#include "Object.h"
#include "JString.h"
#include "JTime.h"
// #include <thread.h>


namespace cmlabs {

class SysInfo : public Object {
public:
	SysInfo();
	virtual ~SysInfo();

	virtual Object* clone() const;
	virtual bool equals(const Object* o2) const;
	virtual JString print();
	virtual JString toHTML();
	virtual JString toXML();
	virtual JString toXML(JString tag, JString content);
	virtual bool fromXML(XMLNode* node);

	virtual bool clear();
	virtual bool update();

	int getOSVersionMajor();
	int getOSVersionMinor();
	int getOSVersionSub();
	int getOSVersionBuild();
	JString getOSVersionText();

	JString osName;
	JString osType;
	JString osVersionString;
	double osTotalMemory;
	double cpuSpeed;
	int cpuCount;
	JString cpuArchitecture;
	JString hostname;

//	double procMemoryUsage;
	double procAverageCPUUsageKernel;
	double procAverageCPUUsageUser;
	double procAverageCPUUsage;
	double procCurrentCPUUsageKernel;
	double procCurrentCPUUsageUser;
	double procCurrentCPUUsage;
};

class ThreadStat : public Object {
public:
	ThreadStat();
	Object* clone() const;

	struct timeval timeLast;
	longlong userLast;
	longlong kernelLast;

	longlong realTimeElapse;
	longlong cpuUserUsage;
	longlong cpuKernelUsage;
	longlong cpuUsage;
	double percentKernelCPU;
	double percentUserCPU;
	double percentCPU;

	bool isEmpty();
	JString print();
};

class JMutex;
class Message;
class PerfStat : public Object  
{
public:
	PerfStat();
	PerfStat(JString xml);
	PerfStat(XMLNode* node);
	PerfStat(const PerfStat &p);
	virtual ~PerfStat();
	PerfStat& operator=(const PerfStat &p);
	Object* clone() const;
	bool fromXML(XMLNode* node);
	JString toXML();
	JString print();
	char* toBinary(int* len);
	bool fromBinary(char* data, int len);

	bool isOlderThan(long ms);

	JString name;
	JTime created;
	JTime lastUpdated;

	long runCount;
	long msgWoken;
	long msgStored;
	long msgPosted;
	long msgReceived;
	long msgSent;
	long msgProcessed;
	long msgMax;

	double totalRunSeconds;
	double totalCPUSeconds;
	double deltaSeconds;
	double deltaRunSeconds;
	double deltaCPUSeconds;
	double currentCPUUsage;
	double currentSystemUsage;
	double averageSystemUsage;
	double totalSystemUsage;

	long streamsCount;
	long streamsTotalSize;
	double streamsTotalSizeEver;
	double streamTotalInRate;
	double streamTotalOutRate;

	Message* lastMessageTrigger;
	Message* lastMessagePosted;
	
	// Will clone object
	bool setLastMessageTrigger(const Message* msg);
	// Will clone object
	bool setLastMessagePost(const Message* msg);

protected:
	JMutex mutex;


};


class JThread
{
public:
	JThread(bool start = false, int pri = 0);
	virtual ~JThread();

	virtual void run();
	bool start();
	
	bool join();
	bool terminate();
	bool suspend();
	bool resume();

	bool waitThread(long ms);

	bool isRunning();
	bool isThread();

	JString printThreadStatistics(ThreadStat newStat);

	//! get stat on local thread
	ThreadStat getLocalThreadStatistics();
	//! reset stat on local thread
	bool resetLocalThreadStatistics();
	//! print stat on local thread
	JString printLocalThreadStatistics();
	
	//! get stat on own (calling) thread
	ThreadStat getCallingThreadStatistics();
	//! reset stat on own (calling) thread
	bool resetCallingThreadStatistics();
	//! print stat on own (calling) thread
	JString printCallingThreadStatistics();

	ThreadStat getCurrentCPUUsage();
	ThreadStat getAverageCPUUsage();

	ThreadStat getCPUUsage(HANDLE aThread);

	int getPriority();
	bool setPriority(int pri);

	bool signalThread(int signo);
	bool waitSignal(int signo);
	void yield();

//	bool setTimer(unsigned long ms);
//	unsigned long getTimer();
//	bool endTimer();

	bool unitTest();
	JString unitTestString;
	bool priorityTest();

	JString name;
	JSemaphore* publicSemaphore;
	JSemaphore* testSemaphore;
	JMutex* testMutex;

	JMutex resetMutex;

	bool linuxThreadHasFinished;

	// Public OS Information
	JString osGetNameVersion();
	JString osGetName();
	JString osGetVersion();
	JString osGetMachineArch();
	JString osGetMachineName();
	JString osGetMachineAddress();
	JString osGetType();
	double osGetTotalMemory();
	double osGetCPUSpeed();
	int osGetCPUCount();

	JTime getStartTime();
	JTime getEndTime();

	ThreadStat procAverageCPUUsage();
	ThreadStat procCurrentCPUUsage();
	JString getComputerName();
	int procMemUsage();

	bool addToAllThreadsStats(HANDLE aThread);
	bool addToAllThreadsStats(HANDLE aThread, ThreadStat* stat);
	ThreadStat getAllThreadsStats();
	bool removeFromAllThreadsStats(HANDLE aThread);

	JString runOSTextCommand(JString cmd);

	static Collection getProgramTrace();

	JString localhostName;
	JTime threadStartTime, threadEndTime;
	HANDLE hThread;


protected:

	#ifdef WIN32
		// LARGE_INTEGER usecFreq;
		double usecFreq;
	#else
		double usecFreq;
	#endif // WIN32


	int priority;
	ThreadStat stat;
	ThreadStat procLastStat;
	ThreadStat procStartStat;

	int convPriorityToOS(int pri);
	int convPriorityToInternal(int pri);

	ObjectDictionary statDict;
	ThreadStat getCPUUsageOnThread(HANDLE aThread);
	bool resetCPUUsageOnThread(HANDLE aThread);
	ThreadStat getStoredCPUUsageOnThread(HANDLE aThread);

	HANDLE getCallingThreadID();

//	virtual void OnTimer();
//	virtual void OnException();
//	virtual void OnSignal(int signo); 

private:
};

#ifdef WIN32
	static DWORD WINAPI staticRunThread(LPVOID lpParam);
#else
	void* staticRunThread(void* arg);
#endif

} // namespace cmlabs

#include "JSemaphore.h"

namespace cmlabs {

class JThreadTester : public JThread
{
public:
	JThreadTester(int pri);
	~JThreadTester();

	void run();
	int ver;
};

} // namespace cmlabs

#endif // !defined(AFX_JTHREAD_H__7D7254A3_4145_4D83_94DE_1343EB810C61__INCLUDED_)
