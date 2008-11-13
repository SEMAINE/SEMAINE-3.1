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


// Queue.h: interface for the Queue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__BD4FC98D_2207_460D_9D2D_CB0E36E84650__INCLUDED_)
#define AFX_QUEUE_H__BD4FC98D_2207_460D_9D2D_CB0E36E84650__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include <thread.h>
// #include "Object.h"
#include "Dictionary.h"
#include "JSemaphore.h"

namespace cmlabs {

class Queue : public Object  
{
public:
	Queue();
	virtual ~Queue();

	bool unitTest();

	Object* clone() const {return NULL;}
	bool isEmpty();
	int getCount();
	bool removeAll();

	//! Add and return newly created id
	JString add(JString str);
	JString add(JString id, JString str);

	//! use to block until Queue is no longer empty
	JString waitForNewEntryID(long ms);
	JString waitForNewEntry(JString id, long ms);
	JString waitForNewEntry(long ms);

	JString retrieveEntry(JString id);
	JString retrieveEntry(int pos);
	JString viewEntry(JString id);
	JString viewEntry(int pos);

protected:

	Dictionary* dict;
	JMutex mutex;
	JSemaphore sem;

};

} // namespace cmlabs

#endif // !defined(AFX_QUEUE_H__BD4FC98D_2207_460D_9D2D_CB0E36E84650__INCLUDED_)
