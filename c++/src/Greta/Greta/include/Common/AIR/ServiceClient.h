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
// ServiceClient.h: interface for the ServiceClient class.
//
//////////////////////////////////////////////////////////////////////

#include "CppAIRPlug.h"
#include "Specs.h"

namespace cmlabs {

#if !defined(_SERVICECLIENT_H_)
#define _SERVICECLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ServiceClient : public CppAIRPlug
{
public:
	ServiceClient(JString name, TCPLocation cnsServer, MessageAcceptor* messageAcceptor, int timeout);
    virtual ~ServiceClient();

	virtual void run();
	virtual bool init();
	virtual Message* acceptMessage(Message* msg);

	Dictionary getListOfServices(JString type = "");
	bool login(JString service, JString oldConvID = "");
	bool isConnectedToService();
	bool logout();
	bool checkConnectionToService();
	bool reconnectToService();
	bool reconnectToConversation();

	bool serviceConversationTest();

	bool startLogin(JString service, JString oldConvID = "");
	bool startLogout(JString convID);
	bool setTimeout(int timeout);

	bool sendToService(Message* msg);

	virtual bool connectNotify(const JString& name, NetworkConnection* con);
	virtual bool disconnectNotify(const JString& name, NetworkConnection* con);
	virtual bool primaryReconnectNotify();
	virtual bool primaryDisconnectNotify();

	JString status;
	ClientProfile clientProfile;
	JTime disconnectTime;

protected:

	bool connectToService(const TCPLocation& providerLoc, JString convID, JString authCode, JString service);

	JString currentService;
	JString lastService;
	TCPLocation serviceProviderLoc;

	int disconnectTimeout;
	JString shouldLogoutConvID;
	JString delayedLoginToService;
	JString delayedOldConvID;
};

#endif // _SERVICECLIENT_H_
} // namespace cmlabs

