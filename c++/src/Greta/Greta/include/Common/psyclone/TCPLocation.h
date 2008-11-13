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


// TCPLocation.h: interface for the TCPLocation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPLOCATION_H__38AB0349_6FF9_4354_890F_32E68B0DD66C__INCLUDED_)
#define AFX_TCPLOCATION_H__38AB0349_6FF9_4354_890F_32E68B0DD66C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "XMLParser.h"

namespace cmlabs {

#define NO_LISTEN -1

typedef struct _IPNumber
{
	unsigned char a;
	unsigned char b;
	unsigned char c;
	unsigned char d;
} IPNumber;

class TCPLocation : public Object  
{
public:
	TCPLocation();
//	TCPLocation(TCPLocation &loc);
	TCPLocation(const JString& xml);
	TCPLocation(XMLNode* node);
	TCPLocation(const JString& str, const JString& alias);
	TCPLocation(const JString& str, int portNum);
	TCPLocation(const JString& str, int portNum, const JString& alias);
	virtual ~TCPLocation();

	Object* clone() const;
	bool equals(const TCPLocation& loc) const;
	bool isValid() const;
	JString asText() const;
	JString getLocation() const;
	JString toXML() ;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	bool set(const JString&  str);
	JString print();
	JString printList();
	Collection getAllAddresses();

	bool setIPAddress(JString ip);
	JString getIPAddress() const;

	bool isNoListen() const;

	bool isConnectedViaCNS;
	JString addr;
	int port;
	JString name;
	JString authentication;
	JString additional;

	JString procid;
	void* pCommunicator;
	IPNumber ipnumber;
	JString netProtocolName;

};

} // namespace cmlabs

#endif // !defined(AFX_TCPLOCATION_H__38AB0349_6FF9_4354_890F_32E68B0DD66C__INCLUDED_)
