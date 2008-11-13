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


// NetHTTPProtocol.h: interface for the NetHTTPProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETHTTPPROTOCOL_H__14CEFE7D_EACA_4D21_97CF_52588341E65E__INCLUDED_)
#define AFX_NETHTTPPROTOCOL_H__14CEFE7D_EACA_4D21_97CF_52588341E65E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetProtocol.h"

namespace cmlabs {

class HTTPRequest;

class NetHTTPProtocol : public NetProtocol  
{
public:
	NetHTTPProtocol();
	virtual ~NetHTTPProtocol();

	Object* clone() const;

	bool checkBufferForCompatibility(char* buffer, int length);
	bool initializeConversation(JSocket* socket);
	bool sendObject(JSocket* socket, Message* msg, bool isReply);
	Message* receiveObject(JSocket* socket, int timeout);

	HTTPRequest* recHTTP;

};

} // namespace cmlabs

#endif // !defined(AFX_NETHTTPPROTOCOL_H__14CEFE7D_EACA_4D21_97CF_52588341E65E__INCLUDED_)
