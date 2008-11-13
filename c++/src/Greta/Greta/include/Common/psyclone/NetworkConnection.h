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


// NetworkConnection.h: interface for the NetworkConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKCONNECTION_H__D7F7AF51_1DA2_4001_AA17_12E69A8AAA3A__INCLUDED_)
#define AFX_NETWORKCONNECTION_H__D7F7AF51_1DA2_4001_AA17_12E69A8AAA3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "standard.h"
#include <stdlib.h>
#include <stdio.h>

#include "Object.h"
#include "JThread.h"
#include "JString.h"
#include "JTime.h"
#include "TCPReceiver.h"
#include "TCPLocation.h"
#include "NetProtocol.h"
#include "JSocket.h"
#include "HTML.h"
#include "ObjectQueue.h"
#include "TimeSeries.h"

namespace cmlabs {

class Network;
class HTTPRequest;
class HTMLPage;

class NetworkConnection : public JThread, public Object
{
public:
	//! Constructor with an existing socket
	NetworkConnection(JSocket* con, NetProtocol* netProtocol, TCPReceiver* rec);

	//! Constructor to connect to a remote server
	NetworkConnection(const TCPLocation& loc, NetProtocol* netProtocol, TCPReceiver* rec);

	//! Constructor to connect to a remote server
	NetworkConnection(const JString& addr, int port, NetProtocol* netProtocol, TCPReceiver* rec);

	//! Restarting... almost constructor...
	bool restart(JSocket* con, NetProtocol* netProtocol, TCPReceiver* rec);
	
	//! Restarting... almost constructor...
	bool restart(const TCPLocation& loc, NetProtocol* netProtocol, TCPReceiver* rec);

	//! Restarting... almost constructor...
	bool restart(const JString& addr, int port, NetProtocol* netProtocol, TCPReceiver* rec);

	virtual ~NetworkConnection();
	
	Object* clone() const {return NULL;}
	JString getLocalHostname();
	JString getLocalIPAddress();
	JString getRemoteHostname();
	TCPLocation getRemoteLocation();
	TCPLocation getLastConnectedTo();
	bool unitTest();
	void init(TCPReceiver* rec);
	bool initializeAsReceiver(TCPReceiver* rec);
	void run();
	bool terminate();

	bool isErrorRecoverable();
	bool isConnected();
	bool isConnectedTo(const TCPLocation& loc);
	bool isConnectedTo(const JString& name);
	bool isPersistent();
	bool hasTerminated();
	bool isReset();
	bool isBroken();


	int read(char* buffer, int maxcount, long timeout);
	JString readString(unsigned int numBytes, unsigned int msTimeout);
	JString readln(unsigned int msTimeout);
	bool write(char* buffer, int length);
	bool writeString(const JString& str);
	bool writeln(const JString& str);

	Message* sendReceiveObject(Message* msg, unsigned int msTimeout);
	Message* receiveObject(unsigned int msTimeout);
	bool sendObject(Message* msg, bool isReply);
	bool sendObjectWhenReady(Message* msg, int timeout = 3000);

	double getSendProgress();
	double getReceiveProgress();

	long msSinceLastRead();
	long msSinceLastWrite();
	long msSinceLastActivity();

	bool shouldContinue;
	bool isDoneRunning;
	bool isTerminated;
	long timeout;

	bool hasNoParent();
	bool reset();

	JString print();

	bool isReceiver();
	bool isLocalCallback();
	bool isRemoteCallback();
	bool isIncomingConnection();

	bool setKeepAliveTimeout(long timeout);
	long getKeepAliveTimeout();
	bool useKeepAlive(bool doKeepAlive);
	bool sendKeepAlive();
	bool ping(const JString& from, const JString& to, int timeout = 5000);

	// Benchmarking
	bool trainNetworkProfile();
	bool trainNetworkProfile(ConnectionProfile* profile);
	Message* performBenchmark(Message* msg);
	long pingRemotePeer();
	long uploadServerTest(long size);
	long downloadServerTest(long size);
	bool sendLocalProfile();
	bool getRemoteProfile();

	bool allowMessageDropping(bool allow);

	Network* parent;

	TCPReceiver* receiver;
	NetProtocol* netprotocol;
	JString conID;
	JString remoteName;
	bool createdAsReceiver;

	HTTPRequest* recHTTP;

	ConnectionProfile localProfile;
	ConnectionProfile remoteProfile;

private:
	int count;

	JSocket* mySocket;
	JMutex conMutex;

	bool passive;
	void resetBuffer();
	JTime lastRead;
	JTime lastWrite;
	long keepAliveTimeout;
	TCPLocation lastConnectedTo;
	ObjectQueue receivedMessages;
	ObjectQueue toSendMessages;
	JSemaphore toSendSemaphore;
	int toSendMaxQueueSize;
	bool allowMessageDrop;
};

} // namespace cmlabs

#endif // !defined(AFX_NETWORKCONNECTION_H__D7F7AF51_1DA2_4001_AA17_12E69A8AAA3A__INCLUDED_)
