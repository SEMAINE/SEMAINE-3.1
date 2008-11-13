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

#include "Component.h"
#include "MediaConnection.h"
#include "Messenger.h"
#include "MessageProcessor.h"

#if !defined(_CPPAIRPLUG_H_)
#define _CPPAIRPLUG_H_

namespace cmlabs {


#define AIRHEARTBEATTIME 10000

class Trigger : public Object {
public:
	JString whiteboard;
	JString type;
	JString retrieveXML;
	bool allowSelfTriggering;

	Trigger(JString whiteboard, JString type);
	Trigger(JString whiteboard, JString type, JString retrieveXML);
	JString toXML();
	Object* clone() const;
};

class CppAIRPlug : public Component, public MessageAcceptor
{
public:
	CppAIRPlug(JString name, TCPLocation cnsServer);
	CppAIRPlug(JString name, TCPLocation cnsServer, MessageAcceptor* messageAcceptor);
	CppAIRPlug(JString name, JString host, int port);
	CppAIRPlug(JString name, JString host, int port, MessageAcceptor* messageAcceptor);
	virtual ~CppAIRPlug();

	virtual bool init();
	virtual bool init(const JString& registration);
	virtual void run();

	bool shareResources(const JString& moduleName, const JString& xml);
	bool shareResources(ModuleSpec* spec);

	bool pingServer();
	bool isServerAvailable();
	bool setPriority(double pri);

	bool requestCurrentPhaseSpec();
	bool processMediaInformation(PhaseSpec* newSpec);

	virtual bool primaryReconnectNotify();
	virtual bool primaryDisconnectNotify();
	virtual bool connectNotify(const JString& name, NetworkConnection* con);
	virtual bool disconnectNotify(const JString& name, NetworkConnection* con);

	virtual Message* acceptMessage(Message* hmsg);

	bool postMessage(JString whiteboard, JString messageType, JString cc);
	bool postMessage(JString whiteboard, JString messageType, JString content, JString language, JString cc);
	bool postMessage(JString whiteboard, Message* msg, JString cc);

	bool sendRegistration(JString xml);
	bool overwriteRegistration(JString xml);
	bool addTrigger(JString whiteboard, JString type, bool allowSelfTriggering = false);
	bool addTrigger(JString whiteboard, JString type, JString retrieveXML, bool allowSelfTriggering = false);
	bool removeTrigger(JString id);
	bool removeAllTriggers();
	ObjectDictionary* getTriggers();
	bool sendTriggers();
	MediaConnection* connectToMediaServer(JString mediaName);
	bool reconnectToMediaServer(JString mediaName, MediaConnection* mediacon);

    JString getServerID();
    JString getServerName();

	// Get Module name
	JString getName();
	// Get name of trigger
	JString getTriggerAlias();
	// Get first active crank name
	JString getCurrentCrankName();
	// Get all active crank name
	Collection getCurrentCrankNames();
	// Get number of triggers up until now
	int getTriggerCount();
	// Get list of post/output destinations
	Collection getDestinations();

	// Get a list of parameter names
	Collection getParameterNames();

	// Test whether I am in Context
	bool inContext();
	// return current Context name
	JString getCurrentContextName();
	// return current Phase name
	JString getCurrentPhaseName();

	// return current Context start time
	JTime getCurrentContextStart();
	// return a Context start time
	JTime getContextStart(const JString& context);
	// return current Phase start time
	JTime getCurrentPhaseStart();

	// Has any new trigger message arrived
	Message* waitForNewMessage(int timeout);
	// How many trigger messages are waiting
	int getInputMessageCount();
	// How many output messages have not yet been sent
	int getOutputMessageCount();
	// Get the current Wakeup Message
	Message* getWakeupMessage();
	// Get the current Trigger Message
	Message* getTriggerMessage();
	// Get the current number of retrieved messages
	int getRetrievedMessageCount();
	// Get the full collection of the retrieved messages
	ObjectCollection* getAllRetrievedMessages();
	// Get a specific retrieved message
	Message* getRetrievedMessage(int pos);
	// Get the current phase spec
	PhaseSpec* getPhaseSpec();
	// Retrieve additional messages from a source
	ObjectCollection* retrieveMessages(const JString& retrieveSpecXML);
	// Retrieve additional messages from a source
	ObjectCollection* retrieveMessages(RetrieveSpec* retrieveSpec);
	// Retrieve additional messages from a source
	ObjectCollection* retrieveMessages(const ObjectCollection& retrieveSpecs);

	// Put an output message in the outbox
	bool addOutputMessage(Message* msg);
	// Put an output message in the outbox
	bool addOutputMessage(Message* msg, const JString& destination);
	// Send one message
	bool postOutputMessage(Message* msg);
	// Send one message
	bool postOutputMessage(Message* msg, const JString& destination);
	// Send all messages in the outbox
	bool sendOutputMessages();
	// Send message to all destinations
	bool postMessageToAllDestinations(Message* msg);

	// Get a list of stream names
	Collection getStreamNames();
	// Get a specific stream by name
	MediaConnection* getStreamConnection(const JString& name);
	// Wait for a specific stream by name
	MediaConnection* waitForStreamConnection(const JString& name, int timeout);
	// Last full registration sent
	JString lastFullRegistration;


	ObjectDictionary* triggers;
	ObjectDictionary storedMediaServers;
	ObjectDictionary oldMediaServers;
	ObjectDictionary deletedMediaServers;
	Messenger* messenger;
	JTime lastStatus;
	bool sendStatus();
	bool noStatus();

private:
	bool switchModuleAlias(const JString& name);
//	ObjectTable recentMessages;
	JMutex phaseSpecUpdateMutex;
	ObjectDictionary* messengers;
	ObjectQueue messageQueue;
};

} // namespace cmlabs

#endif // _CPPAIRPLUG_H_
