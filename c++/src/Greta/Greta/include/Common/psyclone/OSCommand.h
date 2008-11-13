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


// OSCommand.h: interface for the OSCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OSCOMMAND_H__1682DCA5_6086_49F9_8026_B40B6B373A56__INCLUDED_)
#define AFX_OSCOMMAND_H__1682DCA5_6086_49F9_8026_B40B6B373A56__INCLUDED_

#include "Debug.h"

#ifndef WIN32
#ifdef _WIN32
#define WIN32
#endif
#endif

#ifdef WIN32
	#include <windows.h>
	#define popen _popen
	#define pclose _pclose
#else
	#include <errno.h>
	#include <sys/wait.h>
	#include <fcntl.h>
#endif

#include "Object.h"
#include "JString.h"
#include "JTime.h"
#include "Collections.h"

namespace cmlabs {

class OSCommand : public Object  
{
public:
	OSCommand();
	OSCommand(const JString& cmdline);
	~OSCommand();

	Object* clone() const {return NULL;}
	bool unitTest();

	bool setBinary();
	bool setASCII();
	JString getCommand();

	bool start();
	bool terminate();

	// Queries
	bool hasBeenStarted();
	bool isRunning();
	bool hasTerminated();
	bool waitForProcessToFinish(int timeout);
	int getProcID();
	

	// Standard Input, Output and Error
	bool writeToStandardInput(const JString& text);
	bool writeToStandardInput(char* bytes, int len);

	bool hasOutput();
	int getOutputCharCount();
	bool hasErrors();
	int getErrorCharCount();
	int getReturnCode();

	JString getCurrentOutput();
	JString getCurrentError();
	JString getNewOutput();
	JString getNewError();

	JString readOutputLine(int timeout);
	Collection readCurrentLines();
	char* readOutputBytes(int length, int* readcount, int timeout = 0);

	JString waitForProcessToFinishAndGetOutput(int timeout);
	JString waitForProcessToFinishAndGetError(int timeout);

	JTime getStartedTime();
	JTime getEndedTime();

	JString alias;
	bool consoleOutput;
	JString status;

private:
	JString command;
	int procID;
	int returncode;
	bool binary;

	bool init();
	int readIntoBuffers();
	bool resizeOutputBuffer(int max);
	bool resizeErrorBuffer(int max);
	bool cleanUp();

	char* outbuffer;
	int outbuffercount;
	int outbuffermax;
	char* errbuffer;
	int errbuffercount;
	int errbuffermax;

	JString outputHistory;
	JString errorHistory;
	JTime startedTime;
	JTime endedTime;

	// OS specific stuff...
	#ifdef WIN32
		PROCESS_INFORMATION procInfo;
		HANDLE proc_stdin;
		HANDLE proc_stdout;
		HANDLE proc_stderr;
	#else //WIN32
		FILE* proc_stdin;
		FILE* proc_stdout;
		FILE* proc_stderr;

		int stdin_pipe[2];
		int stdout_pipe[2];
		int stderr_pipe[2];
	#endif //WIN32
	




};

} // namespace cmlabs

#endif // !defined(AFX_OSCOMMAND_H__1682DCA5_6086_49F9_8026_B40B6B373A56__INCLUDED_)
