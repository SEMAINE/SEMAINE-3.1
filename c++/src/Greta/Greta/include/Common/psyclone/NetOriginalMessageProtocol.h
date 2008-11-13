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

#if !defined(NETORIGINALMESSAGEPROTOCOL_H)
#define NETORIGINALMESSAGEPROTOCOL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetProtocol.h"

namespace cmlabs {

class NetOriginalMessageProtocol : public NetProtocol  
{
public:
	NetOriginalMessageProtocol();
	virtual ~NetOriginalMessageProtocol();

	Object* clone() const;

	bool checkBufferForCompatibility(char* buffer, int length);
	bool initializeConversation(JSocket* socket);
	bool sendObject(JSocket* socket, Message* msg, bool isReply);
	Message* receiveObject(JSocket* socket, int timeout);
};

} // namespace cmlabs

#endif // !defined(NETORIGINALMESSAGEPROTOCOL_H)
