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


// TCPReceiver.h: interface for the TCPReceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NETPROTOCOL_H_)
#define _NETPROTOCOL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "Message.h"
// #include <socket.h>

namespace cmlabs {

class JSocket;
class NetProtocol;

class NetProtocol : public Object  
{
public:
	NetProtocol(JString pname);
	virtual ~NetProtocol();

	JString name;
	int netTimeout;

	NetProtocol* prev;
	NetProtocol* next;

	bool isLocalCallback;
	bool isRemoteCallback;

	virtual bool equals(NetProtocol* protocol);
	virtual Object* clone() const = 0;
	virtual bool checkBufferForCompatibility(char* buffer, int length) = 0;
	virtual bool initializeConversation(JSocket* socket) = 0;
	virtual bool initializeAsReceiver(JSocket* socket, JString from);

	virtual bool sendObject(JSocket* socket, Object* obj, bool isReply);
	virtual bool sendObject(JSocket* socket, Message* msg, bool isReply) = 0;
	virtual Message* receiveObject(JSocket* socket, int timeout) = 0;

	double getSendProgress();
	int currentUnsentBufferSize;
	int currentSentBufferSize;

	double getReceiveProgress();
	int currentUnreceivedBufferSize;
	int currentReceivedBufferSize;


//	virtual void tcpInput(JString str, NetworkConnection* con);
//	void tcpInput(JString str);
//	virtual bool OnAccept(const InetHostAddress &ia, tpport_t portIn);
};

} // namespace cmlabs

#include "Network.h"

#endif // !defined(_NETPROTOCOL_H_)
