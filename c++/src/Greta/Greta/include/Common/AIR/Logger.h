/***************************** License ************************************

  Copyright (c) 2005 by Communicative Machines
  http://www.cmlabs.com   All rights reserved.
  
  This library is released under a 
  Communicative Machines Public Binary Library (PBL) License version 1.0
  which should be included with the distribution that included this file.

  The library is free software in the sense that you can use and 
  redistribute it under the terms of the PBL as published by Communicative 
  Machines; http://www.cmlabs.com/licenses/pbl/

  Information about OpenAIR is available at
  http://www.mindmakers.org/openair

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**************************************************************************/


// Logger.h: interface for the Logger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_H__3214601F_B0B0_48D3_9DB1_44FB19D0FB8E__INCLUDED_)
#define AFX_LOGGER_H__3214601F_B0B0_48D3_9DB1_44FB19D0FB8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"

namespace cmlabs {


class LogEntry : public Object {
public:
	LogEntry(int loglevel, JString valfrom, JString valorigin, JString valcontent);
	LogEntry(Message* msg);
	Object* clone() const;
	JString print() ;
	JString printMSOnly();

	int count;
	int level;
	JString component;
	JString origin;
	JString content;
	JTime time;
};

class Logger : public Communicator  
{
public:
	Logger(const TCPLocation& cnsServer);
	virtual ~Logger();

	void run();

	Message* receiveMsg(Message* msg);

	HTMLPage* serveHTTPRequest(const JString& origin, HTTPRequest* request);
	Message* serveTelnetInput(const JString& id, const JString& origin, const JString& text);

private:
	JString printEntry(LogEntry* entry, JString bgcolor, JString font);
	JString getHTMLFrameSet();
	JString getHTMLTop();

	ObjectCollection entries;
	JTime lastConsolePrint;
	int count;
};

} // namespace cmlabs

#endif // !defined(AFX_LOGGER_H__3214601F_B0B0_48D3_9DB1_44FB19D0FB8E__INCLUDED_)
