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


// DataSample.h: interface for the DataSample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DATASAMPLE_H_)
#define _DATASAMPLE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "JTime.h"
#include "Collections.h"
#include "XMLParser.h"

namespace cmlabs {

class DataSample;
class Bitmap;
class HTMLPage;

class DataSample : public Object  
{
public:
	DataSample(const JString& type = "Raw", const JString& samplename = "");
	DataSample(XMLNode* node);
	virtual ~DataSample();

	virtual Object* clone() const;
	virtual DataSample* cloneWithoutData();

	virtual JString print();

	virtual JString toHTML();
	virtual HTMLPage* toHTMLBitmap();
	virtual JString toXML();
	virtual bool fromXML(XMLNode* node);

	JString getType();
	bool setType(const JString& type);

	bool isInUse();
	bool isLocked();
	bool setInUseBy(const JString& user, int timeout = 0);
	bool removeInUseBy(const JString& user);
	bool lockBy(const JString& locker);

	bool setTimestamp(const JTime& time);
	bool setParameters(Dictionary& dict);
	bool setDuration(long durationMS);
	bool giveData(char* sampledata, long datasize, long durationMS = 0);
	bool setDataCopy(char* sampledata, long datasize, long durationMS = 0);
	bool setDataLink(char* sampledata, long datasize, long durationMS = 0);

	JString getName();
	JTime getTimestamp();
	JTime getStartTime();
	JTime getEndTime();
	long getDuration();
	Dictionary getAllParameters();
	long getDataSize();
	long getDataMemUse();
	char* getDataCopy();
	char* getDataLink();
	bool clearDataFields();

	bool equals(const Object* o2) const;
	bool equals( const DataSample &s2 ) const;
	int compare(const Object* o2) const;
	int compareTo( const DataSample &s2 ) const;

	JString getParam(const JString& param);
	int getParamInt(const JString& param);
	double getParamFloat(const JString& param);

	bool setParam(const JString& param, const JString& value);
	bool setParamInt(const JString& param, int value);
	bool setParamFloat(const JString& param, double value);

	int getWidth();
	int getHeight();
	bool setWidth(int w);
	bool setHeight(int h);

	Object* getObject();
	bool setObject(Object* obj);

	bool readFromFile(const JString& filename);
	bool writeToFile(const JString& filename);

// ***************************************************************
// Binary Functions
// ***************************************************************

	//! Get the size of binary data chunk, -1 for all chunks
	long getBinarySize(int chunk);
	//! Get the number of data chunks
	int getBinaryChunkCount();
	//! Get a pointer to the binary data at chunk number 'chunk'
	char* getBinaryDataPointer(int chunk);
	//! Set the data after separate reception
	bool setBinaryChunk(int chunk, long binsize, char* data);

	long toBinaryBuffer(int chunk, char* buffer, int maxlen);
	bool fromBinaryBuffer(int chunk, char* buffer, long len);

	long getLongFromBytes(char* buffer);
	bool writeLongToBytes(char* buffer, long val);

	bool markForDeletion();
	bool isMarkedForDeletion();

	JString id;
	JString name;
	JString type;
	JTime timestamp;
	Dictionary params;
	char* data;
	char* link;
	long size;
	long duration;
	int width;
	int height;
	bool markedForDeletion;
	double sortfield;
	ObjectDictionary* channels;

	Object* object;
	Object* aux;

protected:

	ObjectDictionary users;
	JString lockedBy;
};

} // namespace cmlabs

#include "HTML.h"

#endif // _DATASAMPLE_H_
