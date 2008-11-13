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


// CollectionBase.h: interface for the CollectionBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_COLLECTIONBASE_H_)
#define _COLLECTIONBASE_H_

#include "ObjectTable.h"

namespace cmlabs {

class CollectionBase : public Object  
{
public:
	CollectionBase();
//	CollectionBase(const CollectionBase &Coll);
	CollectionBase(const JString& xml);
	CollectionBase(XMLNode* node);
	virtual ~CollectionBase();

	ObjectTable objectTable;

	virtual Object* clone() const;
	bool isCollection() const;

	virtual bool addAll(const CollectionBase* c);
	virtual bool copyAll(const CollectionBase* c);
	virtual bool takeAll(CollectionBase* c);
	virtual bool addAll(const CollectionBase& c);
	virtual bool copyAll(const CollectionBase& c);
	virtual bool takeAll(CollectionBase& c);

	bool equals(const Object* o2) const;
//	virtual const CollectionBase& operator =(const CollectionBase& c);
	virtual bool operator ==(CollectionBase& c);
	virtual bool operator !=(CollectionBase& c);

	int getCount() const;
	int length() const;

	bool removeFirst();
	bool removeLast();
	bool remove(int pos);
	bool removeAll();

	JString toHTML();
	JString toXML();
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);

	JString print() const;
	JString printListLine(const JString& separator, const JString& equalsign = " = ", const JString& quotes = "") const;

	bool sortKeys();
	bool sortValues();
	bool sortKeysReverse();
	bool sortValuesReverse();

	bool removeNoDelete(int pos);
	bool removeAllNoDelete();
	void noDelete();
	void doDelete();


// ***************************************************************
// Binary Functions
// ***************************************************************

	//! Get the size of binary data chunk, -1 for all chunks
	long getBinarySize(int chunk);
	//! Get the number of data chunks
	int getBinaryChunkCount();
	//! Write binary data to buffer at chunk number 'chunk'
	long toBinaryBuffer(int chunk, char* buffer, int maxlen);
	//! Set the data after separate reception
	bool fromBinaryBuffer(int chunk, char* buffer, long len);

	//! Get the binary data containing objects
	ObjectCollection* getBinaryDataObjects();

};

} // namespace cmlabs


#endif // !defined(_COLLECTIONBASE_H_)
