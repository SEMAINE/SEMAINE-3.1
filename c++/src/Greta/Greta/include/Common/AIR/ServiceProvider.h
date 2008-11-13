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


//////////////////////////////////////////////////////////////////////
// ServiceProvider.h: interface for the ServiceProvider class.
//
//////////////////////////////////////////////////////////////////////

#include "CppAIRPlug.h"
#include "DynamicLoader.h"
#include "ServiceClient.h"

namespace cmlabs {

#if !defined(_SERVICEPROVIDER_H_)
#define _SERVICEPROVIDER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ServicePlug;

typedef int (* DLLStringCheck)(JString challenge, JString response, JString truthbase);
typedef int (* DLLServiceRun)(JString id, JString name, ClientProfile* profile, ServicePlug* servicePlug);

class ServiceProvider : public CppAIRPlug
{
public:
	ServiceProvider(JString name, TCPLocation cnsServer);
    virtual ~ServiceProvider();

	virtual bool init();
	bool init(ServiceSpec* spec, XMLNode* setup);
	virtual void run();

	virtual Message* acceptMessage(Message* hmsg);
	bool postMessage(Message* msg);

	bool endConversation(JString convID);

	virtual bool connectNotify(const JString& name, NetworkConnection* con);
	virtual bool disconnectNotify(const JString& name, NetworkConnection* con);
	virtual bool primaryReconnectNotify();
	virtual bool primaryDisconnectNotify();
	bool disconnectNotify(const JString& name);

	DynamicLoader loader;

private:

	bool doMaintenance();
	bool sendHeartbeat();
	bool reportStartConversation(JString convID, ClientProfile* client);
	bool reportEndConversation(JString convID);
	bool reconnectConversation(JString convID);
	bool parseConfiguration();
	bool parseConfiguration(ServiceSpec& serviceSpec, XMLNode* setup);
	bool verifyAuthentication(JString convID, JString authCode);
	ObjectCollection* calculateOutputMessages(Message* msg);
	PhaseSpec* getCurrentPhaseSpec();
	XMLNode* setupNode;
	XMLParser setupParser;

	Message* createConversation(JString convID, ClientProfile* profile);
	
	ServiceSpec serviceSpec;
	ObjectDictionary conversations;
	ObjectDictionary servicePlugs;
	ObjectDictionary oldPlugs;
	Dictionary clientsConvID;
	ObjectDictionary disconnectedConvIDs;

	JString currentContext;
	int currentPhaseNum;
	int clientTimeout;
};

#endif // _SERVICEPROVIDER_H_
} // namespace cmlabs
