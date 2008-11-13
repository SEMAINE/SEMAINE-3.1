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


// Component.h: interface for the Component class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPONENT_H__7AEE66D8_9510_401A_AF68_FC6FC65815E1__INCLUDED_)
#define AFX_COMPONENT_H__7AEE66D8_9510_401A_AF68_FC6FC65815E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"
#include "Specs.h"
#include "MediaConnection.h"
#include "API.h"

namespace cmlabs {

class MessageAcceptor {
public:
	virtual Message* acceptMessage(Message* msg) = 0;
};

class Component : public Communicator  
{
public:
	Component(JString name, TCPLocation cnsHost);
	Component(JString name, JString host, int port);
	virtual ~Component();

	// *** Standard functionality - do not overwrite

	virtual bool init();
	virtual bool initNoListen();

	//! Reroute an incoming message
	bool setMessageAcceptor(MessageAcceptor* messageAcceptor);

	//! Route an incoming message
	Message* receiveMsg(Message* msg);

	//! Create new Message from content of msg
	Message* createMessageFromWakeup(Message* msg);

	//! Post a request and wait for reply
	Message* getAPIReply(JString request);

	//! Post a request Object and wait for reply
	Message* getAPIReply(Object* requestObject);

	//! Add message to component's inbox
	bool addToInBox(Message* msg);

	//! Wait for new message in the component's inbox
	virtual Message* waitForMessage(int timeout);

	//! Only for TESTING - have to overwrite!!!
	virtual void run();

	//! When request is detected using APICallback this method is called to reply
	virtual Object* apiCall(Object* request);

	//! Whiteboard Registration
	// bool registerTrigger(JString bbname, JString type);

	//! Whiteboard Retrieve
	// bool whiteboardRetrieve(JString fromTime, JString toTime, JString from, JString type, JString bbname);

	//! Posting messages to another Component
	bool postMessage(JString to, Message* msg);

	//! Posting messages to another Component
	bool postMessage(JString to, JString type);

	//! Posting messages to another Component
	bool postMessage(JString to, JString type, JString content, JString language);

	//! Shutdown cleans up the API, then calls Communicator::shutdown()
	bool shutdown();

	JString runAIRClientTest();
	JString apiTest();
	Message* receiveTestServer(Message* msg);
	JString endAIRTest();

	bool isConnected();
	bool openTwoWayConnectionTo(JString name);
	MediaConnection* getMediaConnectionTo(JString server);
	MediaConnection* getExistingMediaConnectionTo(JString server);
	MediaConnection* openNewMediaConnectionTo(JString server);
	bool closeMediaConnection(MediaConnection* con);

	virtual bool connectNotify(const JString& name, NetworkConnection* con);
	virtual bool disconnectNotify(const JString& name, NetworkConnection* con);
	virtual bool primaryReconnectNotify();
	virtual bool primaryDisconnectNotify();

	ObjectCollection mediaConnections;

	MessageAcceptor* msgAcceptor;


protected:

	//! A component's inbox
	ObjectQueue* inBox;
	//! The API interface, may me NULL
	API* api;

	//! Specifies the object type returned by the api object
	JString apiReturnType;

	//! Mutex for API calls
	JMutex apiMutex;

};

} // namespace cmlabs

#endif // !defined(AFX_COMPONENT_H__7AEE66D8_9510_401A_AF68_FC6FC65815E1__INCLUDED_)
