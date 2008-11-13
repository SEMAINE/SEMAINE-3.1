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


// BinaryBuffer.h: interface for the VideoBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINARYBUFFER_H__BF475DE2_2471_4C38_A8C6_895D616B3DF7__INCLUDED_)
#define AFX_BINARYBUFFER_H__BF475DE2_2471_4C38_A8C6_895D616B3DF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "JTime.h"
#include "Dictionary.h"
#include "ObjectCollection.h"
// #include <thread.h>
#include "JSemaphore.h"

namespace cmlabs {

class BinaryBufferEntry : public Object {
public:
	BinaryBufferEntry() { object = NULL; }
	virtual ~BinaryBufferEntry() { if (object != NULL) delete(object); }
	Object* clone() const {return NULL;}

	long count;
	JTime timestamp;
	JString source;
	JString datatype;
	Dictionary inUseBy;
	Object* object;
	void* next;
	void* prev;
};

class BinaryBuffer : public Object  
{
public:
	BinaryBuffer(long size = 100);
	virtual ~BinaryBuffer();

	bool addEntry(JTime stamp, JString source, JString datatype, Object* obj);
	bool reset();
	bool deleteAll();

	Object* clone() const {return NULL;}
	JString print() ;
	JString printLast() ;

	BinaryBufferEntry* getLastEntry();

	BinaryBufferEntry* waitForNextEntryAfter(JTime t, long ms = 1000);
	BinaryBufferEntry* getNextEntryAfter(JTime t);

	long count;
	long buffersize;

protected:

	ObjectCollection* inUseTable;

	JMutex mutex;

	BinaryBufferEntry* first;
	BinaryBufferEntry* last;
	BinaryBufferEntry* current;

	JString print(BinaryBufferEntry* entry) ;
	bool checkInUseTable();
};

} // namespace cmlabs

#endif // !defined(AFX_BINARYBUFFER_H__BF475DE2_2471_4C38_A8C6_895D616B3DF7__INCLUDED_)
