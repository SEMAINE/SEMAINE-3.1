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


// ObjectRequestQueue.h: interface for the ObjectRequestQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTREQUESTQUEUE_H__D8862297_CBBA_405D_8011_F7F570CC14F4__INCLUDED_)
#define AFX_OBJECTREQUESTQUEUE_H__D8862297_CBBA_405D_8011_F7F570CC14F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "ObjectQueue.h"
#include "RequestQueue.h"

namespace cmlabs {

class ObjectRequestQueue : public Object  
{
public:
	ObjectRequestQueue();
	virtual ~ObjectRequestQueue();

	// Requesting side
	
	Object* clone() const {return NULL;}
	JString enterRequest(Object* req);
	Object* waitForReply(JString id, long ms);
	

	// Replying side
	
	bool isEmpty();
	int getCount();

	bool waitForNewRequestToAppear(int ms);
	JString waitForNewRequestID(int ms);
	Object* getRequest(JString id);
	bool reply(JString id, Object* rep);

	bool unitTest();

	ObjectQueue* inQueue;
	ObjectQueue* outQueue;

protected:


};

} // namespace cmlabs

#endif // !defined(AFX_OBJECTREQUESTQUEUE_H__D8862297_CBBA_405D_8011_F7F570CC14F4__INCLUDED_)
