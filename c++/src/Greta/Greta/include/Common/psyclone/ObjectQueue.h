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


// ObjectQueue.h: interface for the ObjectQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTQUEUE_H__49E44B86_BFB0_463A_BB0A_998CA086D94B__INCLUDED_)
#define AFX_OBJECTQUEUE_H__49E44B86_BFB0_463A_BB0A_998CA086D94B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include <thread.h>
#include "Object.h"
#include "ObjectDictionary.h"
#include "JSemaphore.h"
#include "Queue.h"

namespace cmlabs {

class ObjectQueue : public Object  
{
public:
	ObjectQueue();
	virtual ~ObjectQueue();

	bool unitTest();

	Object* clone() const {return NULL;} 
	bool isEmpty();
	int getCount();
	bool removeAll();
	ObjectCollection* retrieveAll();

	//! Add and return newly created id
	JString add(Object* obj);
	JString add(JString id, Object* obj);

	//! use to block until Queue is no longer empty
	JString waitForNewEntryID(long ms);
	bool waitForNewEntryToAppear(long ms);
	Object* waitForNewEntry(long ms);
	Object* waitForNewEntry(JString id, long ms);
	Object* retrieveEntry(JString id);
	Object* retrieveEntry(int pos);
	Object* viewEntry(JString id);
	Object* viewEntry(int pos);

	ObjectDictionary* dict;

protected:

	JMutex mutex;
	JSemaphore sem;

};

} // namespace cmlabs

#endif // !defined(AFX_OBJECTQUEUE_H__49E44B86_BFB0_463A_BB0A_998CA086D94B__INCLUDED_)
