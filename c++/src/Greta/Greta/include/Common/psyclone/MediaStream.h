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

#if !defined(_MEDIASTREAM_H_)
#define _MEDIASTREAM_H_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "Collections.h"
#include "InfoItem.h"
#include "DataSample.h"
#include "JSemaphore.h"

namespace cmlabs {

class MediaStream : public Object
{
public:
	MediaStream(const JString& streamname, long hardMaxSize = -1, long softMaxSize = -1, long maxCount = -1);
	virtual ~MediaStream();
	Object* clone() const;

	JString print() ;

	bool handleMultipleWriters(bool allow);

	bool addSample(DataSample* sample);
	bool addSamples(ObjectCollection* samples, bool deleteCol = true);
	bool deleteSample(DataSample* sample);
	bool deleteSample(int pos);

	bool deleteSampleAt(const JTime& time);
	bool deleteSamplesOlderThan(const JTime& time);
	bool deleteSamplesNewerThan(const JTime& time);
	bool deleteSamplesBetween(const JTime& fromTime, const JTime& toTime);

	long getTotalDataSize();
	long getTotalDataMemUse();
	int getCount();

	double getInputDataRate();
	double getOutputDataRate();
	double getTotalDataEverHandled();

	JTime getOldestSampleTime();
	JTime getNewestSampleTime();
	ObjectCollection* getDataSampleList();
	ObjectCollection* getTimestampList();

	DataSample* getOldestSample();
	DataSample* getNewestSample();
	DataSample* getSample(const JString& id);
	DataSample* getSampleAt(const JTime& time);
	DataSample* getFirstSampleAfter(const JString& id);
	DataSample* getFirstSampleAfter(const JTime& time);
	DataSample* getLastSampleBefore(const JTime& time);
	ObjectCollection* getSamplesBetween(const JTime& fromTime, const JTime& toTime);

	DataSample* waitForFirstSampleAfter(const JTime& time, long ms);
	DataSample* waitForLastSampleAfter(const JTime& time, long ms);
	DataSample* waitForFirstSampleAfter(const JString& id, long ms);
	DataSample* waitForLastSampleAfter(const JString& id, long ms);

	bool createChannel(const JString& name, const JString& fieldname);
	bool destroyChannel(const JString& name);
	ObjectCollection* searchChannel(const JString& name, double val1, double val2);

	InfoItem* getInfo();

	bool doMaintenance();

protected:

	bool addChannelSample(DataSample* sample);
	bool removeChannelSample(DataSample* sample);

//	long getTotalDataMemUseNoBlock();

//	TimeSeries dataSamples;
	ObjectTable dataSamples;
	JTime lastAccessTime;
	ObjectCollection deletedSamples;

	Dictionary channelNames;
	ObjectDictionary channels;

	JSemaphore newDataSem;
	JMutex accessMutex;

	JString name;
	long softMax;
	long hardMax;
	long maxCount;
	long maxBackupCount;
	long currentSize;

	bool allowMultipleWriters;
	double bytesEverHandled;
	ObjectTable outputData;
};

} // namespace cmlabs

#endif //_MEDIASTREAM_H_

