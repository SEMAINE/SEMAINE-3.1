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


// Collection.h: interface for the Collection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLECTION_H__CA9E28D1_CE17_4E79_9147_47BC3C70087F__INCLUDED_)
#define AFX_COLLECTION_H__CA9E28D1_CE17_4E79_9147_47BC3C70087F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CollectionBase.h"

namespace cmlabs {

class Collection : public CollectionBase  
{
public:
	Collection();
	Collection(const JString& xml);
	Collection(XMLNode *node);
	virtual ~Collection();
	Collection(const Collection &c);
	const Collection& operator = (const Collection& c);
	bool addAll(const CollectionBase* c);
	bool addAll(const CollectionBase& c);

	bool unitTest();
	
	bool contains(const JString& value) const;
	int getPos(const JString& value) const;
	
	JString getFirst();
	JString getLast();
	JString getNext();
	JString getPrevious();
	JString get(int pos);
	JString get(int pos) const;
	JString operator [](int pos) const;

	bool add(const JString& value);
	bool addFirst(const JString& value);
	bool addLast(const JString& value);
	bool addAfter(int pos, const JString& value);
	bool addBefore(int pos, const JString& value);
	bool replace(int pos, const JString& value);
	int replace(const JString& oldvalue, const JString& value);
	int removeEntries(const JString& value);
	bool remove(const JString& value);
	bool remove(int pos);

	bool removeNoDelete(int pos);
	bool removeAllNoDelete();
	void noDelete();
	void doDelete();

};

} // namespace cmlabs

#endif // !defined(AFX_COLLECTION_H__CA9E28D1_CE17_4E79_9147_47BC3C70087F__INCLUDED_)
