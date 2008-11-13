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


// NetMessageProtocol.h: interface for the NetMessageProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETMESSAGEPROTOCOL_H__E994EC4A_FB03_4B4D_B8C8_8FE27A8687C2__INCLUDED_)
#define AFX_NETMESSAGEPROTOCOL_H__E994EC4A_FB03_4B4D_B8C8_8FE27A8687C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetProtocol.h"
#include "DataSample.h"

namespace cmlabs {

class NetMessageProtocol : public NetProtocol  
{
public:
	NetMessageProtocol();
	virtual ~NetMessageProtocol();

	Object* clone() const;

	bool checkBufferForCompatibility(char* buffer, int length);
	bool initializeConversation(JSocket* socket);
	bool initializeAsReceiver(JSocket* socket, JString from);
	bool sendObject(JSocket* socket, Message* msg, bool isReply);
	Message* receiveObject(JSocket* socket, int timeout);

	long getLongFromBytes(char* buffer);
	bool writeLongToBytes(char* buffer, long val);
};

} // namespace cmlabs

#endif // !defined(AFX_NETMESSAGEPROTOCOL_H__E994EC4A_FB03_4B4D_B8C8_8FE27A8687C2__INCLUDED_)
