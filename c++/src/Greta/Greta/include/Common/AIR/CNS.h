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


// CNS.h: interface for the CNS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CNS_H__B27A93AB_444D_449A_AF21_BF12DBC63DF2__INCLUDED_)
#define AFX_CNS_H__B27A93AB_444D_449A_AF21_BF12DBC63DF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"

// #define CNSSTARTPORT 2000

namespace cmlabs {


class CNS : public Communicator  
{
public:
	CNS(const TCPLocation& cnsloc);
	virtual ~CNS();

	bool init();
	void run();

	bool unitTest();
	
	Message* receiveMsg(Message* msg);

	HTMLPage* serveHTTPRequest(const JString& origin, HTTPRequest* request);
	Message* serveTelnetInput(const JString& id, const JString& origin, const JString& text);

	bool setWebInterface(Communicator* webInterfaceServer);
	bool unbind(JString cname);
	bool unbind(TCPLocation* loc);

private:

	JMutex cnsMutex;
	Dictionary lastPort;
	Communicator* webServer;

	Message* componentUnbind(Message* msg);
	Message* componentBind(Message* msg, bool isCommunicator = true);
	Message* lookupComponent(JString name, JString from);
	int getNextAvailablePortOnAddress(JString addr);
	Message* listComponents(JString from);

	bool isAuthentic(TCPLocation *loc);
	void dumpCNSCache();

	// HTML Page Generators
	HTMLPage* htmlSystemOverview(HTTPRequest* request);
	HTMLPage* htmlComponentStatus(JString comp);

	ObjectDictionary* auxCache;
	Collection remoteIPs;
	Collection usedComponentNames;
};

} // namespace cmlabs

#endif // !defined(AFX_CNS_H__B27A93AB_444D_449A_AF21_BF12DBC63DF2__INCLUDED_)
