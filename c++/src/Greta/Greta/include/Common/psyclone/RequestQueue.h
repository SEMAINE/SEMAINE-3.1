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


// RequestQueue.h: interface for the RequestQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTQUEUE_H__241F10F8_5316_43DB_84CE_F0E4D414BC58__INCLUDED_)
#define AFX_REQUESTQUEUE_H__241F10F8_5316_43DB_84CE_F0E4D414BC58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "Queue.h"

namespace cmlabs {

class RequestQueue : public Object  
{
public:
	RequestQueue();
	virtual ~RequestQueue();

	// Requesting side
	
	Object* clone() const {return NULL;}
	JString enterRequest(JString req);
	JString waitForReply(JString id, long ms);
	

	// Replying side
	
	bool isEmpty();
	int getCount();

	JString waitForNewRequestID(int ms);
	JString getRequest(JString id);
	bool reply(JString id, JString rep);

	bool unitTest();

protected:

	Queue* inQueue;
	Queue* outQueue;

};

} // namespace cmlabs

#endif // !defined(AFX_REQUESTQUEUE_H__241F10F8_5316_43DB_84CE_F0E4D414BC58__INCLUDED_)
