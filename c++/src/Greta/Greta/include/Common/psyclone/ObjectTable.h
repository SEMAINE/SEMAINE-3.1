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


// ObjectTable.h: interface for the ObjectTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OBJECTTABLE_H_)
#define _OBJECTTABLE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"

namespace cmlabs {

class CollectionEntry;

struct HashEntry {
	unsigned long hash;
	CollectionEntry *entry;
	struct HashEntry *next;
};

struct HashTable {
	unsigned int length;
	unsigned int count;
	unsigned int limit;
	unsigned int prime;
	struct HashEntry **table;
};

static inline unsigned int hash2index(unsigned int length, unsigned long hash) {
	return (hash%length);
};

struct HashTable *create_hashtable(unsigned int minsize);
unsigned long hash(unsigned long hashkey);
static int hashtable_expand(struct HashTable *h);
unsigned int hashtable_count(const struct HashTable *h);
bool hashtable_insert_key(struct HashTable *h, CollectionEntry* entry);
bool hashtable_insert_value(struct HashTable *h, CollectionEntry* entry);
CollectionEntry* hashtable_search_key(const struct HashTable *h, const Object* key);
CollectionEntry* hashtable_search_value(const struct HashTable *h, const Object* value);
ObjectCollection* hashtable_search_values(const struct HashTable *h, const Object* value);
bool hashtable_remove_key(struct HashTable *h, const CollectionEntry* entry);
bool hashtable_remove_value(struct HashTable *h, const CollectionEntry* entry);
void hashtable_destroy(struct HashTable *h);

class XMLNode;

class CollectionEntry : public Object
{
public:
	CollectionEntry() {
		key = value = NULL;
		next = prev = NULL;
		keyhash = valuehash = 0;
	}
	CollectionEntry(Object* key, Object* value) {
		this->key = key;
		this->value = value;
		next = prev = NULL;
		keyhash = valuehash = 0;
	}
	Object* clone() const {return new CollectionEntry(key, value);}
	Object* key;
	Object* value;
	CollectionEntry* next;
	CollectionEntry* prev;
	unsigned long keyhash;
	unsigned long valuehash;
};

class ObjectTable;
class SortedEntry : public Object {
public:
	SortedEntry(double val, Object* obj, ObjectTable* table) {
		this->value = val;
		this->object = obj;
		this->table = table;
	}
	bool equals(const Object* o2) const {
		if ((o2 == NULL) || (!isSameClass(o2))) return false;
		return (this->value == ((SortedEntry*)o2)->value);
	}
	int compare(const Object* o2) const {
		if ((o2 == NULL) || (!isSameClass(o2))) return 0;
		if (value == ((SortedEntry*)o2)->value) return 0;
		return (value > ((SortedEntry*)o2)->value ? 1 : -1);
	}
	unsigned long getHash() const {
		unsigned long key = (unsigned long) (value*1000);
		return (key >> 3) * 2654435761u;
	}
	Object* clone() const {return NULL;}
	double value;
	Object* object;
	ObjectTable* table;
};


#define SORTNONE 0
#define SORTBYKEY 1
#define SORTBYVALUE 2
#define SORTREVERSE 4
#define FINDREVERSE 8

class Collection;
class ObjectCollection;
class Dictionary;
class ObjectDictionary;

class JMutex;
class ObjectTable : public Object  
{
public:
	ObjectTable();
	ObjectTable(const ObjectTable &Coll);
	ObjectTable(const JString& xml);
	ObjectTable(XMLNode* node);
	virtual ~ObjectTable();

	JString type;
	bool deleteOnDestroy;
	int count;
	int currentPos;
	CollectionEntry* current;
	CollectionEntry* first;
	CollectionEntry* last;
	int sorting;

	Object* clone() const;
	bool isCollection();
	bool unitTest();

	bool addAll(const ObjectTable* c);
	bool copyAll(const ObjectTable* c);
	bool takeAll(ObjectTable* c);
	bool addAll(const ObjectTable& c);
	bool copyAll(const ObjectTable& c);
	bool takeAll(ObjectTable& c);
	bool addAll(const CollectionEntry* f, bool copy = false);

	const ObjectTable& operator =(const ObjectTable& c);
	bool equals(const Object* o2) const;
	bool operator ==(ObjectTable& c);
	bool operator !=(ObjectTable& c);

	int getCount() const;
	int getPos(const Object* obj) const;
	int getPosKey(const Object* key) const;

	bool containsKey(const Object* key) const;
	bool contains(const Object* value) const;

	bool removeFirst();
	bool removeLast();
	bool remove(int pos);
	bool removeAll();
	bool removeAllNoDelete();	
	bool removeNoDelete(int pos);
	bool removeNoDelete(const Object* key);
	int removeNoDelete(const Object* value, bool removeAll);

	ObjectCollection* getAllKeysCopy();

	JString toHTML() ;
	JString toXML() ;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);

	JString print() const;
	JString printListLine(const JString& separator, const JString& equalsign = " = ", const JString& quotes = "") const;

	bool sortKeys();
	bool sortValues();
	bool sortKeysReverse();
	bool sortValuesReverse();

	void noDelete();
	void doDelete();



	int addAndReturnPosition(Object* key, Object* value);
	ObjectCollection* getAllBetweenKeys(const Object* key1, const Object* key2);
	ObjectCollection* getAllBetween(const Object* value1, const Object* value2);
	Object* getFirstAfterKey(const Object* key);
	Object* getFirstAfter(const Object* value);
	Object* getLastBeforeKey(const Object* key);
	Object* getLastBefore(const Object* value);
	int removeAllBetweenKeys(const Object* key1, const Object* key2);
	int removeAllBetween(const Object* value1, const Object* value2);



	ObjectCollection* getAllEntriesBetweenKeys(const Object* key1, const Object* key2);
	ObjectCollection* getAllEntriesBetween(const Object* value1, const Object* value2);
	CollectionEntry* getFirstEntryAfterKey(const Object* key);
	CollectionEntry* getFirstEntryAfter(const Object* value);
	CollectionEntry* getLastEntryBeforeKey(const Object* key);
	CollectionEntry* getLastEntryBefore(const Object* value);
	int removeAllEntriesBetweenKeys(const Object* key1, const Object* key2);
	int removeAllEntriesBetween(const Object* value1, const Object* value2);
	int removeAllBeforeKey(const Object* key);
	int removeAllBefore(const Object* value);
	int removeAllAfterKey(const Object* key);
	int removeAllAfter(const Object* value);




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

	bool add(Object* key, Object* value);
	bool addLast(Object* key, Object* value);
	bool addFirst(Object* key, Object* value);
	bool addAfter(const Object* obj, Object* key, Object* value);
	bool addBefore(const Object* obj, Object* key, Object* value);
	bool addBefore(int pos, Object* key, Object* value);
	bool addAfter(int pos, Object* key, Object* value);
	bool addBeforeEntry(CollectionEntry* entry, Object* key, Object* value);
	bool addAfterEntry(CollectionEntry* entry, Object* key, Object* value);
	bool addBetweenEntries(CollectionEntry* prevItem, CollectionEntry* nextItem, Object* key, Object* value);
//	bool addBetweenEntriesNoBlock(CollectionEntry* prevItem, CollectionEntry* nextItem, Object* key, Object* value);

	bool replace(const Object* obj, Object* key, Object* value);
	bool replace(int pos, Object* key, Object* value);
	bool replaceEntry(CollectionEntry* entry, Object* key, Object* value);

	bool remove(const Object* key);
	int remove(const Object* value, bool removeAll);
	bool removeEntry(CollectionEntry* entry);

	Object* get(const Object* key) const;
	Object* get(int pos) const;
	Object* get(int pos);
	Object* getNext();
	Object* getPrevious();
	Object* getFirst();
	Object* getLast();

	Object* getKey(int pos) const;
	Object* getKey(int pos);
	Object* getFirstKey();
	Object* getLastKey();
	Object* getNextKey();
	Object* getPreviousKey();

	CollectionEntry* getEntryKey(const Object* key) const;
	CollectionEntry* getEntryValue(const Object* key) const;
	CollectionEntry* getEntry(int pos) const;
	CollectionEntry* getEntry(int pos);
//	CollectionEntry* getEntryNoBlock(int pos);
	CollectionEntry* getNextEntry();
	CollectionEntry* getPreviousEntry();
	CollectionEntry* getFirstEntry();
	CollectionEntry* getLastEntry();
	int getEntryPos(CollectionEntry* entry) const;

	CollectionEntry** findClosestKeys(const Object* key) const;
	CollectionEntry** findClosestValues(const Object* value) const;


	JString print(const Object* key) const;
	JString printEntry(const CollectionEntry* item, const JString& separator = " = ", const JString& quotes = "") const;

	bool isKeyBigger(CollectionEntry* entry1, CollectionEntry* entry2);
	bool isValueBigger(CollectionEntry* entry1, CollectionEntry* entry2);

	CollectionEntry* createEntry(Object* key, Object* value);
	JMutex* accessMutex;
	HashTable* hashTable;
//	bool removeEntryNoBlock(CollectionEntry* entry);
};

} // namespace cmlabs

#endif // !defined(_OBJECTTABLE_H_)
