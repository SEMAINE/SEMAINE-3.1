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


// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT_H__D0E87E36_5CA6_4DB2_B512_46D23CDE9966__INCLUDED_)
#define AFX_OBJECT_H__D0E87E36_5CA6_4DB2_B512_46D23CDE9966__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "standard.h"

namespace cmlabs {

class Dictionary;
class JString;
class XMLNode;
class SysInfo;


//! Base object for all other objects
class Object  
{
public:
	Object();
	virtual ~Object();

	static double getRandomNumber();
	static JString createUniqueID();
	static JString createUniqueID(const JString& title);

	//! Try to create an object by analysing the XML
	static Object* createObjectFromXML(const JString& xml);
	static Object* createObjectFromXML(XMLNode* node);

	static SysInfo getSystemInfo();
	static bool wait(long ms);

	virtual bool unitTest();
//	virtual bool unitTest() = 0;
	virtual bool equals(const Object* o2) const;
	virtual int compare(const Object* o2) const;
//	virtual bool equals(Object* o2) = 0;
	virtual JString print();
	virtual JString toHTML();
	virtual JString toXML();
	virtual bool fromXML(const JString& xml);
	virtual bool fromXML(XMLNode* node);
	virtual Object* clone() const = 0;
//	bool wait(long ms) const;

	void addUnitTestLog(const JString& str);
	JString getUnitTestLog();

	//! Get the class name
	virtual JString getClass() const;
	virtual bool isSameClass(const Object* o2) const;
	virtual bool isCollection() const;
	virtual unsigned long getHash() const;

	//! Get the size of binary data chunk, -1 for all chunks
	virtual long getBinarySize(int chunk);
	//! Get the number of data chunks
	virtual int getBinaryChunkCount();
	//! Write binary data to buffer at chunk number 'chunk'
	virtual long toBinaryBuffer(int chunk, char* buffer, int maxlen);
	//! Set the data after separate reception
	virtual bool fromBinaryBuffer(int chunk, char* buffer, long len);

	//! Get the OS process ID
	int getProcID();
	char* unitTestLog;

private:
//	JString generateUUID() const;
//	char* classname;
//	JString unitTestLog;
};

#define INDEX_TYPE 6
#define INDEX_VARIATION 8
#define TYPE_RANDOM_BASED 4

} // namespace cmlabs

#include "JString.h"


#endif // !defined(AFX_OBJECT_H__D0E87E36_5CA6_4DB2_B512_46D23CDE9966__INCLUDED_)
