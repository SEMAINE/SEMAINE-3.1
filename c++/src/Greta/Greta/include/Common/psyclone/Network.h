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


// Network.h: interface for the Network class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORK_H__6B5DFBFC_F919_4991_9301_8342407B2A37__INCLUDED_)
#define AFX_NETWORK_H__6B5DFBFC_F919_4991_9301_8342407B2A37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include <config.h>
// #include <macros.h>
// #include <thread.h>
// #include <socket.h>
// #include <iostream.h>
#include "standard.h"
#include <stdlib.h>
#include <stdio.h>
#include "JSemaphore.h"
#include "JSocket.h"

#include "types.h"
#include "TCPLocation.h"
#include "Message.h"
#include "JTime.h"

#include "ObjectCollection.h"
// class NetworkConnection;

#include "NetProtocol.h"
#include "NetMessageProtocol.h"
#include "NetOriginalMessageProtocol.h"
#include "NetHTTPProtocol.h"
#include "NetTelnetProtocol.h"
#include "NetworkConnection.h"
#include "JThread.h"

namespace cmlabs {

class ConnectionProfile;

class Network : public JThread, public Object, public TCPReceiver
{
public:
	Network(TCPReceiver* recv = NULL);
	virtual ~Network();
	bool init(int portNum = -1);
	void run();
	
	Object* clone() const {return NULL;}
	bool isInitialized;
	bool shouldContinue;
	bool isDoneRunning;
	JString getLocalHostname();
	JString getLocalIPAddress();
	Collection getLocalIPAddresses();
	JString getRemoteHostname();
	JString getIPAddressFromName(const JString& name);	
	IPNumber getIPNumberFromName(const JString& name);	
	int getConnectionCount();
	JString print() ;

	NetworkConnection* openConnection(const TCPLocation& loc);
	NetworkConnection* openConnection(const TCPLocation& loc, NetProtocol* netprotocol);
	NetworkConnection* createNetworkConnectionTo(const TCPLocation& loc, TCPReceiver* rec);
	NetworkConnection* openIncomingConnection(JSocket* sock, NetProtocol* protocol);
//	NetworkConnection* createNetworkConnectionTo(TCPLocation loc, TCPReceiver* rec = NULL, JString protocol = "");
//	NetworkConnection* openIncomingConnection(JSocket* sock, TCPReceiver* rec = NULL);
	NetworkConnection* openReceivingConnection(const TCPLocation& loc);
	NetworkConnection* openReceivingConnection(const TCPLocation& loc, NetProtocol* netprotocol);
	bool checkReceivingConnections();
	bool checkOpenConnections();
	bool checkPrimaryConnection();
	bool setPrimaryConnection(const JString& addr, int port, const JString& name);
	bool setPrimaryConnection(const TCPLocation& loc);
	bool isPrimaryConnection(const JString& name);
	bool tryReconnectingLostConnections();

	NetworkConnection* findResetConnection();
	NetworkConnection* findConnectedConnection(const TCPLocation& loc, NetProtocol* netprotocol);
	NetworkConnection* findConnectedConnection(const JString& name);
	bool isCurrentlyConnectedTo(const JString& name);

	bool addProtocol(NetProtocol* protocol);
	bool removeProtocol(NetProtocol* protocol);
	bool removeAllProtocols();
	NetProtocol* detectProtocol(JSocket* socket, long timeout);
	NetProtocol* getPreferredNetProtocol();
	bool setPreferredNetProtocol(NetProtocol* prefProtocol);

	bool trainNetworkProfile(const TCPLocation& loc, ConnectionProfile* profile);

	// For Unit Testing only...
	bool unitTest();
	Message* receiveTCPInput(Message* msg);
	Message* netObjectReceive(Message* msg, NetworkConnection* con);
	JString getName();
	int getPort();

	TCPReceiver* receiver;
	NetProtocol* preferredNetProtocol;
	int netCheckTimeout;
	NetworkConnection* primaryCon;
	TCPLocation* primaryLoc;

	ObjectDictionary* remoteReceiverCons;

protected:

	JSocket* mySocket;
	int port;
	JString localIPAddress;
	Collection localIPAddresses;
	JString localHostname;

	NetProtocol* netProtocols;
	JMutex primaryMutex;

	ObjectDictionary lostSendConnections;
	ObjectDictionary lostReceiveConnections;

	ObjectCollection* cons;
	ObjectDictionary* receiverCons;
	void deleteConnections();
};

} // namespace cmlabs



#endif // !defined(AFX_NETWORK_H__6B5DFBFC_F919_4991_9301_8342407B2A37__INCLUDED_)
