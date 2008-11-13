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


#include "Message.h"
#include "Specs.h"
#include "MediaConnection.h"
#include "InfoItem.h"
#include <stdarg.h>

#ifndef _MESSENGER_H_
#define _MESSENGER_H_

namespace cmlabs {

class Messenger : public Object {
public:
	Messenger(const JString& name, MessageSender* sender, const JString& wakeupMessageType, const JString& phaseChangeType);
	~Messenger();
	Object* clone() const {return NULL;}

	// Crank Interface
	// Get Module name
	JString getName();
	// Get number of triggers up until now
	int getTriggerCount();
	// Get name of trigger
	JString getTriggerAlias();
	// Ask whether crank should continue or exit
	bool shouldContinueRunning();
	// Get list of post/output destinations
	Collection getDestinations();
	// Is this a wakeup message
	bool isWakeupMessage(Message* msg);
	// Log
	bool log(int level, char *format, ...);
	// Setting status entry
	bool setStatus(const JString& name, const JString& value);
	// Getting status entry
	JString getStatus(const JString& name);

	// Have a look at first trigger message
	bool processFirstMessage();
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
	ObjectCollection* retrieveMessages(const JString& retrieveFrom, const JString& retrieveSpecXML);
	// Retrieve additional messages from a source
	ObjectCollection* retrieveMessages(const JString& retrieveFrom, RetrieveSpec* retrieveSpec);
	// Retrieve additional messages from a source
	ObjectCollection* retrieveMessages(const JString& retrieveFrom, const ObjectCollection& retrieveSpecs);

	// Put an output message in the outbox
	bool addOutputMessage(Message* msg);
	// Put an output message in the outbox
	bool addOutputMessage(Message* msg, JString destination);
	// Send all messages in the outbox
	bool sendOutputMessages();
	// Send message to all destinations
	bool postMessageToAllDestinations(Message* msg);
	// Send an output message
	bool postOutputMessage(Message* msg);

	bool sendPhaseChange(const JString& dest = "");

	// return current Context name
	JString getCurrentContextName();
	// return current Phase name
	JString getCurrentPhaseName();

	JTime getCurrentContextStart();
	JTime getContextStart(const JString& context);
	JTime getCurrentPhaseStart();

	ObjectDictionary* getParameterSpecs(JString module);
	Parameter* getParameterSpec(JString module, JString parameter);

	JString getParameterString(JString module, JString parameter);
	int getParameterInteger(JString module, JString parameter);
	double getParameterDouble(JString module, JString parameter);
	JString getParameter(JString module, JString parameter);

	bool setParameterString(JString module, JString parameter, JString value);
	bool setParameterInteger(JString module, JString parameter, int value);
	bool setParameterDouble(JString module, JString parameter, double value);
	bool setParameter(JString module, JString parameter, JString value);

	bool increaseParameter(JString module, JString parameter, int steps);
	bool decreaseParameter(JString module, JString parameter, int steps);
	bool resetParameter(JString module, JString parameter);

	bool addParameterItem(JString module, JString parameter, JString value);
	bool removeParameterItem(JString module, JString parameter, JString value);

	bool addParameterItem(JString param, JString value);
	bool removeParameterItem(JString param, JString value);
	Collection getParameterItems(JString param);

	// Query catalog
	InfoItem* queryCatalog(const JString& catalog, Message* query);

	// Connect to a media server by name
	MediaConnection* connectToMediaServer(JString mediaName);
	// Recomment to a mediaserver by name
	bool reconnectToMediaServer(JString mediaName, MediaConnection* mediacon);
	// Get the server session ID
	JString getServerID();
	// Get the server name
	JString getServerName();

	// Get a list of parameter names
	Collection getParameterNames();
	// Get a specific parameter by name
	Parameter* getParameter(const JString& name);
	// Get a list of stream names
	Collection getStreamNames();
	// Get a specific stream by name
	MediaConnection* getStreamConnection(const JString& name);

	bool addParameter(JString paramname, JString xml);
	bool hasParameter(JString name);
	JString getParameterString(JString param);
	int getParameterInteger(JString param);
	double getParameterDouble(JString param);

	bool setParameterString(JString param, JString value);
	bool setParameterInteger(JString param, int value);
	bool setParameterDouble(JString param, double value);

	bool ping(JString module);
	TCPLocation resolve(const JString& name, bool forceCNSLookup = false);

	// Module Interface
	bool addInputMessage(Message* msg);
	bool addParameter(const JString& name, Parameter* parameter);
	bool addStream(const JString& name, MediaConnection* media);
	bool clearStreams();
	bool terminate();

	bool processPhaseSpec(PhaseSpec* newSpec, JTime t);

	ObjectTable* recentMessages;
	PerfStat* perfStat;
	int verbose;
	double priority;

private:
	MessageSender* messageSender;
	JString name;
	JString wakeupMessageType;
	JString phaseChangeType;

//	ObjectQueue inputMessages;
	ObjectCollection outputMessages;
	ObjectDictionary parameters;
	ObjectDictionary streams;
	Message* currentMessage;
	PhaseSpec* currentPhaseSpec;

	int totalTriggerCount;
	bool isStillRunning;
	bool shouldContinue;

	JTime startCrankTime;
	JTime startProcessTime;
	JThread aThread;
	JMutex mutex;
	JMutex phaseMutex;

	SortedObjectCollection inQueue;
	JMutex inQueueMutex;
	JSemaphore inQueueSemaphore;
	Message* getNextMessage(long ms);

	bool checkNewPhaseSpec(PhaseSpec* newSpec, JTime msgtime);
	bool clearOldMessagesFromQueue(JTime stamp);
};

} // namespace cmlabs

#endif // _MESSENGER_H_














