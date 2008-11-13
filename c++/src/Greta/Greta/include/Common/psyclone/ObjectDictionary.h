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


// ObjectDictionary.h: interface for the ObjectDictionary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTDICTIONARY_H__F779901A_22FC_4BBF_B741_54E6987384DE__INCLUDED_)
#define AFX_OBJECTDICTIONARY_H__F779901A_22FC_4BBF_B741_54E6987384DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Collection.h"

namespace cmlabs {

class ObjectDictionary : public CollectionBase  
{
public:
	ObjectDictionary();
	ObjectDictionary(const JString& xml);
	ObjectDictionary(XMLNode *node);
	virtual ~ObjectDictionary();
	ObjectDictionary(const ObjectDictionary &c);
	const ObjectDictionary& operator = (const ObjectDictionary& c);

	bool contains(const JString& key) const;
	bool containsIgnoreCase(const JString& key) const;

	Object* get(int pos);
	Object* get(int pos) const;
	Object* operator [](int pos) const;
	Object* operator [](const JString& key) const;
	Object* get(const JString& key) const;
	Object* getIgnoreCase(const JString& key) const;
	Object* getFirst();
	Object* getNext();
	Object* getLast();
	Object* getPrevious();
	Collection getAllKeys();

	bool put(const JString& key, Object* obj);
	bool remove(const JString& key);
	bool remove(int pos);
	bool putFirst(const JString& key, Object* obj);
	bool putLast(const JString& key, Object* obj);
	bool putAfter(int pos, const JString& key, Object* obj);

	JString getKey(int pos);
	JString getKey(int pos) const;
	JString getFirstKey();
	JString getLastKey();
	JString getNextKey();
	JString getPreviousKey();

	bool removeNoDelete(const JString& key);
	bool removeNoDelete(int pos);

	bool unitTest();

};

} // namespace cmlabs

#endif // !defined(AFX_OBJECTDICTIONARY_H__F779901A_22FC_4BBF_B741_54E6987384DE__INCLUDED_)
