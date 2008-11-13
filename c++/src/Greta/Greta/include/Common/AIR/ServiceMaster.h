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


// ServiceMaster.h: interface for the ServiceMaster class.
//
//////////////////////////////////////////////////////////////////////

#include "DynamicLoader.h"
#include "Component.h"
#include "ServiceProvider.h"

namespace cmlabs {

#if !defined(_SERVICEMASTER_H_)
#define _SERVICEMASTER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXCHALLENGERETRIES 2

class ServiceMaster : public Component
{
public:
	ServiceMaster(const TCPLocation& cnsServer);
	virtual ~ServiceMaster();

	bool init();
	void run();

	Message* receiveMsg(Message* msg);

	JString runAIRServicesTest();

	HTMLPage* serveHTTPRequest(const JString& origin, HTTPRequest* request);
	Message* serveTelnetInput(const JString& id, const JString& origin, const JString& text);

private:

	bool parseConfiguration();
	JString getHTML();
	JString getHTMLConversation(JString convID);

	Message* login(JString from, JString service, ClientProfile* profile);
	Message* login(JString from, JString service, ClientProfile* profile, JString convID);
	Message* response(JString from, ClientProfile* profile, JString convID, JString challenge = "", JString responseText = "");

	JString generateConversationID(JString service);
	JString generateAuthentication(JString convID);

	Message* generateChallenge(JString challenge, JString from, ClientProfile* profile, JString convID);
	Message* generateRechallenge(JString challenge, JString from, ClientProfile* profile, JString convID);
	Message* generateAuthorisationMessage(JString from, JString service, ClientProfile* profile, JString convID);

	TCPLocation findAvailableService(JString serviceName);
	bool checkResponse(JString challenge, JString responseText, JString truth);
	bool checkResponseLibrary(JString challenge, JString responseText, JString libraryName);

	Message* receiveHeartbeat(JString from, ObjectDictionary* heartbeat);

	Dictionary servicelist;
	ObjectDictionary providers;
	ObjectDictionary heartbeats;
	ObjectDictionary convbeats;

	ObjectDictionary standardChallenges;
	ObjectDictionary currentChallenges;
	ObjectDictionary failedChallenges;

	ObjectDictionary currentConversations;
	ObjectDictionary previousConversations;

	int maxOldConversations;

	DynamicLoader libraryLoader;
};

#endif // _SERVICEMASTER_H_
} // namespace cmlabs
