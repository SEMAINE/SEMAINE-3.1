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


// ObjectCollection.h: interface for the ObjectCollection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTCOLLECTION_H__25C22A69_E0EB_4A15_AB89_C649DE66339E__INCLUDED_)
#define AFX_OBJECTCOLLECTION_H__25C22A69_E0EB_4A15_AB89_C649DE66339E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Collection.h"

namespace cmlabs {

class ObjectCollection : public CollectionBase  
{
public:
	ObjectCollection();
	ObjectCollection(const JString& xml);
	ObjectCollection(XMLNode *node);
	virtual ~ObjectCollection();
	ObjectCollection(const ObjectCollection &c);
	const ObjectCollection& operator = (const ObjectCollection& c);

	bool unitTest();

	bool contains(Object* obj) const;
	int getPos(Object* obj) const;

	Object* getFirst();
	Object* getLast();
	Object* getNext();
	Object* getPrevious();
	Object* get(int pos);
	Object* get(int pos) const;
	Object* operator [](int pos) const;

	bool add(Object* obj);
	bool addFirst(Object* obj);
	bool addLast(Object* obj);
	bool addAfter(int pos, Object* obj);
	bool addBefore(int pos, Object* obj);
	bool replace(int pos, Object* obj);
	int replace(Object* oldobj, Object* obj);
	int removeEntries(Object* obj);

	bool remove(Object* obj);
	bool remove(int pos);
	bool removeNoDelete(int pos);
	int removeNoDelete(Object* obj);

};

} // namespace cmlabs

#endif // !defined(AFX_OBJECTCOLLECTION_H__25C22A69_E0EB_4A15_AB89_C649DE66339E__INCLUDED_)
