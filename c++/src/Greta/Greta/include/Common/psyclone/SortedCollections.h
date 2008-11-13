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


// SortedCollection.h: interface for the SortedCollection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SORTEDCOLLECTIONS_H_)
#define _SORTEDCOLLECTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Collections.h"

namespace cmlabs {

class SortedCollection;
class SortedCollection : public Collection
{
public:
	SortedCollection();
	SortedCollection(const JString& xml);
	SortedCollection(XMLNode *node);
	virtual ~SortedCollection();

	bool unitTest();

	int addAndReturnPosition(const JString& value);
	int removeAllBetween(const JString& str1, const JString& str2);

	SortedCollection getAllBetween(const JString& str1, const JString& str2);
	JString getFirstAfter(const JString& str);
	JString getLastBefore(const JString& str);

	bool replace(int pos, const JString& value) { return false; }
	bool addFirst(const JString& value) { return false; }
	bool addLast(const JString& value) { return false; }
	bool addAfter(int pos, const JString& value) { return false; }
	bool addBefore(int pos, const JString& value) { return false; }
};


class SortedObjectCollection;
class SortedObjectCollection : public ObjectCollection
{
public:
	SortedObjectCollection();
	SortedObjectCollection(const JString& xml);
	SortedObjectCollection(XMLNode *node);
	virtual ~SortedObjectCollection();

	bool unitTest();

	int addAndReturnPosition(Object* value);
	int removeAllBetween(const Object* obj1, const Object* obj2);

	SortedObjectCollection getAllBetween(const Object* obj1, const Object* obj2);
	JString getFirstAfter(const Object* obj);
	JString getLastBefore(const Object* obj);

	bool replace(int pos, const Object* value) { return false; }
	bool addFirst(const Object* value) { return false; }
	bool addLast(const Object* value) { return false; }
	bool addAfter(int pos, const Object* value) { return false; }
	bool addBefore(int pos, const Object* value) { return false; }
};

class SortedDictionary : public Dictionary
{
public:
	SortedDictionary();
	SortedDictionary(const JString& xml);
	SortedDictionary(XMLNode *node);
	virtual ~SortedDictionary();

	bool unitTest();

	int putAndReturnPosition(const JString& key, const JString& value);
	int removeAllBetween(const JString& key1, const JString& key2);

	Dictionary getAllBetween(const JString& key1, const JString& key2);
	JString getFirstAfter(const JString& key);
	JString getLastBefore(const JString& key);

	bool replace(int pos, const JString& key, const JString& value) { return false; }
	bool putFirst(const JString& key, const JString& value) { return false; }
	bool putLast(const JString& key, const JString& value) { return false; }
	bool putAfter(int pos, const JString& key, const JString& value) { return false; }
	bool putBefore(int pos, const JString& key, const JString& value) { return false; }
};

class SortedObjectDictionary : public ObjectDictionary
{
public:
	SortedObjectDictionary();
	SortedObjectDictionary(const JString& xml);
	SortedObjectDictionary(XMLNode *node);
	virtual ~SortedObjectDictionary();

	bool unitTest();

	int putAndReturnPosition(const JString& key, Object* value);
	int removeAllBetween(const JString& key1, const JString& key2);

	ObjectDictionary getAllBetween(const JString& key1, const JString& key2);
	JString getFirstAfter(const JString& key);
	JString getLastBefore(const JString& key);

	bool replace(int pos, const JString& key, const Object* value) { return false; }
	bool putFirst(const JString& key, const Object* value) { return false; }
	bool putLast(const JString& key, const Object* value) { return false; }
	bool putAfter(int pos, const JString& key, const Object* value) { return false; }
	bool putBefore(int pos, const JString& key, const Object* value) { return false; }
};

} // namespace cmlabs

#endif // !defined(_SORTEDCOLLECTIONS_H_)
