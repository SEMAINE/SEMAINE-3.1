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


// TimeSeries.h: interface for the ObjectDictionary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TIMESERIES_H_)
#define _TIMESERIES_H_

#include "Object.h"
#include "JTime.h"
#include "Collections.h"

namespace cmlabs {

class TimeSeries : public Object
{
public:
	TimeSeries();
	TimeSeries(const TimeSeries &ts);
	TimeSeries(const JString& xml);
	TimeSeries(XMLNode *node);
	virtual ~TimeSeries();

	const TimeSeries& operator = (const TimeSeries& profile);
	Object* clone() const;

	bool contains(const JTime& time);
	int getCount();

	Object* get(int pos);
	//! Will only return the first if more with same time exist
	Object* get(const JTime& time);
	Object* getFirstAfter(const JTime& time);
	Object* getLastBefore(const JTime& time);
	ObjectCollection* getAllBetween(const JTime& t1, const JTime& t2);
	ObjectCollection* getAllBefore(const JTime& time);
	ObjectCollection* getAllAfter(const JTime& time);
	bool put(const JTime& time, Object* obj);
	bool remove(const JTime& time);
	bool removeOldest();
	bool removeNewest();
	bool removeAllBetween(const JTime& t1, const JTime& t2);
	bool removeAllBefore(const JTime& time);
	bool removeAllAfter(const JTime& time);
	bool removeAll();

	JTime getTime(int pos);
	JTime getFirstTime();
	JTime getLastTime();
	JTime getNextTime();
	JTime getPreviousTime();

	Object* getOldest();
	Object* getNewest();

	JString print() ;
	bool unitTest();

protected:

	SortedCollection times;
	ObjectDictionary dict;
};

} // namespace cmlabs

#endif // !defined(_TIMESERIES_H_)
