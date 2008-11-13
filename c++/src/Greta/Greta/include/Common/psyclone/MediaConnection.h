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


// MediaConnection.h: interface for the MediaConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MEDIACONNECTION_H_)
#define _MEDIACONNECTION_H_

#include "MediaServer.h"
#include "InfoItem.h"
#include "ObjectQueue.h"

namespace cmlabs {

class PreFetchRequest : public Object {
public:
	PreFetchRequest(const JTime& t, const JString& command = "", int ms = 0);
	PreFetchRequest(const JString& id, const JString& command = "", int ms = 0);
	Object* clone() const;
	bool equals(const Object* obj) const;
	int getTimeout();
	JTime requestTime;
	JTime time;
	JString id;
	JString cmd;
	int timeout;
};


class MediaConnection : public Object, public JThread, public TCPReceiver
{
public:
	MediaConnection(const JString& name, const TCPLocation& serverlocation, long hardBufferMaxSize = -1, long softBufferMaxSize = -1);
	virtual ~MediaConnection();
	Object* clone() const;

	bool init();
	bool initWithTraining();
	bool reinit(const JString& name, const TCPLocation& serverlocation, long hardBufferMaxSize = -1, long softBufferMaxSize = -1);
	bool reinitWithTraining(const JString& name, const TCPLocation& serverlocation, long hardBufferMaxSize = -1, long softBufferMaxSize = -1);
	void run();
	JString getServerName();
	JString getName();
	Message* netObjectReceive(Message *msg, NetworkConnection *con);
	bool isConnected();
	bool reset();

	bool allowMessageDropping(bool allow);
	bool handleMultipleWriters(bool allow);
	bool isInSameExecutable(const TCPLocation& loc);
//	bool benchmark();

	// API for remote information
	JTime getOldestTimestamp();
	JTime getNewestTimestamp();
	long getCount();
	long getSize();
	ObjectCollection* getDataSampleList();

	InfoItem* getInfo();

	// API for remove retrieval
	//! Either from local buffer or from remote
	DataSample* getOldestDataSample();
	DataSample* getNewestDataSample();
	//! Either from local buffer or from remote
	DataSample* getDataSample(const JTime& time);
	//! Either from local buffer or from remote
	ObjectCollection* getDataSamples(const JTime& t1, const JTime& t2);


	//! Start a retrieve to local buffer
	bool prefetchDataSamples(const JTime& t1, const JTime& t2);
	bool prefetchDataSample(const JTime& t);

	bool prefetchLastDataSampleAter(const JTime& t);
	bool prefetchFirstDataSampleAter(const JTime& t);

	bool hasRequestBeenRequested(PreFetchRequest* req);
	bool isRequestPending(PreFetchRequest* req);
	bool hasRequestBeenExecuted(PreFetchRequest* req);
	bool didRequestSucceed(PreFetchRequest* req);
	bool didRequestFail(PreFetchRequest* req);

	DataSample* waitForRequestToComplete(PreFetchRequest* req, long ms);

	DataSample* waitForFirstSampleAfter(const JTime& time, long ms);
	DataSample* waitForLastSampleAfter(const JTime& time, long ms);
	DataSample* waitForDataSample(const JTime& time, long ms);

	DataSample* getDataSample(const JString& id);
	DataSample* waitForFirstSampleAfter(const JString& id, long ms);
	DataSample* waitForLastSampleAfter(const JString& id, long ms);

	// API for local buffer information
	JTime getOldestBufferTimestamp();
	JTime getNewestBufferTimestamp();
	long getBufferCount();

	// API for remote adding
	//! Add one sample
	bool addDataSample(DataSample* sample);
	//! Add many samples
	bool addDataSamples(ObjectCollection* samples);


	ObjectCollection* askRemoteServerForDataCollection(const JString& question, Object* obj, int timeout = -1);
	DataSample* askRemoteServerForData(const JString& question, Object* obj, int timeout = -1);

	bool sendSampleToRemoteServer(const JString& question, DataSample* sample);
	bool sendSamplesToRemoteServer(const JString& question, ObjectCollection* samples);

	JString askRemoteServerForType(const JString& question, int timeout = -1);
	JString askRemoteServerForString(const JString& question, int timeout = -1);
	Object* askRemoteServerForObject(const JString& question, int timeout = -1);
	Message* askRemoteServer(Message* msg, int timeout = -1);

	long pingServer();
//	long downloadServerTest(long size);
//	long uploadServerTest(long size);

	bool startContinuousBackgroundReceive();
	bool stopContinuousBackgroundReceive();
	bool isInContinuousReceive();
//	DataSample* receiveContinuousSample(int timeout);

	ConnectionProfile getConnectionProfile();
	ConnectionProfile getServerConnectionProfile();
//	ConnectionProfile* serverProfile;
//	ConnectionProfile* clientProfile;

	bool createChannel(const JString& name, const JString& fieldname);
	bool destroyChannel(const JString& name);
	ObjectCollection* searchChannel(const JString& name, double val1, double val2);
	bool subscribeChannel(const JString& channel, double val1, double val2);
	bool unsubscribeChannel(const JString& channel, double val1, double val2);
	bool unsubscribeAllChannels();
	bool addSubscriptionEvent(DataSample* sample);
	DataSample* waitForSubscriptionEvent(int timeout);
	int getSubscriptionEventQueueSize();

protected:

	NetworkConnection* serverCon;
	NetworkConnection* continuousCon;

	JString myName;
	MediaServer* mediaServer;
//	Network* network;
	bool shouldContinue;
	MediaStream* mediaStream;
	TCPLocation server;

	JSemaphore newDataAvailable;
	JMutex connectionAccess;

	ObjectQueue requests;
	ObjectCollection pendingRequests;
	ObjectCollection successRequests;
	ObjectCollection failedRequests;

	ObjectQueue* eventQueue;
	Collection subscriptions;
	bool allowMessageDrop;
};

} // namespace cmlabs

#endif //_MEDIACONNECTION_H_

