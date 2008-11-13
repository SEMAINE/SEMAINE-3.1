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


// MediaServer.h: interface for the MediaServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MEDIASERVER_H_)
#define _MEDIASERVER_H_

#include "MediaStream.h"
#include "Network.h"
#include "InfoItem.h"
#include "NetworkConnection.h"

namespace cmlabs {

class SubscriptionEntry : public Object {
public:
	SubscriptionEntry(const JString& channel, double val1, double val2) {
		this->channel = channel;
		this->val1 = val1;
		this->val2 = val2;
	}
	bool equals(const Object* o2) const {
		if ((o2 == NULL) || (!isSameClass(o2))) return false;
		SubscriptionEntry* entry = (SubscriptionEntry*) o2;
		return ((channel.equalsIgnoreCase(entry->channel)) && (val1 == entry->val1) && (val2 == entry->val2));
	}
	unsigned long getHash() const {
		return channel.getHash();
	}
	Object* clone() const {return new SubscriptionEntry(channel, val1, val2);}
	double val1, val2;
	JString channel;
};

class MediaServer : public Object, public JThread, public TCPReceiver
{
public:
	MediaServer(const JString& servername, long hardMaxSize = -1, long softMaxSize = -1, long maxCount = -1);
	virtual ~MediaServer();
	Object* clone() const;
	bool unitTest();


	//! Setup network on port. If false try other port...
	bool init(int port);
	void run();

	bool handleMultipleWriters(bool allow);

	TCPLocation getLocation();

	JString getName();
	Message* netObjectReceive(Message *msg, NetworkConnection *con);
	HTMLPage* serveHTTPRequest(const JString& origin, HTTPRequest* request);
	Message* serveTelnetInput(const JString& id, const JString& origin, const JString& text);

	long getTotalDataSize();
	long getTotalDataMemUse();
	int getCount();
	double getInputDataRate();
	double getOutputDataRate();
	double getTotalDataEverHandled();
	int getConnectionCount();
	ConnectionProfile getConnectionProfile(const JString& client);
	InfoItem* getInfo();


	// API for remote information
	JTime getOldestTimestamp();
	JTime getNewestTimestamp();
	ObjectCollection* getDataSampleList();

	// API for remote retrieval
	DataSample* getOldestSample(const JString& clientname);
	DataSample* getNewestSample(const JString& clientname);
	DataSample* getDataSample(const JString& clientname, const JTime& time);
	DataSample* getDataSample(const JTime& time);
	ObjectCollection* getDataSamples(const JString& clientname, const JTime& t1, const JTime& t2);
	ObjectCollection* getDataSamples(const JTime& t1, const JTime& t2);

	// API for remote adding
	//! Add one sample
	bool addDataSample(const JString& clientname, DataSample* sample);
	bool addDataSample(DataSample* sample);
	//! Add many samples
	bool addDataSamples(const JString& clientname, ObjectCollection* samples);
	bool addDataSamples(ObjectCollection* samples);

	DataSample* waitForFirstSampleAfter(const JString& clientname, const JTime& time, long ms);
	DataSample* waitForFirstSampleAfter(const JTime& time, long ms);
	DataSample* waitForLastSampleAfter(const JString& clientname, const JTime& time, long ms);
	DataSample* waitForLastSampleAfter(const JTime& time, long ms);
	DataSample* waitForSampleAt(const JString& clientname, const JTime& time, long ms);
	DataSample* waitForSampleAt(const JTime& time, long ms);

	DataSample* getSample(const JString& clientname, const JString& id);
	DataSample* getFirstSampleAfter(const JString& clientname, const JString& id);
	DataSample* waitForFirstSampleAfter(const JString& clientname, const JString& id, long ms);
	DataSample* waitForLastSampleAfter(const JString& clientname, const JString& id, long ms);
	DataSample* getSample(const JString& id);
	DataSample* getFirstSampleAfter(const JString& id);
	DataSample* waitForFirstSampleAfter(const JString& id, long ms);
	DataSample* waitForLastSampleAfter(const JString& id, long ms);

	ObjectDictionary serverProfiles;
	ObjectDictionary serverNetworkProfiles;
//	ObjectDictionary clientProfiles;

	bool createChannel(const JString& name, const JString& fieldname);
	bool destroyChannel(const JString& name);
	ObjectCollection* searchChannel(const JString& name, double val1, double val2);
	bool subscribeChannel(const JString& connection, const JString& channel, double val1, double val2);
	bool unsubscribeChannel(const JString& connection, const JString& channel, double val1, double val2);
	bool unsubscribeChannel(const JString& connection, SubscriptionEntry* entry);
	bool unsubscribeAllChannels(const JString& connection);
	bool subscriptionMatch(ObjectCollection* subs, DataSample* sample);
	ObjectDictionary directSubscribers;

protected:

	JString name;
	MediaStream* mediaStream;
	Network* network;
	bool shouldContinue;
	ObjectCollection continuousCons;
	JTime lastContinuousSend;

	ObjectDictionary subscriptions;

	bool addDataStat(const JString& name, long upbytes, long downbytes, const JTime& time = JTime());
//	bool addNetworkDataStat(JString name, long upbytes, long downbytes, JTime time = JTime());
};



class MediaTestServer : public JThread
{
public:
	MediaTestServer(const JString& servername, int expectedConnections, bool continuous = false);
	virtual ~MediaTestServer();

	bool init(int testport);
	void run();
	bool isTestComplete();
	TCPLocation getLocation();

protected:
	bool isContinuous;
	int numCons;
	MediaServer* mediaServer;
};

MediaTestServer* createMediaTestServer(int& port, const JString& name, int count, bool continuous);
bool runMediaTestClient(const JString& name, const JString& host, int testport);
bool runMediaTestClient(const JString& name, const TCPLocation& loc);

} // namespace cmlabs

#endif //_MEDIASERVER_H_















