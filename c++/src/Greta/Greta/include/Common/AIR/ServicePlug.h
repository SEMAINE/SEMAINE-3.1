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
// ServicePlug.h: interface for the ServicePlug class.
//
//////////////////////////////////////////////////////////////////////

#include "ServiceProvider.h"

namespace cmlabs {

#if !defined(_SERVICEPLUG_H_)
#define _SERVICEPLUG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ServicePlug : public JThread, public Object
{
public:
	ServicePlug(JString convID, ServiceProvider* provider, ClientProfile* profile, DLLServiceRun serviceRun);
    virtual ~ServicePlug();

	Object* clone() const { return NULL; }
	virtual bool init();
	bool acceptMessage(Message* msg);
	bool terminate();

	virtual void run();

	TCPLocation resolve(JString name);
	bool ping(JString name);
	bool pingServer();
	bool pingClient();

	Message* sendReceiveMsgToClient(Message* msg);
	bool sendMsgToClient(Message* msg);
	Message* waitForMessage(int timeout);

	bool checkConnectionToClient();
	bool clientIsNowLoggedOut();

//	bool postMessage(JString whiteboard, JString messageType, JString cc);
//	bool postMessage(JString whiteboard, JString messageType, JString content, JString language, JString cc);
	bool postMessage(Message* msg);
//	bool addTrigger(JString whiteboard, JString type, bool allowSelfTriggering = false);
//	bool addTrigger(JString whiteboard, JString type, JString retrieveXML, bool allowSelfTriggering = false);
//	bool removeTrigger(JString id);
//	bool removeAllTriggers();
	ObjectCollection* retrieveMessages(JString xml);
	MediaConnection* getMediaConnectionTo(JString server);
	MediaConnection* openNewMediaConnectionTo(JString server);
	bool closeMediaConnection(MediaConnection* con);

	bool shouldContinue;
	XMLNode* setupNode;

	JString convID;
	JString client;

protected:

	ClientProfile* clientProfile;
	ServiceProvider* parent;
	DLLServiceRun serviceRun;

	ObjectQueue receivedMessages;
//	ObjectQueue messagesFromParent;
};

int testServiceRun(JString id, JString name, ClientProfile* profile, ServicePlug* servicePlug);

#endif // _SERVICEPLUG_H_
} // namespace cmlabs
