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

#if !defined(AFX_TCPRECEIVER_H__2612E3AE_8C69_4717_8F4C_65BA8B3D10D1__INCLUDED_)
#define AFX_TCPRECEIVER_H__2612E3AE_8C69_4717_8F4C_65BA8B3D10D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "Message.h"
// #include <socket.h>

namespace cmlabs {

class NetworkConnection;

class TCPReceiver // : public Object  
{
public:
	TCPReceiver();
	virtual ~TCPReceiver();

	virtual void tcpInput(JString str, NetworkConnection* con);

	virtual Message* netObjectReceive(Message* msg, NetworkConnection* con);

	virtual bool connectNotify(const JString& name, NetworkConnection* con);
	virtual bool disconnectNotify(const JString& name, NetworkConnection* con);

	virtual bool primaryReconnectNotify();
	virtual bool primaryDisconnectNotify();

	virtual JString getName() = 0;


//	void tcpInput(JString str);
//	virtual bool OnAccept(const InetHostAddress &ia, tpport_t portIn);
};

} // namespace cmlabs

#endif // !defined(AFX_TCPRECEIVER_H__2612E3AE_8C69_4717_8F4C_65BA8B3D10D1__INCLUDED_)
